#include "GarageLocationManager.h"

void UGarageLocationManager::GenerateFromSettlementProvider(const TScriptInterface<ISettlementAnchorProvider>& SettlementProvider)
{
    Garages.Reset();

    if (!SettlementProvider)
    {
        return;
    }

    TArray<FTransform> SmallAnchors;
    TArray<FTransform> LargeAnchors;

    const bool bHasAnchors = ISettlementAnchorProvider::Execute_GetGarageAnchors(SettlementProvider.GetObject(), SmallAnchors, LargeAnchors);
    if (!bHasAnchors || SmallAnchors.Num() == 0)
    {
        return;
    }

    Garages.Add(BuildGarageFromAnchor(SmallAnchors[0], true));

    for (const FTransform& Anchor : LargeAnchors)
    {
        Garages.Add(BuildGarageFromAnchor(Anchor, false));
    }
}

bool UGarageLocationManager::GetMainGarage(FGarageLocationData& OutMainGarage) const
{
    const FGarageLocationData* Main = Garages.FindByPredicate([](const FGarageLocationData& Item)
    {
        return Item.bIsMainGarage;
    });

    if (!Main)
    {
        return false;
    }

    OutMainGarage = *Main;
    return true;
}

FGarageLocationData UGarageLocationManager::BuildGarageFromAnchor(const FTransform& AnchorTransform, bool bMain)
{
    FGarageLocationData Out;
    Out.GarageTransform = AnchorTransform;
    Out.bIsMainGarage = bMain;

    const FVector Right = AnchorTransform.GetRotation().GetRightVector();
    const FVector Forward = AnchorTransform.GetRotation().GetForwardVector();

    Out.ParkingSpots.Add(FTransform(AnchorTransform.GetRotation(), AnchorTransform.GetLocation() + Forward * 450.0f + Right * 350.0f));
    Out.ParkingSpots.Add(FTransform(AnchorTransform.GetRotation(), AnchorTransform.GetLocation() + Forward * 450.0f - Right * 350.0f));
    Out.ParkingSpots.Add(FTransform(AnchorTransform.GetRotation(), AnchorTransform.GetLocation() + Forward * 620.0f + Right * 350.0f));

    return Out;
}
