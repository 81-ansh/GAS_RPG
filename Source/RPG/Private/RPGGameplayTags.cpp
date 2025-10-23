// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameplayTags.h"

#include "GameplayTagsManager.h"

FRPGGameplayTags FRPGGameplayTags::GameplayTags;

void FRPGGameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armour"), FString("Reduces damage taken, improves block chance"));
}
