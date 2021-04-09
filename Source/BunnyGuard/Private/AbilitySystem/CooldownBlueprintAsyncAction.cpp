// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "AbilitySystem/CooldownBlueprintAsyncAction.h"

UCooldownBlueprintAsyncAction* UCooldownBlueprintAsyncAction::ListenForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer InCooldownTags, bool UseServerCooldown)
{
	UCooldownBlueprintAsyncAction* ListenForCooldownChange = NewObject<UCooldownBlueprintAsyncAction>();
	ListenForCooldownChange->ASC = AbilitySystemComponent;
	ListenForCooldownChange->CooldownTags = InCooldownTags;
	ListenForCooldownChange->UseServerCooldown = UseServerCooldown;

	if (!IsValid(AbilitySystemComponent) || InCooldownTags.Num() < 1)
	{
		ListenForCooldownChange->EndTask();
		return nullptr;
	}

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenForCooldownChange, &UCooldownBlueprintAsyncAction::OnActiveGameplayEffectAddedCallback);

	TArray<FGameplayTag> CooldownTagArray;
	InCooldownTags.GetGameplayTagArray(CooldownTagArray);

	for (FGameplayTag CooldownTag : CooldownTagArray)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenForCooldownChange, &UCooldownBlueprintAsyncAction::CooldownTagChanged);
	}

	return ListenForCooldownChange;
}

void UCooldownBlueprintAsyncAction::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);

		TArray<FGameplayTag> CooldownTagArray;
		CooldownTags.GetGameplayTagArray(CooldownTagArray);

		for (FGameplayTag CooldownTag : CooldownTagArray)
		{
			ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UCooldownBlueprintAsyncAction::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	TArray<FGameplayTag> CooldownTagArray;
	CooldownTags.GetGameplayTagArray(CooldownTagArray);

	for (FGameplayTag CooldownTag : CooldownTagArray)
	{
		if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
		{
			float TimeRemaining = 0.0f;
			float Duration = 0.0f;

			FGameplayTagContainer CooldownTagContainer(GrantedTags.GetByIndex(0));
			GetCooldownRemainingForTag(CooldownTagContainer, TimeRemaining, Duration);

			if (ASC->GetOwnerRole() == ROLE_Authority)
			{
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (!UseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			{
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (UseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr)
			{
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (UseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			{
				OnCooldownBegin.Broadcast(CooldownTag, -1.0f, -1.0f);
			}
		}
	}
}

void UCooldownBlueprintAsyncAction::CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		OnCooldownEnd.Broadcast(CooldownTag, -1.0f, -1.0f);
	}
}

bool UCooldownBlueprintAsyncAction::GetCooldownRemainingForTag(FGameplayTagContainer InCooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (IsValid(ASC) && InCooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = ASC->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}
