// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API AEnemy : public ACharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AEnemy();
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

};
