// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	DestructibleActor.cpp: ADestructibleActor methods.
=============================================================================*/


#include "EnginePrivate.h"
#include "PhysicsEngine/PhysXSupport.h"
#include "PhysicsEngine/DestructibleActor.h"

ADestructibleActor::ADestructibleActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DestructibleComponent = ObjectInitializer.CreateDefaultSubobject<UDestructibleComponent>(this, TEXT("DestructibleComponent0"));
	DestructibleComponent->bCanEverAffectNavigation = bAffectNavigation;
	RootComponent = DestructibleComponent;
}

#if WITH_EDITOR
bool ADestructibleActor::GetReferencedContentObjects( TArray<UObject*>& Objects ) const
{
	if (DestructibleComponent && DestructibleComponent->SkeletalMesh)
	{
		Objects.Add(DestructibleComponent->SkeletalMesh);
	}
	return true;
}
#endif // WITH_EDITOR



/** Returns DestructibleComponent subobject **/
UDestructibleComponent* ADestructibleActor::GetDestructibleComponent() const { return DestructibleComponent; }
