// 


#include "Characters/Abilities/GDGA_CharacterNewSkill.h"
#include "Characters/GDCharacterBase.h"
#include "GASDocumentation/GASDocumentation.h"
//#include "AbilitySystemComponent.h"
#include "Characters/Abilities/GDAbilitySystemComponent.h"

#include "GameplayTagContainer.h"

UGDGA_CharacterNewSkill::UGDGA_CharacterNewSkill()
{
	AbilityInputID = EGDAbilityInputID::None;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NewSkill")));
}

void UGDGA_CharacterNewSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		const AGDCharacterBase* Character = CastChecked<AGDCharacterBase>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
		
		if (Character) {
			
			auto asc = Cast<UGDAbilitySystemComponent>(Character->GetAbilitySystemComponent());

			if (asc)
			{
				for (const FGameplayAbilitySpec& AbilitySpec : asc->GetActivatableAbilities())
				{
					if (AbilitySpec.Ability == nullptr)
						continue;

					FGameplayTagContainer FailureTags;

					if (!AbilitySpec.IsActive())
					{
						if (AbilitySpec.Ability->CanActivateAbility(AbilitySpec.Handle, ActorInfo, nullptr, nullptr, &FailureTags) == false)
						{


							float Cooldown = AbilitySpec.Ability->GetCooldownTimeRemaining(ActorInfo);
							if (Cooldown > 0.f)
							{
								FGameplayTag tag_cooldown = FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.Ability5"));
								TArray<FActiveGameplayEffectHandle> array = asc->GetActiveEffectsHasTag(tag_cooldown);
								for (auto Handle : array) {


									float NewDuration = Cooldown + this->offset_cd;

									asc->SetGameplayEffectDurationHandle(Handle, NewDuration);
								}
							}
						}
					}
				}
			}

			
		}

		
	}
}

bool UGDGA_CharacterNewSkill::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayTagContainer * SourceTags, const FGameplayTagContainer * TargetTags, OUT FGameplayTagContainer * OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UGDGA_CharacterNewSkill::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

// Epic's comment
/**
 *	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go wrong by calling
 *	StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability, it would need to make sure the
 *	Montage that *it* played was still playing, and if so, to cancel it. If this is something we need to support, we may need some
 *	light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
 */
void UGDGA_CharacterNewSkill::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGDGA_CharacterNewSkill::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}