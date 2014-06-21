// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleInterface.h"
#include "ISequencer.h"

#include "Toolkits/AssetEditorToolkit.h"
#include "Toolkits/IToolkit.h"

class ISequencerObjectBindingManager;

/**
 * A delegate which will create an auto-key handler
 */
DECLARE_DELEGATE_RetVal_OneParam( TSharedRef<class FMovieSceneTrackEditor>, FOnCreateTrackEditor, TSharedRef<ISequencer> );


/**
 * The public interface of SequencerModule
 */
class ISequencerModule : public IModuleInterface
{

public:

	/**
	 * Creates a new instance of a standalone sequencer that can be added to other UIs
	 *
	 * @param 	InRootMovieScene	The movie scene to edit
	 * @return	Interface to the new editor
	 */
	virtual TSharedPtr<ISequencer> CreateSequencer( UMovieScene* InRootMovieScene, TSharedRef<ISequencerObjectBindingManager> ObjectBindingManager ) = 0;

	/**
	 * Creates a new instance of a Sequencer, the editor for MovieScene assets in an asset editor
	 *
	 * @param	Mode					Mode that this editor should operate in
	 * @param	InitToolkitHost			When Mode is WorldCentric, this is the level editor instance to spawn this editor within
	 * @param	ObjectToEdit			The object to start editing
	 *
	 * @return	Interface to the new editor
	 */
	virtual TSharedPtr<ISequencer> CreateSequencerAssetEditor( const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UMovieScene* InRootMovieScene, bool bEditWithinLevelEditor ) = 0;

	/** 
	 * Registers a delegate that will create an editor for a track in each sequencer 
	 *
	 * @param InOnCreateTrackEditor	Delegate to register
	 */
	virtual void RegisterTrackEditor( FOnCreateTrackEditor InOnCreateTrackEditor ) = 0;

	/** 
	 * Unregisters a previously registered delegate for creating a track editor
	 *
	 * @param InOnCreateTrackEditor	Delegate to unregister
	 */
	virtual void UnRegisterTrackEditor( FOnCreateTrackEditor InOnCreateTrackEditor ) = 0;
};

