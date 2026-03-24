#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "VehicleInventory.h"
#include "SaveGameManager.generated.h"

USTRUCT(BlueprintType)
struct FGarageSaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    bool bFirstLaunch = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    int32 WorldSeed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FName SelectedVehicleId = TEXT("default");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FVector GarageLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FRotator GarageRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FOwnedVehicleRecord> Vehicles;
};

UCLASS()
class UOpenWorldSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FGarageSaveData Data;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveLoaded);

UCLASS(BlueprintType)
class USaveGameManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Save")
    bool LoadOrCreate();

    UFUNCTION(BlueprintCallable, Category="Save")
    bool SaveNow();

    UFUNCTION(BlueprintCallable, Category="Save")
    void SetSaveData(const FGarageSaveData& InData);

    UFUNCTION(BlueprintPure, Category="Save")
    const FGarageSaveData& GetSaveData() const { return CachedData; }

    UPROPERTY(BlueprintAssignable, Category="Save")
    FOnSaveLoaded OnSaveLoaded;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FString SlotName = TEXT("WorldGarageSlot");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    int32 UserIndex = 0;

private:
    UPROPERTY()
    FGarageSaveData CachedData;
};
