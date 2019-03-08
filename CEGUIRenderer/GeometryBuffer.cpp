#include "GeometryBuffer.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace CEGUI
{
	Urho3DGeometryBuffer(Urho3D::Renderer& owner, CEGUI::RefCounted<RenderMaterial> renderMaterial);
	virtual ~Urho3DGeometryBuffer()
	{

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

	void Urho3DGeometryBuffer::finaliseVertexAttributes()
	{

	}
}