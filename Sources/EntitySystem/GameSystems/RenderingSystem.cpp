#include "../../Eagle.h"

namespace ProjectEagle
{
	RenderComponent::RenderComponent()
	{
		componentTypeID = RENDER_COMPONENT_TYPE_ID;
	}

	TransformationComponent::TransformationComponent()
	{
		componentTypeID = TRANSFORMATION_COMPONENT_TYPE_ID;

		scale.set(1, 1);

		rotation.set(0, 0, 0);
		center.set(0, 0, 0);
	}

	ColorComponent::ColorComponent()
	{
		componentTypeID = COLOR_COMPONENT_TYPE_ID;

		color;
	}

	TextureComponent::TextureComponent()
	{
		componentTypeID = TEXTURE_COMPONENT_TYPE_ID;

		texture = 0;

		sourcePosition.set(0, 0);
		sourceDimensions.set(-1, -1);
	}

	void TextureComponent::loadTexture(string addressOnDisk)
	{
		texture = resourceManager.loadTexture((char *)addressOnDisk.c_str());

		sourceDimensions.set(texture->getWidth(), texture->getHeight());
	}

	SpriteSheetAnimationComponent::SpriteSheetAnimationComponent()
	{
		componentTypeID = SPRITESHEETANIMATION_COMPONENT_TYPE_ID;

		currentFrame = 0;
		currentRow = 0;

		frameCount = 1;
		animationColumnCount = 1;

		animationDirection = 1;

		frameInterval = INFINITE;
		passedTime = 0.0f;
	}

	ParticleEmitterComponent::ParticleEmitterComponent()
	{
		componentTypeID = PARTICLEEMITTER_COMPONENT_TYPE_ID;

		maxParticleCount = 100;

		particleCount = 0;
		aliveParticleCount = 0;

		particleList = new Particle[maxParticleCount];

		particleVertexList = new ParticleVertex[maxParticleCount];

		inactiveParticleList = new int[maxParticleCount];
		inactiveParticleListTopIndex = 0;

		maximumDistance = 500;
		direction = 0;
		minimumAlpha = 50;
		maximumAlpha = 255;
		minimumRed = 0;
		maximumRed = 255;
		minimumGreen = 0;
		maximumGreen = 255;
		minimumBlue = 0;
		maximumBlue = 255;
		spread = 2 * pi;
		velocity = 50.0f;
		scale = 1.0f;
		addTimer.reset();
		addDelay = 1;
		particleLifespan = 10000;
		alphaDecrement = 50;
		removalAlphaDecrement = 1;
		sourceLineLength = 1;
		sourceLineDirection = 0;
		noise = 0.0;
		frictionFactor = 1.0;
		acceleration = Vector2(0, 0);
		particlesPerAdd = 1;
		maximumParticleCreationDistance = 1;
		lastPosition = position;

		texture = 0;
	}

	ParticleEmitterComponent::~ParticleEmitterComponent()
	{
		/*delete[](particleList);
		delete[](particleVertexList);
		delete[](transformedParticleVertexList);
		delete[](inactiveParticleList);*/
	}

	void ParticleEmitterComponent::releaseData()
	{
		delete[](particleList);
		delete[](particleVertexList);
		delete[](inactiveParticleList);

		particleCount = 0;
	}

