// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EditorViewportClient.h"

//////////////////////////////////////////////////////////////////////////
// ELocalAxesMode

namespace ELocalAxesMode
{
	enum Type
	{
		None,
		Selected,
		All,
		NumAxesModes
	};
};

//////////////////////////////////////////////////////////////////////////
// ELocalAxesMode

namespace EDisplayInfoMode
{
	enum Type
	{
		None,
		Basic,	
		Detailed,
		NumInfoModes
	};
};

/////////////////////////////////////////////////////////////////////////
// FAnimationViewportClient

class FAnimationViewportClient : public FEditorViewportClient
{
protected:

	/** Skeletal Mesh Component used for preview */
	TWeakObjectPtr<UDebugSkelMeshComponent> PreviewSkelMeshComp;

	/** Function to display bone names*/
	void ShowBoneNames( FCanvas* Canvas, FSceneView* View );

	/** Function to display warning and info text on the viewport */
	void DisplayInfo( FCanvas* Canvas, FSceneView* View, bool bDisplayAllInfo );

public:
	FAnimationViewportClient( FPreviewScene& InPreviewScene, TWeakPtr<FPersona> InPersonaPtr );
	virtual ~FAnimationViewportClient();

	// FEditorViewportClient interface
	virtual FLinearColor GetBackgroundColor() const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Draw(const FSceneView* View,FPrimitiveDrawInterface* PDI) override;
	virtual void DrawCanvas( FViewport& InViewport, FSceneView& View, FCanvas& Canvas ) override;
	virtual FSceneView* CalcSceneView(FSceneViewFamily* ViewFamily) override;
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad=false) override;
	virtual void ProcessClick(class FSceneView& View, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual bool InputWidgetDelta( FViewport* Viewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale ) override;
	virtual void TrackingStarted( const struct FInputEventState& InInputState, bool bIsDragging, bool bNudge ) override;
	virtual void TrackingStopped() override;
	virtual FWidget::EWidgetMode GetWidgetMode() const override;
	virtual void SetWidgetMode(FWidget::EWidgetMode InWidgetMode) override;
	virtual bool CanSetWidgetMode(FWidget::EWidgetMode NewMode) const override;
	virtual FVector GetWidgetLocation() const override;
	virtual FMatrix GetWidgetCoordSystem() const override;
	virtual ECoordSystem GetWidgetCoordSystemSpace() const override;
	virtual void SetWidgetCoordSystemSpace(ECoordSystem NewCoordSystem) override;
	virtual void SetViewMode(EViewModeIndex InViewModeIndex) override;
	virtual void SetViewportType(ELevelViewportType InViewportType) override;
	// End of FEditorViewportClient interface

	/** Draw call to render UV overlay */
	void DrawUVsForMesh(FViewport* InViewport, FCanvas* InCanvas, int32 InTextYPos);

	/** Callback for toggling the camera lock flag. */
	virtual void SetCameraFollow();

	/** Callback for checking the camera lock flag. */
	bool IsSetCameraFollowChecked() const;

	/** Function to set the mesh component used for preview */
	void SetPreviewMeshComponent(UDebugSkelMeshComponent* InPreviewSkelMeshComp);

	/** Function to display bone names*/
	void ShowBoneNames(FViewport* Viewport, FCanvas* Canvas);

	/** Function to show/hide grid in the viewport */
	void OnToggleShowGrid();

	/** Function to check whether grid is displayed or not */
	bool IsShowingGrid() const;

	/** Function to show/hide floor in the viewport */
	void OnToggleShowFloor();

	/** Function to check whether grid is displayed or not */
	bool IsShowingFloor() const;

	/** Function to show/hide Sky in the viewport */
	void OnToggleShowSky();

	/** Function to check whether grid is displayed or not */
	bool IsShowingSky() const;

	/** Function to mute/unmute audio in the viewport */
	void OnToggleMuteAudio();

	/** Function to check whether audio is muted or not */
	bool IsAudioMuted() const;

	/** Function to set background color */
	void SetBackgroundColor(FLinearColor InColor);

	/** Function to get current brightness value */ 
	float GetBrightnessValue() const;

	/** Function to set brightness value */
	void SetBrightnessValue(float Value);

	/** Function to set Local axes mode for the ELocalAxesType */
	void SetLocalAxesMode(ELocalAxesMode::Type AxesMode);

	/** Local axes mode checking function for the ELocalAxesType */
	bool IsLocalAxesModeSet(ELocalAxesMode::Type AxesMode) const;

	/** Get the Bone local axis mode */
	ELocalAxesMode::Type GetLocalAxesMode() const {return LocalAxesMode;}

	/** Returns the desired target of the camera */
	FSphere GetCameraTarget();

	/** Sets up the viewports camera (look-at etc) based on the current preview target*/
	void UpdateCameraSetup();

	/* Places the viewport camera at a good location to view the supplied sphere */
	void FocusViewportOnSphere( FSphere& Sphere );

	/* Places the viewport camera at a good location to view the preview target */
	void FocusViewportOnPreviewMesh();

	/* Sets the playback scale for this viewport. 1.0 == Normal, 0.5 == Half speed etc */
	void SetPlaybackScale(float PlaybackScale) {AnimationPlaybackScale = PlaybackScale;}

	/** Callback for toggling the normals show flag. */
	void ToggleShowNormals();

	/** Callback for checking the normals show flag. */
	bool IsSetShowNormalsChecked() const;

	/** Callback for toggling the tangents show flag. */
	void ToggleShowTangents();

	/** Callback for checking the tangents show flag. */
	bool IsSetShowTangentsChecked() const;

	/** Callback for toggling the binormals show flag. */
	void ToggleShowBinormals();

	/** Callback for checking the binormals show flag. */
	bool IsSetShowBinormalsChecked() const;

	/** Callback for toggling UV drawing in the viewport */
	void ToggleDrawUVOverlay();

	/** Callback for checking whether the UV drawing is switched on. */
	bool IsSetDrawUVOverlayChecked() const;

	/** Returns the UV Channel that will be drawn when Draw UV Overlay is turned on */
	int32 GetUVChannelToDraw() const { return UVChannelToDraw; }

	/** Sets the UV Channel that will be drawn when Draw UV Overlay is turned on */
	void SetUVChannelToDraw(int32 UVChannel) { UVChannelToDraw = UVChannel; }

	void ClearSelectedWindActor()
	{
		SelectedWindActor = NULL;
	}

	/* Returns the floor height offset */	
	float GetFloorOffset() const;

	/* Sets the floor height offset, saves it to config and invalidates the viewport so it shows up immediately */
	void SetFloorOffset(float NewValue);

	/* create AWindDirectionalSource actor to apply wind to clothes for preview */
	TWeakObjectPtr<AWindDirectionalSource> CreateWindActor(UWorld* World);
	/** if set to true, shows the wind actor to control wind direction 
	*   and enables the wind strength slider
	*/
	void EnableWindActor(bool bEnableWind);
	/** Function to set wind strength (0.0 - 1.0) */
	void SetWindStrength(float SliderPos);
	/** Function to get slide value used to represent wind strength */
	float GetWindStrengthSliderValue() const;

	/** Function to get wind strength label */
	FString GetWindStrengthLabel() const;
	/** Function to set gravity scale (0.0 - 4.0) */
	void SetGravityScale(float SliderPos);
	/** Function to get slide value used to represent gravity scale */
	float GetGravityScaleSliderValue() const;
	/** Function to get gravity scale label */
	FString GetGravityScaleLabel() const;
	/** Function to set mesh stat drawing state */
	void OnSetShowMeshStats(int32 ShowMode);
	/** Whether or not mesh stats are being displayed */
	bool IsShowingMeshStats() const;
	/** Whether detailed mesh stats are being displayed or basic mesh stats */
	bool IsDetailedMeshStats() const;

	int32 GetShowMeshStats() const;

public:

	/** persona config options **/
	UPersonaOptions* ConfigOption;

private:
	/** Weak pointer back to the FPersona that owns us */
	TWeakPtr<FPersona> PersonaPtr;

	// Current widget mode
	FWidget::EWidgetMode WidgetMode;

	/** True when the user is manipulating a bone widget. */
	bool bManipulating;

	/** add follow option @todo change to enum later - we share editorviewportclient, which is only problem*/
	bool bCameraFollow;

	/** True when we're in an editor transaction (moving/rotating sockets) */
	bool bInTransaction;

	/** Control where we display local axes for bones/sockets */
	ELocalAxesMode::Type LocalAxesMode;

	/** Scale value so we can slow down or speed up playback in the viewport */
	float AnimationPlaybackScale;

	/** User selected color using color picker */
	FLinearColor SelectedHSVColor;

	/** User specified gravity scale value (0.0 - 1.0) */
	float GravityScaleSliderValue;

	/** Previous information of a wind actor */
	FVector PrevWindLocation;
	FRotator PrevWindRotation;
	float PrevWindStrength;
	TWeakObjectPtr<AWindDirectionalSource> WindSourceActor;

	/** Flag for displaying the UV data in the viewport */
	bool bDrawUVs;

	/** Which UV channel to draw */
	int32 UVChannelToDraw;

	enum GridParam
	{
		MinCellCount = 64,
		MinGridSize = 2,
		MaxGridSize	= 50,
	};

	/** Editor accessory components **/
	UStaticMeshComponent* EditorFloorComp;
	UStaticMeshComponent* EditorSkyComp;
	UExponentialHeightFogComponent* EditorHeightFogComponent;

	/** Wind actor used for preview */
	TWeakObjectPtr<AWindDirectionalSource> SelectedWindActor;

	/** Focus on the preview component the next time we draw the viewport */
	bool bFocusOnDraw;

	/** Distance to trace for physics bodies */
	const float BodyTraceDistance;
private:
	int32 FindSelectedBone() const;
	class USkeletalMeshSocket* FindSelectedSocket() const;
	void SetSelectedBackgroundColor(const FLinearColor& RGBColor, bool bSave = true);
	void SaveGridSize(float NewGridSize, bool bSave = true);
	void SetCameraTargetLocation(const FSphere &BoundSphere, float DeltaSeconds);

	/** Draws Mesh Bones in foreground **/
	void DrawMeshBones(USkeletalMeshComponent * MeshComponent, FPrimitiveDrawInterface* PDI) const;
	/** Draws the given array of transforms as bones */
	void DrawBonesFromTransforms(TArray<FTransform>& Transforms, UDebugSkelMeshComponent * MeshComponent, FPrimitiveDrawInterface* PDI, FLinearColor BoneColour, FLinearColor RootBoneColour) const;
	/** Draws Bones for compressed animation **/
	void DrawMeshBonesCompressedAnimation(UDebugSkelMeshComponent * MeshComponent, FPrimitiveDrawInterface* PDI) const;
	/** Draw Bones for non retargeted animation. */
	void DrawMeshBonesNonRetargetedAnimation(UDebugSkelMeshComponent * MeshComponent, FPrimitiveDrawInterface* PDI) const;
	/** Draws Bones for Additive Base Pose */
	void DrawMeshBonesAdditiveBasePose(UDebugSkelMeshComponent * MeshComponent, FPrimitiveDrawInterface* PDI) const;
	/** Draws Bones for RequiredBones with WorldTransform **/
	void DrawBones(const USkeletalMeshComponent* MeshComponent, const TArray<FBoneIndexType> & RequiredBones, const TArray<FTransform> & WorldTransforms, FPrimitiveDrawInterface* PDI, const TArray<FLinearColor> BoneColours, float LineThickness=0.f) const;
	/** Draw Sub set of Bones **/
	void DrawMeshSubsetBones(const USkeletalMeshComponent* MeshComponent, const TArray<int32>& BonesOfInterest, FPrimitiveDrawInterface* PDI) const;
	/** Draws Gizmo for the Transform in foreground **/
	void RenderGizmo(const FTransform& Transform, FPrimitiveDrawInterface* PDI) const;
	/** Draws Mesh Sockets in foreground - bUseSkeletonSocketColor = true for grey (skeleton), false for red (mesh) **/
	void DrawSockets( TArray<class USkeletalMeshSocket*>& Sockets, FPrimitiveDrawInterface* PDI, bool bUseSkeletonSocketColor ) const;

	TWeakObjectPtr<AWindDirectionalSource> FindSelectedWindActor() const;
};
