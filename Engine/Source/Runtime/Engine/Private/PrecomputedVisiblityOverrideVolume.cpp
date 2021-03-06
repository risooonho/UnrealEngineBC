// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "EnginePrivate.h"
#include "Lightmass/PrecomputedVisibilityOverrideVolume.h"

APrecomputedVisibilityOverrideVolume::APrecomputedVisibilityOverrideVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->BodyInstance.bEnableCollision_DEPRECATED = false;
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	bColored = true;
	BrushColor.R = 25;
	BrushColor.G = 120;
	BrushColor.B = 90;
	BrushColor.A = 255;

}
