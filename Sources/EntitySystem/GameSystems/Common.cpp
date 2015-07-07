#include "../../Eagle.h"

namespace ProjectEagle
{
	WidthHeightComponent::WidthHeightComponent()
	{
		componentTypeID = WIDTHHEIGHT_COMPONENT_TYPE_ID;

		width = 0;
		height = 0;
	}

	LifeSpanComponent::LifeSpanComponent()
	{
		componentTypeID = LIFESPAN_COMPONENT_TYPE_ID;

		length = 1;
	}

	LifeSystem::LifeSystem()
	{
	}

	void LifeSystem::update(float frameTime)
	{
		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			LifeSpanComponent *lifeSpanComponent = (LifeSpanComponent *)getComponent(&listIterator->second, LIFESPAN_COMPONENT_TYPE_ID);

			if(lifeSpanComponent)
			{
				lifeSpanComponent->length -= frameTime;

				if(lifeSpanComponent->length < 0)
				{
					entitySystem.removeEntity((listIterator)->first);
				}
			}
		}
	}
};