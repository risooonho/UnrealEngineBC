// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilitySystemPrivatePCH.h"
#include "GameplayCueNotify_HitImpact.h"

UGameplayCueNotify_HitImpact::UGameplayCueNotify_HitImpact(const class FObjectInitializer& PCIP)
: Super(PCIP)
{

}

bool UGameplayCueNotify_HitImpact::HandlesEvent(EGameplayCueEvent::Type EventType) const
{
	return (EventType == EGameplayCueEvent::Executed);
}

bool UGameplayCueNotify_HitImpact::NeedsInstanceForEvent(EGameplayCueEvent::Type EventType) const
{
	return false;
}

void UGameplayCueNotify_HitImpact::HandleGameplayCue(AActor* Self, EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters)
{
	check(EventType == EGameplayCueEvent::Executed);
	check(Self);
	
	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
	if (HitResult)
	{
		if (ParticleSystem)
		{
			UGameplayStatics::SpawnEmitterAtLocation(Self, ParticleSystem, HitResult->ImpactPoint, HitResult->ImpactNormal.Rotation(), true);
		}


	}
	else
	{
		ABILITY_LOG(Warning, TEXT("GameplayCue %s was called on GameplayCueNotify but there was no HitResult to spawn an impact from."), *Parameters.MatchedTagName.ToString(), *GetName() );
	}
}
