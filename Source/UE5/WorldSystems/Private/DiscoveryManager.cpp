#include "DiscoveryManager.h"

#include "WorldMapManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UDiscoveryManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (GetGameInstance())
    {
        WorldMapManager = GetGameInstance()->GetSubsystem<UWorldMapManager>();
    }
}

void UDiscoveryManager::Tick(float DeltaTime)
{
    UpdateAccumulator += DeltaTime;
    if (UpdateAccumulator < UpdateInterval)
    {
        return;
    }
    UpdateAccumulator = 0.0f;

    RefreshPlayerLocation();

    for (FDiscoverableEntry& Entry : Discoverables)
    {
        if (Entry.bDiscovered)
        {
            continue;
        }

        if (FVector::DistSquared(Entry.WorldLocation, CachedPlayerLocation) <= FMath::Square(Entry.DiscoveryRadius))
        {
            Entry.bDiscovered = true;
            if (WorldMapManager)
            {
                WorldMapManager->RegisterOrUpdateMarker(Entry.MarkerId, Entry.MarkerType, Entry.WorldLocation, true);
            }
        }
    }
}

TStatId UDiscoveryManager::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UDiscoveryManager, STATGROUP_Tickables);
}

void UDiscoveryManager::RegisterDiscoverable(const FDiscoverableEntry& Entry)
{
    if (Entry.MarkerId.IsNone())
    {
        return;
    }

    Discoverables.RemoveAll([&Entry](const FDiscoverableEntry& Existing)
    {
        return Existing.MarkerId == Entry.MarkerId;
    });

    Discoverables.Add(Entry);

    if (WorldMapManager)
    {
        WorldMapManager->RegisterOrUpdateMarker(Entry.MarkerId, Entry.MarkerType, Entry.WorldLocation, Entry.bDiscovered);
    }
}

bool UDiscoveryManager::MarkDiscovered(FName MarkerId)
{
    for (FDiscoverableEntry& Entry : Discoverables)
    {
        if (Entry.MarkerId == MarkerId)
        {
            Entry.bDiscovered = true;
            if (WorldMapManager)
            {
                WorldMapManager->RegisterOrUpdateMarker(Entry.MarkerId, Entry.MarkerType, Entry.WorldLocation, true);
            }
            return true;
        }
    }

    return false;
}

void UDiscoveryManager::RefreshPlayerLocation()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (APlayerController* PC = World->GetFirstPlayerController())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            CachedPlayerLocation = Pawn->GetActorLocation();
            if (WorldMapManager)
            {
                WorldMapManager->UpdatePlayerLocation(CachedPlayerLocation);
            }
        }
    }
}
