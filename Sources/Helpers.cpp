#include "Helpers.h"

namespace ProjectEagle
{
	Vector2::Vector2()
	{
		x = y = 0;
	}

	Vector2::Vector2(const Vector2 &v)
	{
		x = v.x;
		y = v.y;
	}

	Vector2::Vector2(float x, float y)
	{
		set(x, y);
	}

	void Vector2::set(float x1, float y1)
	{
		x = x1;
		y = y1;
	}

	void Vector2::set(const Vector2 &v)
	{
		x = v.x;
		y = v.y;
	}

	void Vector2::operator += (const Vector2 &v)
	{
		x += v.x;
		y += v.y;
	}

	void Vector2::operator -= (const Vector2 &v)
	{
		x -= v.x;
		y -= v.y;
	}

	void Vector2::operator *= (const Vector2 &v)
	{
		x *= v.x;
		y *= v.y;
	}

	void Vector2::operator /= (const Vector2 &v)
	{
		x /= v.x;
		y /= v.y;
	}

	bool Vector2::operator == (const Vector2 &v)
	{
		return ((v.x - 0.0001f < x && x < v.x + 0.0001f) && (v.y - 0.0001f < y && y < v.y + 0.0001f));
	}

	bool Vector2::operator != (const Vector2 &v)
	{
		return !((v.x - 0.0001f < x && x < v.x + 0.0001f) && (v.y - 0.0001f < y && y < v.y + 0.0001f));
	}

	Vector2 Vector2::operator = (const Vector2 &v)
	{
		set(v);

		return Vector2(x, y);
	}

	Vector2 Vector2::operator * (const float &d)
	{
		Vector2 a;
		a.x = x * d;
		a.y = y * d;
		
		return a;
	}

	Vector2 Vector2::operator / (const float &d)
	{
		Vector2 a;
		a.x = x / d;
		a.y = y / d;
		
		return a;
	}

	Vector2 Vector2::operator * (const Vector2 &d)
	{
		Vector2 a;
		a.x = x * d.x;
		a.y = y * d.y;
		
		return a;
	}

	Vector2 Vector2::operator / (const Vector2 &d)
	{
		Vector2 a;
		a.x = x / d.x;
		a.y = y / d.y;
		
		return a;
	}

	Vector2 Vector2::operator + (const Vector2 &d)
	{
		Vector2 a;
		a.x = x + d.x;
		a.y = y + d.y;
		
		return a;
	}

	Vector2 Vector2::operator - (const Vector2 &d)
	{
		Vector2 a;
		a.x = x - d.x;
		a.y = y - d.y;
		
		return a;
	}

	void Vector2::operator *= (const float &d)
	{
		x *= d;
		y *= d;
	}

	void Vector2::operator /= (const float &d)
	{
		x /= d;
		y /= d;
	}

	float Vector2::distance(const Vector2 &v)
	{
		return sqrt((v.x - x)*(v.x - x) + (v.y - y)*(v.y - y));
	}

	float Vector2::length()
	{
		return sqrt(x*x + y*y);
	}

	Vector2 Vector2::normal()
	{
		float currentLength = length();
		float l;
		if(currentLength == 0)
		{
			l = 0;
		}
		else
		{
			l = 1.0 / currentLength;
		}

		float nx = x * l;
		float ny = y * l;

		return Vector2(nx, ny);
	}

	void Vector2::normalize()
	{
		set(this->normal());
	}

	Vector2 Vector2::getProjected(Vector2 axis)
	{
		return axis * (x * axis.x + y * axis.y) / (axis.x * axis.x + axis.y * axis.y);
	}

	void Vector2::project(Vector2 axis)
	{
		set(this->getProjected(axis));
	}

	float Vector2::dotProduct(const Vector2 &v)
	{
		return x * v.x + y * v.y;
	}

	Vector3::Vector3()
	{
		x = y = z = 0;
	}

