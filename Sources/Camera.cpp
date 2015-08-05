#include "Camera.h"

#include "MathSystem.h"

namespace ProjectEagle
{
	DirectX::XMVECTOR XMVector(float x, float y, float z)
	{
		DirectX::XMVECTOR output;

		output = DirectX::XMVectorSet(x, y, z, 0);

		return output;
	}

	Vector3 XMVectorToVector3(DirectX::XMVECTOR vector)
	{
		Vector3 output;

		output.x = DirectX::XMVectorGetX(vector);
		output.y = DirectX::XMVectorGetY(vector);
		output.z = DirectX::XMVectorGetZ(vector);

		return output;
	}

	Camera::Camera()
	{
		m_type = CameraType_FreeLook;

		m_upXM = XMVector(0.0f, 1.0f, 0.0f);
		m_rightXM = XMVector(1.0f, 0.0f, 0.0f);
		m_lookXM = XMVector(0.0f, 0.0f, 1.0f);

		m_target = Vector3(0.0f, 0.0f, 0.0f);

		m_fov = pi / 4;

		m_nearRange = 0.001f;
		m_farRange = 10000.0f;

		m_yaw = 0;
		m_pitch = 0;
		m_roll = 0;
	}

	CameraType Camera::getType()
	{
		return m_type;
	}

	void Camera::setType(CameraType cameraType)
	{
		m_type = cameraType;
	}

	void Camera::initialize()
	{
		m_aspectRatio = m_viewportDimensions.x / m_viewportDimensions.y;
		m_target = Vector3((float)m_viewportDimensions.x / 2.0, m_viewportDimensions.y / 2.0, 0.0f);

		resetPosition();
	}

	Camera::~Camera(void)
	{
	}

	void Camera::setPerspective(float fov, float aspectRatio, float nearRange, float farRange)
	{
		m_fov = fov;
		m_aspectRatio = aspectRatio;
		m_nearRange = nearRange;
		m_farRange = farRange;
	}

	float Camera::getNearRange()
	{
		return m_nearRange;
	}

	void Camera::setNearRange(float value)
	{
		m_nearRange = value;
	}

	float Camera::getFarRange()
	{
		return m_farRange;
	}

	void Camera::setFarRange(float value)
	{
		m_farRange = value;
	}

	float Camera::getAspectRatio()
	{
		return m_aspectRatio;
	}

	void Camera::setAspectRatio(float value)
	{
		m_aspectRatio = value;
	}

	float Camera::getFOV()
	{
		return m_fov;
	}

	void Camera::setFOV(float value)
	{
		m_fov = value;
	}

	Vector2 Camera::getViewportDimensions()
	{
		return m_viewportDimensions;
	}

	void Camera::setViewportDimensions(Vector2 dimensions)
	{
		m_viewportDimensions = dimensions;
	}

