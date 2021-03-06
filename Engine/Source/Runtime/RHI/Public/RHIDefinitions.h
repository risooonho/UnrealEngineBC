// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	RHIDefinitions.h: Render Hardware Interface definitions
		(that don't require linking).
=============================================================================*/

#ifndef __RHIDEFINITIONS_H__
#define __RHIDEFINITIONS_H__

#include "Core.h"

enum EShaderFrequency
{
	SF_Vertex			= 0,
	SF_Hull				= 1,
	SF_Domain			= 2,
	SF_Pixel			= 3,
	SF_Geometry			= 4,
	SF_Compute			= 5,

	SF_NumFrequencies	= 6,
	SF_NumBits			= 3,
};
static_assert(SF_NumFrequencies <= (1 << SF_NumBits), "SF_NumFrequencies will not fit on SF_NumBits");

/** @warning: update *LegacyShaderPlatform* when the below changes */
enum EShaderPlatform
{
	SP_PCD3D_SM5		= 0,
	SP_OPENGL_SM4		= 1,
	SP_PS4				= 2,
	/** Used when running in Feature Level ES2 in OpenGL. */
	SP_OPENGL_PCES2		= 3,
	SP_XBOXONE			= 4,
	SP_PCD3D_SM4		= 5,
	SP_OPENGL_SM5		= 6,
	/** Used when running in Feature Level ES2 in D3D11. */
	SP_PCD3D_ES2		= 7,
	SP_OPENGL_ES2		= 8,
	SP_OPENGL_ES2_WEBGL = 9, 
	SP_OPENGL_ES2_IOS	= 10,
	SP_METAL			= 11,
	SP_OPENGL_SM4_MAC	= 12,
	SP_METAL_MRT		= 13,
	SP_OPENGL_ES31_EXT	= 14,

	SP_NumPlatforms		= 15,
	SP_NumBits			= 5,
};
static_assert(SP_NumPlatforms <= (1 << SP_NumBits), "SP_NumPlatforms will not fit on SP_NumBits");

enum ERenderQueryType
{
	// e.g. WaitForFrameEventCompletion()
	RQT_Undefined,
	// Result is the number of samples that are not culled (divide by MSAACount to get pixels)
	RQT_Occlusion,
	// Result is time in micro seconds = 1/1000 ms = 1/1000000 sec
	RQT_AbsoluteTime,
};

/** Maximum number of miplevels in a texture. */
enum { MAX_TEXTURE_MIP_COUNT = 14 };

/** The maximum number of vertex elements which can be used by a vertex declaration. */
enum { MaxVertexElementCount = 16 };

/** The alignment in bytes between elements of array shader parameters. */
enum { ShaderArrayElementAlignBytes = 16 };

/** The number of render-targets that may be simultaneously written to. */
enum { MaxSimultaneousRenderTargets = 8 };

/** The number of UAVs that may be simultaneously bound to a shader. */
enum { MaxSimultaneousUAVs = 8 };

/**
 * The RHI's feature level indicates what level of support can be relied upon.
 * Note: these are named after graphics API's like ES2 but a feature level can be used with a different API (eg ERHIFeatureLevel::ES2 on D3D11)
 * As long as the graphics API supports all the features of the feature level (eg no ERHIFeatureLevel::SM5 on OpenGL ES2)
 */
namespace ERHIFeatureLevel
{
	enum Type
	{
		/** Feature level defined by the core capabilities of OpenGL ES2. */
		ES2,
		/** Feature level defined by the core capabilities of OpenGL ES3.1 & Metal. */
		ES3_1,
		/** Feature level defined by the capabilities of DX10 Shader Model 4. */
		SM4,
		/** Feature level defined by the capabilities of DX11 Shader Model 5. */
		SM5,
		Num
	};
};

enum ESamplerFilter
{
	SF_Point,
	SF_Bilinear,
	SF_Trilinear,
	SF_AnisotropicPoint,
	SF_AnisotropicLinear,
};

enum ESamplerAddressMode
{
	AM_Wrap,
	AM_Clamp,
	AM_Mirror,
	/** Not supported on all platforms */
	AM_Border
};

enum ESamplerCompareFunction
{
	SCF_Never,
	SCF_Less
};

enum ERasterizerFillMode
{
	FM_Point,
	FM_Wireframe,
	FM_Solid
};

enum ERasterizerCullMode
{
	CM_None,
	CM_CW,
	CM_CCW
};

enum EColorWriteMask
{
	CW_RED   = 0x01,
	CW_GREEN = 0x02,
	CW_BLUE  = 0x04,
	CW_ALPHA = 0x08,

	CW_NONE  = 0,
	CW_RGB   = CW_RED | CW_GREEN | CW_BLUE,
	CW_RGBA  = CW_RED | CW_GREEN | CW_BLUE | CW_ALPHA,
	CW_RG    = CW_RED | CW_GREEN,
	CW_BA    = CW_BLUE | CW_ALPHA,
};

enum ECompareFunction
{
	CF_Less,
	CF_LessEqual,
	CF_Greater,
	CF_GreaterEqual,
	CF_Equal,
	CF_NotEqual,
	CF_Never,
	CF_Always
};

enum EStencilOp
{
	SO_Keep,
	SO_Zero,
	SO_Replace,
	SO_SaturatedIncrement,
	SO_SaturatedDecrement,
	SO_Invert,
	SO_Increment,
	SO_Decrement
};

enum EBlendOperation
{
	BO_Add,
	BO_Subtract,
	BO_Min,
	BO_Max,
	BO_ReverseSubtract,
};

enum EBlendFactor
{
	BF_Zero,
	BF_One,
	BF_SourceColor,
	BF_InverseSourceColor,
	BF_SourceAlpha,
	BF_InverseSourceAlpha,
	BF_DestAlpha,
	BF_InverseDestAlpha,
	BF_DestColor,
	BF_InverseDestColor,
	BF_ConstantBlendFactor,
	BF_InverseConstantBlendFactor
};

enum EVertexElementType
{
	VET_None,
	VET_Float1,
	VET_Float2,
	VET_Float3,
	VET_Float4,
	VET_PackedNormal,	// FPackedNormal
	VET_UByte4,
	VET_UByte4N,
	VET_Color,
	VET_Short2,
	VET_Short4,
	VET_Short2N,		// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
	VET_Half2,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
	VET_Half4,
	VET_MAX
};

enum ECubeFace
{
	CubeFace_PosX=0,
	CubeFace_NegX,
	CubeFace_PosY,
	CubeFace_NegY,
	CubeFace_PosZ,
	CubeFace_NegZ,
	CubeFace_MAX
};

enum EUniformBufferUsage
{
	// the uniform buffer is temporary, used for a single draw call then discarded
	UniformBuffer_SingleDraw = 0,
	// the uniform buffer is used for multiple draw calls but only for the current frame
	UniformBuffer_SingleFrame,
	// the uniform buffer is used for multiple draw calls, possibly across multiple frames
	UniformBuffer_MultiFrame,
};

/** The base type of a value in a uniform buffer. */
enum EUniformBufferBaseType
{
	UBMT_INVALID,
	UBMT_BOOL,
	UBMT_INT32,
	UBMT_UINT32,
	UBMT_FLOAT32,
	UBMT_STRUCT,
	UBMT_SRV,
	UBMT_UAV,
	UBMT_SAMPLER,
	UBMT_TEXTURE
};

struct FRHIResourceTableEntry
{
public:
	static CONSTEXPR uint32 GetEndOfStreamToken()
	{
		return 0xffffffff;
	}

	static uint32 Create(uint16 UniformBufferIndex, uint16 ResourceIndex, uint16 BindIndex)
	{
		return ((UniformBufferIndex & RTD_Mask_UniformBufferIndex) << RTD_Shift_UniformBufferIndex) |
			((ResourceIndex & RTD_Mask_ResourceIndex) << RTD_Shift_ResourceIndex) |
			((BindIndex & RTD_Mask_BindIndex) << RTD_Shift_BindIndex);
	}

	static inline uint16 GetUniformBufferIndex(uint32 Data)
	{
		return (Data >> RTD_Shift_UniformBufferIndex) & RTD_Mask_UniformBufferIndex;
	}

	static inline uint16 GetResourceIndex(uint32 Data)
	{
		return (Data >> RTD_Shift_ResourceIndex) & RTD_Mask_ResourceIndex;
	}

	static inline uint16 GetBindIndex(uint32 Data)
	{
		return (Data >> RTD_Shift_BindIndex) & RTD_Mask_BindIndex;
	}

private:
	enum EResourceTableDefinitions
	{
		RTD_NumBits_UniformBufferIndex	= 8,
		RTD_NumBits_ResourceIndex		= 16,
		RTD_NumBits_BindIndex			= 8,

		RTD_Mask_UniformBufferIndex		= (1 << RTD_NumBits_UniformBufferIndex) - 1,
		RTD_Mask_ResourceIndex			= (1 << RTD_NumBits_ResourceIndex) - 1,
		RTD_Mask_BindIndex				= (1 << RTD_NumBits_BindIndex) - 1,

		RTD_Shift_BindIndex				= 0,
		RTD_Shift_ResourceIndex			= RTD_Shift_BindIndex + RTD_NumBits_BindIndex,
		RTD_Shift_UniformBufferIndex	= RTD_Shift_ResourceIndex + RTD_NumBits_ResourceIndex,
	};
	static_assert(RTD_NumBits_UniformBufferIndex + RTD_NumBits_ResourceIndex + RTD_NumBits_BindIndex <= sizeof(uint32) * 8, "RTD_* values must fit in 32 bits");
};

enum EResourceLockMode
{
	RLM_ReadOnly,
	RLM_WriteOnly,
	RLM_Num
};

/** limited to 8 types in FReadSurfaceDataFlags */
enum ERangeCompressionMode
{
	// 0 .. 1
	RCM_UNorm,
	// -1 .. 1
	RCM_SNorm,
	// 0 .. 1 unless there are smaller values than 0 or bigger values than 1, then the range is extended to the minimum or the maximum of the values
	RCM_MinMaxNorm,
	// minimum .. maximum (each channel independent)
	RCM_MinMax,
};

enum EPrimitiveType
{
	PT_TriangleList,
	PT_TriangleStrip,
	PT_LineList,
	PT_QuadList,
	PT_PointList,
	PT_1_ControlPointPatchList,
	PT_2_ControlPointPatchList,
	PT_3_ControlPointPatchList,
	PT_4_ControlPointPatchList,
	PT_5_ControlPointPatchList,
	PT_6_ControlPointPatchList,
	PT_7_ControlPointPatchList,
	PT_8_ControlPointPatchList,
	PT_9_ControlPointPatchList,
	PT_10_ControlPointPatchList,
	PT_11_ControlPointPatchList,
	PT_12_ControlPointPatchList,
	PT_13_ControlPointPatchList,
	PT_14_ControlPointPatchList,
	PT_15_ControlPointPatchList,
	PT_16_ControlPointPatchList,
	PT_17_ControlPointPatchList,
	PT_18_ControlPointPatchList,
	PT_19_ControlPointPatchList,
	PT_20_ControlPointPatchList,
	PT_21_ControlPointPatchList,
	PT_22_ControlPointPatchList,
	PT_23_ControlPointPatchList,
	PT_24_ControlPointPatchList,
	PT_25_ControlPointPatchList,
	PT_26_ControlPointPatchList,
	PT_27_ControlPointPatchList,
	PT_28_ControlPointPatchList,
	PT_29_ControlPointPatchList,
	PT_30_ControlPointPatchList,
	PT_31_ControlPointPatchList,
	PT_32_ControlPointPatchList,
	PT_Num,
	PT_NumBits = 6
};

static_assert(PT_Num <= (1 << 8), "EPrimitiveType doesn't fit in a byte");
static_assert(PT_Num <= (1 << PT_NumBits), "PT_NumBits is too small");

/**
 *	Resource usage flags - for vertex and index buffers.
 */
enum EBufferUsageFlags
{
	// Mutually exclusive write-frequency flags
	BUF_Static            = 0x0001, // The buffer will be written to once.
	BUF_Dynamic           = 0x0002, // The buffer will be written to occasionally.
	BUF_Volatile          = 0x0004, // The buffer will be written to frequently.

	// Mutually exclusive bind flags.
	BUF_UnorderedAccess   = 0x0008, // Allows an unordered access view to be created for the buffer.

	/** Create a byte address buffer, which is basically a structured buffer with a uint32 type. */
	BUF_ByteAddressBuffer = 0x0020,
	/** Create a structured buffer with an atomic UAV counter. */
	BUF_UAVCounter        = 0x0040,
	/** Create a buffer that can be bound as a stream output target. */
	BUF_StreamOutput      = 0x0080,
	/** Create a buffer which contains the arguments used by DispatchIndirect or DrawIndirect. */
	BUF_DrawIndirect      = 0x0100,
	/** 
	 * Create a buffer that can be bound as a shader resource. 
	 * This is only needed for buffer types which wouldn't ordinarily be used as a shader resource, like a vertex buffer.
	 */
	BUF_ShaderResource    = 0x0200,

	/**
	 * Request that this buffer is directly CPU accessible
	 * (@todo josh: this is probably temporary and will go away in a few months)
	 */
	BUF_KeepCPUAccessible = 0x0400,

	/**
	 * Provide information that this buffer will contain only one vertex, which should be delivered to every primitive drawn.
	 * This is necessary for OpenGL implementations, which need to handle this case very differently (and can't handle GL_HALF_FLOAT in such vertices at all).
	 */
	BUF_ZeroStride        = 0x0800,

	/** Buffer should go in fast vram (hint only) */
	BUF_FastVRAM          = 0x1000,

	// Helper bit-masks
	BUF_AnyDynamic      = (BUF_Dynamic|BUF_Volatile),
};

#define ENUM_RHI_RESOURCE_TYPES(EnumerationMacro) \
	EnumerationMacro(SamplerState,None) \
	EnumerationMacro(RasterizerState,None) \
	EnumerationMacro(DepthStencilState,None) \
	EnumerationMacro(BlendState,None) \
	EnumerationMacro(VertexDeclaration,None) \
	EnumerationMacro(VertexShader,None) \
	EnumerationMacro(HullShader,None) \
	EnumerationMacro(DomainShader,None) \
	EnumerationMacro(PixelShader,None) \
	EnumerationMacro(GeometryShader,None) \
	EnumerationMacro(ComputeShader,None) \
	EnumerationMacro(BoundShaderState,None) \
	EnumerationMacro(UniformBuffer,None) \
	EnumerationMacro(IndexBuffer,None) \
	EnumerationMacro(VertexBuffer,None) \
	EnumerationMacro(StructuredBuffer,None) \
	EnumerationMacro(Texture,None) \
	EnumerationMacro(Texture2D,Texture) \
	EnumerationMacro(Texture2DArray,Texture) \
	EnumerationMacro(Texture3D,Texture) \
	EnumerationMacro(TextureCube,Texture) \
	EnumerationMacro(TextureReference,Texture) \
	EnumerationMacro(RenderQuery,None) \
	EnumerationMacro(Viewport,None) \
	EnumerationMacro(UnorderedAccessView,None) \
	EnumerationMacro(ShaderResourceView,None)

/** An enumeration of the different RHI reference types. */
enum ERHIResourceType
{
	RRT_None,

#define DECLARE_RESOURCETYPE_ENUM(Type,ParentType) RRT_##Type,
	ENUM_RHI_RESOURCE_TYPES(DECLARE_RESOURCETYPE_ENUM)
#undef DECLARE_RESOURCETYPE_ENUM

	RRT_Num
};

/** Flags used for texture creation */
enum ETextureCreateFlags
{
	TexCreate_None					= 0,

	// Texture can be used as a render target
	TexCreate_RenderTargetable		= 1<<0,
	// Texture can be used as a resolve target
	TexCreate_ResolveTargetable		= 1<<1,
	// Texture can be used as a depth-stencil target.
	TexCreate_DepthStencilTargetable= 1<<2,
	// Texture can be used as a shader resource.
	TexCreate_ShaderResource		= 1<<3,

	// Texture is encoded in sRGB gamma space
	TexCreate_SRGB					= 1<<4,
	// Texture will be created without a packed miptail
	TexCreate_NoMipTail				= 1<<5,
	// Texture will be created with an un-tiled format
	TexCreate_NoTiling				= 1<<6,
	// Texture that may be updated every frame
	TexCreate_Dynamic				= 1<<8,
	// Allow silent texture creation failure
	// @warning:	When you update this, you must update FTextureAllocations::FindTextureType() in Core/Private/UObject/TextureAllocations.cpp
	TexCreate_AllowFailure			= 1<<9,
	// Disable automatic defragmentation if the initial texture memory allocation fails.
	// @warning:	When you update this, you must update FTextureAllocations::FindTextureType() in Core/Private/UObject/TextureAllocations.cpp
	TexCreate_DisableAutoDefrag		= 1<<10,
	// Create the texture with automatic -1..1 biasing
	TexCreate_BiasNormalMap			= 1<<11,
	// Create the texture with the flag that allows mip generation later, only applicable to D3D11
	TexCreate_GenerateMipCapable	= 1<<12,
	// UnorderedAccessView (DX11 only)
	// Warning: Causes additional synchronization between draw calls when using a render target allocated with this flag, use sparingly
	// See: GCNPerformanceTweets.pdf Tip 37
	TexCreate_UAV					= 1<<16,
	// Render target texture that will be displayed on screen (back buffer)
	TexCreate_Presentable			= 1<<17,
	// Texture data is accessible by the CPU
	TexCreate_CPUReadback			= 1<<18,
	// Texture was processed offline (via a texture conversion process for the current platform)
	TexCreate_OfflineProcessed		= 1<<19,
	// Texture needs to go in fast VRAM if available (HINT only)
	TexCreate_FastVRAM				= 1<<20,
	// by default the texture is not showing up in the list - this is to reduce clutter, using the FULL option this can be ignored
	TexCreate_HideInVisualizeTexture= 1<<21,
	// Texture should be created in virtual memory, with no physical memory allocation made
	// You must make further calls to RHIVirtualTextureSetFirstMipInMemory to allocate physical memory
	// and RHIVirtualTextureSetFirstMipVisible to map the first mip visible to the GPU
	TexCreate_Virtual				= 1<<22,
	// Creates a RenderTargetView for each array slice of the texture
	// Warning: if this was specified when the resource was created, you can't use SV_RenderTargetArrayIndex to route to other slices!
	TexCreate_TargetArraySlicesIndependently	= 1<<23,
	// Texture that may be shared with DX9 or other devices
	TexCreate_Shared = 1 << 24,
};

/**
 * Async texture reallocation status, returned by RHIGetReallocateTexture2DStatus().
 */
enum ETextureReallocationStatus
{
	TexRealloc_Succeeded = 0,
	TexRealloc_Failed,
	TexRealloc_InProgress,
};

/**
 * Action to take when a rendertarget is set.
 */
enum class ERenderTargetLoadAction
{
	ENoAction,
	ELoad,
	EClear,
};

/**
 * Action to take when a rendertarget is unset or at the end of a pass. 
 */
enum class ERenderTargetStoreAction
{
	ENoAction,
	EStore,
	EMultisampleResolve,
};

/**
 * Common render target use cases
 */
enum class ESimpleRenderTargetMode
{
	// These will all store out color and depth
	EExistingColorAndDepth,			// Color = Existing, Depth = Existing
	EUninitializedColorAndDepth,	// Color = ????, Depth = ????
	EUninitializedColorExistingDepth,// Color = ????, Depth = Existing
	EUninitializedColorClearDepth,	// Color = ????, Depth = Default
	EClearToDefault,				// Default Color = (0,0,0,0), Default Depth = 0.0f
	EClearColorToBlack,				// Color = (0,0,0,0), Depth = Existing
	EClearColorToBlackWithFullAlpha,// Color = (0,0,0,1), Depth = Existing
	EClearColorToWhite,				// Color = (1,1,1,1), Depth = Existing
	EClearDepthToOne,				// Color = Existing, Depth = 1.0
	EExistingContents_NoDepthStore,	// Load existing contents, but don't store depth out

	// If you add an item here, make sure to add it to DecodeRenderTargetMode() as well!
};

inline bool IsPCPlatform(const EShaderPlatform Platform)
{
	return Platform == SP_PCD3D_SM5 || Platform == SP_PCD3D_SM4 || Platform == SP_PCD3D_ES2 || Platform ==  SP_OPENGL_SM4 || Platform == SP_OPENGL_SM4_MAC || Platform == SP_OPENGL_SM5 || Platform == SP_OPENGL_PCES2;
}

/** Whether the shader platform corresponds to the ES2 feature level. */
inline bool IsES2Platform(const EShaderPlatform Platform)
{
	return Platform == SP_PCD3D_ES2 || Platform == SP_OPENGL_PCES2 || Platform == SP_OPENGL_ES2 || Platform == SP_OPENGL_ES2_WEBGL || Platform == SP_OPENGL_ES2_IOS || Platform == SP_METAL; 
}

/** Whether the shader platform corresponds to the ES2 feature level. */
inline bool IsMobilePlatform(const EShaderPlatform Platform)
{
	return IsES2Platform(Platform) || Platform == SP_METAL;
}

inline bool IsOpenGLPlatform(const EShaderPlatform Platform)
{
	return Platform == SP_OPENGL_SM4 || Platform == SP_OPENGL_SM4_MAC || Platform == SP_OPENGL_SM5 || Platform == SP_OPENGL_PCES2
		|| Platform == SP_OPENGL_ES2 || Platform == SP_OPENGL_ES2_WEBGL || Platform == SP_OPENGL_ES2_IOS || Platform == SP_OPENGL_ES31_EXT;
}

inline bool IsConsolePlatform(const EShaderPlatform Platform)
{
	return Platform == SP_PS4 || Platform == SP_XBOXONE;
}

inline ERHIFeatureLevel::Type GetMaxSupportedFeatureLevel(EShaderPlatform InShaderPlatform)
{
	switch(InShaderPlatform)
	{
	case SP_PCD3D_SM5:
	case SP_OPENGL_SM5:
	case SP_PS4:
	case SP_XBOXONE:
	case SP_OPENGL_ES31_EXT:
		return ERHIFeatureLevel::SM5;
	case SP_PCD3D_SM4:
	case SP_OPENGL_SM4:
	case SP_OPENGL_SM4_MAC:
	case SP_METAL_MRT:
		return ERHIFeatureLevel::SM4;
	case SP_PCD3D_ES2:
	case SP_OPENGL_PCES2:
	case SP_OPENGL_ES2:
	case SP_OPENGL_ES2_WEBGL:
	case SP_OPENGL_ES2_IOS:
		return ERHIFeatureLevel::ES2;
	case SP_METAL:
		return ERHIFeatureLevel::ES3_1;
	default:
		check(0);
		return ERHIFeatureLevel::Num;
	}	
}

/** Returns true if the feature level is supported by the shader platform. */
inline bool IsFeatureLevelSupported(EShaderPlatform InShaderPlatform, ERHIFeatureLevel::Type InFeatureLevel)
{
	switch(InShaderPlatform)
	{
	case SP_PCD3D_SM5:
		return InFeatureLevel <= ERHIFeatureLevel::SM5;
	case SP_PCD3D_SM4:
		return InFeatureLevel <= ERHIFeatureLevel::SM4;
	case SP_PCD3D_ES2:
		return InFeatureLevel <= ERHIFeatureLevel::ES2;
	case SP_OPENGL_PCES2:
		return InFeatureLevel <= ERHIFeatureLevel::ES2;
	case SP_OPENGL_ES2:
		return InFeatureLevel <= ERHIFeatureLevel::ES2;
	case SP_OPENGL_ES2_WEBGL: 
		return InFeatureLevel <= ERHIFeatureLevel::ES2;
	case SP_OPENGL_ES2_IOS: 
		return InFeatureLevel <= ERHIFeatureLevel::ES2;
	case SP_OPENGL_SM4:
		return InFeatureLevel <= ERHIFeatureLevel::SM4;
	case SP_OPENGL_SM4_MAC:
		return InFeatureLevel <= ERHIFeatureLevel::SM4;
	case SP_OPENGL_SM5:
		return InFeatureLevel <= ERHIFeatureLevel::SM5;
	case SP_PS4:
		return InFeatureLevel <= ERHIFeatureLevel::SM5;
	case SP_XBOXONE:
		return InFeatureLevel <= ERHIFeatureLevel::SM5;
	case SP_METAL: 
		return InFeatureLevel <= ERHIFeatureLevel::ES3_1;
	case SP_METAL_MRT:
		return InFeatureLevel <= ERHIFeatureLevel::SM4;
	case SP_OPENGL_ES31_EXT:
		return InFeatureLevel <= ERHIFeatureLevel::SM5;
	default:
		return false;
	}	
}

inline bool RHISupportsTessellation(const EShaderPlatform Platform)
{
	if (IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5))
	{
		return ((Platform == SP_PCD3D_SM5) || (Platform == SP_XBOXONE) || (Platform == SP_OPENGL_SM5) || (Platform == SP_OPENGL_ES31_EXT));
	}
	return false;
}

