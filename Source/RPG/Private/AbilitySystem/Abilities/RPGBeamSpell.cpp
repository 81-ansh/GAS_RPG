// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/RPGBeamSpell.h"

#include "AbilitySystem/RPGAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void URPGBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void URPGBeamSpell::StoreOwnerVariables()
{
	
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor); 
	}
}

void URPGBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &URPGBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &URPGBeamSpell::PrimaryTargetDied);
		}
	}
}

void URPGBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	
	TArray<AActor*> OverlappingActors;
	URPGAbilitySystemLibrary::GetLivePlayersWithinRadius(GetAvatarActorFromActorInfo(), OverlappingActors, ActorsToIgnore, 850.f, MouseHitActor->GetActorLocation());
	
	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	URPGAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());
	
	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &URPGBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &URPGBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

void URPGBeamSpell::RemoveOnDeathBindingFromPrimaryTarget()
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor);
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &ThisClass::PrimaryTargetDied);
	}
}
 
void URPGBeamSpell::RemoveOnDeathBindingFromAdditionalTarget(AActor* AdditionalTarget)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AdditionalTarget);
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &ThisClass::AdditionalTargetDied);
	}
}
