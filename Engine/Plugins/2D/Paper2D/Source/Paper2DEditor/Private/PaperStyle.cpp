// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Paper2DEditorPrivatePCH.h"
#include "PaperStyle.h"
#include "SlateStyle.h"

#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaperStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

FString FPaperStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = FPaths::EnginePluginsDir() / TEXT("2D/Paper2D/Content");
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FPaperStyle::StyleSet = NULL;
TSharedPtr< class ISlateStyle > FPaperStyle::Get() { return StyleSet; }

void FPaperStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet("PaperStyle"));
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	// Tile map editor
	{
		StyleSet->Set("PaperEditor.EnterTileMapEditMode", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_TileMapEdModeIcon_40x"), Icon40x40));

		StyleSet->Set("PaperEditor.SelectPaintTool", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_PaintBrush_40x"), Icon40x40));
		StyleSet->Set("PaperEditor.SelectEraserTool", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Eraser_40x"), Icon40x40));
		StyleSet->Set("PaperEditor.SelectFillTool", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_PaintBucket_40x"), Icon40x40));
	}

	// Sprite editor
	{
		StyleSet->Set("SpriteEditor.SetShowGrid", new IMAGE_BRUSH(TEXT("Icons/icon_MatEd_Grid_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowGrid.Small", new IMAGE_BRUSH(TEXT("Icons/icon_MatEd_Grid_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowSourceTexture", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_ShowSpriteSheetButton_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowSourceTexture.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_ShowSpriteSheetButton_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowBounds", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Bounds_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowBounds.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Bounds_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowCollision", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Collision_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowCollision.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Collision_40x"), Icon20x20));

		StyleSet->Set("SpriteEditor.SetShowSockets", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowSockets_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowSockets.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowSockets_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowNormals", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Normals_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowNormals.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Normals_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowPivot", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowPivot_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowPivot.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowPivot_40x"), Icon20x20));

		StyleSet->Set("SpriteEditor.ToggleAddPolygonMode", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Paper2D_AddPolygon_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.ToggleAddPolygonMode.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Paper2D_AddPolygon_40x"), Icon20x20));

		StyleSet->Set("SpriteEditor.SnapAllVertices", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Paper2D_SnapToPixelGrid_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SnapAllVertices.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Paper2D_SnapToPixelGrid_40x"), Icon20x20));

		StyleSet->Set("SpriteEditor.EnterViewMode", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Paper2D_ViewSprite_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.EnterViewMode.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Paper2D_ViewSprite_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.EnterCollisionEditMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_EditCollision_40x", Icon40x40));
		StyleSet->Set("SpriteEditor.EnterCollisionEditMode.Small", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_EditCollision_40x", Icon20x20));
		StyleSet->Set("SpriteEditor.EnterSourceRegionEditMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_EditSourceRegion_40x", Icon40x40));
		StyleSet->Set("SpriteEditor.EnterSourceRegionEditMode.Small", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_EditSourceRegion_40x", Icon20x20));
		StyleSet->Set("SpriteEditor.EnterRenderingEditMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_RenderGeom_40x", Icon40x40));
		StyleSet->Set("SpriteEditor.EnterRenderingEditMode.Small", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_RenderGeom_40x", Icon20x20));
		StyleSet->Set("SpriteEditor.EnterAddSpriteMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_AddSprite_40x", Icon40x40));
		StyleSet->Set("SpriteEditor.EnterAddSpriteMode.Small", new IMAGE_PLUGIN_BRUSH("Icons/icon_Paper2D_AddSprite_40x", Icon20x20));
	}

	// Flipbook editor
	{
		StyleSet->Set("FlipbookEditor.SetShowGrid", new IMAGE_BRUSH(TEXT("Icons/icon_MatEd_Grid_40x"), Icon40x40));
		StyleSet->Set("FlipbookEditor.SetShowGrid.Small", new IMAGE_BRUSH(TEXT("Icons/icon_MatEd_Grid_40x"), Icon20x20));
		StyleSet->Set("FlipbookEditor.SetShowBounds", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Bounds_40x"), Icon40x40));
		StyleSet->Set("FlipbookEditor.SetShowBounds.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Bounds_40x"), Icon20x20));
		StyleSet->Set("FlipbookEditor.SetShowCollision", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Collision_40x"), Icon40x40));
		StyleSet->Set("FlipbookEditor.SetShowCollision.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Collision_40x"), Icon20x20));
		StyleSet->Set("FlipbookEditor.SetShowPivot", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowPivot_40x"), Icon40x40));
		StyleSet->Set("FlipbookEditor.SetShowPivot.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowPivot_40x"), Icon20x20));

		StyleSet->Set("FlipbookEditor.RegionGrabHandle", new BOX_BRUSH("Sequencer/ScrubHandleWhole", FMargin(6.f / 13.f, 10 / 24.f, 6 / 13.f, 10 / 24.f)));
		StyleSet->Set("FlipbookEditor.RegionBody", new BOX_BRUSH("Common/Scrollbar_Thumb", FMargin(4.f / 16.f)));
		StyleSet->Set("FlipbookEditor.RegionBorder", new BOX_BRUSH("Common/CurrentCellBorder", FMargin(4.f / 16.f), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)));
	}

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH

void FPaperStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}