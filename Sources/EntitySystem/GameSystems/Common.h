#ifndef _COMMON_H
#define _COMMON_H 1

#include "../../Eagle.h"

#define WIDTHHEIGHT_COMPONENT_TYPE_ID 00
#define LIFESPAN_COMPONENT_TYPE_ID 01

namespace ProjectEagle
{
	class WidthHeightComponent : public GameComponent
	{
	public:
		WidthHeightComponent();

		float width;
		float height;
	};

	class LifeSpanComponent : public GameComponent
	{
	public:
		LifeSpanComponent();

		float length;
	};

	class LifeSystem : public GameSystem
	{
	public:
		LifeSystem();

		virtual void update(float frameTime);
	};
};

#endif