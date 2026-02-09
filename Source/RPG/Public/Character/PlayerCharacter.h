// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API APlayerCharacter : public ACharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
public:
	APlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUP_Implementation() override;
	/* End Player Interface */

	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	/* End Combat Interface */

private:
	virtual void InitAbilityActorInfo() override;
};
