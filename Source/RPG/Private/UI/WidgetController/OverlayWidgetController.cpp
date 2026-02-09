// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/MainPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const URPGAttributeSet* RPGAttributeSet = CastChecked<URPGAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(RPGAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(RPGAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(RPGAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(RPGAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	AMainPlayerState* MainPlayerState = CastChecked<AMainPlayerState>(PlayerState);
	MainPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	MainPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);
	
	const URPGAttributeSet* RPGAttributeSet = CastChecked<URPGAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);
	
	if (URPGAbilitySystemComponent* RPGASC = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (RPGASC->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(RPGASC);
		}
		else
		{
			RPGASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}
		
		RPGASC->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					// For example, say that Tag = Message.HealthPotion
					// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(URPGAbilitySystemComponent* RPGAbilitySystemComponent)
{
	//TODO: Get information about all given abilities, look up there ability info, and broadcast it to widgets.
	if (!RPGAbilitySystemComponent->bStartupAbilitiesGiven) return;
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, RPGAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		//TODO: Need a way to figure out the ability tag for a given ability spec.
		FRPGAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(RPGAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = RPGAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	RPGAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	const AMainPlayerState* MainPlayerState = CastChecked<AMainPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = MainPlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out MainPlayerState Blueprint"));
	
	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();
	
	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;
		
		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;
		
		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);
		
		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}
