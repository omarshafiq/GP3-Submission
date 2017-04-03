// Included headers
#include "Camera.h"

#pragma region Constructor
// Initializes a new instance of the Camera class.
Camera::Camera(Vector3D<float> position, Vector3D<float> rotation)
{
	p_Position = position;
	p_Rotation = rotation;
}
#pragma endregion
#pragma region Destructor
// Releases any resources used by this component.
Camera::~Camera()
{
}
#pragma endregion
#pragma region Properties (Get)
// Gets the current position of the camera.
Vector3D<float> Camera::GetPosition()
{
	return p_Position;
}

// Gets the current rotation of the camera.
Vector3D<float> Camera::GetRotation()
{
	return p_Rotation;
}
#pragma endregion
#pragma region Properties (Set)
// Sets the current position of the camera.
bool Camera::SetPosition(Vector3D<float> position)
{
	if (p_Position.x != position.x || p_Position.y != position.y || p_Position.z != position.z)
	{
		p_Position = position;

		return true;
	}

	return false;
}

// Sets the current rotation of the camera.
bool Camera::SetRotation(Vector3D<float> rotation)
{
	if (p_Rotation.x != rotation.x || p_Rotation.y != rotation.y || p_Rotation.z != rotation.z)
	{
		p_Rotation = rotation;

		return true;
	}

	return false;
}
#pragma endregion
#pragma region Public Methods
// Creates a matrix based on the camera's position and rotation.
Matrix3D<float> Camera::CreateViewMatrix()
{
	// Create a matrix based on this camera's rotation
	Matrix3D<float> rotationMatrix = Matrix3D<float>().rotated(p_Rotation);

	// Create a matrix based on this camera's position
	Matrix3D<float> positionMatrix(p_Position);

	// Multiply the matrices together and return the result
	return rotationMatrix * positionMatrix;
}
#pragma endregion