	Vector3::Vector3(const Vector3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	Vector3::Vector3(float x, float y, float z)
	{
		set(x, y, z);
	}

	void Vector3::set(float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}

	void Vector3::set(const Vector3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	void Vector3::operator += (const Vector3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void Vector3::operator -= (const Vector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	void Vector3::operator *= (const Vector3 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	void Vector3::operator /= (const Vector3 &v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}

	bool Vector3::operator == (const Vector3 &v)
	{
		return ((v.x - 0.0001f < x && x < v.x + 0.0001f) && (v.y - 0.0001f < y && y < v.y + 0.0001f) && (v.z - 0.0001f < z && z < v.z + 0.0001f));
	}

	bool Vector3::operator != (const Vector3 &v)
	{
		return !((v.x - 0.0001f < x && x < v.x + 0.0001f) && (v.y - 0.0001f < y && y < v.y + 0.0001f) && (v.z - 0.0001f < z && z < v.z + 0.0001f));
	}

	Vector3 Vector3::operator = (const Vector3 &v)
	{
		set(v);

		return Vector3(x, y, z);
	}

	Vector3 Vector3::operator * (const float &d)
	{
		Vector3 a;
		a.x = x * d;
		a.y = y * d;
		a.z = z * d;
		
		return a;
	}

	Vector3 Vector3::operator / (const float &d)
	{
		Vector3 a;
		a.x = x / d;
		a.y = y / d;
		a.z = z / d;

		return a;
	}

	Vector3 Vector3::operator + (const Vector3 &d)
	{
		Vector3 a;
		a.x = x + d.x;
		a.y = y + d.y;
		a.z = z + d.z;
		
		return a;
	}

	Vector3 Vector3::operator - (const Vector3 &d)
	{
		Vector3 a;
		a.x = x - d.x;
		a.y = y - d.y;
		a.z = z - d.z;
		
		return a;
	}

	Vector3 Vector3::operator * (const Vector3 &d)
	{
		Vector3 a;
		a.x = x * d.x;
		a.y = y * d.y;
		a.z = z * d.z;

		return a;
	}

	Vector3 Vector3::operator / (const Vector3 &d)
	{
		Vector3 a;
		a.x = x / d.x;
		a.y = y / d.y;
		a.z = z / d.z;
		
		return a;
	}

	void Vector3::operator *= (const float &d)
	{
		x *= d;
		y *= d;
		z *= d;
	}

	void Vector3::operator /= (const float &d)
	{
		x /= d;
		y /= d;
		z /= d;
	}

	float Vector3::distance(const Vector3 &v)
	{
		return sqrt((v.x - x)*(v.x - x) + (v.y - y)*(v.y - y) + (v.z - z)*(v.z - z));
	}

	float Vector3::length()
	{
		return sqrt(x*x + y*y + z*z);
	}

	Vector3 Vector3::normal()
	{
		float l;
		if(length() == 0)
		{
			l = 0;
		}
		else
		{
			l = 1 / length();
		}

		float nx = x * l;
		float ny = y * l;
		float nz = z * l;

		return Vector3(nx, ny, nz);
	}

	Rectangle::Rectangle()
	{
		left = top = width = height = 0;
	}

	Rectangle::Rectangle(const Rectangle &rectangle)
	{
		left = rectangle.left;
		top = rectangle.top;
		width = rectangle.width;
		height = rectangle.height;
	}

	Rectangle::Rectangle(Vector2 topLeft, Vector2 dimensions)
	{
		left = topLeft.x;
		top = topLeft.y;
		width = dimensions.x;
		height = dimensions.y;
	}

	Rectangle::Rectangle(float left, float top, float width, float height)
	{
		this->left = left;
		this->top = top;
		this->width = width;
		this->height = height;
	}

	void Rectangle::set(Vector2 topLeft, Vector2 dimensions)
	{
		left = topLeft.x;
		top = topLeft.y;
		width = dimensions.x;
		height = dimensions.y;
	}

	void Rectangle::set(float left, float top, float right, float bottom)
	{
		this->left = left;
		this->top = top;
		this->width = width;
		this->height = height;
	}

	bool Rectangle::isRectangleIntersecting(Rectangle rectangle)
	{
		if(left + width >= rectangle.left)
		{
			if(rectangle.left + rectangle.width >= left)
			{
				if(top + height >= rectangle.top)
				{
					if(rectangle.top + rectangle.height >= top)
					{
						return 1;
					}
				}
			}
		}

		return 0;
	}

	bool Rectangle::isPointIntersecting(Vector2 v)
	{
		if(v.x >= left)
		{
			if(v.x <= left + width)
			{
				if(v.y >= top)
				{
					if(v.y <= top + height)
					{
						return 1;
					}
				}
			}
		}
		return 0;
	}

	bool Rectangle::isPointIntersecting(float x, float y)
	{
		if(x >= left)
		{
			if(x <= left + width)
			{
				if(y >= top)
				{
					if(y <= top + height)
					{
						return 1;
					}
				}
			}
		}

		return 0;
	}

	void Rectangle::setPosition(Vector2 position)
	{
		left = position.x;
		top = position.y;
	}

	void Rectangle::setPosition(float x, float y)
	{
		left = x;
		top = y;
	}

	void Rectangle::move(Vector2 movementVector)
	{
		left += movementVector.x;
		top += movementVector.y;
	}

	void Rectangle::move(float x, float y)
	{
		left += x;
		top += y;
	}

	RotatedRectangle::RotatedRectangle()
	{
		left = top = width = height = 0;
		centerX = centerY = 0;
		rotation = 0;
	}

	RotatedRectangle::RotatedRectangle(const RotatedRectangle &rectangle)
	{
		left = rectangle.left;
		top = rectangle.top;
		width = rectangle.width;
		height = rectangle.height;

		centerX = rectangle.centerX;
		centerY = rectangle.centerY;

		rotation = rectangle.rotation;
	}

	RotatedRectangle::RotatedRectangle(const Rectangle &rectangle, Vector2 center, float rotation)
	{
		this->left = rectangle.left;
		this->top = rectangle.top;
		width = rectangle.width;
		height = rectangle.height;

		this->centerX = center.x;
		this->centerY = center.y;

		this->rotation = rotation;
	}

	RotatedRectangle::RotatedRectangle(float left, float top, float width, float height, float centerX, float centerY, float rotation)
	{
		this->left = left;
		this->top = top;
		this->width = width;
		this->height = height;

		this->centerX = centerX;
		this->centerY = centerY;

		this->rotation = rotation;
	}

	void RotatedRectangle::set(Vector2 topLeft, Vector2 dimensions, Vector2 center, float rotation)
	{
		this->left = topLeft.x;
		this->top = topLeft.y;
		this->width = dimensions.x;
		this->height = dimensions.y;

		this->centerX = center.x;
		this->centerY = center.y;

		this->rotation = rotation;
	}

	void RotatedRectangle::set(float left, float top, float width, float height, float centerX, float centerY, float rotation)
	{
		this->left = left;
		this->top = top;
		this->width = width;
		this->height = height;

		this->centerX = centerX;
		this->centerY = centerY;

		this->rotation = rotation;
	}

	Vector2 rotatePoint(float x, float y, float centerX, float centerY, float angle)
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

	Vector2 rotatePoint(Vector2 pos, Vector2 center, float angle)
	{
		return rotatePoint(pos.x, pos.y, center.x, center.y, angle);
	}

	float distance(float x1, float y1, float x2, float y2)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		
		return sqrt(dx * dx + dy * dy);
	}

	float distance(Vector2 v1, Vector2 v2)
	{
		return distance(v1.x, v1.y, v2.x, v2.y);
	}

	bool RotatedRectangle::isRectangleIntersecting(const RotatedRectangle &rectangle)
	{
		Vector2 rotatedPositionList[8];
		Vector2 rotationCenterList[2], centerList[2], rotatedCenterList[2];

		rotationCenterList[0] = Vector2(left + centerX, top + centerY);

		rotatedPositionList[0] = rotatePoint(Vector2(left, top), rotationCenterList[0], rotation);
		rotatedPositionList[1] = rotatePoint(Vector2(left + width, top), rotationCenterList[0], rotation);
		rotatedPositionList[2] = rotatePoint(Vector2(left + width, top + height), rotationCenterList[0], rotation);
		rotatedPositionList[3] = rotatePoint(Vector2(left, top + height), rotationCenterList[0], rotation);

		rotationCenterList[1] = Vector2(rectangle.left + rectangle.centerX, rectangle.top + rectangle.centerY);

		rotatedPositionList[4] = rotatePoint(Vector2(rectangle.left, rectangle.top), rotationCenterList[1], rectangle.rotation);
		rotatedPositionList[5] = rotatePoint(Vector2(rectangle.left + rectangle.width, rectangle.top), rotationCenterList[1], rectangle.rotation);
		rotatedPositionList[6] = rotatePoint(Vector2(rectangle.left + rectangle.width, rectangle.top + rectangle.height), rotationCenterList[1], rectangle.rotation);
		rotatedPositionList[7] = rotatePoint(Vector2(rectangle.left, rectangle.top + rectangle.height), rotationCenterList[1], rectangle.rotation);

		centerList[0] = Vector2((left + left + width) / 2, (top + top + height) / 2);
		centerList[1] = Vector2((rectangle.left + rectangle.left + rectangle.width) / 2, (rectangle.top + rectangle.top + rectangle.height) / 2);

		rotatedCenterList[0] = (rotatedPositionList[0] + rotatedPositionList[2]) / 2;
		rotatedCenterList[1] = (rotatedPositionList[4] + rotatedPositionList[6]) / 2;

		if(distance(Vector2(left, top), centerList[0]) + distance(Vector2(rectangle.left, rectangle.top), centerList[1]) < distance(rotatedCenterList[0], rotatedCenterList[1]))
		{
			return 0;
		}

		Vector2 axisList[4];

		axisList[0] = rotatedPositionList[0] - rotatedPositionList[1];
		axisList[1] = rotatedPositionList[0] - rotatedPositionList[3];

		axisList[2] = rotatedPositionList[4] - rotatedPositionList[5];
		axisList[3] = rotatedPositionList[4] - rotatedPositionList[7];

		for(int i = 0; i < 4; ++i)
		{
			float dotProductValueList[8];

			dotProductValueList[0] = (rotatedPositionList[0].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[1] = (rotatedPositionList[1].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[2] = (rotatedPositionList[2].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[3] = (rotatedPositionList[3].getProjected(axisList[i])).dotProduct(axisList[i]);

			dotProductValueList[4] = (rotatedPositionList[4].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[5] = (rotatedPositionList[5].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[6] = (rotatedPositionList[6].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[7] = (rotatedPositionList[7].getProjected(axisList[i])).dotProduct(axisList[i]);

			float rectangle0MinimumDotProduct, rectangle0MaximumDotProduct, rectangle1MinimumDotProduct, rectangle1MaximumDotProduct;

			rectangle0MinimumDotProduct = min(min(dotProductValueList[0], dotProductValueList[1]), min(dotProductValueList[2], dotProductValueList[3]));
			rectangle0MaximumDotProduct = max(max(dotProductValueList[0], dotProductValueList[1]), max(dotProductValueList[2], dotProductValueList[3]));

			rectangle1MinimumDotProduct = min(min(dotProductValueList[4], dotProductValueList[5]), min(dotProductValueList[6], dotProductValueList[7]));
			rectangle1MaximumDotProduct = max(max(dotProductValueList[4], dotProductValueList[5]), max(dotProductValueList[6], dotProductValueList[7]));

			if(!(rectangle0MinimumDotProduct < rectangle1MaximumDotProduct && rectangle1MinimumDotProduct < rectangle0MaximumDotProduct))
			{
				return 0;
			}
		}

		return 1;
	}

	bool RotatedRectangle::isPointIntersecting(Vector2 v)
	{
		Vector2 rotatedPositionList[4];
		Vector2 rotationCenterPosition, centerPosition, rotatedCenterPosition;

		rotationCenterPosition = Vector2(left + centerX, top + centerY);

		rotatedPositionList[0] = rotatePoint(Vector2(left, top), rotationCenterPosition, rotation);
		rotatedPositionList[1] = rotatePoint(Vector2(left + width, top), rotationCenterPosition, rotation);
		rotatedPositionList[2] = rotatePoint(Vector2(left + width, top + height), rotationCenterPosition, rotation);
		rotatedPositionList[3] = rotatePoint(Vector2(left, top + height), rotationCenterPosition, rotation);


		centerPosition = Vector2((left + left + width) / 2, (top + top + height) / 2);
		rotatedCenterPosition = (rotatedPositionList[0] + rotatedPositionList[2]) / 2;

		if(distance(Vector2(left, top), centerPosition) < distance(rotatedCenterPosition, v))
		{
			return 0;
		}

		Vector2 axisList[2];

		axisList[0] = rotatedPositionList[0] - rotatedPositionList[1];
		axisList[1] = rotatedPositionList[0] - rotatedPositionList[3];

		for(int i = 0; i < 2; ++i)
		{
			float dotProductValueList[5];

			dotProductValueList[0] = (rotatedPositionList[0].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[1] = (rotatedPositionList[1].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[2] = (rotatedPositionList[2].getProjected(axisList[i])).dotProduct(axisList[i]);
			dotProductValueList[3] = (rotatedPositionList[3].getProjected(axisList[i])).dotProduct(axisList[i]);

			dotProductValueList[4] = (v.getProjected(axisList[i])).dotProduct(axisList[i]);

			float minimumDotProduct, maximumDotProduct;

			minimumDotProduct = min(min(dotProductValueList[0], dotProductValueList[1]), min(dotProductValueList[2], dotProductValueList[3]));
			maximumDotProduct = max(max(dotProductValueList[0], dotProductValueList[1]), max(dotProductValueList[2], dotProductValueList[3]));

			if(!(dotProductValueList[4] < maximumDotProduct && minimumDotProduct < dotProductValueList[4]))
			{
				return 0;
			}
		}

		return 1;
	}

	bool RotatedRectangle::isPointIntersecting(float x, float y)
	{
		return isPointIntersecting(Vector2(x, y));
	}

	void RotatedRectangle::setPosition(Vector2 position)
	{
		left = position.x;
		top = position.y;
	}

	void RotatedRectangle::setPosition(float x, float y)
	{
		left = x;
		top = y;
	}

	void RotatedRectangle::move(Vector2 movementVector)
	{
		left += movementVector.x;
		top += movementVector.y;
		centerX += movementVector.x;
		centerY += movementVector.y;
	}

	void RotatedRectangle::move(float x, float y)
	{
		left += x;
		top += y;
		centerX += x;
		centerY += y;
	}

	bool isCharNumeric(char c)
	{
		if(c > '0' - 1 && c < '9' + 1) return 1;

		return 0;
	}

	bool isCharAlphabetic(char c)
	{
		if((c > 'a' - 1 && c < 'z' + 1) || (c > 'A' - 1 && c < 'Z' + 1)) return 1;

		return 0;
	}

	bool isCharWhitespace(char c)
	{
		if(c == ' ' || c == '\t' || c == '\n') return 1;

		return 0;
	}

	bool isCharDelimiter(char c)
	{
		if(c == ';' || c == ':' || c == ',' || c == '"' || c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')' || isCharWhitespace(c)) return 1;

		return 0;
	}

	bool isStringInt(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		if(!isCharNumeric(s[0]) && s[0] != '-')
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 1; i < stringLength; ++i)
		{
			if(!isCharNumeric(s[i]))
			{
				return 0;
			}
		}

		return 1;
	}

	bool isStringFloat(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		if(!isCharNumeric(s[0]) && s[0] != '-')
		{
			return 0;
		}

		bool radixPointFound = 0;

		int stringLength = s.length();

		for(int i = 1; i < stringLength; ++i)
		{
			if(isCharNumeric(s[i]))
			{
				continue;
			}
			else if(s[i] == '.' && !radixPointFound)
			{
				radixPointFound = 1;
				continue;
			}

			return 0;
		}

		if(radixPointFound)
		{
			return 1;
		}
		
		return 0;
	}

	bool isStringAlphabetic(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 0; i < stringLength; ++i)
		{
			if(!isCharAlphabetic(s[i])) return 0;
		}

		return 1;
	}

	bool isStringWhitespace(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 0; i < stringLength; ++i)
		{
			if(!isCharWhitespace(s[i]))
			{
				return 0;
			}
		}

		return 1;
	}

	bool isStringValidIdentifier(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		if(!isCharAlphabetic(s[0]))
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 1; i < stringLength; ++i)
		{
			if(!isCharNumeric(s[i]) && !isCharAlphabetic(s[i]) && s[i] != '_')
			{
				return 0;
			}
		}

		return 1;
	}

	/*int asciiHexToInt(string hexString)
	{
		if(hexString.length() < 3) return 0;

		if(hexString[0] != '0' || (hexString[1] != 'x' && hexString[1] != 'X')) return 0;

		int value = 0;

		int power = 0;

		for(int i = hexString.length() - 1; i > 1; --i)
		{
			string currentCharacter = "";
			currentCharacter += hexString[i];
			
			switch(currentCharacter[0])
			{
			case 'A':
			case 'a':
				currentCharacter = "10";

				break;

			case 'B':
			case 'b':
				currentCharacter = "11";

				break;

			case 'C':
			case 'c':
				currentCharacter = "12";

				break;

			case 'D':
			case 'd':
				currentCharacter = "13";

				break;

			case 'E':
			case 'e':
				currentCharacter = "14";

				break;

			case 'F':
			case 'f':
				currentCharacter = "15";

				break;
			}

			value += power(16, power) * atoi(currentCharacter.c_str());

			power++;
		}

		return value;
	}*/
};