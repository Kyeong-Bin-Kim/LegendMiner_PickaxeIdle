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
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = CameraDistance;
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    // 카메라
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
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
    if (PickaxeComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("PickaxeComponent found!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PickaxeComponent not found!"));
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
                CurrentRotation.Normalize();

                float RotationSpeed = 5.f;
                FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
                SetActorRotation(NewRotation);
            }
            AddMovementInput(Direction, CurrentSpeed / MaxSpeed);
        }
    }
    // 광석을 바라보고 있는 경우, 회전 로직
    else if (bLookingAtOre)
    {
        RotateTowardsOre(DeltaTime);
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

    // 컴포넌트가 없으면 1.0(보너스 없음)
    return 1.0f;
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

    FRotator CurrentRotation = GetActorRotation();
    float RotationSpeed = 2.f;
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);

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