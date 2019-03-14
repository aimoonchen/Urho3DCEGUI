#include "Renderer.h"
#include "CEGUI/GUIContext.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/System.h"
#include "CEGUI/Logger.h"
#include "glm/gtc/type_ptr.hpp"
#include <unordered_map>

#include "Urho3D/Math/Matrix4.h"

#include "GeometryBuffer.h"
#include "TextureTarget.h"
#include "Texture.h"
//#include "WindowTarget.h"
#include "ResourceProvider.h"
#include "ImageCodec.h"
#include "ShaderWrapper.h"

namespace CEGUI
{
	using TextureTargetList = std::vector<TextureTarget*>;
	using GeometryBufferList = std::vector<Urho3DGeometryBuffer*>;
	using TextureMap = std::unordered_map<String, Urho3DTexture*>;

	struct Urho3DRenderer_impl
	{
		Urho3DRenderer_impl() :
			//! TODO: Currently there is no way to do this easily using Ogre
			d_maxTextureSize(2048),
			d_ogreRoot(Ogre::Root::getSingletonPtr()),
#if !defined(CEGUI_USE_OGRE_COMPOSITOR2)
			d_previousVP(0),
#else
			d_frameListener(0),
			d_dummyScene(0),
			d_dummyCamera(0),
			d_workspace(0),
#endif
#ifdef CEGUI_USE_OGRE_HLMS
			//d_renderTarget(nullptr),
			d_hlmsMacroblock(nullptr),
			d_hlmsBlendblock(nullptr),
			d_hlmsSamplerblock(nullptr),
#endif
			d_activeBlendMode(BlendMode::Invalid),
			d_makeFrameControlCalls(true),
			d_useGLSL(false),
			d_useGLSLES(false),
			d_useHLSL(false),
			d_useGLSLCore(false),
			d_texturedShaderWrapper(0),
			d_colouredShaderWrapper(0)
		{}


		//! String holding the renderer identification text.
		static String d_rendererID;
		//! What the renderer considers to be the current display size.
		Sizef d_displaySize;
		//! The default RenderTarget
		OgreWindowTarget* d_defaultTarget;
		//! Container used to track texture targets.
		TextureTargetList d_textureTargets;
		//! Container used to track geometry buffers.
		GeometryBufferList d_geometryBuffers;
		//! Container used to track textures.
		TextureMap d_textures;
		//! What the renderer thinks the max texture size is.
		unsigned int d_maxTextureSize;
		//! OGRE root object ptr
		Ogre::Root* d_ogreRoot;
		//! Pointer to the render system for Ogre.
		Ogre::RenderSystem* d_renderSystem;
#if !defined(CEGUI_USE_OGRE_COMPOSITOR2)
		//! Pointer to the previous viewport set in render system.
		Ogre::Viewport* d_previousVP;
		//! Previous projection matrix set on render system.
		Ogre::Matrix4 d_previousProjMatrix;
#else

		//! This is used to get notifications when our scene is rendered
		//! no longer static because it requires a pointer to this
		OgreGUIRenderQueueListener* d_frameListener;

		//! Used to render at the correct time
		Ogre::SceneManager* d_dummyScene;
		//! This might not be needed, but it's here
		Ogre::Camera* d_dummyCamera;

		Ogre::CompositorWorkspace* d_workspace;

		//! Makes all scene names unique
		static int s_createdSceneNumber;

		//! Allows the initialization to remain the same by automatically
		//! initializing the Compositor if it isn't already
		static bool s_compositorResourcesInitialized;

#endif

#ifdef CEGUI_USE_OGRE_HLMS
		//! OGRE render target
		//Ogre::RenderTarget* d_renderTarget;
		//! HLMS block used to set macro parameters
		const Ogre::HlmsMacroblock* d_hlmsMacroblock;
		//! HLMS block used to set blending parameters
		const Ogre::HlmsBlendblock* d_hlmsBlendblock;
		//! HLMS block used to set sampling parameters
		const Ogre::HlmsSamplerblock* d_hlmsSamplerblock;

		//! HLMS cache used to store pso
		Ogre::SharedPtr<Ogre::PsoCacheHelper> d_hlmsCache;
#endif

