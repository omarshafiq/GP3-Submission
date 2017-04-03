// Included headers
#include "Model.h"

#pragma region Constructor
// Initializes a new instance of the Model class.
Model::Model(File objectFile, bool collidable, bool flag, Vector3D<float> position, Vector3D<float> scale, Vector3D<float> size) : p_Texture(nullptr)
{
	// Initialize member variables
	p_Collidable = collidable;
	p_Flag = flag;
	p_ObjectFile = objectFile;
	p_Position = position;
	p_Rotation = Matrix3D<float>();
	p_Scale = scale;
	p_Size = Vector3D<float>(size.x * p_Scale.x, size.y * p_Scale.y, size.z * p_Scale.z);
}
#pragma endregion
#pragma region Destructor
// Releases any resources used by this component.
Model::~Model()
{
	// Do nothing
}
#pragma endregion
#pragma region Properties (Get)
// Gets a value indicating whether this model can be collided with.
bool Model::GetCollidable()
{
	return p_Collidable;
}

// Gets a value containing the user-specified flag.
bool Model::GetFlag()
{
	return p_Flag;
}

// Gets the .obj file associated with this model.
File Model::GetObjectFile()
{
	return p_ObjectFile;
}

// Gets the current position of this model.
Vector3D<float> Model::GetPosition()
{
	return p_Position;
}

// Gets the current orientation of this model.
Matrix3D<float> Model::GetRotation()
{
	return p_Rotation;
}

// Gets the current scale of this model.
Vector3D<float> Model::GetScale()
{
	return p_Scale;
}

// Gets the current size of this model.
Vector3D<float> Model::GetSize()
{
	return p_Size;
}

// Gets the texture associated with this model.
DemoTexture* Model::GetTexture()
{
	return p_Texture;
}
#pragma endregion
#pragma region Properties (Set)
// Sets a value indicating whether this model can be collided with.
bool Model::SetCollidable(bool collidable)
{
	if (p_Collidable != collidable)
	{
		p_Collidable = collidable;

		return true;
	}

	return false;
}

// Sets a value containing the user-specified flag.
bool Model::SetFlag(bool flag)
{
	if (p_Flag != flag)
	{
		p_Flag = flag;

		return true;
	}

	return false;
}

// Sets the .obj file associated with this model.
bool Model::SetObjectFile(File objectFile)
{
	if (p_ObjectFile != objectFile)
	{
		p_ObjectFile = objectFile;

		return true;
	}

	return false;
}

// Sets the current position of this model.
bool Model::SetPosition(Vector3D<float> position)
{
	if (p_Position.x != position.x || p_Position.y != position.y || p_Position.z != position.z)
	{
		p_Position = position;

		return true;
	}

	return false;
}

// Sets the current orientation of this model.
bool Model::SetRotation(Matrix3D<float> rotation)
{
	for (int i = 0; i < 16; i++)
	{
		if (p_Rotation.mat[i] != rotation.mat[i])
		{
			p_Rotation = rotation;

			return true;
		}
	}

	return true;
}

// Sets the current scale of this model.
bool Model::SetScale(Vector3D<float> scale)
{
	if (p_Scale.x != scale.x || p_Scale.y != scale.y || p_Scale.z != scale.z)
	{
		p_Scale = scale;

		return true;
	}

	return false;
}

// Sets the current size of this model.
bool Model::SetSize(Vector3D<float> size)
{
	if (p_Size.x != size.x || p_Size.y != size.y || p_Size.z != size.z)
	{
		p_Size = size;

		return true;
	}

	return false;
}

// Sets the texture associated with this model.
bool Model::SetTexture(DemoTexture* texture)
{
	if (p_Texture == nullptr || p_Texture != texture)
	{
		p_Texture = texture;

		return true;
	}

	return false;
}
#pragma endregion
#pragma region Public Methods
// Creates a matrix based on the model's position, rotation and scale
Matrix3D<float> Model::CreateModelMatrix(bool rotating, bool wKeyDown, bool sKeyDown, bool aKeyDown, bool dKeyDown, float speed)
{
	// Create a matrix based on the model's scale
	Matrix3D<float> scaleMatrix;
	scaleMatrix.mat[0] = p_Scale.x;
	scaleMatrix.mat[5] = p_Scale.y;
	scaleMatrix.mat[10] = p_Scale.z;

	// Create a matrix based on this model's position
	Matrix3D<float> positionMatrix(p_Position);

	// Rotate the model, if required
	if (rotating)
	{
		if (wKeyDown) p_Rotation *= Matrix3D<float>().rotated(Vector3D<float>(-speed * 2.5f, 0.0f, 0.0f));
		if (sKeyDown) p_Rotation *= Matrix3D<float>().rotated(Vector3D<float>(speed * 2.5f, 0.0f, 0.0f));
		if (aKeyDown) p_Rotation *= Matrix3D<float>().rotated(Vector3D<float>(0.0f, 0.0f, speed * 2.5f));
		if (dKeyDown) p_Rotation *= Matrix3D<float>().rotated(Vector3D<float>(0.0f, 0.0f, -speed * 2.5f));
	}

	// Multiply the matrices together and return the result
	return scaleMatrix * p_Rotation * positionMatrix;
}

// Disposes the Shape object associated with this model.
void Model::DisposeShape()
{
	p_Shape = nullptr;
}

// Disposes the texture associated with this model.
void Model::DisposeTexture()
{
	p_TextureGL.release();
}

// Loads the Shape object associated with this model.
void Model::LoadShape(OpenGLContext& openGLContext, File file)
{
	p_Shape = new Shape(openGLContext, file);
}

// Loads the texture associated with this model
void Model::LoadTexture(File file)
{
	p_Texture = new TextureFromFile(file);
}

// Renders this model to the specified OpenGL context.
void Model::Render(OpenGLContext& openGLContext, Attributes& attributes)
{
	// Bind the texture to the current OpenGL context
	p_Texture->load(p_TextureGL);
	p_TextureGL.bind();

	// Draw the shape
	p_Shape->draw(openGLContext, attributes);
}
#pragma endregion