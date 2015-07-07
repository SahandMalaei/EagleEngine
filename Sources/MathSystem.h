#ifndef _MATHSYSTEM_H
#define _MATHSYSTEM_H 1

#include "Eagle.h"

namespace ProjectEagle
{
	const float pi = 3.1415926535897932384626433832795;
	const float piOver180 = pi / 180;
	const float piUnder180 = 180 / pi;
	const float sqrt2 = 1.4142135623;

	class MathSystem
	{
	private:
		int randomSeed;

		void seedRandom(int seed);

	public:
		MathSystem();

		float abs(float number);

		float roundToUpper(float number){return (long int)(number + 0.5);}
		float roundToLower(float number){return (long int)(number);}
		float round(float number){return (long int)(number);}
		
		float power(float number, int power);

		float toDegrees(float radians);
		float toRadians(float degrees);
		float wrapAngleDegrees(float degrees);
		float wrapAngleRadians(float radians);

		float linearVelocityX(float angle);
		float linearVelocityY(float angle);
		Vector2 linearVelocity(float angle);
		float angleToTarget(float x0, float y0, float x1, float y1);
		float angleToTarget(Vector2& source, Vector2& target);

		float distance(float x1, float y1, float x2, float y2);
		float distance(Vector2& v1, Vector2& v2);
		float length(Vector2& vec);
		float length(float x, float y);

		float dotProduct(float x0, float y0, float x1, float y1){return x0 * x1 + y0 * y1;}
		float dotProduct(Vector2 vector0, Vector2 vector1){return dotProduct(vector0.x, vector0.y, vector1.x, vector1.y);}

		int random();
		int random(int max);
		int random(int min, int max);

		float randomFloat();
		float randomFloat(float max);
		float randomFloat(float min, float max);

		Vector2 rotatePoint(float x, float y, float centerX, float centerY, float angle);
		Vector2 rotatePoint(Vector2 pos, Vector2 center, float angle){return rotatePoint(pos.x, pos.y, center.x, center.y, angle);}

		bool approximateEquals(float x, float y, float threshold = 0.0001);

		float weightedAverage(float number0, float number1, float weight);
	};
};

#endif