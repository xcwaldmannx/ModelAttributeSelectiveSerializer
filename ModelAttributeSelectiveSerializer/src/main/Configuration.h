#pragma once

#include <cstdint>

namespace mass
{

	enum class TypeSize
	{
		NO_TYPE = 0x00,
		VEC1    = 0x01,
		VEC2    = 0x02,
		VEC3    = 0x03,
		VEC4    = 0x04,
	};

	struct VertexStride
	{
		uint32_t size() const
		{
			return
				(static_cast<uint32_t>(mPosition) * 4) +
				(static_cast<uint32_t>(mNormal)   * 4) +
				(static_cast<uint32_t>(mColor)    * 4) +
				(static_cast<uint32_t>(mTexCoord) * 4);
		}

		TypeSize mPosition = TypeSize::NO_TYPE;
		TypeSize mNormal   = TypeSize::NO_TYPE;
		TypeSize mColor    = TypeSize::NO_TYPE;
		TypeSize mTexCoord = TypeSize::NO_TYPE;
	};

	struct Configuration
	{
		VertexStride mVertexStride{};

		bool mHasNormals    = false;
		bool mHasColors     = false;
		bool mHasTexCoords  = false;
		bool mHasTransforms = false;
		bool mHasAnimations = false;
	};

}
