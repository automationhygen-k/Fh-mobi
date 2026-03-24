#include "VehicleInputComponent.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"
#include "Components/InputComponent.h"

UVehicleInputComponent::UVehicleInputComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UVehicleInputComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UVehicleInputComponent::BindPlayerInput(APlayerController* PlayerController)
{
    if (!PlayerController || !PlayerController->InputComponent) return;

    UInputComponent* IC = PlayerController->InputComponent;
    IC->BindAxis(TEXT("VehicleThrottle"), this, &UVehicleInputComponent::OnThrottle);
    IC->BindAxis(TEXT("VehicleBrake"), this, &UVehicleInputComponent::OnBrake);
    IC->BindAxis(TEXT("VehicleSteer"), this, &UVehicleInputComponent::OnSteer);

    IC->BindAction(TEXT("VehicleHandbrake"), IE_Pressed, this, &UVehicleInputComponent::OnHandbrakePressed);
    IC->BindAction(TEXT("VehicleHandbrake"), IE_Released, this, &UVehicleInputComponent::OnHandbrakeReleased);
    IC->BindAction(TEXT("VehicleGearUp"), IE_Pressed, this, &UVehicleInputComponent::OnGearUp);
    IC->BindAction(TEXT("VehicleGearDown"), IE_Pressed, this, &UVehicleInputComponent::OnGearDown);
}

void UVehicleInputComponent::SetTouchSteeringNormalized(float SteeringWheelAxis)
{
    InputState.Steering = FMath::Clamp(SteeringWheelAxis, -1.0f, 1.0f);
}

void UVehicleInputComponent::SetTouchThrottleBrake(float ThrottleValue, float BrakeValue)
{
    InputState.Throttle = FMath::Clamp(ThrottleValue, 0.0f, 1.0f);
    InputState.Brake = FMath::Clamp(BrakeValue, 0.0f, 1.0f);
}

void UVehicleInputComponent::ClearGearOffset()
{
    InputState.GearOffset = 0;
}

void UVehicleInputComponent::OnThrottle(float Value)
{
    InputState.Throttle = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UVehicleInputComponent::OnBrake(float Value)
{
    InputState.Brake = FMath::Clamp(Value, 0.0f, 1.0f);
}

void UVehicleInputComponent::OnSteer(float Value)
{
    InputState.Steering = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UVehicleInputComponent::OnHandbrakePressed()
{
    InputState.bHandbrake = true;
}

void UVehicleInputComponent::OnHandbrakeReleased()
{
    InputState.bHandbrake = false;
}

void UVehicleInputComponent::OnGearUp()
{
    InputState.GearOffset += 1;
}

void UVehicleInputComponent::OnGearDown()
{
    InputState.GearOffset -= 1;
}
