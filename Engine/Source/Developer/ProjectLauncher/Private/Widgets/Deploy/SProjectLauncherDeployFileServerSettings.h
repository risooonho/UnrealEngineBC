// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once


/**
 * Implements the deploy-to-device settings panel.
 */
class SProjectLauncherDeployFileServerSettings
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SProjectLauncherDeployFileServerSettings) { }
	SLATE_END_ARGS()

public:

	/**
	 * Constructs the widget.
	 *
	 * @param InArgs The Slate argument list.
	 * @param InModel The data model.
	 */
	void Construct(	const FArguments& InArgs, const FProjectLauncherModelRef& InModel );


private:

	// Callback for check state changes of the 'Hide Window' check box.
	void HandleHideWindowCheckBoxCheckStateChanged( ESlateCheckBoxState::Type NewState );

	// Callback for determining the checked state of the 'Hide Window' check box.
	ESlateCheckBoxState::Type HandleHideWindowCheckBoxIsChecked( ) const;

	// Callback for check state changes of the 'Streaming Server' check box.
	void HandleStreamingServerCheckBoxCheckStateChanged( ESlateCheckBoxState::Type NewState );

	// Callback for determining the checked state of the 'Streaming Server' check box.
	ESlateCheckBoxState::Type HandleStreamingServerCheckBoxIsChecked( ) const;

private:

	// Holds a pointer to the data model.
	FProjectLauncherModelPtr Model;
};
