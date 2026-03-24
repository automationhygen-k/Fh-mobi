#include "SaveGameManager.h"

#include "Kismet/GameplayStatics.h"

void USaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadOrCreate();
}

bool USaveGameManager::LoadOrCreate()
{
    if (UOpenWorldSaveGame* Loaded = Cast<UOpenWorldSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)))
    {
        CachedData = Loaded->Data;
        OnSaveLoaded.Broadcast();
        return true;
    }

    CachedData = FGarageSaveData();
    const bool bSaved = SaveNow();
    OnSaveLoaded.Broadcast();
    return bSaved;
}

bool USaveGameManager::SaveNow()
{
    UOpenWorldSaveGame* SaveObj = Cast<UOpenWorldSaveGame>(UGameplayStatics::CreateSaveGameObject(UOpenWorldSaveGame::StaticClass()));
    if (!SaveObj)
    {
        return false;
    }

    SaveObj->Data = CachedData;
    return UGameplayStatics::SaveGameToSlot(SaveObj, SlotName, UserIndex);
}

void USaveGameManager::SetSaveData(const FGarageSaveData& InData)
{
    CachedData = InData;
}
