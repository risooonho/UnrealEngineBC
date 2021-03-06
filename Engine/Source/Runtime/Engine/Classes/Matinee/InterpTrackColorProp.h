// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Matinee/InterpTrackVectorBase.h"
#include "InterpTrackColorProp.generated.h"

UCLASS(MinimalAPI, meta=( DisplayName = "Color Property Track" ) )
class UInterpTrackColorProp : public UInterpTrackVectorBase
{
	GENERATED_UCLASS_BODY()

	/** Name of property in Group  AActor  which this track mill modify over time. */
	UPROPERTY(Category=InterpTrackColorProp, VisibleAnywhere)
	FName PropertyName;


	// Begin UInterpTrack interface
	virtual int32 AddKeyframe(float Time, UInterpTrackInst* TrInst, EInterpCurveMode InitInterpMode) override;
	virtual bool CanAddKeyframe( UInterpTrackInst* TrackInst ) override;
	virtual void UpdateKeyframe(int32 KeyIndex, UInterpTrackInst* TrInst) override;
	virtual void PreviewUpdateTrack(float NewPosition, UInterpTrackInst* TrInst) override;
	virtual void UpdateTrack(float NewPosition, UInterpTrackInst* TrInst, bool bJump) override;
	virtual const FString	GetEdHelperClassName() const override;
	virtual const FString	GetSlateHelperClassName() const override;
	virtual class UTexture2D* GetTrackIcon() const override;
	// End UInterpTrack interface
};



