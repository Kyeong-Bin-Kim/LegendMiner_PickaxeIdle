#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
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
            UE_LOG(LogTemp, Warning, TEXT("Cached PlayerInventoryWidget Successfully!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cache PlayerInventoryWidget!"));
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

        if (Distance <= AcceptableDistance)
        {
            bMovingToTarget = false;
            CurrentSpeed = 0.f;
            FindClosestOre(); // 광석 찾기
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
        // 광석을 바라보고 있는 경우, 회전 로직
        if (bLookingAtOre)
        {
            RotateTowardsOre(DeltaTime);
        }
    }


    // 이동 속도 업데이트 (애니메이션 블루프린트에서 사용 가능)
    Speed = GetVelocity().Size();
}

void APlayerCharacter::SetTargetLocation(const FVector& NewTargetLocation)
{
    if (bMining)
    {
        StopMining();
    }

    FVector AdjustedLocation = NewTargetLocation;
    AdjustedLocation.Z = GetActorLocation().Z;
    TargetLocation = AdjustedLocation;
    bMovingToTarget = true;
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

void APlayerCharacter::RotateTowardsOre(float DeltaTime)
{
    if (!TargetOre) return;

    FVector DirectionToOre = (TargetOre->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = DirectionToOre.Rotation();
    TargetRotation.Pitch = 0.f;
    TargetRotation.Roll = 0.f;

	//bMining = true; -- 회전 용

    //GetCharacterMovement()->Velocity = GetActorForwardVector() * 200.0f; -- 회전 용

    FRotator CurrentRotation = GetActorRotation();

    float RotationSpeed = 5.f;
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);

    //bMining = false; -- 회전 용

    float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));
    if (AngleDiff < 3.0f)
    {
        bLookingAtOre = false;
        StartMining(); // 광석 채굴 로직 시작
    }
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
        TargetOre = nullptr; // 필요하다면, 더 이상 타겟이 없음을 표시
    }
}

void APlayerCharacter::StopMiningAndRestart()
{
	bLookingAtOre = false;
	StopMining();
    GetWorld()->GetTimerManager().SetTimer(FindOreTimerHandle, this, &APlayerCharacter::FindClosestOre, 0.5f, false);
}

UPickaxeComponent* APlayerCharacter::GetPickaxeComponent() const
{
    return PickaxeComponent;
}
