#include "Eagle.h"

namespace ProjectEagle
{
	ConvexHull::ConvexHull()
	{
		vertexCount = 0;
	}

	ConvexHull::~ConvexHull()
	{
	}

	void ConvexHull::setAsRectangle(float x, float y, float width, float height)
	{
		vertexCount = 4;
		vertexList[0].set(x, y);
		vertexList[1].set(x + width, y);
		vertexList[2].set(x + width, y + height);
		vertexList[3].set(x, y + height);
	}

	void ConvexHull::setAsQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
	{
		vertexCount = 4;
		vertexList[0].set(x1, y1);
		vertexList[1].set(x2, y2);
		vertexList[2].set(x3, y3);
		vertexList[3].set(x4, y4);
	}

	void ConvexHull::setAsPolygon(float x, float y, int sides, int radius)
	{
		vertexCount = sides;
		for(int i = 0; i < sides; ++i)
		{
			float a = i * pi * 2 / sides;
			vertexList[i].x = cos(a) * radius + x;
			vertexList[i].y = sin(a) * radius + y;
		}
	}

	void ConvexHull::setAsEllipse(float x, float y, float rx, float ry)
	{
		vertexCount = 40;
		for(int i = 0; i < 40; ++i)
		{
			float a = i * pi / 20;
			vertexList[i].x = cos(a) * rx + x;
			vertexList[i].y = sin(a) * ry + y;
		}

		this->Rotate(x, y, 0.001);
	}

	void ConvexHull::Move(float vx, float vy)
	{
		for(int i = 0; i < vertexCount; ++i)
		{
			vertexList[i].move(vx, vy);
		}
	}

	void ConvexHull::Rotate(float centerX, float centerY, float angle)
	{
		for(int i = 0; i < vertexCount; ++i)
		{
			vertexList[i] = math.rotatePoint(vertexList[i].x, vertexList[i].y, centerX, centerY, angle);
		}
	}

	void ConvexHull::draw(DWORD Color, float depth)
	{
		Vertex v[50];
		for(int i = 0; i < vertexCount; ++i)
		{
			v[i].x = vertexList[i].x;
			v[i].y = vertexList[i].y;
			v[i].z = -depth;
			v[i].color = Color;
		}
	}

	void ConvexHull::drawTransformed(DWORD Color, float depth)
	{
		Vertex v[50];
		for(int i = 0; i < vertexCount; ++i)
		{
			v[i].x = vertexList[i].x;
			v[i].y = vertexList[i].y;
			v[i].z = depth;
			v[i].color = Color;
		}
	}

	Light2D::Light2D()
	{
		//vertexBuffer = eagle.getVertexBuffer();
		position.x = position.y = 0;
		//depth = 0;
		visible = 1;
		shadowOffset = 0;

		//lightMap = resourceManager.loadTexture("Data/Graphics/LightMap.png");
	}

	Light2D::~Light2D()
	{
	}

	/*void Light2D::addCaster(Sprite *s)
	{
		casters[numCasters] = s;
		for(int i = 0; i < s->getNumShadowCasters(); ++i)
		{
			v[numCasters][i] = s->getShadowCaster(i);
		}
		Point p; p.x = p.y = -123456;
		v[numCasters][s->getNumShadowCasters()] = &p;
		//casterRotList[numCasters] = 0;

		numCasters++;
	}*/

