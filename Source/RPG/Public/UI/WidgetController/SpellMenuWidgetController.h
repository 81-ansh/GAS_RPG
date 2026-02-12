// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API USpellMenuWidgetController : public UOverlayWidgetController
{
	GENERATED_BODY()
	
public:
	
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
};
