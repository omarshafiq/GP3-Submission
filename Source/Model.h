#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

// Included headers
#include "../JuceLibraryCode/JuceHeader.h"
#include "Structures.h"

// Represents a 3D model that can be moved, rotated and collided with.
class Model
{
public:
#pragma region Constructor/Destructor
	Model(File objectFile, bool collidable, bool flag, Vector3D<float> position, Vector3D<float> scale, Vector3D<float> size);
	~Model();
#pragma endregion
#pragma region Properties (Get)
	bool GetCollidable();
	bool GetFlag();
	File GetObjectFile();
	Vector3D<float> GetPosition();
	Matrix3D<float> GetRotation();
	Vector3D<float> GetScale();
	Vector3D<float> GetSize();
	DemoTexture* GetTexture();
#pragma endregion
#pragma region Properties (Set)
	bool SetCollidable(bool collidable);
	bool SetFlag(bool flag);
	bool SetObjectFile(File objectFile);
	bool SetPosition(Vector3D<float> position);
	bool SetRotation(Matrix3D<float> rotation);
	bool SetScale(Vector3D<float> scale);
	bool SetSize(Vector3D<float> size);
	bool SetTexture(DemoTexture* texture);
#pragma endregion
#pragma region Public Methods
	Matrix3D<float> CreateModelMatrix(bool rotating, bool wKeyDown, bool sKeyDown, bool aKeyDown, bool dKeyDown, float speed);
	void DisposeShape();
	void DisposeTexture();
	void LoadShape(OpenGLContext& openGLContext, File file);
	void LoadTexture(File file);
	void Render(OpenGLContext& openGLContext, Attributes& attributes);
#pragma endregion
private:
#pragma region Members
	// Member variables
	bool p_Collidable;
	bool p_Flag;
	File p_ObjectFile;
	Vector3D<float> p_Position;
	Matrix3D<float> p_Rotation;
	Vector3D<float> p_Scale;
	ScopedPointer<Shape> p_Shape;
	Vector3D<float> p_Size;
	DemoTexture* p_Texture;
	OpenGLTexture p_TextureGL;
#pragma endregion
};

#endif  // MODEL_H_INCLUDED