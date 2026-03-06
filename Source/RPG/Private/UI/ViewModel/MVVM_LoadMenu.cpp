// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadMenu.h"

#include "Game/RPGGameInstance.h"
#include "Game/RPGGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadMenu::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_0->SlotIndex = 0;
	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_1->SlotIndex = 1;
	
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlots.Add(2, LoadSlot_2);
	LoadSlot_2->SlotIndex = 2;
	
	SetNumLoadSlots(LoadSlots.Num());
}

UMVVM_LoadSlot* UMVVM_LoadMenu::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadMenu::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	ARPGGameModeBase* GameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode))
	{
		GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Magenta, FString("Please switch to Single Player"));
		return;
	}
	LoadSlots[Slot]->SetMapName(GameMode->DefaultMapName);
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SlotStatus = Taken;
	LoadSlots[Slot]->PlayerStartTag = GameMode->DefaultPlayerStartTag;
	
	GameMode->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();
	
	URPGGameInstance* GameInstance = Cast<URPGGameInstance>(GameMode->GetGameInstance());
	GameInstance->LoadSlotName = LoadSlots[Slot]->GetLoadSlotName();
	GameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
	GameInstance->PlayerStartTag = GameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadMenu::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadMenu::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadMenu::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		ARPGGameModeBase::DeleteSlot(SelectedSlot->GetLoadSlotName(), SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadMenu::PlayButtonPressed()
{
	ARPGGameModeBase* GameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
	URPGGameInstance* GameInstance = Cast<URPGGameInstance>(GameMode->GetGameInstance());
	GameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	GameInstance->LoadSlotName = SelectedSlot->GetLoadSlotName();
	GameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;
	
	if (IsValid(SelectedSlot))
	{
		GameMode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadMenu::LoadData()
{
	ARPGGameModeBase* GameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode)) return;
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		ULoadMenuSaveGame* SaveObject = GameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(), LoadSlot.Key);
		const FString PlayerName = SaveObject->PlayerName;
		TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SaveSlotStatus;
		
		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->InitializeSlot();
		
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
	}
}

void UMVVM_LoadMenu::SetNumLoadSlots(int32 InNumLoadSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InNumLoadSlots);
}
