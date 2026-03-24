#include "VehicleAudioComponent.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UVehicleAudioComponent::UVehicleAudioComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UVehicleAudioComponent::BeginPlay()
{
    Super::BeginPlay();

    if (EngineLoopSound)
    {
        AudioComponent = UGameplayStatics::SpawnSoundAttached(EngineLoopSound, GetOwner()->GetRootComponent());
        if (AudioComponent)
        {
            AudioComponent->bAutoActivate = true;
            AudioComponent->Play();
        }
    }
}

void UVehicleAudioComponent::UpdateEngineSound(float RPMNormalized, float ThrottleInput)
{
    if (!AudioComponent) return;

    const float Pitch = FMath::Lerp(MinPitch, MaxPitch, FMath::Clamp(RPMNormalized, 0.0f, 1.0f));
    const float Volume = FMath::Lerp(0.35f, 1.0f, FMath::Clamp(ThrottleInput, 0.0f, 1.0f));

    AudioComponent->SetPitchMultiplier(Pitch);
    AudioComponent->SetVolumeMultiplier(Volume);
}
