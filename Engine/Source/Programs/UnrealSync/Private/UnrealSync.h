// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "CoreUObject.h"
#include "P4DataCache.h"

/**
 * Main program function.
 */
void RunUnrealSync(const TCHAR* CommandLine);

/**
 * Class to store date needed for sync monitoring thread.
 */
class FSyncingThread;

/**
* Interface of sync command line provider widget.
*/
class ILabelNameProvider
{
public:
	/**
	 * Gets label name from current options picked by user.
	 *
	 * @returns Label name.
	 */
	virtual FString GetLabelName() const = 0;

	/**
	 * Gets message to display when sync task has started.
	 */
	virtual FString GetStartedMessage() const
	{
		return "Sync started.";
	}

	/**
	 * Gets message to display when sync task has finished.
	 */
	virtual FString GetFinishedMessage() const
	{
		return "Sync finished.";
	}

	/**
	* Gets game name from current options picked by user.
	*
	* @returns Game name.
	*/
	virtual FString GetGameName() const
	{
		return CurrentGameName;
	}

	/**
	 * Reset data.
	 *
	 * @param GameName Current game name.
	 */
	virtual void ResetData(const FString& GameName)
	{
		CurrentGameName = GameName;
	}

	/**
	 * Refresh data.
	 *
	 * @param GameName Current game name.
	 */
	virtual void RefreshData(const FString& GameName)
	{
		CurrentGameName = GameName;
	}

	/**
	 * Tells if this particular widget has data ready for sync.
	 *
	 * @returns True if ready. False otherwise.
	 */
	virtual bool IsReadyForSync() const = 0;

private:
	/* Current game name. */
	FString CurrentGameName;
};

/**
 * Helper class with functions used to sync engine.
 */
class FUnrealSync
{
public:
	/* On data loaded event delegate. */
	DECLARE_DELEGATE(FOnDataLoaded);

	/* On data reset event delegate. */
	DECLARE_DELEGATE(FOnDataReset);

	/* On sync finished event delegate. */
	DECLARE_DELEGATE_OneParam(FOnSyncFinished, bool);
	/* On sync log chunk read event delegate. */
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnSyncProgress, const FString&);

	/**
	 * Gets latest label for given game name.
	 *
	 * @param GameName Current game name.
	 *
	 * @returns Latest label for current game.
	 */
	static FString GetLatestLabelForGame(const FString& GameName);

	/**
	 * Gets promoted labels for given game.
	 *
	 * @param GameName Current game name.
	 *
	 * @returns Array of promoted labels for given game.
	 */
	static TSharedPtr<TArray<FString> > GetPromotedLabelsForGame(const FString& GameName);

	/**
	 * Gets promotable labels for given game since last promoted.
	 *
	 * @param GameName Current game name.
	 *
	 * @returns Array of promotable labels for given game since last promoted.
	 */
	static TSharedPtr<TArray<FString> > GetPromotableLabelsForGame(const FString& GameName);

	/**
	 * Gets all labels.
	 *
	 * @returns Array of all labels names.
	 */
	static TSharedPtr<TArray<FString> > GetAllLabels();

	/**
	 * Gets possible game names.
	 *
	 * @returns Array of possible game names.
	 */
	static TSharedPtr<TArray<FString> > GetPossibleGameNames();

	/**
	 * Gets shared promotable display name.
	 *
	 * @returns Shared promotable display name.
	 */
	static const FString& GetSharedPromotableDisplayName();

	/**
	 * Registers event that will be trigger when data is loaded.
	 *
	 * @param OnDataLoaded Delegate to call when event happens.
	 */
	static void RegisterOnDataLoaded(const FOnDataLoaded& OnDataLoaded);

	/**
	 * Registers event that will be trigger when data is reset.
	 *
	 * @param OnDataReset Delegate to call when event happens.
	 */
	static void RegisterOnDataReset(const FOnDataReset& OnDataReset);

	/**
	 * Start async loading of the P4 label data in case user wants it.
	 */
	static void StartLoadingData();

	/**
	 * Tells that labels names are currently being loaded.
	 *
	 * @returns True if labels names are currently being loaded. False otherwise.
	 */
	static bool IsLoadingInProgress()
	{
		return LoaderThread.IsValid() && LoaderThread->IsInProgress();
	}

	/**
	 * Terminates background P4 data loading process.
	 */
	static void TerminateLoadingProcess();

	/**
	 * Method to receive p4 data loading finished event.
	 *
	 * @param Data Loaded data.
	 */
	static void OnP4DataLoadingFinished(TSharedPtr<FP4DataCache> Data);

	/**
	 * Tells if has valid P4 data loaded.
	 *
	 * @returns If P4 data was loaded.
	 */
	static bool HasValidData();

	/**
	 * Tells if loading has finished.
	 *
	 * @returns True if loading has finished. False otherwise.
	 */
	static bool LoadingFinished();

	/**
	 * Gets labels from the loaded cache.
	 *
	 * @returns Array of loaded labels.
	 */
	static const TArray<FP4Label>& GetLabels() { return Data->GetLabels(); }

	/**
	 * Launches UAT UnrealSync command with given command line and options.
	 *
	 * @param bArtist Perform artist sync?
	 * @param bPreview Perform preview sync?
	 * @param LabelNameProvider Object that will provide label name to syncing thread.
	 * @param OnSyncFinished Delegate to run when syncing is finished.
	 * @param OnSyncProgress Delegate to run when syncing has made progress.
	 */
	static void LaunchSync(bool bArtist, bool bPreview, ILabelNameProvider& LabelNameProvider, const FOnSyncFinished& OnSyncFinished, const FOnSyncProgress& OnSyncProgress);

	/**
	 * Performs the actual sync with given params.
	 *
	 * @param bArtist Perform artist sync?
	 * @param bPreview Perform preview sync?
	 * @param Label Chosen label name.
	 * @param Game Chosen game.
	 * @param OnSyncProgress Delegate to run when syncing has made progress. 
	 */
	static bool Sync(bool bArtist, bool bPreview, const FString& Label, const FString& Game, const FOnSyncProgress& OnSyncProgress);
private:
	/* Tells if loading has finished. */
	static bool bLoadingFinished;

	/* Data loaded event. */
	static FOnDataLoaded OnDataLoaded;

	/* Data reset event. */
	static FOnDataReset OnDataReset;

	/* Cached data ptr. */
	static TSharedPtr<FP4DataCache> Data;

	/* Background loading process monitoring thread. */
	static TSharedPtr<FP4DataLoader> LoaderThread;

	/* Background syncing process monitoring thread. */
	static TSharedPtr<FSyncingThread> SyncingThread;
};
