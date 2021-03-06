// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// ReimportTextureFactory
//=============================================================================

#pragma once
#include "ScriptFactory.h"
#include "ReimportScriptFactory.generated.h"

/**
* Script Blueprint re-import factory
*/
UCLASS(MinimalApi, collapsecategories, EarlyAccessPreview)
class UReimportScriptFactory : public UScriptFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	class UScriptBlueprint* OriginalScript;

	virtual bool ConfigureProperties() override;

	// Begin FReimportHandler interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	// End FReimportHandler interface
};



