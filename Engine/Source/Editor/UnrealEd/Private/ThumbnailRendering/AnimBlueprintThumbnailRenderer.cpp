// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "UnrealEd.h"

// FPreviewScene derived helpers for rendering
#include "ThumbnailHelpers.h"
#include "EngineModule.h"
#include "RendererInterface.h"

UAnimBlueprintThumbnailRenderer::UAnimBlueprintThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ThumbnailScene = nullptr;
}

void UAnimBlueprintThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas)
{
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(Object);
	if (AnimBlueprint != nullptr)
	{
		if ( ThumbnailScene == nullptr )
		{
			ThumbnailScene = new FAnimBlueprintThumbnailScene();
		}

		if(ThumbnailScene->SetAnimBlueprint(AnimBlueprint))
		{
			FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(RenderTarget, ThumbnailScene->GetScene(), FEngineShowFlags(ESFIM_Game))
				.SetWorldTimes(FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime));

			ViewFamily.EngineShowFlags.DisableAdvancedFeatures();
			ViewFamily.EngineShowFlags.MotionBlur = 0;
			ViewFamily.EngineShowFlags.LOD = 0;

			ThumbnailScene->GetView(&ViewFamily, X, Y, Width, Height);
			GetRendererModule().BeginRenderingViewFamily(Canvas, &ViewFamily);
			ThumbnailScene->SetAnimBlueprint(nullptr);
		}
	}
}

void UAnimBlueprintThumbnailRenderer::BeginDestroy()
{
	if ( ThumbnailScene != nullptr )
	{
		delete ThumbnailScene;
		ThumbnailScene = nullptr;
	}

	Super::BeginDestroy();
}
