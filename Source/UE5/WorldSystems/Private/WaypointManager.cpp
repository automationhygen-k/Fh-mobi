#include "WaypointManager.h"

void UWaypointManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (GetGameInstance())
    {
        RouteManager = GetGameInstance()->GetSubsystem<UNavigationRouteManager>();
    }
}

bool UWaypointManager::SetActiveWaypoint(const FWaypointData& Waypoint, const FVector& RouteStartLocation, const TScriptInterface<IRoadRouteProvider>& RouteProvider)
{
    if (!RouteManager || Waypoint.WaypointId.IsNone())
    {
        return false;
    }

    ActiveWaypoint = Waypoint;
    ActiveRoute = RouteManager->GetOrBuildRoute(RouteStartLocation, Waypoint.TargetLocation, RouteProvider);
    bHasActiveWaypoint = ActiveRoute.bIsValid;

    if (bHasActiveWaypoint)
    {
        OnWaypointChanged.Broadcast(ActiveWaypoint);
    }

    return bHasActiveWaypoint;
}

void UWaypointManager::ClearActiveWaypoint()
{
    bHasActiveWaypoint = false;
    ActiveWaypoint = FWaypointData();
    ActiveRoute = FNavigationRouteData();
    OnWaypointChanged.Broadcast(ActiveWaypoint);
}
