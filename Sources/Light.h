#ifndef _LIGHT_H
#define _LIGHT_H 1

#include "Helpers.h"
#include "Graphics.h"

#define LIGHT_POINT 0x00000001
#define LIGHT_SPOT 0x00000002
#define LIGHT_DIRECTIONAL 0x00000003

#define CONVEX_HULL_MAX_VERTEX_COUNT 50

namespace ProjectEagle
{
	class ConvexHull
	{
	private:
		int vertexCount;
		Vector2 vertexList[50];
	public:
		ConvexHull();
		~ConvexHull();
		void setVertexCount(int n){vertexCount = n;}
		int getVertexCount(){return vertexCount;}
		void setVertex(int index, float x, float y){vertexList[index].set(x, y);}
		Vector2 getVertex(int index){return vertexList[index];}
		void addVertex(float x, float y){vertexList[vertexCount].set(x, y); vertexCount++;}
		void setAsRectangle(float x, float y, float width, float height);
		void setAsQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
		void setAsPolygon(float x, float y, int sides, int radius);
		void setAsEllipse(float x, float y, float rx, float ry);
		void Move(float vx, float vy);
		void Rotate(float centerX, float centerY, float angle);
		void draw(DWORD Color, float depth);
		void drawTransformed(DWORD Color, float depth);
	};

	struct LightProperties
	{
		Vector2 position;
		ColorValue color;
		//DWORD ambientColor;
		float radius;
		float sourceRadius;

		void set(Vector2 lightPosition, DWORD lightColor, float lightRadius, float lightSourceRadius = 0) {position = lightPosition; color = lightColor; radius = lightRadius; sourceRadius = lightSourceRadius;}
		//void set(float x, float y, DWORD Color, float Radius, float SourceRadius) {position.set(x, y); color = Color; radius = Radius; sourceRadius = SourceRadius;}
		/*void setPosition(Vector2 Position){position = Position;}
		void setPosition(float x, float y){position.set(x, y);}
		void setColor(DWORD Color){color = Color;}
		void setRadius(float Radius){radius = Radius;}
		void setSourceRadius(float SourceRadius){sourceRadius = SourceRadius;}*/
	};

	class Light2D
    {
    private:
		void findEdges(LightProperties light, ConvexHull object, int *index0, int *index1);

	public:
		Light2D();
		~Light2D();
		Vector2 position;
		//int depth;
		bool visible;
		int shadowOffset;
		//void addCaster(Sprite *s);
		void draw(ConvexHull *objectList, int objectCount, LightProperties *lightList, int lightCount, float depth);
		void drawTransformed(ConvexHull *objectList, int objectCount, LightProperties *lightList, int lightCount, float depth);
		//void setRadius(float r){radius = r;}
		//float getRadius(){return radius;}
		void rotatePosition(float cx, float cy, float theta);
		//void setColorRGBA(int r, int g, int b, int a){color = D3DCOLOR_RGBA(r, g, b, a); colorValues.r = r; colorValues.g = g; colorValues.b = b; colorValues.a = a;}
		//DWORD getColor(){return color;}
	};
};

#endif