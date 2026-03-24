#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MapMarker.h"
#include "NavigationRouteManager.h"
#include "WaypointManager.generated.h"

UENUM(BlueprintType)
enum class EWaypointType : uint8
{
    Manual,
    Settlement,
    FuelStation,
    Garage,
    Mission
};

USTRUCT(BlueprintType)
struct FWaypointData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waypoint")
    FName WaypointId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waypoint")
    EWaypointType WaypointType = EWaypointType::Manual;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waypoint")
    FVector TargetLocation = FVector::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaypointChanged, const FWaypointData&, WaypointData);

UCLASS(BlueprintType)
class UWaypointManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Waypoint")
    bool SetActiveWaypoint(const FWaypointData& Waypoint, const FVector& RouteStartLocation, const TScriptInterface<IRoadRouteProvider>& RouteProvider);

    UFUNCTION(BlueprintCallable, Category="Waypoint")
    void ClearActiveWaypoint();

    UFUNCTION(BlueprintPure, Category="Waypoint")
    bool HasActiveWaypoint() const { return bHasActiveWaypoint; }

    UFUNCTION(BlueprintPure, Category="Waypoint")
    FWaypointData GetActiveWaypoint() const { return ActiveWaypoint; }

    UFUNCTION(BlueprintPure, Category="Waypoint")
    FNavigationRouteData GetActiveRoute() const { return ActiveRoute; }

    UPROPERTY(BlueprintAssignable, Category="Waypoint")
    FOnWaypointChanged OnWaypointChanged;

private:
    UPROPERTY()
    TObjectPtr<UNavigationRouteManager> RouteManager;

    UPROPERTY()
    FWaypointData ActiveWaypoint;

    UPROPERTY()
    FNavigationRouteData ActiveRoute;

    bool bHasActiveWaypoint = false;
};