	void ParticleEmitterComponent::operator = (const ParticleEmitterComponent &p)
	{
		maxParticleCount = p.maxParticleCount;

		particleCount = p.particleCount;
		aliveParticleCount = p.aliveParticleCount;

		setMax(p.maxParticleCount);

		inactiveParticleListTopIndex = p.inactiveParticleListTopIndex;

		for(int i = 0; i < maxParticleCount; ++i)
		{
			particleList[i] = p.particleList[i];

			particleVertexList[i] = p.particleVertexList[i];

			inactiveParticleList[i] = p.inactiveParticleList[i];
		}

		maximumDistance = p.maximumDistance;
		direction = p.direction;
		minimumAlpha = p.minimumAlpha;
		maximumAlpha = p.maximumAlpha;
		minimumRed = p.minimumRed;
		maximumRed = p.maximumRed;
		minimumGreen = p.minimumGreen;
		maximumGreen = p.maximumGreen;
		minimumBlue = p.minimumBlue;
		maximumBlue = p.maximumBlue;
		spread = p.spread;
		velocity = p.velocity;
		scale = p.scale;
		addTimer.reset();
		addDelay = p.addDelay;
		particleLifespan = p.particleLifespan;
		alphaDecrement = p.alphaDecrement;
		removalAlphaDecrement = p.removalAlphaDecrement;
		sourceLineLength = p.sourceLineLength;
		sourceLineDirection = p.sourceLineDirection;
		noise = p.noise;
		frictionFactor = p.frictionFactor;
		acceleration = p.acceleration;
		particlesPerAdd = p.particlesPerAdd;
		maximumParticleCreationDistance = 1;

		position = p.position;
		lastPosition = p.lastPosition;

		texture = p.texture;
	}

	void ParticleEmitterComponent::reset()
	{
		particleCount = 0;

		delete[](particleList);
		particleList = new Particle[maxParticleCount];

		delete[](particleVertexList);
		particleVertexList = new ParticleVertex[maxParticleCount];

		delete[](inactiveParticleList);
		inactiveParticleList = new int[maxParticleCount];

		inactiveParticleListTopIndex = 0;
	}

	void ParticleEmitterComponent::resetParticle(int index, Vector3 offsetPosition)
	{
		if(index < 0) return;

		particleList[index].enabled = 1;
		float variation = (rand() % (int)((spread + 0.01) * 100.0)) / 100.0;
		float dir = direction;
		float dir2 = sourceLineDirection;

		float vx = cos(dir - spread / 2 + variation); // * speedVar;
		float vy = sin(dir - spread / 2 + variation); // * speedVar;

		int randomPointDistance = 0;

		if(moveLength) randomPointDistance = rand() % moveLength;

		Vector3 moveVector = moveNormal * randomPointDistance;

		int dist = math.random(-(sourceLineLength / 2), sourceLineLength / 2);
		particleList[index].x = position.x + cos(dir2 + pi / 2) * dist + moveVector.x - maximumParticleCreationDistance + rand() % (maximumParticleCreationDistance * 2) + offsetPosition.x;
		particleList[index].y = position.y + sin(dir2 + pi / 2) * dist + moveVector.y - maximumParticleCreationDistance + rand() % (maximumParticleCreationDistance * 2) + offsetPosition.y;
		particleList[index].z = -position.z + moveVector.z + offsetPosition.z;

		particleList[index].velocityX = vx * velocity;
		particleList[index].velocityY = vy * velocity;

		int r = rand() % (int)(maximumRed - minimumRed + 1) + minimumRed;
		int g = rand() % (int)(maximumGreen - minimumGreen + 1) + minimumGreen;
		int b = rand() % (int)(maximumBlue - minimumBlue + 1) + minimumBlue;
		int a = rand() % (int)(maximumAlpha - minimumAlpha + 1) + minimumAlpha;
		particleList[index].color.r = r;
		particleList[index].color.g = g;
		particleList[index].color.b = b;
		particleList[index].color.a = a;

		particleList[index].quickFade = 0;

		particleList[index].creationTime = eagle.getTimer()->getPassedTimeMilliseconds();

		particleList[index].accelerationX = acceleration.x;
		particleList[index].accelerationY = acceleration.y;
	}

	void ParticleEmitterComponent::add(Vector3 offsetPosition)
	{
		if(inactiveParticleListTopIndex > 0)
		{
			resetParticle(inactiveParticleList[inactiveParticleListTopIndex], offsetPosition);

			inactiveParticleListTopIndex--;

			return;
		}

		if(particleCount < maxParticleCount)
		{
			resetParticle(particleCount, offsetPosition);

			particleCount++;
		}
		else particleList[rand() % particleCount].quickFade = 1;
	}

