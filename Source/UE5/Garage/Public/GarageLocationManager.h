#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GarageLocationManager.generated.h"

USTRUCT(BlueprintType)
struct FGarageLocationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    FTransform GarageTransform = FTransform::Identity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    TArray<FTransform> ParkingSpots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    bool bIsMainGarage = false;
};

UINTERFACE(BlueprintType)
class USettlementAnchorProvider : public UInterface
{
    GENERATED_BODY()
};

class ISettlementAnchorProvider
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Settlement")
    bool GetGarageAnchors(TArray<FTransform>& OutSmallSettlementAnchors, TArray<FTransform>& OutLargeSettlementAnchors) const;
};

UCLASS(BlueprintType)
class UGarageLocationManager : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Garage|Location")
    void GenerateFromSettlementProvider(const TScriptInterface<ISettlementAnchorProvider>& SettlementProvider);

    UFUNCTION(BlueprintPure, Category="Garage|Location")
    const TArray<FGarageLocationData>& GetAllGarages() const { return Garages; }

    UFUNCTION(BlueprintPure, Category="Garage|Location")
    bool GetMainGarage(FGarageLocationData& OutMainGarage) const;

private:
    static FGarageLocationData BuildGarageFromAnchor(const FTransform& AnchorTransform, bool bMain);

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Garage|Location", meta=(AllowPrivateAccess="true"))
    TArray<FGarageLocationData> Garages;
};
