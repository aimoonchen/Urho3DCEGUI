#pragma once
#include "CEGUI/Renderer.h"
#include "CEGUI/Sizef.h"
#include "CEGUI/Config.h"

#include "Urho3D/Urho3D.h"

namespace Urho3D
{
	class RenderSurface;
	class Matrix4;
}
namespace CEGUI
{
	class Urho3DGeometryBuffer;
	class Urho3DTexture;
	class Urho3DResourceProvider;
	class Urho3DImageCodec;
	class Urho3DWindowTarget;
	struct Urho3DRenderer_impl;

	class URHO3D_API Urho3DRenderer : public Renderer
	{
	public:
		static Urho3DRenderer& bootstrapSystem(const int abi = CEGUI_VERSION_ABI);
		static Urho3DRenderer& bootstrapSystem(Urho3D::RenderSurface& target, const int abi = CEGUI_VERSION_ABI);
		static void destroySystem();

		static Urho3DRenderer& create(const int abi = CEGUI_VERSION_ABI);
		static Urho3DRenderer& create(Urho3D::RenderSurface& target, const int abi = CEGUI_VERSION_ABI);
		static void destroy(Urho3DRenderer& renderer);

		static Urho3DResourceProvider& createUrho3DResourceProvider();
		static void destroyUrho3DResourceProvider(Urho3DResourceProvider& rp);

		static Urho3DImageCodec& createUrho3DImageCodec();
		static void destroyUrho3DImageCodec(Urho3DImageCodec& ic);

		static glm::mat4 urho3DToGlmMatrix(const Urho3D::Matrix4& matrix);
		static Urho3D::Matrix4 glmToUrho3DMatrix(const glm::mat4& matrix);

		Texture& createTexture(const String& name, Urho3D::SharedPtr<Urho3D::Texture>& tex, bool take_ownership = false);
		void setupRenderingBlendMode(const BlendMode mode, bool force = false);

		// implement Renderer interface
		RenderTarget& getDefaultRenderTarget() override;
		GeometryBuffer& createGeometryBufferTextured(CEGUI::RefCounted<RenderMaterial> renderMaterial) override;
		GeometryBuffer& createGeometryBufferColoured(CEGUI::RefCounted<RenderMaterial> renderMaterial) override;
		TextureTarget* createTextureTarget(bool addStencilBuffer) override;
		void destroyTextureTarget(TextureTarget* target) override;
		void destroyAllTextureTargets() override;
		Texture& createTexture(const String& name) override;
		Texture& createTexture(const String& name, const String& filename, const String& resourceGroup) override;
		Texture& createTexture(const String& name, const Sizef& size) override;
		void destroyTexture(Texture& texture) override;
		void destroyTexture(const String& name) override;
		void destroyAllTextures() override;
		Texture& getTexture(const String& name) const override;
		bool isTextureDefined(const String& name) const override;
		void setDisplaySize(const Sizef& sz) override;
		const Sizef& getDisplaySize() const override;
		unsigned int getMaxTextureSize() const override;
		const String& getIdentifierString() const override;
		bool isTexCoordSystemFlipped() const override;
		//
		void beginRendering() override;
		void endRendering() override;
		RefCounted<RenderMaterial> createRenderMaterial(const DefaultShaderType shaderType) const override;
	protected:
		Urho3DRenderer_impl* d_pimpl;
	};
}