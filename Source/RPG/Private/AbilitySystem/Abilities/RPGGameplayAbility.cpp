// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/RPGGameplayAbility.h"

FString URPGGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol Lol", Level);
}

FString URPGGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage. </>"), Level);
}

FString URPGGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"), Level);
}
