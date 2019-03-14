#pragma once
#include <string>
#include "Renderer.h"
#include "CEGUI/ShaderWrapper.h"

#include "glm/glm.hpp"

#include "Urho3D/Urho3D.h"
namespace CEGUI
{
	class URHO3D_API Urho3DShaderWrapper : public ShaderWrapper
	{
	public:
		~Urho3DShaderWrapper();
	};
}