// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "NiagaraComponent.h"
#include "AbilitySystemComponent.h"
#include "RPGGameplayTags.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGAbilitySystemLibrary.h"
#include "AbilitySystem/RPGAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/CharacterBase.h"
#include "Game/LoadMenuSaveGame.h"
#include "Game/RPGGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainPlayerController.h"
#include "Player/MainPlayerState.h"
#include "UI/HUD/RPGHUD.h"

APlayerCharacter::APlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1000.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	CharacterClass = ECharacterClass::Elementalist;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init ability actor info for the server
	InitAbilityActorInfo();
	LoadProgress();
	
	if (ARPGGameModeBase* GameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->LoadWorldState(GetWorld());
	}
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the client
	InitAbilityActorInfo();
}

void APlayerCharacter::AddToXP_Implementation(int32 InXP)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToXP(InXP);
}

void APlayerCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void APlayerCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

int32 APlayerCharacter::GetXP_Implementation() const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetXP();
}

int32 APlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 APlayerCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 APlayerCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void APlayerCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToLevel(InPlayerLevel);
	
	if (URPGAbilitySystemComponent* ASC = Cast<URPGAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->UpdateAbilityStatuses(MainPlayerState->GetPlayerLevel());
	}
}

void APlayerCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToAttributePoints(InAttributePoints);
}

void APlayerCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 APlayerCharacter::GetAttributePoints_Implementation() const
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetAttributePoints();
}

int32 APlayerCharacter::GetSpellPoints_Implementation() const
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetSpellPoints();
}

void APlayerCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(GetController()))
	{
		MainPlayerController->ShowMagicCircle(DecalMaterial);
		MainPlayerController->bShowMouseCursor = false;
	}
}

void APlayerCharacter::HideMagicCircle_Implementation()
{
	if (AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(GetController()))
	{
		MainPlayerController->HideMagicCircle();
		MainPlayerController->bShowMouseCursor = true;
	}
}

void APlayerCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	ARPGGameModeBase* GameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		ULoadMenuSaveGame* SaveData = GameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;
		
		SaveData->PlayerStartTag = CheckpointTag;
		
		if (AMainPlayerState* MainPlayerState = Cast<AMainPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = MainPlayerState->GetPlayerLevel();
			SaveData->XP = MainPlayerState->GetXP();
			SaveData->SpellPoints = MainPlayerState->GetSpellPoints();
			SaveData->AttributePoints = MainPlayerState->GetAttributePoints();
		}
		SaveData->Strength = URPGAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = URPGAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = URPGAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = URPGAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		
		SaveData->bFirstTimeLoadIn = false;
		
		if (!HasAuthority()) return;
		
		URPGAbilitySystemComponent* ASC = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda([this, ASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
		{
			const FGameplayTag AbilityTag = ASC->GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo = URPGAbilitySystemLibrary::GetAbilityInfo(this);
			FRPGAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			
			FSavedAbility SavedAbility;
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityLevel = AbilitySpec.Level;
			SavedAbility.AbilitySlot = ASC->GetSlotFromAbilityTag(AbilityTag);
			SavedAbility.AbilityStatus = ASC->GetStatusFromAbilityTag(AbilityTag);
			SavedAbility.AbilityType = Info.AbilityType;
			SavedAbility.AbilityTag = AbilityTag;
			
			SaveData->SavedAbilities.AddUnique(SavedAbility);
		});
		ASC->ForEachAbility(SaveAbilityDelegate);
		GameMode->SaveInGameProgressData(SaveData);
	}
}

int32 APlayerCharacter::GetPlayerLevel_Implementation()
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetPlayerLevel();
}

void APlayerCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);
	
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		ARPGGameModeBase* GameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (GameMode)
		{
			GameMode->PlayerDied(this);
		}
	});
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	TopDownCameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void APlayerCharacter::OnRep_Stunned()
{
	if (URPGAbilitySystemComponent* ASC = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FRPGGameplayTags& GameplayTags = FRPGGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			ASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			ASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void APlayerCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void APlayerCharacter::LoadProgress()
{
	ARPGGameModeBase* GameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		ULoadMenuSaveGame* SaveData = GameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;
		
		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			
			if (URPGAbilitySystemComponent* ASC = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent))
			{
				ASC->AddCharacterAbilitiesFromSavedData(SaveData);
			}
			
			if (AMainPlayerState* MainPlayerState = Cast<AMainPlayerState>(GetPlayerState()))
			{
				MainPlayerState->SetLevel(SaveData->PlayerLevel);
				MainPlayerState->SetXP(SaveData->XP);
				MainPlayerState->SetAttributePoints(SaveData->AttributePoints);
				MainPlayerState->SetSpellPoints(SaveData->SpellPoints);
			}
			URPGAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
		}
	}
}

void APlayerCharacter::InitAbilityActorInfo()
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MainPlayerState, this);
	Cast<URPGAbilitySystemComponent>(MainPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = MainPlayerState->GetAbilitySystemComponent();
	AttributeSet = MainPlayerState->GetAttributeSet();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FRPGGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APlayerCharacter::StunTagChanged);
	

	if (AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(GetController()))
	{
		if (ARPGHUD* RPGHUD = Cast<ARPGHUD>(MainPlayerController->GetHUD()))
		{
			RPGHUD->InitOverlay(MainPlayerController, MainPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}
