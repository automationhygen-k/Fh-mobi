#include "VehicleCameraManager.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

UVehicleCameraManager::UVehicleCameraManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.02f;
}

void UVehicleCameraManager::BeginPlay()
{
    Super::BeginPlay();
    ApplyModeSettings();
}

void UVehicleCameraManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Camera) return;

    const float TargetFOV = BaseFOV + FMath::Clamp(SpeedKmh / 180.0f, 0.0f, 1.0f) * MaxFOVBonus;
    Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, 4.0f));
}

void UVehicleCameraManager::SetCameraMode(EVehicleCameraMode NewMode)
{
    CameraMode = NewMode;
    ApplyModeSettings();
}

void UVehicleCameraManager::OnCollisionShake(float Intensity)
{
    if (!CollisionShakeClass) return;
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;
    if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
    {
        PC->ClientStartCameraShake(CollisionShakeClass, FMath::Clamp(Intensity, 0.2f, 2.0f));
    }
}

void UVehicleCameraManager::ApplyModeSettings()
{
    if (!SpringArm || !Camera) return;

    switch (CameraMode)
    {
        case EVehicleCameraMode::ThirdPerson:
            SpringArm->TargetArmLength = 420.0f;
            SpringArm->SocketOffset = FVector(0.0f, 0.0f, 60.0f);
            SpringArm->CameraLagSpeed = 8.0f;
            break;
        case EVehicleCameraMode::CloseFollow:
            SpringArm->TargetArmLength = 260.0f;
            SpringArm->SocketOffset = FVector(0.0f, 0.0f, 40.0f);
            SpringArm->CameraLagSpeed = 10.0f;
            break;
        case EVehicleCameraMode::Hood:
            SpringArm->TargetArmLength = 40.0f;
            SpringArm->SocketOffset = FVector(120.0f, 0.0f, 50.0f);
            SpringArm->CameraLagSpeed = 14.0f;
            break;
    }
}
