// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "DestructibleMeshEditorPrivatePCH.h"
#include "ApexDestructibleAssetImport.h"
#include "Engine/DestructibleMesh.h"

UDestructibleChunkParamsProxy::UDestructibleChunkParamsProxy(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UDestructibleChunkParamsProxy::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent )
{
	check(DestructibleMesh != NULL && DestructibleMesh->FractureSettings != NULL);

	if (DestructibleMesh->FractureSettings->ChunkParameters.Num() > ChunkIndex)
	{
		DestructibleMesh->FractureSettings->ChunkParameters[ChunkIndex] = ChunkParams;
	}

	BuildDestructibleMeshFromFractureSettings(*DestructibleMesh, NULL);
	DestructibleMeshEditorPtr.Pin()->RefreshViewport();
}
