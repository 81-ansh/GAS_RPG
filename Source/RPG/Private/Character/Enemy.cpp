// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"

void AEnemy::HighlightActor()
{
	bHighlighted = true;
}

void AEnemy::UnHighlightActor()
{
	bHighlighted = false;
}