	void Light2D::draw(ConvexHull *objectList, int objectCount, LightProperties *lightList, int lightCount, float depth)
	{
		if(!visible) return;

		DWORD ambientColor = Color_Black;

		for(int i = 0; i < lightCount; ++i)
		{
			// Clear the alpha values in the backbuffer

			// Draw the light gradient

			//graphics.fillGradientEllipse(lightList[i].position, lightList[i].radius, lightList[i].radius, 0, 0, COLOR_RGBA_DWORD(255, 255, 255, 255), COLOR_RGBA_DWORD(255, 255, 255, 0), depth);

			// Remove the shadows from the light gradient

			//Vector2 edge0UmbraVector, edge1UmbraVector;

			for(int j = 0; j < objectCount; ++j)
			{
				int index0, index1;

				findEdges(lightList[i], objectList[j], &index0, &index1);

				if(index0 == index1)
				{
					continue;
				}

				/*Vector2 edge0, edge1;
				edge0 = objectList[j].getVertex(index0);
				edge1 = objectList[j].getVertex(index1);


				float distanceToStartingVertex = math.distance(lightList[i].position, edge0);
				
				Vector2 centerVector;
				centerVector = edge0 - lightList[i].position;

				Vector2 perpendicularVector = Vector2(-centerVector.y, centerVector.x).normal();

				Vector2 penumbraVector = centerVector - perpendicularVector * lightList[i].sourceRadius;
				edge0UmbraVector = (centerVector + perpendicularVector * lightList[i].sourceRadius).normal();

				if(distanceToStartingVertex)
				{
					int newAlpha = COLOR_GET_A(lightList[i].color) * (1.0 - distanceToStartingVertex / lightList[i].radius);

					graphics.fillTriangle(edge0, lightList[i].position + penumbraVector.normal() * lightList[i].radius * 100, lightList[i].position + edge0UmbraVector.normal() * lightList[i].radius * 100, COLOR_RGBA(0, 0, 0, 255), COLOR_RGBA(0, 0, 0, 255), COLOR_RGBA(0, 0, 0, 255), depth, lightMap);
				}

				float distanceToEndingVertex = math.distance(lightList[i].position, edge1);

				centerVector = edge1 - lightList[i].position;

				perpendicularVector = Vector2(-centerVector.y, centerVector.x).normal();

				penumbraVector = centerVector + perpendicularVector * lightList[i].sourceRadius;
				edge1UmbraVector = (centerVector - perpendicularVector * lightList[i].sourceRadius).normal();

				if(distanceToEndingVertex)
				{
					int newAlpha = COLOR_GET_A(lightList[i].color) * (1.0 - distanceToEndingVertex / lightList[i].radius);

					graphics.fillTriangle(edge1, lightList[i].position + penumbraVector.normal() * lightList[i].radius * 100, lightList[i].position + edge1UmbraVector.normal() * lightList[i].radius * 100, COLOR_RGBA(0, 0, 0, 255), COLOR_RGBA(0, 0, 0, 255), COLOR_RGBA(0, 0, 0, 255), depth, lightMap);
				}*/

				if(index0 == index1)
				{
					graphics.fillEllipse(lightList[i].position, lightList[i].radius, lightList[i].radius, 0, 0, COLOR_RGBA(0, 0, 0, 0), depth);

					continue;
				}

				int vertexCount = objectList[j].getVertexCount();

				int loopStart, loopEnd;
				int difference;

				if(index0 < index1)
				{
					loopStart = index0;
					loopEnd = index1;
				}
				else
				{
					loopStart = index0;
					loopEnd = vertexCount + index1;
				}

				difference = loopEnd - loopStart - 1;

				int l = 0;

				for(int k = loopStart; k < loopEnd; ++k)
				{
					Vector2 edge0, edge1;
					edge0 = objectList[j].getVertex(k % vertexCount);
					edge1 = objectList[j].getVertex((k + 1) % vertexCount);

					Vector2 normal0, normal1;
					normal0 = (edge0 - lightList[i].position).normal();
					normal1 = (edge1 - lightList[i].position).normal();

					/*normal0 = ((edge0UmbraVector * (difference - l) + edge1UmbraVector * l) / difference).normal();
					l++;
					normal1 = ((edge0UmbraVector * (difference - l) + edge1UmbraVector * l) / difference).normal();*/

					graphics.fillQuadrilateral(edge0.x, edge0.y,
						edge1.x, edge1.y,
						edge0.x + lightList[i].radius * normal0.x * 1000, edge0.y + lightList[i].radius * normal0.y * 1000,
						edge1.x + lightList[i].radius * normal1.x * 1000, edge1.y + lightList[i].radius * normal1.y * 1000,
						COLOR_RGBA(0, 0, 0, 0), COLOR_RGBA(0, 0, 0, 0), COLOR_RGBA(0, 0, 0, 0), COLOR_RGBA(0, 0, 0, 0), depth);
				}
			}

			//graphics.setAmbientLightColor(lightList[i].color.getDWORD());

			graphics.fillRectangle(lightList[i].position - Vector2(lightList[i].radius, lightList[i].radius), lightList[i].position + Vector2(lightList[i].radius, lightList[i].radius), COLOR_RGBA(255, 255, 255, 255), depth);

			//for(int j = 0; j < objectCount; ++j) objectList[j].draw(COLOR_RGB(255, 255, 255).getDWORD(), depth);
		}

		//graphics.setAmbientLightColor(ambientColor);
	}

