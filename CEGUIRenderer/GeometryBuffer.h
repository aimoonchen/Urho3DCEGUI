#pragma once
#include "CEGUI/GeometryBuffer.h"
#include "Renderer.h"
#include "ShaderWrapper.h"
#include "CEGUI/Rectf.h"

#include "Urho3D/Urho3D.h"
#include "Urho3D/Container/Ptr.h"

namespace Urho3D
{
	class Renderer;
	class VertexBuffer;
}
namespace CEGUI
{
	class URHO3D_API Urho3DGeometryBuffer : public GeometryBuffer
	{
	public:
		enum MANUALOBJECT_TYPE
		{
			MT_COLOURED,
			MT_TEXTURED,
			MT_INVALID
		};

		Urho3DGeometryBuffer(Urho3DRenderer& owner, Urho3D::Renderer& rs, CEGUI::RefCounted<RenderMaterial> renderMaterial);
		virtual ~Urho3DGeometryBuffer();

		// Overrides of virtual and abstract methods from GeometryBuffer
		void draw() const override;
		void appendGeometry(const float* vertex_data, std::size_t array_size) override;
		void reset() override;

		void finaliseVertexAttributes(MANUALOBJECT_TYPE type);
	protected:
		//
		void updateMatrix() const;

		Urho3DRenderer& d_owner;
		Urho3D::Renderer& d_renderSystem;
		//! model matrix cache
		mutable glm::mat4 d_matrix;

		Urho3D::SharedPtr<Urho3D::VertexBuffer> d_vertex_buffer{ nullptr };
	};
}