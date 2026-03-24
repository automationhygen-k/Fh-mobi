#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnvironmentPreset.generated.h"

class UNiagaraSystem;
class UMaterialInterface;

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Clear UMETA(DisplayName="Clear"),
    Cloudy UMETA(DisplayName="Cloudy"),
    LightRain UMETA(DisplayName="Light Rain"),
    HeavyRain UMETA(DisplayName="Heavy Rain"),
    Foggy UMETA(DisplayName="Foggy")
};

UCLASS(BlueprintType)
class UEnvironmentPreset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
    FName PresetName = TEXT("SunnyDay");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weather")
    EWeatherType WeatherType = EWeatherType::Clear;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Atmosphere", meta=(ClampMin="0.0", ClampMax="0.08"))
    float FogDensity = 0.004f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Atmosphere")
    FLinearColor FogColor = FLinearColor(0.65f, 0.75f, 0.85f, 1.0f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Atmosphere")
    TSoftObjectPtr<UMaterialInterface> SkyboxMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Atmosphere", meta=(ClampMin="0.0", ClampMax="1.0"))
    float CloudDensity = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lighting", meta=(ClampMin="0.0", ClampMax="2.5"))
    float SunIntensity = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lighting")
    FLinearColor AmbientColor = FLinearColor(0.6f, 0.6f, 0.65f, 1.0f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weather", meta=(ClampMin="0.0", ClampMax="1.0"))
    float RainIntensity = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weather", meta=(ClampMin="0.0", ClampMax="1.0"))
    float WindStrength = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Time", meta=(ClampMin="0.0", ClampMax="24.0"))
    float TimeOfDay = 12.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
    TSoftObjectPtr<UNiagaraSystem> RainFX;
};
