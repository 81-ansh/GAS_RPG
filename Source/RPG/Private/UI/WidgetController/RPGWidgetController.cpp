// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/RPGWidgetController.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Player/MainPlayerController.h"
#include "Player/MainPlayerState.h"
#include "AbilitySystem/RPGAttributeSet.h"

void URPGWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController =  WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent =  WCParams.AbilitySystemComponent;
	AttributeSet =  WCParams.AttributeSet;
}

void URPGWidgetController::BroadcastInitialValues()
{
	
}

void URPGWidgetController::BindCallbacksToDependencies()
{
	
}

void URPGWidgetController::BroadcastAbilityInfo()
{
	if (!GetASC()->bStartupAbilitiesGiven) return;
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FRPGAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(RPGAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = RPGAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetASC()->ForEachAbility(BroadcastDelegate);
}

AMainPlayerController* URPGWidgetController::GetPC()
{
	if (MainPlayerController == nullptr)
	{
		MainPlayerController = Cast<AMainPlayerController>(PlayerController);
	}
	return MainPlayerController;
}

AMainPlayerState* URPGWidgetController::GetPS()
{
	if (MainPlayerState == nullptr)
	{
		MainPlayerState = Cast<AMainPlayerState>(PlayerState);
	}
	return MainPlayerState;
}

URPGAbilitySystemComponent* URPGWidgetController::GetASC()
{
	if (RPGAbilitySystemComponent == nullptr)
	{
		RPGAbilitySystemComponent = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent);
	}
	return RPGAbilitySystemComponent;
}

URPGAttributeSet* URPGWidgetController::GetAS()
{
	if (RPGAttributeSet == nullptr)
	{
		RPGAttributeSet = Cast<URPGAttributeSet>(AttributeSet);
	}
	return RPGAttributeSet;
}
