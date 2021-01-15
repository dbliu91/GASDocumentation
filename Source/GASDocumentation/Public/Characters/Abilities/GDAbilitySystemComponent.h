// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GDAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UGDAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameplayEffectDurationChange, struct FActiveGameplayEffect&, float);

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	bool CharacterAbilitiesGiven = false;
	bool StartupEffectsApplied = false;

	FReceivedDamageDelegate ReceivedDamage;

	// Called from GDDamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	virtual void ReceiveDamage(UGDAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	TArray<FActiveGameplayEffectHandle> GetActiveEffectsHasTag(FGameplayTag& tag_to_find);

	bool SetGameplayEffectDurationHandle(FActiveGameplayEffectHandle Handle, float NewDuration);

	FOnGameplayEffectDurationChange onGameplayEffectDurationChangeDelegate;
	
	/** Called when the duration of a gamepaly effect has changed */
	virtual void OnGameplayEffectDurationChange(struct FActiveGameplayEffect& ActiveEffect);
};