	void ParticleEmitterComponent::setMax(int value)
	{
		maxParticleCount = value;
		if(particleCount > value) particleCount = value;

		delete[](particleList);
		particleList = new Particle[value];

		free(particleVertexList);
		particleVertexList = new ParticleVertex[value];

		delete[](inactiveParticleList);
		inactiveParticleList = new int[value];

		inactiveParticleListTopIndex = 0;
	}

	void ParticleEmitterComponent::setColorRange(int r0, int g0, int b0, int a0, int r1, int g1, int b1, int a1)
	{
		minimumRed = r0;
		maximumRed = r1;
		minimumGreen = g0; 
		maximumGreen = g1;
		minimumBlue = b0;
		maximumBlue = b1;
		minimumAlpha = a0;
		maximumAlpha = a1;
	}

#ifndef PLATFORM_WP8
	void ParallelParticleEmitterUpdate::operator() (const tbb::blocked_range<int>& r) const
	{
		DWORD currentTime = eagle.getTimer()->getPassedTimeMilliseconds();

		float frictionEffect = 1.0f - (1.0f - particleEmitter->frictionFactor) * (frameTime);
		float aliveParticleAlphaDecrement = particleEmitter->alphaDecrement * (frameTime);
		float deadParticleAlphaDecrement = (particleEmitter->alphaDecrement + particleEmitter->removalAlphaDecrement) * (frameTime);

		for(int i = r.begin(); i != r.end(); ++i)
		{
			if(!particleList[i].enabled) continue;

			particleEmitter->aliveParticleCount++;

			//float lv = sqrt(particleList[i].speedX * particleList[i].speedX + particleList[i].speedY * particleList[i].speedY);
			/*float theta = atan2(particleList[i].speedY, particleList[i].speedX) + ((float)(math.random(0, 2) - 1)) * particleEmitter->noise;
			particleList[i].speedX = cos(theta) * lv;
			particleList[i].speedY = sin(theta) * lv;*/

			particleList[i].velocityX += ((float)(math.random(0, 2) - 1)) * particleEmitter->noise;
			particleList[i].velocityY += ((float)(math.random(0, 2) - 1)) * particleEmitter->noise;

			particleList[i].x += particleList[i].velocityX * (frameTime);
			particleList[i].y += particleList[i].velocityY * (frameTime);

			particleList[i].velocityX *= frictionEffect;
			particleList[i].velocityY *= frictionEffect;
			//particleList[i].z += particleList[i].speedZ;

			particleList[i].velocityX += particleList[i].accelerationX * (frameTime);
			particleList[i].velocityY += particleList[i].accelerationY * (frameTime);

			//colors[i].a = math.abs(colors[i].a);

			if(particleList[i].quickFade) particleList[i].color.a -= deadParticleAlphaDecrement;
			else particleList[i].color.a -= aliveParticleAlphaDecrement;

			//if((particleList[i].quickFade && particleList[i].color.a < 10) || particleList[i].color.a < 10)

			if(particleList[i].color.a < EAGLE_PARTICLE_SYSTEM_MINIMUM_PARTICLE_ALPHA)
			{
				particleList[i].enabled = 0;
				particleList[i].color.a = 0;

				particleEmitter->inactiveParticleListTopIndex++;

				particleEmitter->inactiveParticleList[particleEmitter->inactiveParticleListTopIndex] = i;

				continue;
			}
			else if(currentTime - particleList[i].creationTime > particleEmitter->particleLifespan)
			{
				particleList[i].quickFade = 1;

				continue;
			}
			else
			{
				int deltaX = particleList[i].x - particleEmitter->position.x;
				int deltaY = particleList[i].y - particleEmitter->position.y;

				if((deltaX) * (deltaX) + (deltaY) * (deltaY) > particleEmitter->maximumDistance * particleEmitter->maximumDistance)
				{
					particleList[i].quickFade = 1;

					continue;
				}
			}
		}
	}
#endif

	void ParticleEmitterComponent::loadTexture(string addressOnDisk)
	{
		texture = resourceManager.loadTexture((char *)addressOnDisk.c_str());
	}

	AnimationSystem::AnimationSystem()
	{
	}

