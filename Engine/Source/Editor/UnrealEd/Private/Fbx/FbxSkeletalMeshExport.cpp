// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*
* Copyright 2010 Autodesk, Inc.  All Rights Reserved.
*
* Permission to use, copy, modify, and distribute this software in object
* code form for any purpose and without fee is hereby granted, provided
* that the above copyright notice appears in all copies and that both
* that copyright notice and the limited warranty and restricted rights
* notice below appear in all supporting documentation.
*
* AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS.
* AUTODESK SPECIFICALLY DISCLAIMS ANY AND ALL WARRANTIES, WHETHER EXPRESS
* OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTY
* OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE OR NON-INFRINGEMENT
* OF THIRD PARTY RIGHTS.  AUTODESK DOES NOT WARRANT THAT THE OPERATION
* OF THE PROGRAM WILL BE UNINTERRUPTED OR ERROR FREE.
*
* In no event shall Autodesk, Inc. be liable for any direct, indirect,
* incidental, special, exemplary, or consequential damages (including,
* but not limited to, procurement of substitute goods or services;
* loss of use, data, or profits; or business interruption) however caused
* and on any theory of liability, whether in contract, strict liability,
* or tort (including negligence or otherwise) arising in any way out
* of such code.
*
* This software is provided to the U.S. Government with the same rights
* and restrictions as described herein.
*/

/*=============================================================================
	Implementation of Skeletal Mesh export related functionality from FbxExporter
=============================================================================*/

#include "UnrealEd.h"

#include "FbxExporter.h"

DEFINE_LOG_CATEGORY_STATIC(LogFbxSkeletalMeshExport, Log, All);

