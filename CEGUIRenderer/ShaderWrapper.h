#pragma once
#include <string>
#include "Renderer.h"
#include "CEGUI/ShaderWrapper.h"

#include "glm/glm.hpp"

#include "Urho3D/Urho3D.h"
#include "Urho3D/Container/Ptr.h"

namespace Urho3D
{
	class Renderer;
	class ShaderVariation;
}

namespace CEGUI
{
	class Urho3DRenderer;
	class URHO3D_API Urho3DShaderWrapper : public ShaderWrapper
	{
	public:
		Urho3DShaderWrapper(Urho3DRenderer& owner, Urho3D::Renderer& rs, Urho3D::SharedPtr<Urho3D::ShaderVariation> vs, Urho3D::SharedPtr<Urho3D::ShaderVariation> ps);
		~Urho3DShaderWrapper();
		void prepareForRendering(const ShaderParameterBindings* shaderParameterBindings) override;
	};
}