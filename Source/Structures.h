#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

// Included headers
#include "../JuceLibraryCode/JuceHeader.h"
#include "WavefrontObjParser.h"

// Vertex data to be passed to the shaders
struct Vertex
{
	float position[3];
	float normal[3];
	float colour[4];
	float texCoord[2];
};

// This structure just manages the attributes that the demo shaders use
struct Attributes
{
	Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shader)
	{
		position = createAttribute(openGLContext, shader, "position");
		normal = createAttribute(openGLContext, shader, "normal");
		sourceColour = createAttribute(openGLContext, shader, "sourceColour");
		texureCoordIn = createAttribute(openGLContext, shader, "texureCoordIn");
	}

	void enable(OpenGLContext& openGLContext)
	{
		if (position != nullptr)
		{
			openGLContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
			openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);
		}

		if (normal != nullptr)
		{
			openGLContext.extensions.glVertexAttribPointer(normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
			openGLContext.extensions.glEnableVertexAttribArray(normal->attributeID);
		}

		if (sourceColour != nullptr)
		{
			openGLContext.extensions.glVertexAttribPointer(sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
			openGLContext.extensions.glEnableVertexAttribArray(sourceColour->attributeID);
		}

		if (texureCoordIn != nullptr)
		{
			openGLContext.extensions.glVertexAttribPointer(texureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 10));
			openGLContext.extensions.glEnableVertexAttribArray(texureCoordIn->attributeID);
		}
	}

	void disable(OpenGLContext& openGLContext)
	{
		if (position != nullptr)       openGLContext.extensions.glDisableVertexAttribArray(position->attributeID);
		if (normal != nullptr)         openGLContext.extensions.glDisableVertexAttribArray(normal->attributeID);
		if (sourceColour != nullptr)   openGLContext.extensions.glDisableVertexAttribArray(sourceColour->attributeID);
		if (texureCoordIn != nullptr)  openGLContext.extensions.glDisableVertexAttribArray(texureCoordIn->attributeID);
	}

	ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, texureCoordIn;

private:
	static OpenGLShaderProgram::Attribute* createAttribute(OpenGLContext& openGLContext,
		OpenGLShaderProgram& shader,
		const char* attributeName)
	{
		if (openGLContext.extensions.glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
			return nullptr;

		return new OpenGLShaderProgram::Attribute(shader, attributeName);
	}
};

// This structure just manages the uniform values that the demo shaders use
struct Uniforms
{
	Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shader)
	{
		projectionMatrix = createUniform(openGLContext, shader, "projectionMatrix");
		viewMatrix = createUniform(openGLContext, shader, "viewMatrix");
		modelMatrix = createUniform(openGLContext, shader, "modelMatrix");
		texture = createUniform(openGLContext, shader, "demoTexture");
	}

	ScopedPointer<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix, modelMatrix, texture;

private:
	static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext, OpenGLShaderProgram& shader, const char* uniformName)
	{
		if (openGLContext.extensions.glGetUniformLocation(shader.getProgramID(), uniformName) < 0)
			return nullptr;

		return new OpenGLShaderProgram::Uniform(shader, uniformName);
	}
};

// This loads a 3D model from an OBJ file and converts it into some vertex buffers that we can draw
struct Shape
{
	Shape(OpenGLContext& openGLContext, File file)
	{
		Result result = shapeFile.load(file);
		//Result result = shapeFile.load(BinaryData::teapot_obj);

		if (result.wasOk())
			for (int i = 0; i < shapeFile.shapes.size(); ++i)
				vertexBuffers.add(new VertexBuffer(openGLContext, *shapeFile.shapes.getUnchecked(i)));

	}

	void draw(OpenGLContext& openGLContext, Attributes& attributes)
	{
		for (int i = 0; i < vertexBuffers.size(); ++i)
		{
			VertexBuffer& vertexBuffer = *vertexBuffers.getUnchecked(i);
			vertexBuffer.bind();

			attributes.enable(openGLContext);
			glDrawElements(GL_TRIANGLES, vertexBuffer.numIndices, GL_UNSIGNED_INT, 0);
			attributes.disable(openGLContext);
		}
	}

private:
	struct VertexBuffer
	{
		VertexBuffer(OpenGLContext& context, WavefrontObjFile::Shape& shape) : openGLContext(context)
		{
			numIndices = shape.mesh.indices.size();

			openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
			openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

			Array<Vertex> vertices;
			createVertexListFromMesh(shape.mesh, vertices, Colours::green);

			openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, vertices.size() * (int) sizeof(Vertex),
				vertices.getRawDataPointer(), GL_STATIC_DRAW);

			openGLContext.extensions.glGenBuffers(1, &indexBuffer);
			openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * (int) sizeof(juce::uint32),
				shape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
		}

		~VertexBuffer()
		{
			openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
			openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
		}

		void bind()
		{
			openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		}

		GLuint vertexBuffer, indexBuffer;
		int numIndices;
		OpenGLContext& openGLContext;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexBuffer)
	};

	WavefrontObjFile shapeFile;
	OwnedArray<VertexBuffer> vertexBuffers;

	static void createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour)
	{
		const float scale = 0.2f;
		WavefrontObjFile::TextureCoord defaultTexCoord = { 0.5f, 0.5f };
		WavefrontObjFile::Vertex defaultNormal = { 0.5f, 0.5f, 0.5f };

		for (int i = 0; i < mesh.vertices.size(); ++i)
		{
			const WavefrontObjFile::Vertex& v = mesh.vertices.getReference(i);

			const WavefrontObjFile::Vertex& n
				= i < mesh.normals.size() ? mesh.normals.getReference(i) : defaultNormal;

			const WavefrontObjFile::TextureCoord& tc
				= i < mesh.textureCoords.size() ? mesh.textureCoords.getReference(i) : defaultTexCoord;

			Vertex vert =
			{
				{ scale * v.x, scale * v.y, scale * v.z, },
				{ scale * n.x, scale * n.y, scale * n.z, },
				{ colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
				{ tc.x, tc.y }
			};

			list.add(vert);
		}
	}
};

// These classes are used to load textures from the various sources that the demo uses
struct DemoTexture
{
	virtual ~DemoTexture() {}
	virtual bool load(OpenGLTexture& texture) = 0;

	String name;

	static Image resizeImageToPowerOfTwo(Image image)
	{
		if (!(isPowerOfTwo(image.getWidth()) && isPowerOfTwo(image.getHeight())))
			return image.rescaled(jmin(1024, nextPowerOfTwo(image.getWidth())),
				jmin(1024, nextPowerOfTwo(image.getHeight())));

		return image;
	}
};

// Represents a texture loaded from a file
struct TextureFromFile : public DemoTexture
{
	TextureFromFile(const File& file)
	{
		name = file.getFileName();
		image = resizeImageToPowerOfTwo(ImageFileFormat::loadFrom(file));
	}

	Image image;

	bool load(OpenGLTexture& texture) override
	{
		texture.loadImage(image);
		return true;
	}
};

#endif  // STRUCTURES_H_INCLUDED