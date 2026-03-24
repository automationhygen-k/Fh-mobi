#include "WorldMapManager.h"

void UWorldMapManager::RegisterOrUpdateMarker(FName MarkerId, EMapMarkerType MarkerType, FVector WorldLocation, bool bDiscovered)
{
    if (MarkerId.IsNone())
    {
        return;
    }

    FMapLocationData* Existing = MarkerMap.Find(MarkerId);
    const bool bWasDiscovered = Existing ? Existing->bDiscovered : false;

    FMapLocationData Data;
    Data.MarkerId = MarkerId;
    Data.MarkerType = MarkerType;
    Data.WorldLocation = WorldLocation;
    Data.bDiscovered = bDiscovered;

    MarkerMap.Add(MarkerId, Data);

    if (!bWasDiscovered && bDiscovered)
    {
        OnLocationDiscovered.Broadcast(Data);
    }
}

bool UWorldMapManager::SetMarkerDiscovered(FName MarkerId, bool bDiscovered)
{
    FMapLocationData* Existing = MarkerMap.Find(MarkerId);
    if (!Existing)
    {
        return false;
    }

    const bool bWasDiscovered = Existing->bDiscovered;
    Existing->bDiscovered = bDiscovered;

    if (!bWasDiscovered && bDiscovered)
    {
        OnLocationDiscovered.Broadcast(*Existing);
    }

    return true;
}

bool UWorldMapManager::RemoveMarker(FName MarkerId)
{
    return MarkerMap.Remove(MarkerId) > 0;
}

bool UWorldMapManager::GetMarker(FName MarkerId, FMapLocationData& OutMarkerData) const
{
    const FMapLocationData* Existing = MarkerMap.Find(MarkerId);
    if (!Existing)
    {
        return false;
    }

    OutMarkerData = *Existing;
    return true;
}

TArray<FMapLocationData> UWorldMapManager::GetAllMarkers(bool bOnlyDiscovered) const
{
    TArray<FMapLocationData> Out;
    Out.Reserve(MarkerMap.Num());

    for (const TPair<FName, FMapLocationData>& Pair : MarkerMap)
    {
        if (!bOnlyDiscovered || Pair.Value.bDiscovered)
        {
            Out.Add(Pair.Value);
        }
    }

    return Out;
}

void UWorldMapManager::UpdatePlayerLocation(FVector InPlayerLocation)
{
    PlayerLocation = InPlayerLocation;
    RegisterOrUpdateMarker(TEXT("player"), EMapMarkerType::Player, PlayerLocation, true);
}
