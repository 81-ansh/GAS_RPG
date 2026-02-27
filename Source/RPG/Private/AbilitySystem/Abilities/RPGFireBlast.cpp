// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/RPGFireBlast.h"

FString URPGFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			/* Title */
			"<Title>FIRE BLAST</>\n\n"
			
			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			// Number of FireBalls
			"<Default>Launches %d </>"
			"<Default>fire balls in all directions, each coming back and </>"
			"<Default>exploding upon return, causing </>"
			
			// Damage
			"<Damage>%d</><Default> radial fire damage with a chance to burn</>"
			), Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage);
}

FString URPGFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			/* Title */
			"<Title>NEXT LEVEL: </>\n\n"
			
			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			// Number of FireBalls
			"<Default>Launches %d </>"
			"<Default>fire balls in all directions, each coming back and </>"
			"<Default>exploding upon return, causing </>"
			
			// Damage
			"<Damage>%d</><Default> radial fire damage with a chance to burn</>"
			), Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage);
}

TArray<ARPGFireBall*> URPGFireBlast::SpawnFireBalls()
{
	return TArray<ARPGFireBall*>();
}
