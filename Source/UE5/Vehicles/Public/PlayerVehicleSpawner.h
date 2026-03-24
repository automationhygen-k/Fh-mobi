#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerVehicleSpawner.generated.h"

class APlayerVehicle;
class UCameraComponent;

UINTERFACE(BlueprintType)
class UVehicleSpawnPointProvider : public UInterface
{
    GENERATED_BODY()
};

class IVehicleSpawnPointProvider
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Vehicle|Spawn")
    bool FindSpawnTransform(FTransform& OutTransform) const;
};

UCLASS()
class APlayerVehicleSpawner : public AActor
{
    GENERATED_BODY()

public:
    APlayerVehicleSpawner();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Vehicle|Spawn")
    APlayerVehicle* SpawnPlayerVehicle();

private:
    UPROPERTY(EditAnywhere, Category="Vehicle|Spawn")
    TSubclassOf<APlayerVehicle> PlayerVehicleClass;

    UPROPERTY(EditAnywhere, Category="Vehicle|Spawn")
    TScriptInterface<IVehicleSpawnPointProvider> SpawnPointProvider;

    UPROPERTY(EditAnywhere, Category="Vehicle|Spawn")
    bool bAutoSpawnOnBeginPlay = true;
};
