// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *	RPGGameplayTags
 *
 *	Singleton containing native Gameplay Tags
 */

struct FRPGGameplayTags
{
public:
	static const FRPGGameplayTags& Get() {return GameplayTags;}
	static void InitializeNativeGameplayTags();

	FGameplayTag Attributes_Secondary_Armour;
	
protected:
	
private:
	static FRPGGameplayTags GameplayTags;
};
