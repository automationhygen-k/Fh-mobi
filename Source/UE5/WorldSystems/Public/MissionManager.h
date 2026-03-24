#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MissionData.h"
#include "WaypointManager.h"
#include "MissionManager.generated.h"

class UWorldEventManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionAccepted, const FMissionData&, MissionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionCompleted, const FMissionData&, MissionData);

UCLASS(BlueprintType)
class UMissionManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Mission")
    void SetWorldEventManager(UWorldEventManager* InWorldEventManager);

    UFUNCTION(BlueprintCallable, Category="Mission")
    void AddAvailableMission(const FMissionData& MissionData);

    UFUNCTION(BlueprintCallable, Category="Mission")
    bool AcceptMission(FName MissionId, const FVector& CurrentPlayerLocation, const TScriptInterface<IRoadRouteProvider>& RouteProvider);

    UFUNCTION(BlueprintCallable, Category="Mission")
    bool UpdateMissionProgress(FVector PlayerLocation);

    UFUNCTION(BlueprintPure, Category="Mission")
    bool HasActiveMission() const { return bHasActiveMission; }

    UFUNCTION(BlueprintPure, Category="Mission")
    FMissionData GetActiveMission() const { return ActiveMission; }

    UFUNCTION(BlueprintPure, Category="Mission")
    const TArray<FMissionData>& GetAvailableMissions() const { return AvailableMissions; }

    UPROPERTY(BlueprintAssignable, Category="Mission")
    FOnMissionAccepted OnMissionAccepted;

    UPROPERTY(BlueprintAssignable, Category="Mission")
    FOnMissionCompleted OnMissionCompleted;

private:
    UPROPERTY()
    TObjectPtr<UWaypointManager> WaypointManager;

    UPROPERTY()
    TObjectPtr<UWorldEventManager> WorldEventManager;

    UPROPERTY()
    TArray<FMissionData> AvailableMissions;

    UPROPERTY()
    FMissionData ActiveMission;

    bool bHasActiveMission = false;
};
