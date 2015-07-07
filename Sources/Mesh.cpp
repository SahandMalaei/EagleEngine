#include "Eagle.h"

namespace ProjectEagle
{
	Mesh::Mesh(void)
	{
		m_position.set(0.0f,0.0f,0.0f);

		m_velocity.set(0.0f,0.0f,0.0f);
		m_acceleration.set(0, 0, 0);
	}

	Mesh::~Mesh(void)
	{
	}

	Vector3 Mesh::getPosition()
	{
		return m_position;
	}

	void Mesh::setPosition(Vector3 value)
	{
		m_position = value;
	}

	Vector3 Mesh::getVelocity()
	{
		return m_velocity;
	}

	void Mesh::setVelocity(Vector3 value)
	{
		m_velocity = value;
	}

	Vector3 Mesh::getAcceleration()
	{
		return m_acceleration;
	}

	void Mesh::setAcceleration(Vector3 value)
	{
		m_acceleration = value;
	}

	bool Mesh::Load(char *fileName, char *rootFolder)
	{

		return 1;
	}

	void Mesh::CreateSphere(float radius, int slices, int stacks)
	{
	}

	void Mesh::CreateCube(float width, float height, float depth)
	{
	}

	void Mesh::update()
	{
		m_velocity += m_acceleration;

		m_position += m_velocity;
	}

	void Mesh::render()
	{
	}
};