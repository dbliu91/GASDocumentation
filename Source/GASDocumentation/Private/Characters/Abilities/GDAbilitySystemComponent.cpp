// Copyright 2020 Dan Kestranek.


#include "Characters/Abilities/GDAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"

void UGDAbilitySystemComponent::ReceiveDamage(UGDAbilitySystemComponent * SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

TArray<FActiveGameplayEffectHandle> UGDAbilitySystemComponent::GetActiveEffectsHasTag(FGameplayTag& tag_to_find)
{
	//FGameplayTag tag_cooldown = FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill"));

	TArray<FActiveGameplayEffectHandle> ReturnList;

	FActiveGameplayEffectsContainer*	tempActiveGameplayEffects = &ActiveGameplayEffects;

	{
		for (const FActiveGameplayEffect& effect : tempActiveGameplayEffects)
		{
			FGameplayTagContainer AssetTags;
			effect.Spec.GetAllAssetTags(AssetTags);

			FGameplayTagContainer GrantedTags;
			effect.Spec.GetAllGrantedTags(GrantedTags);

			if (AssetTags.HasTag(tag_to_find) || GrantedTags.HasTag(tag_to_find))
			{
				ReturnList.Add(effect.Handle);
			}
		}
	}

	return ReturnList;
}

bool UGDAbilitySystemComponent::SetGameplayEffectDurationHandle(FActiveGameplayEffectHandle Handle, float NewDuration)
{
	if (!Handle.IsValid())
	{
		return false;
	}

	FActiveGameplayEffect* AGE = (FActiveGameplayEffect*)GetActiveGameplayEffect(Handle);
	if (!AGE)
	{
		return false;
	}

	//FActiveGameplayEffect* AGE = const_cast<FActiveGameplayEffect*>(ActiveGameplayEffect);
	if (NewDuration > 0)
	{
		AGE->Spec.Duration = NewDuration;
	}
	else
	{
		AGE->Spec.Duration = 0.01f;
	}

	AGE->StartServerWorldTime = ActiveGameplayEffects.GetServerWorldTime();
	AGE->CachedStartServerWorldTime = AGE->StartServerWorldTime;
	AGE->StartWorldTime = ActiveGameplayEffects.GetWorldTime();
	ActiveGameplayEffects.MarkItemDirty(*AGE);
	ActiveGameplayEffects.CheckDuration(Handle);

	AGE->EventSet.OnTimeChanged.Broadcast(AGE->Handle, AGE->StartWorldTime, AGE->GetDuration());
	OnGameplayEffectDurationChange(*AGE);

	return true;
}

void UGDAbilitySystemComponent::OnGameplayEffectDurationChange(struct FActiveGameplayEffect& ActiveEffect)
{
	onGameplayEffectDurationChangeDelegate.Broadcast(ActiveEffect, ActiveEffect.Spec.Duration);
}
