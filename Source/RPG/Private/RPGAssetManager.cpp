// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAssetManager.h"
#include "RPGGameplayTags.h"
#include "AbilitySystemGlobals.h"

URPGAssetManager& URPGAssetManager::Get()
{
	check(GEngine);

	URPGAssetManager* RPGAssetManager = Cast<URPGAssetManager>(GEngine->AssetManager);
	return *RPGAssetManager;
}

void URPGAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FRPGGameplayTags::InitializeNativeGameplayTags();

	// This is required to use Target Data!
	UAbilitySystemGlobals::Get().InitGlobalData();
}