		//! What we think is the current blend mode to use
		BlendMode d_activeBlendMode;
		//! Whether _beginFrame and _endFrame will be called.
		bool d_makeFrameControlCalls;
		//! Whether shaders are glsl or hlsl
		bool d_useGLSL;
		//! Whether shaders are glsles
		bool d_useGLSLES;
		//! Whether shaders are hlsl 
		bool d_useHLSL;
		//! Whether we use the ARB glsl shaders or the OpenGL 3.2 Core shader profile (140 core)
		bool d_useGLSLCore;

#ifdef CEGUI_USE_OGRE_HLMS
		//! Vector containing vertex buffers that can be reused
		std::vector<Ogre::v1::HardwareVertexBufferSharedPtr> d_vbPool;
#else
		//! Vector containing vertex buffers that can be reused
		std::vector<Ogre::HardwareVertexBufferSharedPtr> d_vbPool;
#endif //CEGUI_USE_OGRE_HLMS

		Urho3DShaderWrapper* d_texturedShaderWrapper;
		Urho3DShaderWrapper* d_colouredShaderWrapper;
	};

	Urho3DRenderer& Urho3DRenderer::bootstrapSystem(const int abi)
	{
		System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

		if (System::getSingletonPtr())
			throw InvalidRequestException("CEGUI::System object is already initialised.");

#ifdef CEGUI_USE_OGRE_COMPOSITOR2
		createOgreCompositorResources();
#endif

		Urho3DRenderer& renderer = create();
		Urho3DResourceProvider& rp = createUrho3DResourceProvider();
		Urho3DImageCodec& ic = createUrho3DImageCodec();
		System::create(renderer, &rp, static_cast<XMLParser*>(0), &ic);

		return renderer;
	}

	//----------------------------------------------------------------------------//
	Urho3DRenderer& Urho3DRenderer::bootstrapSystem(Urho3D::RenderSurface& target, const int abi)
	{
		System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

		if (System::getSingletonPtr())
			throw InvalidRequestException("CEGUI::System object is already initialised.");

#ifdef CEGUI_USE_OGRE_COMPOSITOR2
		createOgreCompositorResources();
#endif

		Urho3DRenderer& renderer = Urho3DRenderer::create(target);
		Urho3DResourceProvider& rp = createUrho3DResourceProvider();
		Urho3DImageCodec& ic = createUrho3DImageCodec();
		System::create(renderer, &rp, static_cast<XMLParser*>(0), &ic);

		return renderer;
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroySystem()
	{
		System* sys;
		if (!(sys = System::getSingletonPtr()))
			throw InvalidRequestException("CEGUI::System object is not created or was already destroyed.");

		Urho3DRenderer* renderer = static_cast<Urho3DRenderer*>(sys->getRenderer());
		Urho3DResourceProvider* rp = static_cast<Urho3DResourceProvider*>(sys->getResourceProvider());

		Urho3DImageCodec* ic = &static_cast<Urho3DImageCodec&>(sys->getImageCodec());

		System::destroy();
		destroyUrho3DImageCodec(*ic);
		destroyUrho3DResourceProvider(*rp);
		destroy(*renderer);
	}

	//----------------------------------------------------------------------------//
	Urho3DRenderer& Urho3DRenderer::create(const int abi)
	{
		System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

		return *new Urho3DRenderer();
	}

	//----------------------------------------------------------------------------//
	Urho3DRenderer& Urho3DRenderer::create(Urho3D::RenderSurface& target, const int abi)
	{
		System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

		return *new Urho3DRenderer(target);
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroy(Urho3DRenderer& renderer)
	{
		delete &renderer;
	}

	Urho3DResourceProvider& Urho3DRenderer::createUrho3DResourceProvider()
	{
		return *new Urho3DResourceProvider();
	}
	void Urho3DRenderer::destroyUrho3DResourceProvider(Urho3DResourceProvider& rp)
	{
		delete &rp;
	}

	//----------------------------------------------------------------------------//
	Urho3DImageCodec& Urho3DRenderer::createUrho3DImageCodec()
	{
		return *new Urho3DImageCodec();
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroyUrho3DImageCodec(Urho3DImageCodec& ic)
	{
		delete &ic;
	}

	//----------------------------------------------------------------------------//
	//! Conversion function from Ogre to glm
	glm::mat4 Urho3DRenderer::urho3DToGlmMatrix(const Urho3D::Matrix4& matrix)
	{
		return glm::mat4(matrix.m00_, matrix.m01_, matrix.m02_, matrix.m03_,
			matrix.m10_, matrix.m11_, matrix.m12_, matrix.m13_,
			matrix.m20_, matrix.m21_, matrix.m22_, matrix.m23_,
			matrix.m30_, matrix.m31_, matrix.m32_, matrix.m33_);
	}

	//----------------------------------------------------------------------------//
	//! Conversion function from glm to Ogre
	Urho3D::Matrix4 Urho3DRenderer::glmToUrho3DMatrix(const glm::mat4& matrix)
	{
		return Urho3D::Matrix4(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
			matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
			matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
			matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
	}
}