	void Light2D::findEdges(LightProperties light, ConvexHull object, int *index0, int *index1)
	{
		float m1, n1, m2, n2, cx;
		float x1 = light.position.x, y1 = light.position.y;
		int nextIndex, index, prevIndex;
		int nV = object.getVertexCount();
		Vector2 p1, p2, p3, p4;

		Vector2 edges[2];
		int curEdge = 0;
		float al;

		bool u = 0;

		int in[2];

		bool isBackFacing[CONVEX_HULL_MAX_VERTEX_COUNT];

		for(int i = 0; i < CONVEX_HULL_MAX_VERTEX_COUNT; ++i) isBackFacing[i] = 0;

		//int backFacingCount = 0;

		for(int i = 0; i < nV + 1; ++i)
		{
			Vector2 firstVertex = Vector2(object.getVertex(i).x, object.getVertex(i).y);
			int secondIndex = (i + 1) % nV;
			Vector2 secondVertex = Vector2(object.getVertex(secondIndex).x, object.getVertex(secondIndex).y);
			Vector2 middle = (firstVertex + secondVertex) / 2;

			Vector2 L = light.position - middle;

			Vector2 N;
			N.x = firstVertex.y - secondVertex.y;
			N.y = secondVertex.x - firstVertex.x;

			isBackFacing[i] = (N.dotProduct(L) > 0);
			//backFacingCount += isBackFacing[i];
		}

		//if(backFacingCount == nV) return 0;

		int startingIndex = 0;
		int endingIndex = 0;

		for (int i = 0; i < nV; i++)
		{
			//graphics.fillEllipse(object.getVertex(i).x, object.getVertex(i).y, 5, 5, 0, 2 * pi, COLOR_RGB(255, 255, 255), - 5);

			int currentIndex = i % nV;
			int nextEdge = (i + 1) % nV;

			/*if(isBackFacing[i])
			{
				graphics.fillEllipse(object.getVertex(i).x, object.getVertex(i).y, 20, 20, 0, 2 * pi, COLOR_RGB(255, 255, i * (255 / nV) ), - 10);
			}*/

			if (isBackFacing[currentIndex] && !isBackFacing[nextEdge])
			{
				endingIndex = nextEdge;

				//break;
			}

			if (!isBackFacing[currentIndex] && isBackFacing[nextEdge])
			{
				startingIndex = nextEdge;
			}
		}

		//*i1 = edges[0];
		//*i2 = object.getVertex(1);

		*index0 = startingIndex;
		*index1 = endingIndex;

		/*Vector2 i1 = object.getVertex(startingIndex);
		Vector2 i2 = object.getVertex(endingIndex);

		graphics.fillEllipse(i1.x, i1.y, 10, 10, 0, 2 * pi, COLOR_RGB(255, 0, 0), - 10);
		graphics.fillEllipse(i2.x, i2.y, 20, 20, 0, 2 * pi, COLOR_RGB(0, 255, 0), - 10);*/

		//console.print("StartingIndex : " + INT_TO_STRING(startingIndex) + ", EndingIndex : " + INT_TO_STRING(endingIndex));
	}

	void Light2D::rotatePosition(float cx, float cy, float theta)
	{
		position = math.rotatePoint(position.x, position.y, cx, cy, theta);
	}
};