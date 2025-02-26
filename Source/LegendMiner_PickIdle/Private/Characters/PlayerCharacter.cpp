#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OreSpawner.h"
#include "Ore.h"
#include "PickaxeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bMovingToTarget = false;
    bLookingAtOre = false;
    CurrentSpeed = 0.f;
    Speed = 0.f;
    TargetOre = nullptr;

    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));
        SetRootComponent(RootComponent);
    }

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = CameraDistance;
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

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

    PickaxeComponent = FindComponentByClass<UPickaxeComponent>();

    if (PickaxeComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("PickaxeComponent 찾음!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PickaxeComponent를 찾을 수 없음! 블루프린트에서 추가되었는지 확인하세요."));
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bMovingToTarget)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector ToTarget = TargetLocation - CurrentLocation;
        float Distance = ToTarget.Size();

        if (Distance <= AcceptableDistance)
        {
            bMovingToTarget = false;
            CurrentSpeed = 0.f;

            // 가장 가까운 광석 찾기
            FindClosestOre();
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
    else if (bLookingAtOre)
    {
        RotateTowardsOre(DeltaTime);
    }

    Speed = GetVelocity().Size();
}

void APlayerCharacter::SetTargetLocation(const FVector& NewTargetLocation)
{
    FVector AdjustedLocation = NewTargetLocation;
    AdjustedLocation.Z = GetActorLocation().Z;
    TargetLocation = AdjustedLocation;
    bMovingToTarget = true;
}

float APlayerCharacter::GetMiningSpeedBonus() const
{
    if (PickaxeComponent)
    {
        return PickaxeComponent->GetMiningSpeedBonus();
    }

    return 0.0f;
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

            // 광석의 콜리전 안에 있어야 채굴 가능
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
        StartMining();
    }
}

void APlayerCharacter::StartMining()
{
    if (TargetOre)
    {
        TargetOre->StartMining(this);
        bLookingAtOre = false;
    }
}
