#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleInputComponent.generated.h"

class APlayerController;

USTRUCT(BlueprintType)
struct FVehicleInputState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) float Throttle = 0.0f;
    UPROPERTY(BlueprintReadOnly) float Brake = 0.0f;
    UPROPERTY(BlueprintReadOnly) float Steering = 0.0f;
    UPROPERTY(BlueprintReadOnly) bool bHandbrake = false;
    UPROPERTY(BlueprintReadOnly) int32 GearOffset = 0;
};

UCLASS(ClassGroup=(Vehicle), meta=(BlueprintSpawnableComponent))
class UVehicleInputComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleInputComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Vehicle|Input")
    void BindPlayerInput(APlayerController* PlayerController);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Input")
    void SetTouchSteeringNormalized(float SteeringWheelAxis);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Input")
    void SetTouchThrottleBrake(float ThrottleValue, float BrakeValue);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Input")
    const FVehicleInputState& GetInputState() const { return InputState; }

    UFUNCTION(BlueprintCallable, Category="Vehicle|Input")
    void ClearGearOffset();

private:
    void OnThrottle(float Value);
    void OnBrake(float Value);
    void OnSteer(float Value);
    void OnHandbrakePressed();
    void OnHandbrakeReleased();
    void OnGearUp();
    void OnGearDown();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    FVehicleInputState InputState;
};
