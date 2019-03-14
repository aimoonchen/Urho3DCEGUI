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
			//d_ogreRoot(Ogre::Root::getSingletonPtr()),
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
		//Ogre::Root* d_ogreRoot;
		//! Pointer to the render system for Ogre.
		Urho3D::Renderer* d_renderSystem;
#if !defined(CEGUI_USE_OGRE_COMPOSITOR2)
		//! Pointer to the previous viewport set in render system.
		Urho3D::Viewport* d_previousVP;
		//! Previous projection matrix set on render system.
		Urho3D::Matrix4 d_previousProjMatrix;
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
	void Urho3DRenderer::setRenderingEnabled(const bool enabled)
	{
#ifdef CEGUI_USE_OGRE_COMPOSITOR2
		d_pimpl->d_frameListener->setCEGUIRenderEnabled(enabled);
		d_pimpl->d_workspace->setEnabled(false);
#else
		S_frameListener.setCEGUIRenderEnabled(enabled);
#endif // CEGUI_USE_OGRE_COMPOSITOR2
	}

	//----------------------------------------------------------------------------//
	bool Urho3DRenderer::isRenderingEnabled() const
	{
#ifdef CEGUI_USE_OGRE_COMPOSITOR2
		return d_pimpl->d_frameListener->isCEGUIRenderEnabled();
#else
		return S_frameListener.isCEGUIRenderEnabled();
#endif // CEGUI_USE_OGRE_COMPOSITOR2
	}

