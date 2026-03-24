#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldEventManager.generated.h"

UENUM(BlueprintType)
enum class EWorldEventType : uint8
{
    DeliveryMission,
    ScenicViewpoint,
    AbandonedCar,
    RoadsideRepairRequest,
    FuelEmergencyRequest
};

USTRUCT(BlueprintType)
struct FWorldEventData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldEvent")
    FName EventId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldEvent")
    EWorldEventType EventType = EWorldEventType::DeliveryMission;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldEvent")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldEvent")
    bool bActive = true;
};

UINTERFACE(BlueprintType)
class UWorldEventRoadProvider : public UInterface
{
    GENERATED_BODY()
};

class IWorldEventRoadProvider
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="WorldEvent")
    bool GetCandidateRoadPoints(const FVector& PlayerLocation, float MinDistance, float MaxDistance, TArray<FVector>& OutRoadPoints) const;
};

UCLASS(BlueprintType)
class UWorldEventManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="WorldEvent")
    void SetRoadProvider(const TScriptInterface<IWorldEventRoadProvider>& InRoadProvider);

    UFUNCTION(BlueprintCallable, Category="WorldEvent")
    void RegisterBlockedZone(FVector Location, float Radius);

    UFUNCTION(BlueprintCallable, Category="WorldEvent")
    void UpdateEvents(const FVector& PlayerLocation, float DeltaSeconds);

    UFUNCTION(BlueprintCallable, Category="WorldEvent")
    bool CompleteEvent(FName EventId);

    UFUNCTION(BlueprintPure, Category="WorldEvent")
    const TArray<FWorldEventData>& GetActiveEvents() const { return ActiveEvents; }

private:
    bool IsBlockedLocation(const FVector& Candidate) const;
    EWorldEventType PickRandomEventType() const;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WorldEvent", meta=(AllowPrivateAccess="true", ClampMin="1", ClampMax="10"))
    int32 MaxActiveEvents = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WorldEvent", meta=(AllowPrivateAccess="true", ClampMin="1.0"))
    float SpawnInterval = 6.0f;

    UPROPERTY()
    TArray<FWorldEventData> ActiveEvents;

    UPROPERTY()
    TArray<FVector4> BlockedZones;

    UPROPERTY()
    TScriptInterface<IWorldEventRoadProvider> RoadProvider;

    float SpawnTimer = 0.0f;
};
