#include "GeometryBuffer.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace CEGUI
{
	Urho3DGeometryBuffer::Urho3DGeometryBuffer(Urho3DRenderer& owner,
		Urho3D::Graphics& rs, CEGUI::RefCounted<RenderMaterial> renderMaterial) :
		GeometryBuffer(renderMaterial),
		d_owner(owner),
		d_renderSystem(rs),
		d_matrix(1.0)
	{

	}

	//----------------------------------------------------------------------------//
	Urho3DGeometryBuffer::~Urho3DGeometryBuffer()
	{
		//cleanUpVertexAttributes();
	}

	void Urho3DGeometryBuffer::draw() const
	{

	}

	void Urho3DGeometryBuffer::appendGeometry(const float* vertex_data, std::size_t array_size)
	{

	}

	void Urho3DGeometryBuffer::reset()
	{

	}

	void Urho3DGeometryBuffer::finaliseVertexAttributes(MANUALOBJECT_TYPE type)
	{

	}

	void Urho3DGeometryBuffer::updateMatrix() const
	{

	}
}