	void Camera::update()
	{
		m_aspectRatio = m_viewportDimensions.x / m_viewportDimensions.y;

		float screenWidth, screenHeight;

		screenWidth = m_viewportDimensions.x;
		screenHeight = m_viewportDimensions.y;

		m_View = DirectX::XMMatrixIdentity();

		m_upXM = XMVector(0.0f, 1.0f, 0.0f);
		m_rightXM = XMVector(1.0f, 0.0f, 0.0f);
		m_lookXM = XMVector(0.0f, 0.0f, 1.0f);

		DirectX::XMMATRIX yawMatrix;
		yawMatrix = DirectX::XMMatrixRotationAxis(m_upXM, m_yaw);
		m_lookXM = DirectX::XMVector3TransformCoord(m_lookXM, yawMatrix);
		m_rightXM = DirectX::XMVector3TransformCoord(m_rightXM, yawMatrix);

		DirectX::XMMATRIX pitchMatrix;
		pitchMatrix = DirectX::XMMatrixRotationAxis(m_rightXM, m_pitch);
		m_lookXM = DirectX::XMVector3TransformCoord(m_lookXM, pitchMatrix);
		m_upXM = DirectX::XMVector3TransformCoord(m_upXM, pitchMatrix);

		DirectX::XMMATRIX rollMatrix;
		rollMatrix = DirectX::XMMatrixRotationAxis(m_lookXM, m_roll);
		m_rightXM = DirectX::XMVector3TransformCoord(m_rightXM, rollMatrix);
		m_upXM = DirectX::XMVector3TransformCoord(m_upXM, rollMatrix);

		if(m_type == CameraType_FreeLook)
		{
			DirectX::XMVECTOR positionVector = XMVector((m_position.x - screenWidth / 2.0) / (screenHeight / 2.0), (m_position.y) / (-screenHeight / 2) + 1, m_position.z / (screenHeight / 2.0));

			m_View = DirectX::XMMatrixSet(
				DirectX::XMVectorGetX(m_rightXM), DirectX::XMVectorGetX(m_upXM), DirectX::XMVectorGetX(m_lookXM), 0,
				DirectX::XMVectorGetY(m_rightXM), DirectX::XMVectorGetY(m_upXM), DirectX::XMVectorGetY(m_lookXM), 0,
				DirectX::XMVectorGetZ(m_rightXM), DirectX::XMVectorGetZ(m_upXM), DirectX::XMVectorGetZ(m_lookXM), 0,
				- DirectX::XMVectorGetX(DirectX::XMVector3Dot(positionVector, m_rightXM)),
				- DirectX::XMVectorGetX(DirectX::XMVector3Dot(positionVector, m_upXM)),
				- DirectX::XMVectorGetX(DirectX::XMVector3Dot(positionVector, m_lookXM)),
				1);

			/*m_View = DirectX::XMMatrixSet(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			- XMVectorToVector3(positionVector).x,
			- XMVectorToVector3(positionVector).y,
			- XMVectorToVector3(positionVector).z,
			1);*/

		}
		else if(m_type == CameraType_LookAt)
		{
			DirectX::XMVECTOR eyeVector, targetVector;
			eyeVector = XMVector((m_position.x - screenWidth / 2.0) / (screenHeight / 2.0), (m_position.y) / (-screenHeight / 2) + 1, m_position.z / (screenHeight / 2.0));
			targetVector = XMVector((m_target.x - screenWidth / 2.0) / (screenHeight / 2.0), (m_target.y) / (-screenHeight / 2) + 1, m_target.z / (screenHeight / 2.0));

			m_View = DirectX::XMMatrixLookAtLH(eyeVector, targetVector, m_upXM);
		}

		m_Projection = DirectX::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearRange, m_farRange);
	}

	void Camera::moveForward(float amount)
	{
		Vector3 lookVector = XMVectorToVector3(m_lookXM);

		lookVector.y *= -1;

		m_position += lookVector * amount;
	}

	void Camera::moveBackward(float amount)
	{
		Vector3 lookVector = XMVectorToVector3(m_lookXM);

		lookVector.y *= -1;

		m_position += lookVector * -amount;
	}

	void Camera::moveRight(float amount)
	{
		Vector3 rightVector = XMVectorToVector3(m_rightXM);

		rightVector.y *= -1;

		m_position += rightVector * amount;
	}

	void Camera::moveLeft(float amount)
	{
		Vector3 rightVector = XMVectorToVector3(m_rightXM);

		rightVector.y *= -1;

		m_position += rightVector * -amount;
	}

	void Camera::moveUp(float amount)
	{
		Vector3 upVector = XMVectorToVector3(m_upXM);

		upVector.y *= -1;

		m_position += upVector * amount;
	}

	void Camera::moveDown(float amount)
	{
		Vector3 upVector = XMVectorToVector3(m_upXM);

		upVector.y *= -1;

		m_position += upVector * -amount;
	}

	Vector3 Camera::getLook()
	{
		return XMVectorToVector3(m_lookXM);
	}

	Vector3 Camera::getUp()
	{
		return XMVectorToVector3(m_upXM);
	}

	Vector3 Camera::getRight()
	{
		return XMVectorToVector3(m_rightXM);
	}

	void Camera::setRotationYaw(float value)
	{
		m_yaw = value;
	}

	void Camera::setRotationPitch(float value)
	{
		m_pitch = value;
	}

	void Camera::setRotationRoll(float value)
	{
		m_roll = value;
	}

	DirectX::XMMATRIX *Camera::getViewMatrix()
	{
		return &m_View;
	}

	DirectX::XMMATRIX *Camera::getProjectionMatrix()
	{
		return &m_Projection;
	}

	Vector3 Camera::getTarget()
	{
		return m_target;
	}

	void Camera::setTarget(Vector3 value)
	{
		m_target = value;
	}

	void Camera::yawRotate(float value)
	{
		m_yaw += value;
	}

	void Camera::pitchRotate(float value)
	{
		m_pitch += value;
	}

	void Camera::rollRotate(float value)
	{
		m_roll += value;
	}

	Vector3 Camera::getRotation()
	{
		return Vector3(m_yaw, m_pitch, m_roll);
	}

	void Camera::setRotation(float yaw, float pitch, float roll)
	{
		m_yaw = yaw;
		m_pitch = pitch;
		m_roll = roll;
	}

	float Camera::getYawRotation()
	{
		return m_yaw;
	}

	float Camera::getPitchRotation()
	{
		return m_pitch;
	}

	float Camera::getRollRotation()
	{
		return m_roll;
	}

	Vector3 Camera::getPosition()
	{
		return m_position;
	}

	void Camera::setPosition(Vector3 position)
	{
		m_position = position;
	}

	void Camera::move(Vector3 value)
	{
		m_position += value;
	}

	void Camera::resetPosition()
	{
		float z = (-sqrt2 - 1) * m_viewportDimensions.y / 2.0;
		m_position = Vector3(m_viewportDimensions.x / 2.0, m_viewportDimensions.y / 2.0, z);
	}
};