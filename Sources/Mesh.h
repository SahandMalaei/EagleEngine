#ifndef _MESH_H
#define _MESH_H 1

#include "Eagle.h"

namespace ProjectEagle
{
	class Mesh
    {
    private:
		Vector3 m_position;

		Vector3 m_velocity;
		Vector3 m_acceleration;

	public:
		Mesh();
		~Mesh();

		bool Load(char *fileName, char *rootFolder);
		int getFaceCount(){return 0;}
		int getVertexCount(){return 0;}

		void CreateSphere(float radius, int slices, int stacks);
		void CreateCube(float width, float height, float depth);

		void update();
		void render();

		Vector3 getPosition();
		void setPosition(Vector3 value);

		Vector3 getVelocity();
		void setVelocity(Vector3 value);

		Vector3 getAcceleration();
		void setAcceleration(Vector3 value);
	};
};

#endif