#include "MissionManager.h"

#include "WorldEventManager.h"

void UMissionManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (GetGameInstance())
    {
        WaypointManager = GetGameInstance()->GetSubsystem<UWaypointManager>();
        WorldEventManager = GetGameInstance()->GetSubsystem<UWorldEventManager>();
    }
}

void UMissionManager::SetWorldEventManager(UWorldEventManager* InWorldEventManager)
{
    WorldEventManager = InWorldEventManager;
}

void UMissionManager::AddAvailableMission(const FMissionData& MissionData)
{
    if (MissionData.MissionId.IsNone())
    {
        return;
    }

    AvailableMissions.RemoveAll([&MissionData](const FMissionData& Item)
    {
        return Item.MissionId == MissionData.MissionId;
    });

    AvailableMissions.Add(MissionData);
}

bool UMissionManager::AcceptMission(FName MissionId, const FVector& CurrentPlayerLocation, const TScriptInterface<IRoadRouteProvider>& RouteProvider)
{
    if (!WaypointManager || bHasActiveMission)
    {
        return false;
    }

    FMissionData* FoundMission = AvailableMissions.FindByPredicate([MissionId](const FMissionData& Item)
    {
        return Item.MissionId == MissionId;
    });

    if (!FoundMission)
    {
        return false;
    }

    ActiveMission = *FoundMission;
    bHasActiveMission = true;

    FWaypointData Waypoint;
    Waypoint.WaypointId = FName(*FString::Printf(TEXT("mission_wp_%s"), *MissionId.ToString()));
    Waypoint.WaypointType = EWaypointType::Mission;
    Waypoint.TargetLocation = ActiveMission.ObjectiveLocation;

    WaypointManager->SetActiveWaypoint(Waypoint, CurrentPlayerLocation, RouteProvider);

    OnMissionAccepted.Broadcast(ActiveMission);
    return true;
}

bool UMissionManager::UpdateMissionProgress(FVector PlayerLocation)
{
    if (!bHasActiveMission)
    {
        return false;
    }

    const float DistSq = FVector::DistSquared(PlayerLocation, ActiveMission.ObjectiveLocation);
    if (DistSq > FMath::Square(ActiveMission.CompletionRadius))
    {
        return false;
    }

    ActiveMission.bCompleted = true;
    bHasActiveMission = false;

    if (WaypointManager)
    {
        WaypointManager->ClearActiveWaypoint();
    }

    if (WorldEventManager)
    {
        WorldEventManager->CompleteEvent(ActiveMission.MissionId);
    }

    OnMissionCompleted.Broadcast(ActiveMission);
    return true;
}
