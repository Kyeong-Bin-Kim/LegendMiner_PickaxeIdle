#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterController.h"
#include "Ore.h"
#include "PickaxeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimInstance.h"
#include "LegendMinerHUD.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bMovingToTarget = false;
    bLookingAtOre = false;
    bMining = false;

    CurrentSpeed = 0.f;
    Speed = 0.f;
    TargetOre = nullptr;
    CurrentMiningSpeed = 0.f;

    // 루트 컴포넌트
    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));
        SetRootComponent(RootComponent);
    }

    // 스프링암
    PlayerSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    PlayerSpringArm->SetupAttachment(RootComponent);
    PlayerSpringArm->TargetArmLength = CameraDistance;
    PlayerSpringArm->bUsePawnControlRotation = false;
    PlayerSpringArm->bInheritPitch = false;
    PlayerSpringArm->bInheritYaw = false;
    PlayerSpringArm->bInheritRoll = false;

    // 카메라
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(PlayerSpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
    Camera->SetRelativeLocation(CameraLotation);
    Camera->SetRelativeRotation(CameraRotation);

    // 회전은 입력이 아닌 이동 방향 기준
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    MessageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MessageWidget"));
    MessageWidgetComponent->SetupAttachment(RootComponent);
    MessageWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    MessageWidgetComponent->SetDrawSize(FVector2D(300.f, 50.f));
    MessageWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Pickaxe 컴포넌트 찾기
    PickaxeComponent = FindComponentByClass<UPickaxeComponent>();

	// 인벤토리 위젯 찾기
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        ALegendMinerHUD* HUD = Cast<ALegendMinerHUD>(PlayerController->GetHUD());
        if (HUD && HUD->PlayerInventoryWidgetInstance)
        {
            CachedInventoryWidget = HUD->PlayerInventoryWidgetInstance;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("인벤토리 위젯 찾기 실패!"));
        }
    }

    if (UUserWidget* Widget = MessageWidgetComponent->GetUserWidgetObject())
    {
        FloatingWidgetInstance = Cast<UFloatingMessageWidget>(Widget);
        if (!FloatingWidgetInstance)
        {
            UE_LOG(LogTemp, Error, TEXT("❌ 위젯 캐스팅 실패"));
        }

        if (FloatingWidgetInstance)
        {
            FloatingWidgetInstance->SetMessageText(FText::GetEmpty());
        }
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 타겟 위치로 이동 중인 경우
    if (bMovingToTarget)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector ToTarget = TargetLocation - CurrentLocation;
        float Distance = ToTarget.Size();

        // 이동 시간 초과 체크
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if ((CurrentTime - MoveStartTime) >= MaxMoveDuration)
        {
            if (APlayerCharacterController* PC = Cast<APlayerCharacterController>(GetController()))
            {
                PC->SetCanClickToMove(true);
            }
        }

        // 도착 판정 거리 계산 (광석 트리거 반지름 + 허용 거리)
        float ArrivalThreshold = AcceptableDistance;
        if (TargetOre && TargetOre->OreTrigger)
        {
            float TriggerRadius = TargetOre->OreTrigger->GetScaledSphereRadius();
            ArrivalThreshold += TriggerRadius;
        }

        // 도착했는지 체크
        if (Distance <= ArrivalThreshold)
        {
            bMovingToTarget = false;
            CurrentSpeed = 0.f;

            // 입력 다시 허용
            if (APlayerCharacterController* PC = Cast<APlayerCharacterController>(GetController()))
            {
                PC->SetCanClickToMove(true);
            }

            HideFloatingMessage();

            // 광석이면 회전 시작
            if (TargetOre)
            {
                bLookingAtOre = true;
            }
            else
            {
                bLookingAtOre = false;
            }
        }
        else
        {
            const float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
            const float AccelerationRate = 300.f;
            CurrentSpeed = FMath::Min(CurrentSpeed + AccelerationRate * DeltaTime, MaxSpeed);

            FVector Direction = ToTarget.GetSafeNormal();
            if (!Direction.IsNearlyZero())
            {
                FRotator TargetRotation = Direction.Rotation();
                TargetRotation.Pitch = 0.f;
                TargetRotation.Roll = 0.f;
                FRotator CurrentRotation = GetActorRotation();

                // 보간 회전
                float RotationSpeed = 5.f;
                FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
                SetActorRotation(NewRotation);
            }

            AddMovementInput(Direction, CurrentSpeed / MaxSpeed);
        }
    }
    else
    {
        // 광석을 바라보는 중이라면 회전 처리
        if (!bMovingToTarget)
        {
            if (bLookingAtOre && TargetOre)
            {
                RotateTowardsOre(DeltaTime);
            }
        }
    }

    // 애니메이션용 속도 업데이트
    Speed = GetVelocity().Size();
}

