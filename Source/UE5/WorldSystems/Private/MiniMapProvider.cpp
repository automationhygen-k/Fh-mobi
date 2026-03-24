#include "MiniMapProvider.h"

#include "WorldMapManager.h"

FMiniMapSnapshot UMiniMapProvider::BuildSnapshot(const FVector& PlayerLocation, const FRotator& PlayerRotation, float Radius, const TScriptInterface<IMiniMapRoadProvider>& RoadProvider)
{
    FMiniMapSnapshot Snapshot;
    Snapshot.PlayerLocation = PlayerLocation;
    Snapshot.PlayerRotation = PlayerRotation;

    if (!WorldMapManager && GetGameInstance())
    {
        WorldMapManager = GetGameInstance()->GetSubsystem<UWorldMapManager>();
    }

    if (WorldMapManager)
    {
        const TArray<FMapLocationData> Discovered = WorldMapManager->GetAllMarkers(true);
        for (const FMapLocationData& Marker : Discovered)
        {
            if (FVector::DistSquared(Marker.WorldLocation, PlayerLocation) <= FMath::Square(Radius))
            {
                Snapshot.NearbyMarkers.Add(Marker);
            }
        }
    }

    if (RoadProvider)
    {
        IMiniMapRoadProvider::Execute_GetRoadPointsAround(RoadProvider.GetObject(), PlayerLocation, Radius, Snapshot.NearbyRoadPoints);
    }

    return Snapshot;
}
