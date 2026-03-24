#include "WorldEventManager.h"

void UWorldEventManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ActiveEvents.Reset();
    BlockedZones.Reset();
}

void UWorldEventManager::SetRoadProvider(const TScriptInterface<IWorldEventRoadProvider>& InRoadProvider)
{
    RoadProvider = InRoadProvider;
}

void UWorldEventManager::RegisterBlockedZone(FVector Location, float Radius)
{
    BlockedZones.Add(FVector4(Location.X, Location.Y, Location.Z, Radius));
}

void UWorldEventManager::UpdateEvents(const FVector& PlayerLocation, float DeltaSeconds)
{
    SpawnTimer += DeltaSeconds;

    ActiveEvents.RemoveAll([&PlayerLocation](const FWorldEventData& Event)
    {
        return FVector::DistSquared(Event.Location, PlayerLocation) > FMath::Square(1800.0f);
    });

    if (SpawnTimer < SpawnInterval || ActiveEvents.Num() >= MaxActiveEvents || !RoadProvider)
    {
        return;
    }

    SpawnTimer = 0.0f;

    TArray<FVector> Candidates;
    if (!IWorldEventRoadProvider::Execute_GetCandidateRoadPoints(RoadProvider.GetObject(), PlayerLocation, 500.0f, 1200.0f, Candidates))
    {
        return;
    }

    Candidates.RemoveAll([this](const FVector& Location)
    {
        return IsBlockedLocation(Location);
    });

    if (Candidates.Num() == 0)
    {
        return;
    }

    const int32 Choice = FMath::RandRange(0, Candidates.Num() - 1);
    FWorldEventData Event;
    Event.EventId = FName(*FString::Printf(TEXT("event_%d_%d"), static_cast<int32>(PickRandomEventType()), FMath::RandRange(1000, 999999)));
    Event.EventType = PickRandomEventType();
    Event.Location = Candidates[Choice];
    Event.bActive = true;

    ActiveEvents.Add(Event);
}

bool UWorldEventManager::CompleteEvent(FName EventId)
{
    const int32 Removed = ActiveEvents.RemoveAll([EventId](const FWorldEventData& Event)
    {
        return Event.EventId == EventId;
    });

    return Removed > 0;
}

bool UWorldEventManager::IsBlockedLocation(const FVector& Candidate) const
{
    for (const FVector4& Zone : BlockedZones)
    {
        const FVector Center(Zone.X, Zone.Y, Zone.Z);
        if (FVector::DistSquared(Center, Candidate) <= FMath::Square(Zone.W))
        {
            return true;
        }
    }

    return false;
}

EWorldEventType UWorldEventManager::PickRandomEventType() const
{
    return static_cast<EWorldEventType>(FMath::RandRange(0, static_cast<int32>(EWorldEventType::FuelEmergencyRequest)));
}
