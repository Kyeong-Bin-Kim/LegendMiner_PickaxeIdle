#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bMovingToTarget = false;
    CurrentSpeed = 0.f;
    Speed = 0.f;

    // 루트 컴포넌트 설정
    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));
        SetRootComponent(RootComponent);
    }

    // 스프링 암 생성 및 설정
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = CameraDistance;
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    // 카메라 생성 및 설정
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
    Camera->SetRelativeLocation(CameraLotation);
    Camera->SetRelativeRotation(CameraRotation);

    // 캐릭터 회전 및 이동 설정
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
}


void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
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
            CurrentSpeed = 0.f; // 도착 시 속도 초기화
        }
        else
        {
            const float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
            const float AccelerationRate = 300.f; // (유닛/초^2)
            CurrentSpeed = FMath::Min(CurrentSpeed + AccelerationRate * DeltaTime, MaxSpeed);

            // 이동 방향 계산 및 정규화
            FVector Direction = ToTarget.GetSafeNormal();

            if (!Direction.IsNearlyZero())
            {
                FRotator TargetRotation = Direction.Rotation();
                TargetRotation.Pitch = 0.f; // 피치 고정
                TargetRotation.Roll = 0.f;  // 롤 고정

                // 현재 회전값을 정규화
                FRotator CurrentRotation = GetActorRotation();
                CurrentRotation.Normalize();

                float RotationSpeed = 5.f; // 회전 속도 조절
                FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
                SetActorRotation(NewRotation);
            }

            // 이동 실행
            AddMovementInput(Direction, CurrentSpeed / MaxSpeed);
        }
    }
    else
    {
        CurrentSpeed = 0.f;
    }

    // 애니메이션 블루프린트에 전달할 실제 이동 속도 업데이트
    Speed = GetVelocity().Size();
}


void APlayerCharacter::SetTargetLocation(const FVector& NewTargetLocation)
{
    FVector AdjustedLocation = NewTargetLocation;
    AdjustedLocation.Z = GetActorLocation().Z;
    TargetLocation = AdjustedLocation;
    bMovingToTarget = true;
}