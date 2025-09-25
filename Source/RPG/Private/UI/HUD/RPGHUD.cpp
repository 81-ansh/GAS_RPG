// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/RPGHUD.h"
#include "UI/Widget/RPGUserWidget.h"

void ARPGHUD::BeginPlay()
{
	Super::BeginPlay();

	URPGUserWidget* Widget = CreateWidget<URPGUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->AddToViewport();
}
