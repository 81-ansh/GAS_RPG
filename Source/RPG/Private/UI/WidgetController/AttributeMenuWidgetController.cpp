// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "RPGGameplayTags.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/MainPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	URPGAttributeSet* AS = CastChecked<URPGAttributeSet>(AttributeSet);
	check(AttributeInfo)

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	AttributePointsChangedDelegate.Broadcast(GetPS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo)	
	for (auto& Pair : GetAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	
	GetPS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	URPGAbilitySystemComponent* RPGASC = CastChecked<URPGAbilitySystemComponent>(AbilitySystemComponent);
	RPGASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FRPGAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