// 	void Urho3DRenderer::initialiseDisplaySizeWithViewportSize()
// 	{
// 		GLint vp[4];
// 		glGetIntegerv(GL_VIEWPORT, vp);
// 		d_displaySize = Sizef(static_cast<float>(vp[2]),
// 			static_cast<float>(vp[3]));
// 	}
// 
// 	//----------------------------------------------------------------------------//
// 	void Urho3DRenderer::initialiseMaxTextureSize()
// 	{
// 		GLint max_tex_size;
// 		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
// 		d_maxTextureSize = max_tex_size;
// 	}

	//----------------------------------------------------------------------------//
	RenderTarget& Urho3DRenderer::getDefaultRenderTarget()
	{
		return *d_defaultTarget;
	}

	RefCounted<RenderMaterial> Urho3DRenderer::createRenderMaterial(
		const DefaultShaderType shaderType) const
	{
		if (shaderType == DefaultShaderType::Textured)
		{
			RefCounted<RenderMaterial> render_material(new
				RenderMaterial(d_pimpl->d_texturedShaderWrapper));

			return render_material;
		}
		else if (shaderType == DefaultShaderType::Solid)
		{
			RefCounted<RenderMaterial> render_material(new
				RenderMaterial(d_pimpl->d_colouredShaderWrapper));

			return render_material;
		}
		else
		{
			throw RendererException("A default shader of this type does not exist.");

			return RefCounted<RenderMaterial>();
		}
	}

	GeometryBuffer& Urho3DRenderer::createGeometryBufferColoured(CEGUI::RefCounted<RenderMaterial> renderMaterial)
	{
		Urho3DGeometryBuffer* geom_buffer = new Urho3DGeometryBuffer(*this,
			*d_pimpl->d_renderSystem, renderMaterial);

		geom_buffer->addVertexAttribute(VertexAttributeType::Position0);
		geom_buffer->addVertexAttribute(VertexAttributeType::Colour0);
		geom_buffer->finaliseVertexAttributes(Urho3DGeometryBuffer::MT_COLOURED);

		addGeometryBuffer(*geom_buffer);
		return *geom_buffer;
	}

	GeometryBuffer& Urho3DRenderer::createGeometryBufferTextured(CEGUI::RefCounted<RenderMaterial> renderMaterial)
	{
		Urho3DGeometryBuffer* geom_buffer = new Urho3DGeometryBuffer(*this,
			*d_pimpl->d_renderSystem, renderMaterial);

		geom_buffer->addVertexAttribute(VertexAttributeType::Position0);
		geom_buffer->addVertexAttribute(VertexAttributeType::Colour0);
		geom_buffer->addVertexAttribute(VertexAttributeType::TexCoord0);
		geom_buffer->finaliseVertexAttributes(Urho3DGeometryBuffer::MT_TEXTURED);

		addGeometryBuffer(*geom_buffer);
		return *geom_buffer;
	}

	TextureTarget* Urho3DRenderer::createTextureTarget(bool addStencilBuffer)
	{
		TextureTarget* tt = new Urho3DTextureTarget(*this, *d_pimpl->d_renderSystem, addStencilBuffer);
		d_pimpl->d_textureTargets.push_back(tt);
		return tt;
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroyTextureTarget(TextureTarget* target)
	{
		TextureTargetList::iterator i = std::find(d_pimpl->d_textureTargets.begin(),
			d_pimpl->d_textureTargets.end(),
			target);

		if (d_pimpl->d_textureTargets.end() != i)
		{
			d_pimpl->d_textureTargets.erase(i);
			delete target;
		}
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroyAllTextureTargets()
	{
		while (!d_pimpl->d_textureTargets.empty())
			destroyTextureTarget(*d_pimpl->d_textureTargets.begin());
	}

	Texture& Urho3DRenderer::createTexture(const String& name)
	{
		throwIfNameExists(name);

		Urho3DTexture* t = new Urho3DTexture(name);
		d_pimpl->d_textures[name] = t;

		logTextureCreation(name);

		return *t;
	}

	//----------------------------------------------------------------------------//
	Texture& Urho3DRenderer::createTexture(const String& name, const String& filename, const String& resourceGroup)
	{
		throwIfNameExists(name);

		Urho3DTexture* t = new Urho3DTexture(name, filename, resourceGroup);
		d_pimpl->d_textures[name] = t;

		logTextureCreation(name);

		return *t;
	}

	//----------------------------------------------------------------------------//
	Texture& Urho3DRenderer::createTexture(const String& name, const Sizef& size)
	{
		throwIfNameExists(name);

		Urho3DTexture* t = new Urho3DTexture(name, size);
		d_pimpl->d_textures[name] = t;

		logTextureCreation(name);

		return *t;
	}

	Texture& Urho3DRenderer::createTexture(const String& name, Urho3D::SharedPtr<Urho3D::Texture>& tex, bool take_ownership)
	{
		throwIfNameExists(name);

		OgreTexture* t = new OgreTexture(name, tex, take_ownership);
		d_pimpl->d_textures[name] = t;

		logTextureCreation(name);

		return *t;
	}

	void Urho3DRenderer::logTextureCreation(const String& name)
	{
		Logger* logger = Logger::getSingletonPtr();
		if (logger)
			logger->logEvent("[OpenGLRenderer] Created texture: " + name);
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroyTexture(Texture& texture)
	{
		destroyTexture(texture.getName());
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroyTexture(const String& name)
	{
		TextureMap::iterator i = d_textures.find(name);

		if (d_textures.end() != i)
		{
			logTextureDestruction(name);
			delete i->second;
			d_textures.erase(i);
		}
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::logTextureDestruction(const String& name)
	{
		Logger* logger = Logger::getSingletonPtr();
		if (logger)
			logger->logEvent("[OpenGLRenderer] Destroyed texture: " + name);
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::destroyAllTextures()
	{
		while (!d_textures.empty())
			destroyTexture(d_textures.begin()->first);
	}

	//----------------------------------------------------------------------------//
	Texture& Urho3DRenderer::getTexture(const String& name) const
	{
		TextureMap::const_iterator i = d_textures.find(name);

		if (i == d_textures.end())
			throw UnknownObjectException(
				"No texture named '" + name + "' is available.");

		return *i->second;
	}

	//----------------------------------------------------------------------------//
	bool Urho3DRenderer::isTextureDefined(const String& name) const
	{
		return d_textures.find(name) != d_textures.end();
	}

	//----------------------------------------------------------------------------//
	const Sizef& Urho3DRenderer::getDisplaySize() const
	{
		return d_displaySize;
	}
	//----------------------------------------------------------------------------//
	unsigned int Urho3DRenderer::getMaxTextureSize() const
	{
		return d_maxTextureSize;
	}

	//----------------------------------------------------------------------------//
	const String& Urho3DRenderer::getIdentifierString() const
	{
		return d_rendererID;
	}

	//----------------------------------------------------------------------------//
	Texture& Urho3DRenderer::createTexture(const String& name, GLuint tex,
		const Sizef& sz)
	{
		if (d_textures.find(name) != d_textures.end())
			throw AlreadyExistsException(
				"A texture named '" + name + "' already exists.");

		OpenGLTexture* t = createTexture_impl(name);
		t->initialise(tex, sz);
		d_textures[name] = t;

		logTextureCreation(name);

		return *t;
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::setDisplaySize(const Sizef& sz)
	{
		if (sz != d_pimpl->d_displaySize)
		{
			d_pimpl->d_displaySize = sz;

			// FIXME: This is probably not the right thing to do in all cases.
			Rectf area(d_pimpl->d_defaultTarget->getArea());
			area.setSize(sz);
			d_pimpl->d_defaultTarget->setArea(area);
		}
	}

	const CEGUI::Rectf& Urho3DRenderer::getActiveViewPort()
	{
		return d_activeRenderTarget->getArea();
	}

	//----------------------------------------------------------------------------//
	bool Urho3DRenderer::isTexCoordSystemFlipped() const
	{
		return true;
	}

	void Urho3DRenderer::setupRenderingBlendMode(const BlendMode mode, const bool force)
	{
#ifdef CEGUI_USE_OGRE_HLMS
		// Enable force if no blend block has been created
		if (!d_pimpl->d_hlmsBlendblock)
			force = true;

#endif //CEGUI_USE_OGRE_HLMS

		// do nothing if mode appears current (and is not forced)
		if ((d_pimpl->d_activeBlendMode == mode) && !force)
			return;

		d_pimpl->d_activeBlendMode = mode;

#ifdef CEGUI_USE_OGRE_HLMS

		Ogre::HlmsManager* hlmsManager = d_pimpl->d_ogreRoot->getHlmsManager();

		// Blend block
		Ogre::HlmsBlendblock blendblock;
		blendblock.mAlphaToCoverageEnabled = false;
		if (d_pimpl->d_activeBlendMode == BlendMode::RttPremultiplied)
		{
			blendblock.mSourceBlendFactor = Ogre::SBF_SOURCE_ALPHA;
			blendblock.mDestBlendFactor = Ogre::SBF_ONE_MINUS_SOURCE_ALPHA;
		}
		else
		{
			blendblock.mSeparateBlend = true;
			blendblock.mSourceBlendFactor = Ogre::SBF_SOURCE_ALPHA;
			blendblock.mDestBlendFactor = Ogre::SBF_ONE_MINUS_SOURCE_ALPHA;
			blendblock.mSourceBlendFactorAlpha = Ogre::SBF_ONE_MINUS_DEST_ALPHA;
			blendblock.mDestBlendFactorAlpha = Ogre::SBF_ONE;
		}
		d_pimpl->d_hlmsBlendblock = hlmsManager->getBlendblock(blendblock);


#else
		using namespace Ogre;

		if (d_pimpl->d_activeBlendMode == BlendMode::RttPremultiplied)
			d_pimpl->d_renderSystem->_setSceneBlending(SBF_ONE,
				SBF_ONE_MINUS_SOURCE_ALPHA);
		else
			d_pimpl->d_renderSystem->
			_setSeparateSceneBlending(SBF_SOURCE_ALPHA,
				SBF_ONE_MINUS_SOURCE_ALPHA,
				SBF_ONE_MINUS_DEST_ALPHA,
				SBF_ONE);

#endif //CEGUI_USE_OGRE_HLMS
	}

	void Urho3DRenderer::beginRendering()
	{
		// Deprecated OpenGL 2 client states may mess up rendering. They are not added here
		// since they are deprecated and thus do not fit in a OpenGL Core renderer. However
		// this information may be relevant for people combining deprecated and modern
		// functions. In that case disable client states like this: glDisableClientState(GL_VERTEX_ARRAY);

		d_openGLStateChanger->reset();

		// if enabled, restores a subset of the GL state back to default values.
		if (d_isStateResettingEnabled)
			restoreChangedStatesToDefaults(false);

		d_openGLStateChanger->enable(GL_SCISSOR_TEST);
		d_openGLStateChanger->enable(GL_BLEND);

		// force set blending ops to get to a known state.
		setupRenderingBlendMode(BlendMode::Normal, true);
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::endRendering()
	{
		if (d_isStateResettingEnabled)
			restoreChangedStatesToDefaults(true);
	}

	void Urho3DRenderer::constructor_impl(Urho3D::RenderSurface& target)
	{
		d_pimpl->d_renderSystem = d_pimpl->d_ogreRoot->getRenderSystem();

		d_pimpl->d_displaySize.d_width = static_cast<float>(target.getWidth());
		d_pimpl->d_displaySize.d_height = static_cast<float>(target.getHeight());

		//! Checking if OpenGL > 3.2 supported
		if (d_pimpl->d_renderSystem->getName().find("OpenGL 3+") != Ogre::String::npos)
		{
			d_pimpl->d_useGLSLCore = true;
		}

		// create default target & rendering root (surface) that uses it
		d_pimpl->d_defaultTarget =
			new Urho3DWindowTarget(*this, *d_pimpl->d_renderSystem, target);

#ifndef CEGUI_USE_OGRE_HLMS
#if OGRE_VERSION >= 0x10800
#ifndef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
		throw RendererException("RT Shader System not available. However CEGUI relies on shaders for rendering. ");
#endif
#endif
#endif //CEGUI_USE_OGRE_HLMS

		// hook into the rendering process
#ifdef CEGUI_USE_OGRE_COMPOSITOR2

	// Some automatic bootstrapping
		if (!OgreRenderer_impl::s_compositorResourcesInitialized)
		{
			createOgreCompositorResources();
		}

		// Create the dummy scene and camera
		std::stringstream scene_name;
		scene_name << "CEGUI_forWindow_" <<
			OgreRenderer_impl::s_createdSceneNumber++;

		d_pimpl->d_dummyScene = d_pimpl->d_ogreRoot->createSceneManager(
			Ogre::ST_INTERIOR, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD,
			scene_name.str());

		// Unused camera for the scene
		d_pimpl->d_dummyCamera = d_pimpl->d_dummyScene->createCamera(
			"CEGUI_dummy_camera");


		// We will get notified when the workspace is drawn
		d_pimpl->d_frameListener = new OgreGUIRenderQueueListener(this);

		// Create the workspace for rendering
		Ogre::CompositorManager2* manager = d_pimpl->d_ogreRoot->
			getCompositorManager2();

		// The -1 should guarantee this to be rendered last on top of everything
		d_pimpl->d_workspace = manager->addWorkspace(d_pimpl->d_dummyScene,
			&target, d_pimpl->d_dummyCamera, "CEGUI_workspace", true, -1);

		d_pimpl->d_workspace->setListener(d_pimpl->d_frameListener);

#else
		d_pimpl->d_ogreRoot->addFrameListener(&S_frameListener);
#endif // CEGUI_USE_OGRE_COMPOSITOR2

		initialiseShaders();
	}

	void Urho3DRenderer::clearVertexBufferPool()
	{
		d_pimpl->d_vbPool.clear();
	}

	Urho3DRenderer::Urho3DRenderer()
		: d_pimpl(new Urho3DRenderer_impl())
	{
		checkOgreInitialised();

		// get auto created window
		Ogre::RenderWindow* rwnd = d_pimpl->d_ogreRoot->getAutoCreatedWindow();
		if (!rwnd)
			throw RendererException(
				"Ogre was not initialised to automatically create a window, you "
				"should therefore be explicitly specifying a Ogre::RenderTarget in "
				"the OgreRenderer::create function.");

		constructor_impl(*rwnd);
	}

	//----------------------------------------------------------------------------//
	Urho3DRenderer::Urho3DRenderer(Urho3D::RenderSurface& target) :
		d_pimpl(new Urho3DRenderer_impl())
	{
		checkOgreInitialised();

		constructor_impl(target);
	}

	//----------------------------------------------------------------------------//
	Urho3DRenderer::~Urho3DRenderer()
	{
#ifdef CEGUI_USE_OGRE_COMPOSITOR2
		// Remove the listener and then delete the scene
		d_pimpl->d_workspace->setListener(0);

		d_pimpl->d_ogreRoot->destroySceneManager(d_pimpl->d_dummyScene);

		d_pimpl->d_dummyScene = 0;
		d_pimpl->d_dummyCamera = 0;

		// Remove the workspace so the contents aren't rendered anymore
		d_pimpl->d_ogreRoot->getCompositorManager2()->removeWorkspace(
			d_pimpl->d_workspace);

		d_pimpl->d_workspace = 0;

#else
		d_pimpl->d_ogreRoot->removeFrameListener(&S_frameListener);
#endif // CEGUI_USE_OGRE_COMPOSITOR2

		cleanupShaders();

#ifdef CEGUI_USE_OGRE_COMPOSITOR2
		delete d_pimpl->d_frameListener;
#endif

		destroyAllGeometryBuffers();
		destroyAllTextureTargets();
		destroyAllTextures();
		clearVertexBufferPool();

		delete d_pimpl->d_defaultTarget;
		delete d_pimpl;
	}

	//----------------------------------------------------------------------------//
	void Urho3DRenderer::checkOgreInitialised()
	{
// 		if (!d_pimpl->d_ogreRoot)
// 			throw RendererException("The Ogre::Root object has not been "
// 				"created. You must initialise Ogre first!");
// 
// 		if (!d_pimpl->d_ogreRoot->isInitialised())
// 			throw RendererException("Ogre has not been initialised. You must "
// 				"initialise Ogre first!");
	}

	void Urho3DRenderer::throwIfNameExists(const String& name) const
	{
		if (d_pimpl->d_textures.find(name) != d_pimpl->d_textures.end())
			throw AlreadyExistsException("[Urho3DRenderer] Texture already exists: " + name);
	}

	void Urho3DRenderer::initialiseShaders()
	{
#ifdef CEGUI_USE_OGRE_HLMS
		Ogre::HlmsManager* hlmsManager = d_pimpl->d_ogreRoot->getHlmsManager();

		// Macro block
		Ogre::HlmsMacroblock macroblock;
		macroblock.mDepthCheck = false;
		macroblock.mDepthWrite = false;
		macroblock.mDepthBiasConstant = 0;
		macroblock.mDepthBiasSlopeScale = 0;
		macroblock.mCullMode = Ogre::CULL_NONE;
		macroblock.mPolygonMode = Ogre::PM_SOLID;
		macroblock.mScissorTestEnabled = true;
		d_pimpl->d_hlmsMacroblock = hlmsManager->getMacroblock(macroblock);

		// Sampler block
		Ogre::HlmsSamplerblock samplerblock;
		samplerblock.mMinFilter = Ogre::FO_LINEAR;
		samplerblock.mMagFilter = Ogre::FO_LINEAR;
		samplerblock.mMipFilter = Ogre::FO_POINT;
		samplerblock.mU = Ogre::TAM_CLAMP;
		samplerblock.mV = Ogre::TAM_CLAMP;
		samplerblock.mW = Ogre::TAM_CLAMP;
		samplerblock.mCompareFunction = Ogre::NUM_COMPARE_FUNCTIONS;
		d_pimpl->d_hlmsSamplerblock = hlmsManager->getSamplerblock(samplerblock);

		// PSO
		d_pimpl->d_hlmsCache = Ogre::SharedPtr<Ogre::PsoCacheHelper>(
			new Ogre::PsoCacheHelper(d_pimpl->d_renderSystem));

#endif

		Ogre::HighLevelGpuProgramPtr texture_vs;
		Ogre::HighLevelGpuProgramPtr texture_ps;

		Ogre::HighLevelGpuProgramPtr colour_vs;
		Ogre::HighLevelGpuProgramPtr colour_ps;

		d_pimpl->d_useGLSL = Ogre::HighLevelGpuProgramManager::getSingleton().
			isLanguageSupported("glsl");
		d_pimpl->d_useGLSLES = Ogre::HighLevelGpuProgramManager::getSingleton().
			isLanguageSupported("glsles");
		d_pimpl->d_useHLSL = Ogre::HighLevelGpuProgramManager::getSingleton().
			isLanguageSupported("hlsl");

		Ogre::String shaderLanguage;

		if (d_pimpl->d_useGLSL)
		{
			shaderLanguage = "glsl";
		}
		else if (d_pimpl->d_useGLSLES)
		{
			shaderLanguage = "glsles";
		}
		else if (d_pimpl->d_useHLSL)
		{
			shaderLanguage = "hlsl";
		}
		else {
			throw RendererException("Underlying Ogre render system does not support available "
				"shader languages which should be one of glsl, glsles or hlsl "
				"which are required for supporting custom shaders in this CEGUI version");
		}

		// Create vertex shaders
		texture_vs = Ogre::HighLevelGpuProgramManager::getSingleton().
			createProgram("__cegui_internal_texture_vs__",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				shaderLanguage, Ogre::GPT_VERTEX_PROGRAM);

		colour_vs = Ogre::HighLevelGpuProgramManager::getSingleton().
			createProgram("__cegui_internal_colour_vs__",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				shaderLanguage, Ogre::GPT_VERTEX_PROGRAM);

		// Create pixel shaders
		texture_ps = Ogre::HighLevelGpuProgramManager::getSingleton().
			createProgram("__cegui_internal_texture_ps__",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				shaderLanguage, Ogre::GPT_FRAGMENT_PROGRAM);

		colour_ps = Ogre::HighLevelGpuProgramManager::getSingleton().
			createProgram("__cegui_internal_colour_ps__",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				shaderLanguage, Ogre::GPT_FRAGMENT_PROGRAM);

		// We always enter through the main function
		texture_vs->setParameter("entry_point", "main");
		texture_ps->setParameter("entry_point", "main");
		colour_vs->setParameter("entry_point", "main");
		colour_ps->setParameter("entry_point", "main");

		// If we use GLSL
		if (d_pimpl->d_useGLSL)
		{
			// We check if we want to use a GLSL core shader, which is required for the Ogre OpenGL 3+ Renderer
			if (d_pimpl->d_useGLSLCore)
			{
				texture_vs->setParameter("target", "glsl");
				texture_vs->setSource(VertexShaderTextured_GLSL);

				colour_vs->setParameter("target", "glsl");
				colour_vs->setSource(VertexShaderColoured_GLSL);

				texture_ps->setParameter("target", "glsl");
				texture_ps->setSource(PixelShaderTextured_GLSL);

				colour_ps->setParameter("target", "glsl");
				colour_ps->setSource(PixelShaderColoured_GLSL);
			}
			else // else we use regular GLSL shader, as in the normal Ogre OpenGL Renderer
			{
				texture_vs->setParameter("target", "arbvp1");
				texture_vs->setSource(VertexShaderTextured_GLSL_Compat);

				colour_vs->setParameter("target", "arbvp1");
				colour_vs->setSource(VertexShaderColoured_GLSL_Compat);

				texture_ps->setParameter("target", "arbfp1");
				texture_ps->setSource(PixelShaderTextured_GLSL_Compat);

				colour_ps->setParameter("target", "arbfp1");
				colour_ps->setSource(PixelShaderColoured_GLSL_Compat);
			}
		}
		else if (d_pimpl->d_useGLSLES)
		{
			texture_vs->setParameter("target", "glsles");
			texture_vs->setSource(VertexShaderTextured_GLSLES1);

			colour_vs->setParameter("target", "glsles");
			colour_vs->setSource(VertexShaderColoured_GLSLES1);

			texture_ps->setParameter("target", "glsles");
			texture_ps->setSource(PixelShaderTextured_GLSLES1);

			colour_ps->setParameter("target", "glsles");
			colour_ps->setSource(PixelShaderColoured_GLSLES1);
		}
		else // else we use a hlsl shader with an available syntax code
		{
			if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("vs_5_0"))
			{
				texture_vs->setParameter("target", "vs_5_0");
				texture_vs->setSource(VertexShaderTextured_HLSL);

				colour_vs->setParameter("target", "vs_5_0");
				colour_vs->setSource(VertexShaderColoured_HLSL);
			}
			else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("vs_4_0"))
			{
				texture_vs->setParameter("target", "vs_4_0");
				texture_vs->setSource(VertexShaderTextured_HLSL);

				colour_vs->setParameter("target", "vs_4_0");
				colour_vs->setSource(VertexShaderColoured_HLSL);
			}
			else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("vs_2_0"))
			{
				texture_vs->setParameter("target", "vs_2_0");
				texture_vs->setSource(VertexShaderTextured_HLSL);

				colour_vs->setParameter("target", "vs_2_0");
				colour_vs->setSource(VertexShaderColoured_HLSL);
			}
			else// If no shader was compatible
			{
				throw RendererException(
					"OgreRenderer::initialiseShaders: No supported syntax - "
					"unable to compile for vs_5_0, vs_4_0 or vs_2_0");
			}

			if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_5_0"))
			{
				texture_ps->setParameter("target", "ps_5_0");
				texture_ps->setSource(PixelShaderTextured_PS5_HLSL);

				colour_ps->setParameter("target", "ps_5_0");
				colour_ps->setSource(PixelShaderColoured_PS5_HLSL);
			}
			else if (Ogre::GpuProgramManager::getSingletonPtr()->isSyntaxSupported("ps_4_0"))
			{
				texture_ps->setParameter("target", "ps_4_0");
				texture_ps->setSource(PixelShaderTextured_PS5_HLSL);

				colour_ps->setParameter("target", "ps_4_0");
				colour_ps->setSource(PixelShaderColoured_PS5_HLSL);
			}
			else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0"))
			{
				texture_ps->setParameter("target", "ps_2_0");
				texture_ps->setSource(PixelShaderTextured_HLSL);

				colour_ps->setParameter("target", "ps_2_0");
				colour_ps->setSource(PixelShaderColoured_HLSL);
			}
			else
			{
				throw RendererException(
					"OgreRenderer::initialiseShaders: No supported syntax - "
					"unable to compile for ps_5_0, ps_4_0 or ps_2_0");
			}

		}

		// Load all the shaders after setting the source code
		texture_vs->load();
		texture_ps->load();
		colour_vs->load();
		colour_ps->load();

		d_pimpl->d_texturedShaderWrapper = new Urho3DShaderWrapper(*this,
			*d_pimpl->d_renderSystem, texture_vs, texture_ps);

		d_pimpl->d_colouredShaderWrapper = new Urho3DShaderWrapper(*this,
			*d_pimpl->d_renderSystem, colour_vs, colour_ps);
	}

	void Urho3DRenderer::cleanupShaders()
	{
		delete d_pimpl->d_texturedShaderWrapper;
		delete d_pimpl->d_colouredShaderWrapper;
	}
}