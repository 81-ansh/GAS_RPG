// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/RPGAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const URPGAttributeSet* RPGAttributeSet =CastChecked<URPGAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(RPGAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(RPGAttributeSet->GetMaxHealth());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
}
