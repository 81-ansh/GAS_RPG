// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/RPGFireBlast.h"

#include "AbilitySystem/RPGAbilitySystemLibrary.h"
#include "Actor/RPGFireBall.h"

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
	TArray<ARPGFireBall*> FireBalls;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = URPGAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);
	
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		
		ARPGFireBall* FireBall = GetWorld()->SpawnActorDeferred<ARPGFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBalls.Add(FireBall);
		FireBall->FinishSpawning(SpawnTransform);
	}
	
	return FireBalls;
}
