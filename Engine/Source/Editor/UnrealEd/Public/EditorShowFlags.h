// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#ifndef _EditorShowFlags_h_
#define _EditorShowFlags_h_

/**
 * @todo Slate: Copied from wxLevelViewportToolBar.  We need both systems to work for now.                   
 */
class FShowFlagData
{
public:

	/** Name for identifying the show flag */
	FName			ShowFlagName; 
	/** The display name of this show flag */
	FText			DisplayName;
	/** The engine show flag index in FEngineShowFlags */
	uint32			EngineShowFlagIndex;
	/** Which group the flags should show up */
	EShowFlagGroup	Group;

	/** Gesture for activating the show flag */
	FInputGesture InputGesture;

	/**
	 *  FShowFlagData constructor with no gesture associated
	 *
	 * @param InName				The non-localized flag name string
	 * @param InDisplayName			The localized display name for the flag
	 * @param InEngineShowFlagIndex	Index on this flag in the array of flags in FEngineShowFlags
	 * @param InGroup				The group that the flag belongs to (default: SFG_Normal)
	 */
	FShowFlagData(const FString& InName, const FText& InDisplayName, const uint32 InEngineShowFlagIndex, EShowFlagGroup InGroup = SFG_Normal);

	/**
	 *  FShowFlagData constructor with no gesture associated
	 *
	 * @param InName				The non-localized flag name string
	 * @param InDisplayName			The localized display name for the flag
	 * @param InEngineShowFlagIndex	Index on this flag in the array of flags in FEngineShowFlags
	 * @param InGroup				The group that the flag belongs to (default: SFG_Normal)
	 * @param InInputGesture		InputGesture to be used for the flag
	 */
	FShowFlagData(const FString& InName, const FText& InDisplayName, const uint32 InEngineShowFlagIndex, EShowFlagGroup InGroup, const FInputGesture& InInputGesture);

	bool IsEnabled(const FLevelEditorViewportClient* ViewportClient) const;

	void ToggleState(FLevelEditorViewportClient* ViewportClient) const;
};

UNREALED_API TArray<FShowFlagData>& GetShowFlagMenuItems();

#endif