inline bool RHINeedsToSwitchVerticalAxis(EShaderPlatform Platform)
{
	// ES2 needs to flip when rendering to an RT that will be post processed
	return IsES2Platform(Platform) && !IsPCPlatform(Platform) && Platform != SP_METAL;
}

inline bool RHISupportsInstancing(const EShaderPlatform Platform)
{
	//@todo-rco: Add Metal support
	return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM4);// || (Platform == SP_METAL);
}

inline bool RHISupportsSeparateMSAAAndResolveTextures(const EShaderPlatform Platform)
{
	// Metal needs to handle MSAA and resolve textures internally (unless RHICreateTexture2D was changed to take an optional resolve target)
	return Platform != SP_METAL && Platform != SP_METAL_MRT;
}

inline bool RHISupportsComputeShaders(const EShaderPlatform Platform)
{
	//@todo-rco: Add Metal support
	return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);// || (Platform == SP_METAL);
}

inline bool RHISupportsGeometryShaders(const EShaderPlatform Platform)
{
	return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM4) && Platform != SP_METAL_MRT;
}

inline bool RHIHasTiledGPU(const EShaderPlatform Platform)
{
	return Platform == SP_METAL_MRT|| Platform == SP_METAL || Platform == SP_OPENGL_ES2_IOS || Platform == SP_OPENGL_ES2;
}

inline uint32 GetFeatureLevelMaxTextureSamplers(ERHIFeatureLevel::Type FeatureLevel)
{
	if (FeatureLevel == ERHIFeatureLevel::ES2)
	{
		return 8;
	}
	else
	{
		return 16;
	}
}

inline int32 GetFeatureLevelMaxNumberOfBones(ERHIFeatureLevel::Type FeatureLevel)
{
	switch (FeatureLevel)
	{
	case ERHIFeatureLevel::ES2:
		return 75;
	case ERHIFeatureLevel::ES3_1:
	case ERHIFeatureLevel::SM4:
	case ERHIFeatureLevel::SM5:
		return 256;
	default:
		check(0);
	}

	return 0;
}

inline bool IsUniformBufferResourceType(EUniformBufferBaseType BaseType)
{
	return BaseType == UBMT_SRV || BaseType == UBMT_UAV || BaseType == UBMT_SAMPLER || BaseType == UBMT_TEXTURE;
}

#endif	// __RHIDEFINITIONS_H__
