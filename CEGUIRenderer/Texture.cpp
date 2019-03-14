#include "Texture.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/System.h"
#include "ImageCodec.h"

#include "Urho3D/Container/Str.h"
#include "Urho3D/Graphics/Texture.h"

#include <cstdint>

namespace CEGUI
{
	static size_t calculateDataSize(const Sizef size, Texture::PixelFormat fmt)
	{
		switch (fmt)
		{
		case Texture::PixelFormat::Rgba:
			return static_cast<size_t>(size.d_width * size.d_height * 4);

		case Texture::PixelFormat::Rgb:
			return static_cast<size_t>(size.d_width * size.d_height * 3);

		case Texture::PixelFormat::Rgb565:
		case Texture::PixelFormat::Rgba4444:
			return static_cast<size_t>(size.d_width * size.d_height * 2);

		case Texture::PixelFormat::Pvrtc2:
			return (static_cast<size_t>(size.d_width * size.d_height) * 2 + 7) / 8;

		case Texture::PixelFormat::Pvrtc4:
			return (static_cast<size_t>(size.d_width * size.d_height) * 4 + 7) / 8;

		case Texture::PixelFormat::RgbaDxt1:
			return static_cast<size_t>(std::ceil(size.d_width / 4) * std::ceil(size.d_height / 4) * 8);

		case Texture::PixelFormat::RgbaDxt3:
		case Texture::PixelFormat::RgbaDxt5:
			return static_cast<size_t>(std::ceil(size.d_width / 4) * std::ceil(size.d_height / 4) * 16);

		default:
			return 0;
		}
	}

	//----------------------------------------------------------------------------//
	std::uint32_t Urho3DTexture::d_textureNumber = 0;

	//----------------------------------------------------------------------------//
	Urho3DTexture::Urho3DTexture(const String& name) :
		d_isLinked(false),
		d_size(0, 0),
		d_dataSize(0, 0),
		d_texelScaling(0, 0),
		d_name(name)
	{
		createEmptyOgreTexture(Texture::PixelFormat::Rgba);
	}

	//----------------------------------------------------------------------------//
	Urho3DTexture::Urho3DTexture(const String& name, const String& filename,
		const String& resourceGroup) :
		d_isLinked(false),
		d_size(0, 0),
		d_dataSize(0, 0),
		d_texelScaling(0, 0),
		d_name(name)
	{
		loadFromFile(filename, resourceGroup);
	}

	//----------------------------------------------------------------------------//
	Urho3DTexture::Urho3DTexture(const String& name, const Sizef& sz) :
		d_isLinked(false),
		d_size(0, 0),
		d_dataSize(0, 0),
		d_texelScaling(0, 0),
		d_name(name)
	{
// 		d_texture = Ogre::TextureManager::getSingleton().createManual(
// 			getUniqueName(), "General", Ogre::TEX_TYPE_2D,
// 			sz.d_width, sz.d_height, 0,
// 			Ogre::PF_A8B8G8R8);
// 
// 		// throw exception if no texture was able to be created
// 		if (d_texture.isNull())
// 			throw RendererException(
// 				"Failed to create Texture object with spcecified size.");
// 
// 		d_size.d_width = static_cast<float>(d_texture->getWidth());
// 		d_size.d_height = static_cast<float>(d_texture->getHeight());
// 		d_dataSize = sz;
// 		updateCachedScaleValues();
	}

	//----------------------------------------------------------------------------//
	Urho3DTexture::Urho3DTexture(const String& name, Urho3D::SharedPtr<Urho3D::Texture>& tex,
		bool take_ownership) :
		d_isLinked(false),
		d_size(0, 0),
		d_dataSize(0, 0),
		d_texelScaling(0, 0),
		d_name(name)
	{
		setUrho3DTexture(tex, take_ownership);
	}

	//----------------------------------------------------------------------------//
	Urho3DTexture::~Urho3DTexture()
	{
		freeOgreTexture();
	}

