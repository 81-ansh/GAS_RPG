// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RPGGameModeBase.generated.h"

class ULoadMenuSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;

/**
 * 
 */
UCLASS()
class RPG_API ARPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo; 
	
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);
	ULoadMenuSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	
	void TravelToMap(UMVVM_LoadSlot* Slot);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadMenuSaveGameClass;
	
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
protected:
	
	virtual void BeginPlay() override;
};
