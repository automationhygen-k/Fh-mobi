#pragma once

#include "CoreMinimal.h"
#include "ChaosWheeledVehiclePawn.h"
#include "VehicleStatusData.h"
#include "PlayerVehicle.generated.h"

class UVehicleInputComponent;
class UFuelComponent;
class UVehicleDamageComponent;
class UVehicleCameraManager;
class UVehicleAudioComponent;
class UChaosWheeledVehicleMovementComponent;

UCLASS(Blueprintable)
class APlayerVehicle : public AChaosWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    APlayerVehicle();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="Vehicle")
    const FVehicleStatusData& GetVehicleStatusData() const { return StatusData; }

    UFUNCTION(BlueprintCallable, Category="Vehicle|Environment")
    void SetWeatherGripMultiplier(float InGripMultiplier);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Fuel")
    void BeginRefuel();

    UFUNCTION(BlueprintCallable, Category="Vehicle|Fuel")
    void EndRefuel();

    UFUNCTION(BlueprintImplementableEvent, Category="Vehicle|Fuel")
    void OnRefuelPromptRequested(bool bShowPrompt, const FString& PromptText);

protected:
    UFUNCTION()
    void HandleOutOfFuel();

    UFUNCTION()
    void OnHitEvent(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    void UpdateStatusData(float DeltaSeconds);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vehicle|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UVehicleInputComponent> VehicleInput;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vehicle|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UFuelComponent> FuelComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vehicle|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UVehicleDamageComponent> DamageComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vehicle|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UVehicleCameraManager> CameraManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vehicle|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UVehicleAudioComponent> AudioComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vehicle|Status", meta=(AllowPrivateAccess="true"))
    FVehicleStatusData StatusData;

    bool bRefueling = false;
    bool bEngineEnabled = true;
    float WeatherGripMultiplier = 1.0f;
};