	//----------------------------------------------------------------------------//
	const String& Urho3DTexture::getName() const
	{
		return d_name;
	}

	//----------------------------------------------------------------------------//
	const Sizef& Urho3DTexture::getSize() const
	{
		return d_size;
	}

	//----------------------------------------------------------------------------//
	const Sizef& Urho3DTexture::getOriginalDataSize() const
	{
		return d_dataSize;
	}

	//----------------------------------------------------------------------------//
	const glm::vec2& Urho3DTexture::getTexelScaling() const
	{
		return d_texelScaling;
	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::loadFromFile(const String& filename,
		const String& resourceGroup)
	{
		// get and check existence of CEGUI::System object
		System* sys = System::getSingletonPtr();
		if (!sys)
			throw RendererException(
				"CEGUI::System object has not been created!");

		// load file to memory via resource provider
		RawDataContainer texFile;
		sys->getResourceProvider()->loadRawDataContainer(filename, texFile,
			resourceGroup);

		ImageCodec& ic(sys->getImageCodec());

		// if we're using the integrated Ogre codec, set the file-type hint string
		if (ic.getIdentifierString().substr(0, 14) == "OgreImageCodec")
		{
			String type;
			String::size_type i = filename.find_last_of(".");
			if (i != String::npos && filename.length() - i > 1)
				type = filename.substr(i + 1);
			static_cast<Urho3DImageCodec&>(ic).setImageFileDataType(type);
		}

		Texture* res = sys->getImageCodec().load(texFile, this);

		// unload file data buffer
		sys->getResourceProvider()->unloadRawDataContainer(texFile);

		// throw exception if data was load loaded to texture.
		if (!res)
			throw RendererException(
				sys->getImageCodec().getIdentifierString() +
				" failed to load image '" + filename + "'.");
	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::loadFromMemory(const void* buffer, const Sizef& buffer_size,
		PixelFormat pixel_format)
	{
// 		using namespace Urho3D;
// 
// 		if (!isPixelFormatSupported(pixel_format))
// 			throw InvalidRequestException(
// 				"Data was supplied in an unsupported pixel format.");
// 
// 		const size_t byte_size = calculateDataSize(buffer_size, pixel_format);
// 
// 		char* bufferCopy = new char[byte_size];
// 		memcpy(bufferCopy, buffer, byte_size);
// 
// 		const Ogre::PixelBox* pixelBox = new Ogre::PixelBox(static_cast<std::uint32_t>(buffer_size.d_width), static_cast<std::uint32_t>(buffer_size.d_height),
// 			1, toOgrePixelFormat(pixel_format), bufferCopy);
// 		createEmptyOgreTexture(pixel_format);
// 		d_texture->freeInternalResources();
// 		d_texture->setWidth(static_cast<std::uint32_t>(buffer_size.d_width));
// 		d_texture->setHeight(static_cast<std::uint32_t>(buffer_size.d_height));
// 		d_texture->setDepth(1);
// 		d_texture->createInternalResources();
// 		d_texture->getBuffer(0, 0).get()->blitFromMemory(*pixelBox);
// 
// 		// throw exception if no texture was able to be created
// 		if (d_texture.isNull())
// 			throw RendererException(
// 				"Failed to blit to Texture from memory.");
// 
// 		d_size.d_width = static_cast<float>(d_texture->getWidth());
// 		d_size.d_height = static_cast<float>(d_texture->getHeight());
// 		d_dataSize = buffer_size;
// 		updateCachedScaleValues();
	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::blitFromMemory(const void* sourceData, const Rectf& area)
	{
// 		if (d_texture.isNull()) // TODO: exception?
// 			return;
// 
// 		// Ogre doesn't like null data, so skip if the sourceData is null and
// 		// area is zero size
// 		if (sourceData == nullptr)
// 		{
// 			if (static_cast<int>(area.getWidth()) == 0 &&
// 				static_cast<int>(area.getHeight()) == 0)
// 			{
// 				return;
// 			}
// 
// 			// Here we are trying to write to a non-zero size area with null
// 			// ptr for data
// 			throw RendererException("blitFromMemory source is null");
// 		}
// 
// 
// 		// NOTE: const_cast because Ogre takes pointer to non-const here. Rather
// 		// than allow that to dictate poor choices in our own APIs, we choose to
// 		// address the issue as close to the source of the problem as possible.
// 		Ogre::PixelBox pb(static_cast<Ogre::uint32>(area.getWidth()),
// 			static_cast<Ogre::uint32>(area.getHeight()),
// 			1,
// 			d_texture->getFormat(), const_cast<void*>(sourceData));
// 
// 		Ogre::Image::Box box(static_cast<Ogre::uint32>(area.left()),
// 			static_cast<Ogre::uint32>(area.top()),
// 			static_cast<Ogre::uint32>(area.right()),
// 			static_cast<Ogre::uint32>(area.bottom()));
// 		d_texture->getBuffer()->blitFromMemory(pb, box);
	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::blitToMemory(void* targetData)
	{
// 		if (d_texture.isNull()) // TODO: exception?
// 			return;
// 
// 		Ogre::PixelBox pb(static_cast<std::uint32_t>(d_size.d_width), static_cast<std::uint32_t>(d_size.d_height),
// 			1, d_texture->getFormat(), targetData);
// 		d_texture->getBuffer()->blitToMemory(pb);
	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::freeOgreTexture()
	{
// 		if (!d_texture.isNull() && !d_isLinked)
// 			Ogre::TextureManager::getSingleton().remove(d_texture->getHandle());
// 
// 		d_texture.setNull();
	}

	//----------------------------------------------------------------------------//
	Urho3D::String Urho3DTexture::getUniqueName()
	{
// #if OGRE_VERSION < 0x10A00
// 		Ogre::StringUtil::StrStreamType strstream;
// 		strstream << "_cegui_ogre_" << d_textureNumber++;
// 
// 		return strstream.str();
// #endif
// #if OGRE_VERSION >= 0x10A00
// 		Ogre::StringStream strstream;
// 		strstream << "_cegui_ogre_" << d_textureNumber++;
// 
// 		return strstream.str();
// #endif   
		Urho3D::String name;
		name.AppendWithFormat("_cegui_ogre_%d", d_textureNumber++);
		return name;
	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::updateCachedScaleValues()
	{
		//
		// calculate what to use for x scale
		//
		const float orgW = d_dataSize.d_width;
		const float texW = d_size.d_width;

		// if texture and original data width are the same, scale is based
		// on the original size.
		// if texture is wider (and source data was not stretched), scale
		// is based on the size of the resulting texture.
		d_texelScaling.x = 1.0f / ((orgW == texW) ? orgW : texW);

		//
		// calculate what to use for y scale
		//
		const float orgH = d_dataSize.d_height;
		const float texH = d_size.d_height;

		// if texture and original data height are the same, scale is based
		// on the original size.
		// if texture is taller (and source data was not stretched), scale
		// is based on the size of the resulting texture.
		d_texelScaling.y = 1.0f / ((orgH == texH) ? orgH : texH);
	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::setUrho3DTexture(Urho3D::SharedPtr<Urho3D::Texture> texture, bool take_ownership)
	{
// 		freeOgreTexture();
// 
// 		d_texture = texture;
// 		d_isLinked = !take_ownership;
// 
// 		if (!d_texture.isNull())
// 		{
// 			d_size.d_width = static_cast<float>(d_texture->getWidth());
// 			d_size.d_height = static_cast<float>(d_texture->getHeight());
// 			d_dataSize = d_size;
// 		}
// 		else
// 			d_size = d_dataSize = Sizef(0, 0);
// 
// 		updateCachedScaleValues();
	}

	//----------------------------------------------------------------------------//
	Urho3D::SharedPtr<Urho3D::Texture> Urho3DTexture::getUrho3DTexture() const
	{
		return d_texture;
	}

	//----------------------------------------------------------------------------//
	bool Urho3DTexture::isPixelFormatSupported(const PixelFormat fmt) const
	{
		return false;
// 		try
// 		{
// 			return Ogre::TextureManager::getSingleton().
// 				isEquivalentFormatSupported(Ogre::TEX_TYPE_2D,
// 					toOgrePixelFormat(fmt),
// 					Ogre::TU_DEFAULT);
// 		}
// 		catch (InvalidRequestException&)
// 		{
// 			return false;
// 		}
	}

// 	//----------------------------------------------------------------------------//
// 	Ogre::PixelFormat Urho3DTexture::toOgrePixelFormat(const Texture::PixelFormat fmt)
// 	{
// 		switch (fmt)
// 		{
// 		case Texture::PixelFormat::Rgba:       return Ogre::PF_A8B8G8R8;
// 		case Texture::PixelFormat::Rgb:        return Ogre::PF_B8G8R8;
// 		case Texture::PixelFormat::Rgb565:    return Ogre::PF_R5G6B5;
// 		case Texture::PixelFormat::Rgba4444:  return Ogre::PF_A4R4G4B4;
// 		case Texture::PixelFormat::Pvrtc2:     return Ogre::PF_PVRTC_RGBA2;
// 		case Texture::PixelFormat::Pvrtc4:     return Ogre::PF_PVRTC_RGBA4;
// 		case Texture::PixelFormat::RgbaDxt1:  return Ogre::PF_DXT1;
// 		case Texture::PixelFormat::RgbaDxt3:  return Ogre::PF_DXT3;
// 		case Texture::PixelFormat::RgbaDxt5:  return Ogre::PF_DXT5;
// 
// 		default:
// 			throw InvalidRequestException(
// 				"Invalid pixel format translation.");
// 		}
// 	}
// 
// 	//----------------------------------------------------------------------------//
// 	Texture::PixelFormat Urho3DTexture::fromOgrePixelFormat(
// 		const Ogre::PixelFormat fmt)
// 	{
// 		switch (fmt)
// 		{
// 		case Ogre::PF_A8R8G8B8:     return Texture::PixelFormat::Rgba;
// 		case Ogre::PF_A8B8G8R8:     return Texture::PixelFormat::Rgba;
// 		case Ogre::PF_R8G8B8:       return Texture::PixelFormat::Rgb;
// 		case Ogre::PF_B8G8R8:       return Texture::PixelFormat::Rgb;
// 		case Ogre::PF_R5G6B5:       return Texture::PixelFormat::Rgb565;
// 		case Ogre::PF_A4R4G4B4:     return Texture::PixelFormat::Rgba4444;
// 		case Ogre::PF_PVRTC_RGBA2:  return Texture::PixelFormat::Pvrtc2;
// 		case Ogre::PF_PVRTC_RGBA4:  return Texture::PixelFormat::Pvrtc4;
// 		case Ogre::PF_DXT1:         return Texture::PixelFormat::RgbaDxt1;
// 		case Ogre::PF_DXT3:         return Texture::PixelFormat::RgbaDxt3;
// 		case Ogre::PF_DXT5:         return Texture::PixelFormat::RgbaDxt5;
// 
// 		default:
// 			throw InvalidRequestException(
// 				"Invalid pixel format translation.");
// 		}
// 	}

	//----------------------------------------------------------------------------//
	void Urho3DTexture::createEmptyOgreTexture(PixelFormat pixel_format)
	{
		// try to create a Ogre::Texture with given dimensions
// 		d_texture = Ogre::TextureManager::getSingleton().createManual(
// 			getUniqueName(), "General", Ogre::TEX_TYPE_2D,
// 			1, 1, 0,
// 			toOgrePixelFormat(pixel_format));
	}
}