namespace UnFbx
{

/**
 * Adds FBX skeleton nodes to the FbxScene based on the skeleton in the given USkeletalMesh, and fills
 * the given array with the nodes created
 */
FbxNode* FFbxExporter::CreateSkeleton(const USkeletalMesh& SkelMesh, TArray<FbxNode*>& BoneNodes)
{
	const FReferenceSkeleton& RefSkeleton= SkelMesh.RefSkeleton;

	if(RefSkeleton.GetNum() == 0)
	{
		return NULL;
	}

	// Create a list of the nodes we create for each bone, so that children can 
	// later look up their parent
	BoneNodes.Reserve(RefSkeleton.GetNum());

	for(int32 BoneIndex = 0; BoneIndex < RefSkeleton.GetNum(); ++BoneIndex)
	{
		const FMeshBoneInfo& CurrentBone = RefSkeleton.GetRefBoneInfo()[BoneIndex];
		const FTransform& BoneTransform = RefSkeleton.GetRefBonePose()[BoneIndex];

		FbxString BoneName = Converter.ConvertToFbxString(CurrentBone.ExportName);


		// Create the node's attributes
		FbxSkeleton* SkeletonAttribute = FbxSkeleton::Create(Scene, BoneName.Buffer());
		if(BoneIndex)
		{
			SkeletonAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			//SkeletonAttribute->Size.Set(1.0);
		}
		else
		{
			SkeletonAttribute->SetSkeletonType(FbxSkeleton::eRoot);
			//SkeletonAttribute->Size.Set(1.0);
		}
		

		// Create the node
		FbxNode* BoneNode = FbxNode::Create(Scene, BoneName.Buffer());
		BoneNode->SetNodeAttribute(SkeletonAttribute);

		// Set the bone node's local orientation
		FVector UnrealRotation = BoneTransform.GetRotation().Euler();
		FbxVector4 LocalPos = Converter.ConvertToFbxPos(BoneTransform.GetTranslation());
		FbxVector4 LocalRot = Converter.ConvertToFbxRot(UnrealRotation);

		BoneNode->LclTranslation.Set(LocalPos);
		BoneNode->LclRotation.Set(LocalRot);


		// If this is not the root bone, attach it to its parent
		if(BoneIndex)
		{
			BoneNodes[CurrentBone.ParentIndex]->AddChild(BoneNode);
		}


		// Add the node to the list of nodes, in bone order
		BoneNodes.Push(BoneNode);
	}

	return BoneNodes[0];
}


/**
 * Adds an Fbx Mesh to the FBX scene based on the data in the given FStaticLODModel
 */
FbxNode* FFbxExporter::CreateMesh(const USkeletalMesh& SkelMesh, const TCHAR* MeshName)
{
	const FSkeletalMeshResource* SkelMeshResource = SkelMesh.GetImportedResource();
	const FStaticLODModel& SourceModel = SkelMeshResource->LODModels[0];
	const int32 VertexCount = SourceModel.NumVertices;

	// Verify the integrity of the mesh.
	if (VertexCount == 0) return NULL;

	// Copy all the vertex data from the various chunks to a single buffer.
	// Makes the rest of the code in this function cleaner and easier to maintain.  
	TArray<FSoftSkinVertex> Vertices;
	SourceModel.GetVertices(Vertices);
	if (Vertices.Num() != VertexCount) return NULL;

	FbxMesh* Mesh = FbxMesh::Create(Scene, TCHAR_TO_ANSI(MeshName));

	// Create and fill in the vertex position data source.
	Mesh->InitControlPoints(VertexCount);
	FbxVector4* ControlPoints = Mesh->GetControlPoints();
	for (int32 VertIndex = 0; VertIndex < VertexCount; ++VertIndex)
	{
		FVector Position			= Vertices[VertIndex].Position;
		ControlPoints[VertIndex]	= Converter.ConvertToFbxPos(Position);
	}

	// Create Layer 0 to hold the normals
	FbxLayer* LayerZero = Mesh->GetLayer(0);
	if (LayerZero == NULL)
	{
		Mesh->CreateLayer();
		LayerZero = Mesh->GetLayer(0);
	}

	// Create and fill in the per-face-vertex normal data source.
	// We extract the Z-tangent and drop the X/Y-tangents which are also stored in the render mesh.
	FbxLayerElementNormal* LayerElementNormal= FbxLayerElementNormal::Create(Mesh, "");

	LayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
	// Set the normal values for every control point.
	LayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

	for (int32 VertIndex = 0; VertIndex < VertexCount; ++VertIndex)
	{
		FVector Normal			= (FVector)(Vertices[VertIndex].TangentZ);
		FbxVector4 FbxNormal	= Converter.ConvertToFbxPos(Normal);

		LayerElementNormal->GetDirectArray().Add(FbxNormal);
	}

	LayerZero->SetNormals(LayerElementNormal);


	// Create and fill in the per-face-vertex texture coordinate data source(s).
	// Create UV for Diffuse channel.
	const int32 TexCoordSourceCount = SourceModel.NumTexCoords;
	TCHAR UVChannelName[32];
	for (int32 TexCoordSourceIndex = 0; TexCoordSourceIndex < TexCoordSourceCount; ++TexCoordSourceIndex)
	{
		FbxLayer* Layer = Mesh->GetLayer(TexCoordSourceIndex);
		if (Layer == NULL)
		{
			Mesh->CreateLayer();
			Layer = Mesh->GetLayer(TexCoordSourceIndex);
		}

		if (TexCoordSourceIndex == 1)
		{
			FCString::Sprintf(UVChannelName, TEXT("LightMapUV"));
		}
		else
		{
			FCString::Sprintf(UVChannelName, TEXT("DiffuseUV"));
		}

		FbxLayerElementUV* UVDiffuseLayer = FbxLayerElementUV::Create(Mesh, TCHAR_TO_ANSI(UVChannelName));
		UVDiffuseLayer->SetMappingMode(FbxLayerElement::eByControlPoint);
		UVDiffuseLayer->SetReferenceMode(FbxLayerElement::eDirect);

		// Create the texture coordinate data source.
		for (int32 TexCoordIndex = 0; TexCoordIndex < VertexCount; ++TexCoordIndex)
		{
			const FVector2D& TexCoord = Vertices[TexCoordIndex].UVs[TexCoordSourceIndex];
			UVDiffuseLayer->GetDirectArray().Add(FbxVector2(TexCoord.X, -TexCoord.Y + 1.0));
		}

		Layer->SetUVs(UVDiffuseLayer, FbxLayerElement::eTextureDiffuse);
	}

	FbxLayerElementMaterial* MatLayer = FbxLayerElementMaterial::Create(Mesh, "");
	MatLayer->SetMappingMode(FbxLayerElement::eByPolygon);
	MatLayer->SetReferenceMode(FbxLayerElement::eIndexToDirect);
	LayerZero->SetMaterials(MatLayer);


	// Create the per-material polygons sets.
	TArray<uint32> Indices;
	SourceModel.MultiSizeIndexContainer.GetIndexBuffer(Indices);

	int32 SectionCount = SourceModel.Sections.Num();
	for (int32 SectionIndex = 0; SectionIndex < SectionCount; ++SectionIndex)
	{
		const FSkelMeshSection& Section = SourceModel.Sections[SectionIndex];

		int32 MatIndex = Section.MaterialIndex;

		// Static meshes contain one triangle list per element.
		int32 TriangleCount = Section.NumTriangles;

		// Copy over the index buffer into the FBX polygons set.
		for (int32 TriangleIndex = 0; TriangleIndex < TriangleCount; ++TriangleIndex)
		{
			Mesh->BeginPolygon(MatIndex);
			for (int32 PointIndex = 0; PointIndex < 3; PointIndex++)
			{
				Mesh->AddPolygon(Indices[Section.BaseIndex + ((TriangleIndex * 3) + PointIndex)]);
			}
			Mesh->EndPolygon();
		}
	}

	// Create and fill in the vertex color data source.
	FbxLayerElementVertexColor* VertexColor = FbxLayerElementVertexColor::Create(Mesh, "");
	VertexColor->SetMappingMode(FbxLayerElement::eByControlPoint);
	VertexColor->SetReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElementArrayTemplate<FbxColor>& VertexColorArray = VertexColor->GetDirectArray();
	LayerZero->SetVertexColors(VertexColor);

	for (int32 VertIndex = 0; VertIndex < VertexCount; ++VertIndex)
	{
		FLinearColor VertColor = Vertices[VertIndex].Color.ReinterpretAsLinear();
		VertexColorArray.Add( FbxColor(VertColor.R, VertColor.G, VertColor.B, VertColor.A ));
	}

	FbxNode* MeshNode = FbxNode::Create(Scene, TCHAR_TO_ANSI(MeshName));
	MeshNode->SetNodeAttribute(Mesh);



	// Add the materials for the mesh
	int32 MaterialCount = SkelMesh.Materials.Num();

	for(int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		UMaterialInterface* MatInterface = SkelMesh.Materials[MaterialIndex].MaterialInterface;

		FbxSurfaceMaterial* FbxMaterial = NULL;
		if(MatInterface && !FbxMaterials.Find(MatInterface->GetMaterial()))
		{
			FbxMaterial = ExportMaterial(MatInterface->GetMaterial());
		}
		else
		{
			// Note: The vertex data relies on there being a set number of Materials.  
			// If you try to add the same material again it will not be added, so create a 
			// default material with a unique name to ensure the proper number of materials

			TCHAR NewMaterialName[MAX_SPRINTF]=TEXT("");
			FCString::Sprintf( NewMaterialName, TEXT("Fbx Default Material %i"), MaterialIndex );

			FbxMaterial = FbxSurfaceLambert::Create(Scene, TCHAR_TO_ANSI(NewMaterialName));
			((FbxSurfaceLambert*)FbxMaterial)->Diffuse.Set(FbxDouble3(0.72, 0.72, 0.72));
		}

		MeshNode->AddMaterial(FbxMaterial);
	}

	int32 SavedMaterialCount = MeshNode->GetMaterialCount();
	check(SavedMaterialCount == MaterialCount);

	return MeshNode;
}


/**
 * Adds Fbx Clusters necessary to skin a skeletal mesh to the bones in the BoneNodes list
 */
void FFbxExporter::BindMeshToSkeleton(const USkeletalMesh& SkelMesh, FbxNode* MeshRootNode, TArray<FbxNode*>& BoneNodes)
{
	const FSkeletalMeshResource* SkelMeshResource = SkelMesh.GetImportedResource();
	const FStaticLODModel& SourceModel = SkelMeshResource->LODModels[0];
	const int32 VertexCount = SourceModel.NumVertices;

	FbxAMatrix MeshMatrix;

	FbxScene* lScene = MeshRootNode->GetScene();
	if( lScene ) 
	{
		MeshMatrix = MeshRootNode->EvaluateGlobalTransform();
	}
	
	FbxGeometry* MeshAttribute = (FbxGeometry*) MeshRootNode->GetNodeAttribute();
	FbxSkin* Skin = FbxSkin::Create(Scene, "");
	
	const int32 BoneCount = BoneNodes.Num();
	for(int32 BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
	{
		FbxNode* BoneNode = BoneNodes[BoneIndex];

		// Create the deforming cluster
		FbxCluster *CurrentCluster = FbxCluster::Create(Scene,"");
		CurrentCluster->SetLink(BoneNode);
		CurrentCluster->SetLinkMode(FbxCluster::eTotalOne);

		// Add all the vertices that are weighted to the current skeletal bone to the cluster
		// NOTE: the bone influence indices contained in the vertex data are based on a per-chunk
		// list of verts.  The convert the chunk bone index to the mesh bone index, the chunk's
		// boneMap is needed
		int32 VertIndex = 0;
		const int32 ChunkCount = SourceModel.Chunks.Num();
		for(int32 ChunkIndex = 0; ChunkIndex < ChunkCount; ++ChunkIndex)
		{
			const FSkelMeshChunk& Chunk = SourceModel.Chunks[ChunkIndex];

			for(int32 RigidIndex = 0; RigidIndex < Chunk.NumRigidVertices; ++RigidIndex)
			{
				const FRigidSkinVertex& Vert = Chunk.RigidVertices[RigidIndex];

				int32 InfluenceBone		= Chunk.BoneMap[ Vert.Bone ];
				float InfluenceWeight	= 1.f;

				if(InfluenceBone == BoneIndex && InfluenceWeight > 0.f)
				{
					CurrentCluster->AddControlPointIndex(VertIndex, InfluenceWeight);
				}

				++VertIndex;
			}

			for(int32 SoftIndex = 0; SoftIndex < Chunk.NumSoftVertices; ++SoftIndex)
			{
				const FSoftSkinVertex& Vert = Chunk.SoftVertices[SoftIndex];

				for(int32 InfluenceIndex = 0; InfluenceIndex < MAX_TOTAL_INFLUENCES; ++InfluenceIndex)
				{
					int32 InfluenceBone		= Chunk.BoneMap[ Vert.InfluenceBones[InfluenceIndex] ];
					float InfluenceWeight	= Vert.InfluenceWeights[InfluenceIndex] / 255.f;

					if(InfluenceBone == BoneIndex && InfluenceWeight > 0.f)
					{
						CurrentCluster->AddControlPointIndex(VertIndex, InfluenceWeight);
					}
				}

				++VertIndex;
			}
		}

		// Now we have the Patch and the skeleton correctly positioned,
		// set the Transform and TransformLink matrix accordingly.
		CurrentCluster->SetTransformMatrix(MeshMatrix);

		FbxAMatrix LinkMatrix;
		if( lScene )
		{
			LinkMatrix = BoneNode->EvaluateGlobalTransform();
		}

		CurrentCluster->SetTransformLinkMatrix(LinkMatrix);

		// Add the clusters to the mesh by creating a skin and adding those clusters to that skin.
		Skin->AddCluster(CurrentCluster);
	}

	// Add the skin to the mesh after the clusters have been added
	MeshAttribute->AddDeformer(Skin);
}


// Add the specified node to the node array. Also, add recursively
// all the parent node of the specified node to the array.
void AddNodeRecursively(FbxArray<FbxNode*>& pNodeArray, FbxNode* pNode)
{
	if (pNode)
	{
		AddNodeRecursively(pNodeArray, pNode->GetParent());

		if (pNodeArray.Find(pNode) == -1)
		{
			// Node not in the list, add it
			pNodeArray.Add(pNode);
		}
	}
}


/**
 * Add a bind pose to the scene based on the FbxMesh and skinning settings of the given node
 */
void FFbxExporter::CreateBindPose(FbxNode* MeshRootNode)
{
	// In the bind pose, we must store all the link's global matrix at the time of the bind.
	// Plus, we must store all the parent(s) global matrix of a link, even if they are not
	// themselves deforming any model.

	// Create a bind pose with the link list

	FbxArray<FbxNode*> lClusteredFbxNodes;
	int                       i, j;

	if (MeshRootNode && MeshRootNode->GetNodeAttribute())
	{
		int lSkinCount=0;
		int lClusterCount=0;
		switch (MeshRootNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
		case FbxNodeAttribute::eNurbs:
		case FbxNodeAttribute::ePatch:

			lSkinCount = ((FbxGeometry*)MeshRootNode->GetNodeAttribute())->GetDeformerCount(FbxDeformer::eSkin);
			//Go through all the skins and count them
			//then go through each skin and get their cluster count
			for(i=0; i<lSkinCount; ++i)
			{
				FbxSkin *lSkin=(FbxSkin*)((FbxGeometry*)MeshRootNode->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
				lClusterCount+=lSkin->GetClusterCount();
			}
			break;
		}
		//if we found some clusters we must add the node
		if (lClusterCount)
		{
			//Again, go through all the skins get each cluster link and add them
			for (i=0; i<lSkinCount; ++i)
			{
				FbxSkin *lSkin=(FbxSkin*)((FbxGeometry*)MeshRootNode->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
				lClusterCount=lSkin->GetClusterCount();
				for (j=0; j<lClusterCount; ++j)
				{
					FbxNode* lClusterNode = lSkin->GetCluster(j)->GetLink();
					AddNodeRecursively(lClusteredFbxNodes, lClusterNode);
				}

			}

			// Add the patch to the pose
			lClusteredFbxNodes.Add(MeshRootNode);
		}
	}

	// Now create a bind pose with the link list
	if (lClusteredFbxNodes.GetCount())
	{
		// A pose must be named. Arbitrarily use the name of the patch node.
		FbxPose* lPose = FbxPose::Create(Scene, MeshRootNode->GetName());

		// default pose type is rest pose, so we need to set the type as bind pose
		lPose->SetIsBindPose(true);

		for (i=0; i<lClusteredFbxNodes.GetCount(); i++)
		{
			FbxNode*  lKFbxNode   = lClusteredFbxNodes.GetAt(i);
			FbxMatrix lBindMatrix = lKFbxNode->EvaluateGlobalTransform();

			lPose->Add(lKFbxNode, lBindMatrix);
		}

		// Add the pose to the scene
		Scene->AddPose(lPose);
	}
}


/**
 * Add the given skeletal mesh to the Fbx scene in preparation for exporting.  Makes all new nodes a child of the given node
 */
void FFbxExporter::ExportSkeletalMeshToFbx(const USkeletalMesh& SkelMesh, const TCHAR* MeshName, FbxNode* ActorRootNode)
{
	TArray<FbxNode*> BoneNodes;

	// Add the skeleton to the scene
	FbxNode* SkeletonRootNode = CreateSkeleton(SkelMesh, BoneNodes);
	if(SkeletonRootNode)
	{
		ActorRootNode->AddChild(SkeletonRootNode);
	}

	// Add the mesh
	FbxNode* MeshRootNode = CreateMesh(SkelMesh, MeshName);
	if(MeshRootNode)
	{
		ActorRootNode->AddChild(MeshRootNode);
	}

	if(SkeletonRootNode && MeshRootNode)
	{
		// Bind the mesh to the skeleton
		BindMeshToSkeleton(SkelMesh, MeshRootNode, BoneNodes);

		// Add the bind pose
		CreateBindPose(MeshRootNode);
	}
}

} // namespace UnFbx
