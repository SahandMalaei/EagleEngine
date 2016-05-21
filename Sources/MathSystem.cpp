#include "MathSystem.h"

#include <cstdlib>
#include <time.h>
#include <math.h>

#include "Helpers.h"

namespace ProjectEagle
{
	MathSystem::MathSystem()
	{
		srand(time(0));
	}

	float MathSystem::abs(float number)
	{
		return (number >= 0) ? number : -number;
	}

	float MathSystem::power(float number, int power)
	{
		float c = 1;

		for(int i = 0; i < power; ++i)
		{
			c *= number;
		}

		return c;
	}

	float MathSystem::toDegrees(float radians)
	{
		return radians * piUnder180;
	}

	float MathSystem::toRadians(float degrees)
	{
		return degrees * piOver180;
	}

	float MathSystem::wrapAngleDegrees(float degrees)
	{
		float result = fmod(degrees, 360);
		if(result < 0) result += 360;

		return result;
	}

	float MathSystem::wrapAngleRadians(float radians)
	{
		float angle = radians;
		while(angle < 0) angle += pi * 2;
		//float result = fmod(radians, pi * 2);
		while(angle >= pi * 2) angle -= pi * 2;

		return angle;
	}

	float MathSystem::linearVelocityX(float angle)
	{
		return cos(angle);
	}

	float MathSystem::linearVelocityY(float angle)
	{
		return sin(angle);
	}

	Vector2 MathSystem::linearVelocity(float angle)
	{
		float vx = linearVelocityX(angle);
		float vy = linearVelocityY(angle);
		return Vector2(vx, vy);
	}

	float MathSystem::angleToTarget(float x1, float y1, float x2, float y2)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;

		return atan2(dy, dx);
	}

	float MathSystem::angleToTarget(Vector2& v1, Vector2& v2)
	{
		return angleToTarget(v1.x, v1.y, v2.x, v2.y);
	}

	float MathSystem::distance(float x1, float y1, float x2, float y2)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		
		return sqrt(dx * dx + dy * dy);
	}

	float MathSystem::distance(Vector2 v1, Vector2 v2)
	{
		return distance(v1.x, v1.y, v2.x, v2.y);
	}

	float MathSystem::length(float x, float y)
	{
		return sqrt(x * x + y * y);
	}

	float MathSystem::length(Vector2 v)
	{
		return length(v.x, v.y);
	}

	float MathSystem::dotProduct(float x0, float y0, float x1, float y1)
	{
		return x0 * x1 + y0 * y1;
	}

	float MathSystem::dotProduct(Vector2 vector0, Vector2 vector1)
	{
		return vector0.x * vector1.x + vector0.y * vector1.y;
	}

	void MathSystem::seedRandom(int seed)
	{
		randomSeed = seed;
	}

	int MathSystem::random()
	{
		return rand();

		/*static unsigned long x = 123456789, y=362436069, z=521288629;      //period 2^96-1
		unsigned long t;

		x ^= x << 16;
		x ^= x >> 5;
		x ^= x << 1;

		t = x;
		x = y;
		y = z;
		z = t ^ x ^ y;

		return z;*/

		//randomSeed = (214013 * randomSeed + 2531011); 
		//return (randomSeed >> 16) & 0x7FFF; 
	}

	int MathSystem::random(int max)
	{
		return random() % (max);
	}

	int MathSystem::random(int min, int max)
	{
		if(max < min + 1) return 0;

		return (random() % (max - min + 1)) + min;
	}

	float MathSystem::randomFloat()
	{
		return random() / 1000.0f;
	}

	float MathSystem::randomFloat(float max)
	{
		return (random() % 1000) / 1000.0f * max;
	}

	float MathSystem::randomFloat(float min, float max)
	{
		return (random() % 1000) / 1000.0f * (max - min) + min;
	}

	Vector2 MathSystem::rotatePoint(float x, float y, float centerX, float centerY, float angle)
	{
		//p'x = cos(theta) * (px-ox) - sin(theta) * (py-oy) + ox
        //p'y = sin(theta) * (px-ox) + cos(theta) * (py-oy) + oy

		float s = sin(angle);
		float c = cos(angle);

		x -= centerX;
		y -= centerY;

		Vector2 rotatedPoint;
		rotatedPoint.x = x * c - y * s + centerX;
		rotatedPoint.y = x * s + y * c + centerY;

		return rotatedPoint;
	}

	Vector2 MathSystem::rotatePoint(Vector2 pos, Vector2 center, float angle)
	{
		return rotatePoint(pos.x, pos.y, center.x, center.y, angle);
	}

	bool MathSystem::approximateEquals(float x, float y, float threshold)
	{
		if(x + threshold > y && x - threshold < y)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	float MathSystem::weightedAverage(float number0, float number1, float weight)
	{
		return number0 + (number1 - number0) * weight;
	}
};