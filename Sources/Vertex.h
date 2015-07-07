#ifndef _VERTEX_H
#define _VERTEX_H 1

#include "Eagle.h"

#define VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define TRANSFORMED_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define PARTICLE_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define TRANSFORMED_PARTICLE_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

namespace ProjectEagle
{
	struct ColorValue
	{
		float r, g, b, a;

		ColorValue();
		ColorValue(float r, float g, float b, float a);
		ColorValue(float r, float g, float b);
		ColorValue(DWORD color);

		void set(float r, float g, float b, float a);
		void set(float r, float g, float b);

		//DWORD getDWORD();

		ColorValue& operator = (const DWORD &c);
	};

	struct Vertex
	{
		float x, y, z;
		ColorValue color;
		float tu, tv;
	};

	struct ParticleVertex
	{
		float x, y, z;
		ColorValue color;
	};
};

#endif