	void AnimationSystem::update(float frameTime)
	{
		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			TextureComponent *textureComponent = (TextureComponent *)getComponent(&listIterator->second, TEXTURE_COMPONENT_TYPE_ID);
			SpriteSheetAnimationComponent *spriteSheetAnimationComponent = (SpriteSheetAnimationComponent *)getComponent(&listIterator->second, SPRITESHEETANIMATION_COMPONENT_TYPE_ID);
			
			if(textureComponent && spriteSheetAnimationComponent)
			{
				if(spriteSheetAnimationComponent->frameInterval > 0)
				{
					spriteSheetAnimationComponent->passedTime += frameTime;

					if(spriteSheetAnimationComponent->passedTime > spriteSheetAnimationComponent->frameInterval)
					{
						spriteSheetAnimationComponent->passedTime = 0.0f;

						spriteSheetAnimationComponent->currentFrame += spriteSheetAnimationComponent->animationDirection;

						if(spriteSheetAnimationComponent->currentFrame < 0) spriteSheetAnimationComponent->currentFrame = spriteSheetAnimationComponent->frameCount - 1;
						if(spriteSheetAnimationComponent->currentFrame > spriteSheetAnimationComponent->frameCount - 1) spriteSheetAnimationComponent->currentFrame = 0;
					}
				}
				else
				{
					spriteSheetAnimationComponent->currentFrame += spriteSheetAnimationComponent->animationDirection;

					if(spriteSheetAnimationComponent->currentFrame < 0) spriteSheetAnimationComponent->currentFrame = spriteSheetAnimationComponent->frameCount - 1;
					if(spriteSheetAnimationComponent->currentFrame > spriteSheetAnimationComponent->frameCount - 1) spriteSheetAnimationComponent->currentFrame = 0;
				}

				int frameX, frameY;
				if(spriteSheetAnimationComponent->animationColumnCount) 
				{
					frameX = (spriteSheetAnimationComponent->currentFrame % spriteSheetAnimationComponent->animationColumnCount) * textureComponent->sourceDimensions.x;
					frameY = ((spriteSheetAnimationComponent->currentFrame / spriteSheetAnimationComponent->animationColumnCount) + spriteSheetAnimationComponent->currentRow) * textureComponent->sourceDimensions.y;
				}
				else
				{
					frameX = 0;
					frameY = 0;
				}

				textureComponent->sourcePosition.set(frameX, frameY);
			}
		}
	}

	RenderingSystem::RenderingSystem()
	{
	}

	void RenderingSystem::update(float frameTime)
	{
		return;

		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			PositionComponent *position = (PositionComponent *)getComponent(&listIterator->second, POSITION_COMPONENT_TYPE_ID);

			if(!position) continue;

			GameComponentList::iterator componentListIterator;

			for(componentListIterator = ((*listIterator).second).begin(); componentListIterator != ((*listIterator).second).end(); ++componentListIterator)
			{
				if((*componentListIterator)->getType() == PARTICLEEMITTER_COMPONENT_TYPE_ID)
				{
					ParticleEmitterComponent *particleEmitter = (ParticleEmitterComponent *)(*componentListIterator);

					particleEmitter->moveAmount = particleEmitter->lastPosition - position->position;
					particleEmitter->moveNormal = particleEmitter->moveAmount.normal();
					particleEmitter->moveLength = particleEmitter->moveAmount.length();

					if(particleEmitter->addTimer.stopWatch(particleEmitter->addDelay))
					{
						if(particleEmitter->particleCount == 0)
						{
							particleEmitter->moveAmount = Vector3(0, 0, 0);
							particleEmitter->moveNormal = Vector3(0, 0, 0);
							particleEmitter->moveLength = 0;
						}

						for(int j = 0; j < particleEmitter->particlesPerAdd; ++j)
						{
							particleEmitter->add(position->position);
						}

						particleEmitter->addTimer.reset();
					}

					particleEmitter->aliveParticleCount = 0;
#ifndef PLATFORM_WP8
					parallel_for(tbb::blocked_range<int>(0, particleEmitter->particleCount), ParallelParticleEmitterUpdate(particleEmitter, particleEmitter->particleCount, particleEmitter->particleList, frameTime), tbb::auto_partitioner());
#endif

					if(position) particleEmitter->lastPosition = position->position;
				}
			}
		}
	}

	void RenderingSystem::render()
	{
		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			RenderComponent *renderComponent = (RenderComponent *)getComponent(&listIterator->second, RENDER_COMPONENT_TYPE_ID);

			if(!renderComponent) continue;

			PositionComponent *positionComponent = (PositionComponent *)getComponent(&listIterator->second, POSITION_COMPONENT_TYPE_ID);

			if(!positionComponent) continue;

			WidthHeightComponent *dimensionsComponent = (WidthHeightComponent *)getComponent(&listIterator->second, WIDTHHEIGHT_COMPONENT_TYPE_ID);

			if(!dimensionsComponent) continue;

			ColorComponent *colorComponent = (ColorComponent *)getComponent(&listIterator->second, COLOR_COMPONENT_TYPE_ID);
			TextureComponent *textureComponent = (TextureComponent *)getComponent(&listIterator->second, TEXTURE_COMPONENT_TYPE_ID);
			TransformationComponent *transformationComponent = (TransformationComponent *)getComponent(&listIterator->second, TRANSFORMATION_COMPONENT_TYPE_ID);

			ColorValue color;

			if(colorComponent) color = colorComponent->color;
			else color = COLOR_RGB(1, 1, 1);

			float frameX;
			float frameY;

			float width;
			float height;

			float textureWidth;
			float textureHeight;

			if(textureComponent && textureComponent->texture)
			{
				frameX = textureComponent->sourcePosition.x;
				frameY = textureComponent->sourcePosition.y;

				if(textureComponent->sourceDimensions.x < 0)
				{
					textureComponent->sourceDimensions.x = textureComponent->texture->getWidth();
					textureComponent->sourceDimensions.y = textureComponent->texture->getHeight();
				}

				width = textureComponent->sourceDimensions.x;
				height = textureComponent->sourceDimensions.y;

				textureWidth = textureComponent->texture->getWidth();
				textureHeight = textureComponent->texture->getHeight();

				graphics.setTexture(textureComponent->texture);
				//d3dDevice9->SetTexture(0, textureComponent->texture->getD3DTexture());
			}
			else
			{
				frameX = 0;
				frameY = 0;

				width = dimensionsComponent->width;
				height = dimensionsComponent->height;

				textureWidth = 0;
				textureHeight = 0;

				//d3dDevice9->SetTexture(0, 0);
				graphics.setTexture(0);
			}

			Vector2 dimensions;

			if(dimensionsComponent) dimensions.set(dimensionsComponent->width, dimensionsComponent->height);
			else dimensions.set(width, height);

			Vector2 scale(1, 1);

			bool isRotated = 0;

			if(transformationComponent && positionComponent)
			{
				isRotated = 1;

				switch(graphics.getGraphicsAPIType())
				{
				case GraphicsAPI_Direct3D11:
					{
						//DirectX::XMMATRIX matrix = DirectX::XMMatrixTransformation2D(

						DirectX::XMMATRIX translationMatrix;

						translationMatrix = DirectX::XMMatrixTranslation(positionComponent->position.x / graphics.getScreenHeight() * 2
							, -positionComponent->position.y / graphics.getScreenHeight() * 2
							, positionComponent->position.z / graphics.getScreenHeight() * 2);

						graphics.setWorldTransform(&translationMatrix);

						break;
					}
				}
			}
			else if(positionComponent)
			{
				switch(graphics.getGraphicsAPIType())
				{
				case GraphicsAPI_Direct3D11:
					{
						DirectX::XMMATRIX translationMatrix;

						translationMatrix = DirectX::XMMatrixTranslation(positionComponent->position.x / graphics.getScreenHeight() * 2
							, -positionComponent->position.y / graphics.getScreenHeight() * 2
							, positionComponent->position.z / graphics.getScreenHeight() * 2);

						graphics.setWorldTransform(&translationMatrix);

						break;
					}
				}
			}
			else
			{
				switch(graphics.getGraphicsAPIType())
				{
				case GraphicsAPI_Direct3D11:
					{
						graphics.setWorldIdentity();

						break;
					}
				}
			}

			Vertex v[4];

			if(!isRotated)
			{
				v[0].x = 0;
				v[0].y = 0;
				v[0].z = 0;
				v[0].color = color;
				//v[0].weight = 1;
				v[0].tu = (frameX) / textureWidth;
				v[0].tv = (frameY) / textureHeight;
				v[1].x = dimensions.x;
				v[1].y = 0;
				v[1].z = 0;
				v[1].color = color;
				//v[1].weight = 1;
				v[1].tu = (frameX + width/* - 1*/) / textureWidth;
				v[1].tv = (frameY) / textureHeight;
				v[2].x = 0;
				v[2].y = dimensions.y;
				v[2].z = 0;
				v[2].color = color;
				//v[2].weight = 1;
				v[2].tu = (frameX) / textureWidth;
				v[2].tv =  (frameY + height/* - 1*/) / textureHeight;
				v[3].x = dimensions.x;
				v[3].y = dimensions.y;
				v[3].z = 0;
				v[3].color = color;
				//v[3].weight = 1;
				v[3].tu = (frameX + width/* - 1*/) / textureWidth;
				v[3].tv = (frameY + height/* - 1*/) / textureHeight;
			}
			else
			{
				Vector3 position = positionComponent->position;
				Vector3 center = transformationComponent->center;
				Vector3 rotation = transformationComponent->rotation;
				Vector2 scale = transformationComponent->scale;

				Vector2 pos0 = math.rotatePoint(0, 0, center.x, center.y, rotation.z);
				v[0].x = pos0.x * scale.x;
				v[0].y = pos0.y * scale.y;
				v[0].z = 0;
				v[0].color = color;
				v[0].tu = (frameX) / textureWidth;
				v[0].tv = (frameY) / textureHeight;

				Vector2 pos1 = math.rotatePoint(dimensions.x, 0, center.x, center.y, rotation.z);
				v[1].x = pos1.x * scale.x;
				v[1].y = pos1.y * scale.y;
				v[1].z = 0;
				v[1].color = color;
				v[1].tu = (frameX + width) / textureWidth;
				v[1].tv = (frameY) / textureHeight;

				Vector2 pos2 = math.rotatePoint(0, dimensions.y, center.x, center.y, rotation.z);
				v[2].x = pos2.x * scale.x;
				v[2].y = pos2.y * scale.y;
				v[2].z = 0;
				v[2].color = color;
				v[2].tu = (frameX) / textureWidth;
				v[2].tv = (frameY + height) / textureHeight;

				Vector2 pos3 = math.rotatePoint(dimensions.x, dimensions.y, center.x, center.y, rotation.z);
				v[3].x = pos3.x * scale.x;
				v[3].y = pos3.y * scale.y;
				v[3].z = 0;
				v[3].color = color;
				v[3].tu = (frameX + width) / textureWidth;
				v[3].tv = (frameY + height) / textureHeight;
			}

			//d3dDevice9->SetFVF(VERTEX_FORMAT);
			//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)v, sizeof(Vertex));
			
			graphics.drawPrimitive(v, 2, PrimitiveType_TriangleStrip);

			/*switch(graphics.getGraphicsAPIType())
			{
			case GraphicsAPI_Direct3D11:
				{
					//graphics.setWorldIdentity();

					break;
				}
			}*/
		}
	}

	void RenderingSystem::onEntityRemove(int entityID)
	{
		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			if(listIterator->first != entityID) continue;

			GameComponentList::iterator componentListIterator;

			for(componentListIterator = ((*listIterator).second).begin(); componentListIterator != ((*listIterator).second).end(); ++componentListIterator)
			{
				if((*componentListIterator)->getType() == PARTICLEEMITTER_COMPONENT_TYPE_ID)
				{
					ParticleEmitterComponent *particleEmitter = (ParticleEmitterComponent *)(*componentListIterator);

					particleEmitter->releaseData();
				}
			}
		}
	}
};