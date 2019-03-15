#include "ShaderWrapper.h"

namespace CEGUI
{
	Urho3DShaderWrapper::Urho3DShaderWrapper(Urho3DRenderer& owner, Urho3D::Graphics& rs,
		Urho3D::SharedPtr<Urho3D::ShaderVariation> vs, Urho3D::SharedPtr<Urho3D::ShaderVariation> ps)
	{

	}

	Urho3DShaderWrapper::~Urho3DShaderWrapper()
	{

	}

	void Urho3DShaderWrapper::prepareForRendering(const ShaderParameterBindings* shaderParameterBindings)
	{
// #ifdef CEGUI_USE_OGRE_HLMS
// 		d_owner.setGPUPrograms(d_vertexShader, d_pixelShader);
// #else
// 		Ogre::GpuProgram* vs = d_vertexShader->_getBindingDelegate();
// 		d_renderSystem.bindGpuProgram(vs);
// 
// 		Ogre::GpuProgram* ps = d_pixelShader->_getBindingDelegate();
// 		d_renderSystem.bindGpuProgram(ps);
// #endif //CEGUI_USE_OGRE_HLMS
// 
// 		const ShaderParameterBindings::ShaderParameterBindingsMap&
// 			shader_parameter_bindings = shaderParameterBindings->
// 			getShaderParameterBindings();
// 
// 		ShaderParameterBindings::ShaderParameterBindingsMap::const_iterator iter =
// 			shader_parameter_bindings.begin();
// 		ShaderParameterBindings::ShaderParameterBindingsMap::const_iterator end =
// 			shader_parameter_bindings.end();
// 
// 		for (; iter != end; ++iter)
// 		{
// 			const CEGUI::ShaderParameter* parameter = iter->second;
// 			const ShaderParamType parameterType = parameter->getType();
// 
// 			std::map<int, size_t>::const_iterator find_iter = d_paramTypeToIndex.
// 				find(static_cast<int>(parameterType));
// 
// 			if (find_iter == d_paramTypeToIndex.end())
// 			{
// 				std::string errorMessage = std::string("Unknown variable name: \"") +
// 					iter->first + "\"";
// 				throw RendererException(errorMessage);
// 			}
// 
// 			size_t target_index = find_iter->second;
// 
// 			switch (parameterType)
// 			{
// 			case ShaderParamType::Texture:
// 			{
// 				const CEGUI::ShaderParameterTexture* parameterTexture =
// 					static_cast<const CEGUI::ShaderParameterTexture*>(parameter);
// 
// 				const CEGUI::OgreTexture* texture = static_cast<const
// 					CEGUI::OgreTexture*>(parameterTexture->d_parameterValue);
// 
// 				Ogre::TexturePtr actual_texture = texture->getOgreTexture();
// 
// 				if (actual_texture.isNull())
// 				{
// 					throw RendererException("Ogre texture ptr is empty");
// 				}
// 
// #ifdef CEGUI_USE_OGRE_HLMS
// 				d_renderSystem._setTexture(0, true, actual_texture.get());
// #else
// 				d_renderSystem._setTexture(0, true, actual_texture);
// #endif //CEGUI_USE_OGRE_HLMS
// 
// 				d_owner.initialiseTextureStates();
// 
// 				break;
// 			}
// 			case ShaderParamType::Matrix4X4:
// 			{
// 				// This is the "modelViewProjMatrix"
// 				const CEGUI::ShaderParameterMatrix* mat = static_cast<const
// 					CEGUI::ShaderParameterMatrix*>(parameter);
// 
// 				if (d_lastMatrix != mat->d_parameterValue)
// 				{
// 					d_vertexParameters->_writeRawConstants(target_index,
// 						glm::value_ptr(mat->d_parameterValue),
// 						16);
// 					d_lastMatrix = mat->d_parameterValue;
// 				}
// 				break;
// 			}
// 			case ShaderParamType::Float:
// 			{
// 				// This is the alpha value
// 				const CEGUI::ShaderParameterFloat* new_alpha = static_cast<const
// 					CEGUI::ShaderParameterFloat*>(parameter);
// 
// 				if (d_previousAlpha != new_alpha->d_parameterValue)
// 				{
// 					d_previousAlpha = new_alpha->d_parameterValue;
// 
// 					d_pixelParameters->_writeRawConstants(target_index,
// 						&d_previousAlpha, 1);
// 				}
// 
// 				break;
// 			}
// 			default:
// 				throw RendererException("Invalid parameter type");
// 			}
// 		}
// #ifdef CEGUI_USE_OGRE_HLMS
// 
// 		// The PSO needs to be bound before we can bind shader parameters
// 		d_owner.bindPSO(d_renderOp);
// 
// #endif //CEGUI_USE_OGRE_HLMS
// 
// 
// 		// Pass the finalized parameters to Ogre
// 		d_renderSystem.bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM,
// 			d_vertexParameters, Ogre::GPV_ALL);
// 		d_renderSystem.bindGpuProgramParameters(Ogre::GPT_FRAGMENT_PROGRAM,
// 			d_pixelParameters, Ogre::GPV_ALL);
	}
}