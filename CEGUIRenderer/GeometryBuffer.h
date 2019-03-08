#pragma once
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/RendererModules/Ogre/Renderer.h"
#include "CEGUI/RendererModules/Ogre/ShaderWrapper.h"
#include "CEGUI/Rectf.h"

#include "Urho3D/Urho3D.h"

namespace Urho3D
{
	class Renderer;
}
namespace CEGUI
{
	class URHO3D_API Urho3DGeometryBuffer : public GeometryBuffer
	{
	public:
		Urho3DGeometryBuffer(Urho3D::Renderer& owner, CEGUI::RefCounted<RenderMaterial> renderMaterial);
		virtual ~Urho3DGeometryBuffer();

		// Overrides of virtual and abstract methods from GeometryBuffer
		void draw() const override;
		void appendGeometry(const float* vertex_data, std::size_t array_size) override;
		void reset() override;

		void finaliseVertexAttributes();
	protected:
		//
		void updateMatrix() const;

		Urho3D::Renderer& owner_;

		//! model matrix cache
		mutable glm::mat4 d_matrix;
	};
}