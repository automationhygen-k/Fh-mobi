#include "NavigationRouteManager.h"

FNavigationRouteData UNavigationRouteManager::GetOrBuildRoute(const FVector& Start, const FVector& End, const TScriptInterface<IRoadRouteProvider>& RouteProvider)
{
    const FString CacheKey = BuildKey(Start, End);
    if (const FNavigationRouteData* Cached = RouteCache.Find(CacheKey))
    {
        return *Cached;
    }

    FNavigationRouteData NewRoute;
    if (RouteProvider)
    {
        TArray<FVector> Nodes;
        const bool bBuilt = IRoadRouteProvider::Execute_BuildRoadRoute(RouteProvider.GetObject(), Start, End, Nodes);
        if (bBuilt && Nodes.Num() >= 2)
        {
            NewRoute.PathNodes = Nodes;
            NewRoute.bIsValid = true;
        }
    }

    if (!NewRoute.bIsValid)
    {
        NewRoute = BuildFallbackRoute(Start, End);
    }

    float Distance = 0.0f;
    for (int32 Index = 1; Index < NewRoute.PathNodes.Num(); ++Index)
    {
        Distance += FVector::Dist(NewRoute.PathNodes[Index - 1], NewRoute.PathNodes[Index]);
    }

    NewRoute.DistanceMeters = Distance;
    const float AvgSpeedMps = 18.0f;
    NewRoute.EstimatedTravelTimeSeconds = Distance / AvgSpeedMps;

    RouteCache.Add(CacheKey, NewRoute);
    return NewRoute;
}

void UNavigationRouteManager::InvalidateCache()
{
    RouteCache.Reset();
}

FString UNavigationRouteManager::BuildKey(const FVector& Start, const FVector& End)
{
    const FVector QuantStart = Start.GridSnap(500.0f);
    const FVector QuantEnd = End.GridSnap(500.0f);
    return FString::Printf(TEXT("%s_%s"), *QuantStart.ToCompactString(), *QuantEnd.ToCompactString());
}

FNavigationRouteData UNavigationRouteManager::BuildFallbackRoute(const FVector& Start, const FVector& End)
{
    FNavigationRouteData Route;
    Route.PathNodes.Add(Start);
    Route.PathNodes.Add(End);
    Route.bIsValid = true;
    return Route;
}
