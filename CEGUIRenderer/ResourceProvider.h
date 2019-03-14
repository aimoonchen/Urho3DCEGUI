#pragma once
#include "CEGUI/ResourceProvider.h"

#include "Urho3D/Urho3D.h"

namespace CEGUI
{
	class URHO3D_API Urho3DResourceProvider : public ResourceProvider
	{
	public:
		Urho3DResourceProvider();

		void loadRawDataContainer(const String& filename, RawDataContainer& output, const String& resourceGroup);
		void unloadRawDataContainer(RawDataContainer& output);
		size_t getResourceGroupFileNames(std::vector<String>& out_vec, const String& file_pattern, const String& resource_group);
	};
}