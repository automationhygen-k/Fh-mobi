#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraShakeBase.h"
#include "VehicleCameraManager.generated.h"

class USpringArmComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum class EVehicleCameraMode : uint8
{
    ThirdPerson,
    CloseFollow,
    Hood
};

UCLASS(ClassGroup=(Vehicle), meta=(BlueprintSpawnableComponent))
class UVehicleCameraManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleCameraManager();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Vehicle|Camera")
    void SetCameraMode(EVehicleCameraMode NewMode);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Camera")
    void OnCollisionShake(float Intensity);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Camera")
    void SetSpeedKmh(float InSpeedKmh) { SpeedKmh = InSpeedKmh; }

private:
    void ApplyModeSettings();

private:
    UPROPERTY(EditAnywhere, Category="Vehicle|Camera")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(EditAnywhere, Category="Vehicle|Camera")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditAnywhere, Category="Vehicle|Camera")
    TSubclassOf<UCameraShakeBase> CollisionShakeClass;

    UPROPERTY(EditAnywhere, Category="Vehicle|Camera")
    EVehicleCameraMode CameraMode = EVehicleCameraMode::ThirdPerson;

    UPROPERTY(EditAnywhere, Category="Vehicle|Camera", meta=(ClampMin="70", ClampMax="110"))
    float BaseFOV = 85.0f;

    UPROPERTY(EditAnywhere, Category="Vehicle|Camera", meta=(ClampMin="0", ClampMax="25"))
    float MaxFOVBonus = 15.0f;

    float SpeedKmh = 0.0f;
};
