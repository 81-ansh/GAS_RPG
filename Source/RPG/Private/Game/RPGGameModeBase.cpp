// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RPGGameModeBase.h"
#include "EngineUtils.h"
#include "Game/LoadMenuSaveGame.h"
#include "Game/RPGGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "RPG/RPGLogChannel.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void ARPGGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadMenuSaveGameClass);
	ULoadMenuSaveGame* LoadMenuSaveGame = Cast<ULoadMenuSaveGame>(SaveGameObject);
	LoadMenuSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadMenuSaveGame->SaveSlotStatus = Taken;
	LoadMenuSaveGame->MapName = LoadSlot->GetMapName();
	LoadMenuSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(LoadMenuSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

ULoadMenuSaveGame* ARPGGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadMenuSaveGameClass);
	}
	ULoadMenuSaveGame* LoadMenuSaveGame = Cast<ULoadMenuSaveGame>(SaveGameObject);
	return LoadMenuSaveGame;
}

void ARPGGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadMenuSaveGame* ARPGGameModeBase::RetrieveInGameSaveData()
{
	URPGGameInstance* GameInstance = Cast<URPGGameInstance>(GetGameInstance());
	
	const FString InGameLoadSlotName = GameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = GameInstance->LoadSlotIndex;
	
	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void ARPGGameModeBase::SaveInGameProgressData(ULoadMenuSaveGame* SaveObject)
{
	URPGGameInstance* GameInstance = Cast<URPGGameInstance>(GetGameInstance());
	
	const FString InGameLoadSlotName = GameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = GameInstance->LoadSlotIndex;
	GameInstance->PlayerStartTag = SaveObject->PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void ARPGGameModeBase::SaveWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	URPGGameInstance* Instance = Cast<URPGGameInstance>(GetGameInstance());
	check(Instance)
	
	if (ULoadMenuSaveGame* SaveGame = GetSaveSlotData(Instance->LoadSlotName, Instance->LoadSlotIndex))
	{
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}
		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();	// Clear it out, we'll fill it in with "Actors"
		
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;
			
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();
			
			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;
			
			Actor->Serialize(Archive);
			
			SavedMap.SavedActors.AddUnique(SavedActor);
		}
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, Instance->LoadSlotName, Instance->LoadSlotIndex);
	}
}

void ARPGGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	URPGGameInstance* Instance = Cast<URPGGameInstance>(GetGameInstance());
	check(Instance)
	
	if (UGameplayStatics::DoesSaveGameExist(Instance->LoadSlotName, Instance->LoadSlotIndex))
	{
		ULoadMenuSaveGame* SaveGame = Cast<ULoadMenuSaveGame>(UGameplayStatics::LoadGameFromSlot(Instance->LoadSlotName, Instance->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogRPG, Error, TEXT("Failed to load slot"));
			return;
		}
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			
			if (!Actor->Implements<USaveInterface>()) continue;
			
			for (FSavedActor SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}
					FMemoryReader MemoryReader(SavedActor.Bytes);
					
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					Actor->Serialize(Archive);	// Converts binary bytes back into variables
					
					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void ARPGGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->GetLoadSlotName();
	const int32 SlotIndex = Slot->SlotIndex;
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
		
}

AActor* ARPGGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	URPGGameInstance* GameInstance = Cast<URPGGameInstance>(GetGameInstance());
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == GameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

void ARPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName, DefaultMap);
}
