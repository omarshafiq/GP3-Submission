
#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

// Included headers
#include "../JuceLibraryCode/JuceHeader.h"

// Represents a camera that can be positioned/rotated in 3D space.
class Camera
{
public:
#pragma region Constructor/Destructor
	Camera(Vector3D<float> position, Vector3D<float> rotation);
	~Camera();
#pragma endregion
#pragma region Properties (Get)
	Vector3D<float> GetPosition();
	Vector3D<float> GetRotation();
#pragma endregion
#pragma region Properties (Set)
	bool SetPosition(Vector3D<float> position);
	bool SetRotation(Vector3D<float> soration);
#pragma endregion
#pragma region Public Methods
	Matrix3D<float> CreateViewMatrix();
#pragma endregion
private:
#pragma region Members
	// Member variables
	Vector3D<float> p_Position;
	Vector3D<float> p_Rotation;
#pragma endregion
};

#endif  // CAMERA_H_INCLUDED
