#pragma once

enum class TypeSize
{
	DEFAULT = 0x0000,
	UINT8   = 0x0001,
	UINT16  = 0x0002,
	UINT32  = 0x0003,
	UINT64  = 0x0004,
};

struct Configuration
{
	TypeSize mSizePosition;
	TypeSize mSizeNormal;
	TypeSize mSizeIndex;
	TypeSize mSizeColor;

	bool mHasNormals;
	bool mHasColors;
	bool mHasTexCoords;
	bool mHasTransforms;
	bool mHasAnimations;
};