void APlayerCharacter::SetTargetLocation(const FVector& NewTargetLocation)
{
    if (bMining)
    {
        StopMining();
    }

    AOre* ClosestOre = FindClosestOreFromPoint(NewTargetLocation);

    FVector FinalTarget;

    if (ClosestOre)
    {
        TargetOre = ClosestOre;
        FinalTarget = ClosestOre->GetActorLocation();
    }
    else
    {
        TargetOre = nullptr;
        FinalTarget = NewTargetLocation;
    }

    FinalTarget.Z = GetActorLocation().Z;
	TargetLocation = FinalTarget;
    bMovingToTarget = true;
    MoveStartTime = GetWorld()->GetTimeSeconds();

    FText MoveMessage = TargetOre ?
        FText::FromString(TEXT("가까운 광석을 향해 이동 중...")) :
        FText::FromString(TEXT("이동 중..."));

    ShowFloatingMessage(MoveMessage);

    if (APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController()))
    {
        PlayerController->SpawnClickMarkerAtLocation(NewTargetLocation); // 클릭 지점
        if (TargetOre)
        {
            PlayerController->SpawnClickMarkerAtLocation(FinalTarget); // 이동할 광석 앞 위치
        }
    }
}

float APlayerCharacter::GetMiningSpeedBonus() const
{
    // 예: PickaxeComponent가 2.0, 5.0, 10.0 등의 배수를 반환
    if (PickaxeComponent)
    {
        return PickaxeComponent->GetMiningSpeedBonus();
    }

    return 0.0f;
}

float APlayerCharacter::GetMiningSpeed() const
{
    return (TargetOre ? TargetOre->GetMiningTime() : 1.0f);
}

void APlayerCharacter::FindClosestOre()
{
    TArray<AActor*> Ores;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Ore"), Ores);

    float MinDistance = FLT_MAX;
    AOre* ClosestOre = nullptr;

    for (AActor* Ore : Ores)
    {
        float Distance = FVector::Dist(Ore->GetActorLocation(), GetActorLocation());
        AOre* OreActor = Cast<AOre>(Ore);
        if (OreActor && OreActor->OreTrigger)
        {
            float TriggerRadius = OreActor->OreTrigger->GetScaledSphereRadius();
            if (Distance < TriggerRadius && Distance < MinDistance)
            {
                MinDistance = Distance;
                ClosestOre = OreActor;
            }
        }
    }

    if (ClosestOre)
    {
        TargetOre = ClosestOre;
        bLookingAtOre = true;
    }
    else
    {
        TargetOre = nullptr;
        bLookingAtOre = false;
    }
}

AOre* APlayerCharacter::FindClosestOreFromPoint(const FVector& SearchOrigin)
{
    TArray<AActor*> Ores;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Ore"), Ores);

    float MinDistance = FLT_MAX;
    AOre* ClosestOre = nullptr;

    for (AActor* Ore : Ores)
    {
        AOre* OreActor = Cast<AOre>(Ore);
        if (OreActor && OreActor->OreTrigger)
        {
            float Distance = FVector::Dist(OreActor->GetActorLocation(), SearchOrigin);
            float TriggerRadius = OreActor->OreTrigger->GetScaledSphereRadius();

            // 트리거 반지름 + 허용 반경 이내인 경우 유효
            if (Distance <= (TriggerRadius + OreSearchRadius) && Distance < MinDistance)
            {
                MinDistance = Distance;
                ClosestOre = OreActor;
            }
        }
    }

    return ClosestOre;
}

void APlayerCharacter::RotateTowardsOre(float DeltaTime)
{
    if (!TargetOre) return;

    FVector DirectionToOre = (TargetOre->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = DirectionToOre.Rotation();
    TargetRotation.Pitch = 0.f;
    TargetRotation.Roll = 0.f;

    FRotator CurrentRotation = GetActorRotation();

    float RotationSpeed = 5.f;
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);

    float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));
    if (AngleDiff < 3.0f)
    {
        bLookingAtOre = false;
        
        // 광석 채굴 로직 시작
        StartMining();
    }
}

void APlayerCharacter::ShowFloatingMessage(const FText& Message)
{
    UE_LOG(LogTemp, Warning, TEXT("👉 ShowFloatingMessage: %s"), *Message.ToString());

    if (FloatingWidgetInstance)
    {
        FloatingWidgetInstance->SetMessageText(Message);
        MessageWidgetComponent->SetVisibility(true);
    }
}

void APlayerCharacter::HideFloatingMessage()
{
    MessageWidgetComponent->SetVisibility(false);
}

void APlayerCharacter::StartMining()
{
    if (TargetOre)
    {
        bMining = true;
        TargetOre->StartMining(this);
        bLookingAtOre = false;
    }
}

void APlayerCharacter::StopMining()
{
    bMining = false;

    if (TargetOre)
    {
        TargetOre->StopMining();
        TargetOre = nullptr;
    }
}

void APlayerCharacter::StopMiningAndRestart()
{
	bLookingAtOre = false;
	StopMining();

    // 일정 시간 후 채굴 탐색 재시도
    GetWorld()->GetTimerManager().SetTimer(FindOreTimerHandle, this, &APlayerCharacter::FindClosestOre, 0.5f, false);
}

UPickaxeComponent* APlayerCharacter::GetPickaxeComponent() const
{
    return PickaxeComponent;
}
