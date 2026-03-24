#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleAudioComponent.generated.h"

class UAudioComponent;
class USoundBase;

UCLASS(ClassGroup=(Vehicle), meta=(BlueprintSpawnableComponent))
class UVehicleAudioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleAudioComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Vehicle|Audio")
    void UpdateEngineSound(float RPMNormalized, float ThrottleInput);

private:
    UPROPERTY(EditAnywhere, Category="Vehicle|Audio")
    TObjectPtr<USoundBase> EngineLoopSound;

    UPROPERTY(EditAnywhere, Category="Vehicle|Audio", meta=(ClampMin="0.6", ClampMax="2.5"))
    float MinPitch = 0.8f;

    UPROPERTY(EditAnywhere, Category="Vehicle|Audio", meta=(ClampMin="0.6", ClampMax="2.5"))
    float MaxPitch = 1.9f;

    UPROPERTY(Transient)
    TObjectPtr<UAudioComponent> AudioComponent = nullptr;
};
