#ifndef _CAMERA_H
#define _CAMERA_H 1

#include <DirectXMath.h>

#include "Helpers.h"

namespace ProjectEagle
{
	enum CameraType
	{
		CameraType_FreeLook = 0,
		CameraType_LookAt = 1
	};

	class Camera
	{
	private:
		CameraType m_type;

		DirectX::XMMATRIX m_View;
		DirectX::XMMATRIX m_Projection;

		Vector3 m_position;
		Vector3 m_target;

		Vector2 m_viewportDimensions;

		DirectX::XMVECTOR m_upXM;
		DirectX::XMVECTOR m_rightXM;
		DirectX::XMVECTOR m_lookXM;

		float m_nearRange;
		float m_farRange;
		float m_aspectRatio;
		float m_fov;

		float m_yaw;
		float m_pitch;
		float m_roll;

	public:
		Camera(void);
		~Camera(void);

		CameraType getType();
		void setType(CameraType cameraType);

		void initialize();

		void setPerspective(float fov, float aspectRatio, float nearRange, float farRange);
		float getNearRange();
		void setNearRange(float value);
		float getFarRange();
		void setFarRange(float value);
		float getAspectRatio();
		void setAspectRatio(float value);
		float getFOV();
		void setFOV(float value);

		Vector2 getViewportDimensions();
		void setViewportDimensions(Vector2 dimensions);

		void update();

		void yawRotate(float value);
		void pitchRotate(float value);
		void rollRotate(float value);

		Vector3 getRotation();

		float getYawRotation();
		float getPitchRotation();
		float getRollRotation();

		void moveForward(float amount);
		void moveBackward(float amount);
		void moveRight(float amount);
		void moveLeft(float amount);
		void moveUp(float amount);
		void moveDown(float amount);

		Vector3 getPosition();
		void setPosition(Vector3 value);

		void move(Vector3 value);

		void resetPosition();

		void setRotation(float yaw, float pitch, float roll);

		void setRotationYaw(float value);
		void setRotationPitch(float value);
		void setRotationRoll(float value);

		Vector3 getLook();
		Vector3 getUp();
		Vector3 getRight();

		DirectX::XMMATRIX *getViewMatrix();
		DirectX::XMMATRIX *getProjectionMatrix();

		Vector3 getTarget();
		void setTarget(Vector3 value);
	};
};

#endif