#include "MapMarker.h"

AMapMarker::AMapMarker()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
}

void AMapMarker::SetMarkerData(FName InMarkerId, EMapMarkerType InMarkerType, bool bInDiscovered)
{
    MarkerId = InMarkerId;
    MarkerType = InMarkerType;
    bDiscovered = bInDiscovered;
}
