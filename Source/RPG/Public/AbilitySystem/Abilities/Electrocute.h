// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RPGBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UElectrocute : public URPGBeamSpell
{
	GENERATED_BODY()
	
public:
	
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
