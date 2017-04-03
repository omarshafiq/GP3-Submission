#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

// Included headers
#include "../JuceLibraryCode/JuceHeader.h"
#include "Camera.h"
#include "Model.h"
#include "Structures.h"

// This component lives inside our window, and this is where you should put all your controls and content.
class MainContentComponent : public OpenGLAppComponent, public Timer, public KeyListener, public Button::Listener // Inherited classes
{
public:
#pragma region Constructor
	// Initializes a new instance of the MainContentComponent class.
	MainContentComponent() : p_Thread("BackgroundThread") // Initialize background thread
	{
		// Local variables
		bool alternate = false;
		bool flag = false;
		bool found = false;
		bool inGrass = false;
		float scale = 1.0f;

		// Enable multisampling (anti-aliasing)
		openGLContext.setMultisamplingEnabled(true);
		OpenGLPixelFormat format;
		format.multisamplingLevel = 8; // Multiple of two
		openGLContext.setPixelFormat(format);
		openGLContext.setSwapInterval(0);

		// Create a new device manager object,
		// this stores information about input and output audio devices.
		p_DeviceManager = new AudioDeviceManager();
		p_DeviceManager->initialiseWithDefaultDevices(0, 2);

		// Create a new audio device selector component,
		// this allows the user to select their audio device.
		p_DeviceSelectorComponent = new AudioDeviceSelectorComponent(*p_DeviceManager, 0, 256, 0, 256, false, false, false, true);
		addAndMakeVisible(p_DeviceSelectorComponent);
		p_DeviceSelectorComponent->setVisible(false);

		// Add the Show Device Settings button to our component
		addAndMakeVisible(p_SettingsButton);
		p_SettingsButton.setButtonText("Show Device Settings");
		p_SettingsButton.setColour(ToggleButton::textColourId, Colours::white);
		p_SettingsButton.addListener(this);

		// Add the Mute Background Music button to our component
		addAndMakeVisible(p_MuteButton);
		p_MuteButton.setButtonText("Mute Background Music");
		p_MuteButton.setColour(ToggleButton::textColourId, Colours::white);
		p_MuteButton.addListener(this);

		// Add the Bird's Eye View button to our component
		addAndMakeVisible(p_BirdsEyeButton);
		p_BirdsEyeButton.setButtonText("Bird's Eye View (C)");
		p_BirdsEyeButton.setColour(ToggleButton::textColourId, Colours::white);
		p_BirdsEyeButton.addListener(this);

		// Audio setup
		p_AudioSourcePlayer.setSource(&p_TransportSource);
		p_FormatManager.registerBasicFormats();
		p_Thread.startThread(3);

		// Add audio callback
		p_DeviceManager->addAudioCallback(&p_AudioSourcePlayer);

		// Listen for keyStateChanged/keyPressed events
		addKeyListener(this);

		// Create starting line flag at the origin point (0, 0, 0)
		Model* flag_start = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/flag.obj"), false, false, Vector3D<float>(0.0f, 0.0f, 0.0f), Vector3D<float>(1.0f, 1.0f, 1.0f), Vector3D<float>(1.0f, 0.3f, 0.05f));
		flag_start->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/flag_start.jpg"));
		p_Models.add(flag_start);

		// Create finish line flag at the end point (0, 0, -100)
		Model* flag_finish = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/flag.obj"), false, false, Vector3D<float>(0.0f, 0.0f, -100.0f), Vector3D<float>(1.0f, 1.0f, 1.0f), Vector3D<float>(1.0f, 0.3f, 0.05f));
		flag_finish->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/flag_finish.jpg"));
		p_Models.add(flag_finish);

		// Create an array of collidable rock models with random locations, sizes and orientations
		for (float x = -3.8f; x <= 3.8f; x += 0.25f)
		{
			for (float z = 0.0f; z >= -200.0f; z -= Random::getSystemRandom().nextInt(50))
			{
				found = false;

				for (int i = 0; i < p_Models.size(); i++)
				{
					if (p_Models[i]->GetPosition().x == x && p_Models[i]->GetPosition().z == z)
					{
						found = true;
						break;
					}
				}

				if (!found && z < 0.0f)
				{
					scale = Random::getSystemRandom().nextFloat() * 3.0f;
					float y = Random::getSystemRandom().nextFloat() * 3.14f;

					Model* rock = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/rock.obj"), true, false, Vector3D<float>(x, 0.0f, z), Vector3D<float>(scale, scale, scale), Vector3D<float>(0.05f, 0.05f, 0.05f));
					rock->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/rock.jpg"));
					rock->SetRotation(Matrix3D<float>().rotated(Vector3D<float>(0.0f, y, 0.0f)));
					p_Models.add(rock);
				}
			}
		}

		// Create an array of collidable tree models with random locations and sizes
		for (float x = -3.8f; x <= 3.8f; x += 0.25f)
		{
			for (float z = 0.0f; z >= -200.0f; z -= Random::getSystemRandom().nextInt(50))
			{
				found = false;
				inGrass = x <= -1.0f || x > 1.0f;
				scale = Random::getSystemRandom().nextFloat() * 2.0f;

				for (int i = 0; i < p_Models.size(); i++)
				{
					if (p_Models[i]->GetPosition().x == x && p_Models[i]->GetPosition().z == z)
					{
						found = true;
						break;
					}
				}

				if (!found && z < 0.0f && inGrass)
				{
					Model* tree = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/tree.obj"), true, false, Vector3D<float>(x, 0.0f, z), Vector3D<float>(scale, scale, scale), Vector3D<float>(0.1f, 0.3f, 0.1f));
					tree->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/tree.jpg"));
					p_Models.add(tree);
				}
			}
		}

		// Create an array of non-collidable grass models with random locations and sizes
		for (float x = -3.8f; x <= 3.8f; x += 0.1f)
		{
			for (float z = 0.0f; z >= -200.0f; z -= Random::getSystemRandom().nextInt(50))
			{
				found = false;
				inGrass = x <= -1.0f || x > 1.0f;
				scale = Random::getSystemRandom().nextFloat() * 3.0f;

				for (int i = 0; i < p_Models.size(); i++)
				{
					if (p_Models[i]->GetPosition().x == x && p_Models[i]->GetPosition().z == z)
					{
						found = true;
						break;
					}
				}

				if (!found && z < 0.0f && inGrass)
				{
					Model* grass = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/grass.obj"), false, false, Vector3D<float>(x, 0.0f, z), Vector3D<float>(scale, scale, scale), Vector3D<float>(0.05f, 0.05f, 0.0f));
					grass->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/grass.png"));
					p_Models.add(grass);
				}
			}
		}

		// Create an array of collidable bench models with fixed locations and sizes
		for (float x = -0.4f; x <= 0.4f; x += 0.8f)
		{
			for (float z = 5.0f; z >= -100.0f; z -= 10.0f)
			{
				alternate = !alternate;

				// Alternate the position
				if (alternate && z < 5.0f)
				{
					flag = !flag;

					Model* bench = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/bench.obj"), true, flag, Vector3D<float>(x, 0.0f, z), Vector3D<float>(1.0f, 1.0f, 1.0f), Vector3D<float>(0.3f, 0.2f, 0.1f));
					bench->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/bench.jpg"));
					p_Models.add(bench);
				}
			}
		}

		// Create an array of road models with fixed locations and sizes
		for (float z = 0.0f; z >= -200.0f; z -= 8.0f)
		{
			Model* road = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/road.obj"), false, false, Vector3D<float>(0.0f, 0.0f, z), Vector3D<float>(1.0f, 1.0f, 1.0f), Vector3D<float>(2.0f, 0.0f, 2.0f));
			road->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/road.jpg"));
			p_Roads.add(road);
		}

		// Create the sphere model
		p_Sphere = new Model(File::getCurrentWorkingDirectory().getChildFile("Resources/sphere.obj"), false, false, Vector3D<float>(0.0f, 0.1f, 1.0f), Vector3D<float>(1.0f, 1.0f, 1.0f), Vector3D<float>(0.1f, 0.1f, 0.1f));
		p_Sphere->LoadTexture(File::getCurrentWorkingDirectory().getChildFile("Resources/sphere.jpg"));

		// Initialize member variables
		p_Camera = new Camera(Vector3D<float>(0.0f, -1.0f, 6.0f), Vector3D<float>(0.0f, 0.0f, 0.0f));
		p_Countdown = 60;
		p_Speed = 0.0f;
		p_WKeyDown = false;
		p_SKeyDown = false;
		p_AKeyDown = false;
		p_DKeyDown = false;
	}
#pragma endregion
#pragma region Destructor
	// Releases any resources used by this component.
	~MainContentComponent()
	{
		// Remove audio callback
		p_DeviceManager->removeAudioCallback(&p_AudioSourcePlayer);

		// Stop audio playback
		p_TransportSource.stop();

		// Dispose audio components
		p_TransportSource.setSource(nullptr);
		p_AudioSourcePlayer.setSource(nullptr);
		p_CurrentAudioFileSource = nullptr;

		// Delete device manager and device selector component
		p_DeviceSelectorComponent = nullptr;
		p_DeviceManager = nullptr;

		// Shutdown OpenGL
		shutdownOpenGL();
	}
#pragma endregion
#pragma region Overrides
	// Occurs when a button is clicked by the user.
	void buttonClicked(Button* button) override
	{
		if (button == &p_SettingsButton) // Show Device Settings button clicked
		{
			// Toggle visibility of the device selector component
			p_DeviceSelectorComponent->setVisible(button->getToggleState());
		}
		else if (button == &p_MuteButton) // Mute Background Music button clicked
		{
			// Set the output gain (0.0 = 0%, 1.0 = 100%)
			if (button->getToggleState()) p_TransportSource.setGain(0.0f);
			else p_TransportSource.setGain(1.0f);
		}
		else if (button == &p_BirdsEyeButton) // Bird's Eye View button clicked
		{
			// Do nothing
		}
	}

	// Use this method to initialize any OpenGL properties/components
	void initialise() override
	{
		CreateShaders();
	}

	// This event occurs when any key is pressed or released. 
	bool keyStateChanged(bool, Component*) override
	{
		// Determine which keys are currently pressed
		p_WKeyDown = KeyPress::isKeyCurrentlyDown(119);
		p_SKeyDown = KeyPress::isKeyCurrentlyDown(115);
		p_AKeyDown = KeyPress::isKeyCurrentlyDown(97);
		p_DKeyDown = KeyPress::isKeyCurrentlyDown(100);

		return true;
	}

	// This event is raised to indicate that a key has been pressed.
	bool keyPressed(const KeyPress& key, Component*) override
	{
		// Escape key pressed
		if (key == key.escapeKey)
		{
			// Exit the application
			JUCEApplication::getInstance()->systemRequestedQuit();
		}
		else if (key == key.createFromDescription("C"))
		{
			// Toggle the Bird's Eye View button
			p_BirdsEyeButton.setToggleState(!p_BirdsEyeButton.getToggleState(), dontSendNotification);
		}

		return true;
	}

	// You can add your component specific drawing code here!
	// This will draw over the top of the OpenGL background.
	void paint(Graphics& g) override
	{
		// Local variables
		Rectangle<int> area(getLocalBounds());
		String statusText = "";

		// Set graphics colour and font
		g.setColour(Colours::white);
		g.setFont(75);

		if (!isTimerRunning())
		{
			switch (p_Countdown)
			{
			case 0:
				statusText = "You lose";
				break;
			case 60:
				statusText = "Use WASD to move";

				// Send the component to the front of the z-order,
				// this also sends keyboard focus to the component.
				toFront(true);
				break;
			default:
				statusText = "You win!";
				break;
			}
		}

		// Draw status text
		if (statusText != "") g.drawText(statusText, Rectangle<float>(20.0f, 10.0f, area.getWidth() - 40.0f, 100.0f), Justification::centred);

		// Draw countdown
		g.drawText(String(p_Countdown), Rectangle<float>(20.0f, 10.0f, area.getWidth() - 40.0f, 100.0f), Justification::topRight);
	}

	// This event occurs each time a frame is rendered by OpenGL.
	void render() override
	{
		// Local variables
		Rectangle<int> area(getLocalBounds());

		// Update sphere position and determine if it is rolling
		bool rotating = RollSphere();

		// Determine if the OpenGL context is valid
		jassert(OpenGLHelpers::isContextActive());

		// Clear the OpenGL context background
		OpenGLHelpers::clear(Colours::dodgerblue);

		// Having used the Juce 2D renderer, it will have messed-up a whole load of GL state, so
		// we need to initialise some important settings before doing our normal GL 3D drawing.
		
		// Enable alpha testing
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_ALWAYS, 0);

		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Activate the first texture state
		openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);

		// Enable texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Select the shader program
		p_Shader->use();

		// Initialize our uniform values to be sent to the shader
		if (p_Uniforms->projectionMatrix != nullptr) p_Uniforms->projectionMatrix->setMatrix4(CreateProjectionMatrix().mat, 1, false);
		if (p_Uniforms->viewMatrix != nullptr) p_Uniforms->viewMatrix->setMatrix4(p_Camera->CreateViewMatrix().mat, 1, false);
		if (p_Uniforms->texture != nullptr) p_Uniforms->texture->set((GLint)0);

		// Render the sphere
		if (p_Uniforms->modelMatrix != nullptr) p_Uniforms->modelMatrix->setMatrix4(p_Sphere->CreateModelMatrix(rotating, p_WKeyDown, p_SKeyDown, p_AKeyDown, p_DKeyDown, p_Speed).mat, 1, false);
		p_Sphere->Render(openGLContext, *p_Attributes);

		// Render the roads
		for (int i = 0; i < p_Roads.size(); i++)
		{
			// Don't draw roads that are too close or too far from the camera,
			// to prevent clipping and improve rendering speed/performance.
			if (p_Roads[i]->GetPosition().z < p_Sphere->GetPosition().z + 6.0f && p_Roads[i]->GetPosition().z > p_Sphere->GetPosition().z - 79.4f)
			{
				if (p_Uniforms->modelMatrix != nullptr) p_Uniforms->modelMatrix->setMatrix4(p_Roads[i]->CreateModelMatrix(false, false, false, false, false, p_Speed).mat, 1, false);
				p_Roads[i]->Render(openGLContext, *p_Attributes);
			}
		}

		// Render the models
		for (int i = 0; i < p_Models.size(); i++)
		{
			// Don't draw models that are too close or too far from the camera,
			// to prevent clipping and improve rendering speed/performance.
			if (p_Models[i]->GetPosition().z < p_Sphere->GetPosition().z + 2.0f && p_Models[i]->GetPosition().z > p_Sphere->GetPosition().z - 79.4f)
			{
				if (p_Uniforms->modelMatrix != nullptr) p_Uniforms->modelMatrix->setMatrix4(p_Models[i]->CreateModelMatrix(false, false, false, false, false, p_Speed).mat, 1, false);
				p_Models[i]->Render(openGLContext, *p_Attributes);
			}
		}

		// Reset the element buffers so child components are drawn correctly
		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	void resized() override
	{
		// Local variables
		Rectangle<int> area(getLocalBounds());

		// Update component locations and sizes
		if (p_DeviceSelectorComponent != nullptr) p_DeviceSelectorComponent->setBounds(20, 120, area.getWidth() - 40, area.getHeight() - 240);
		p_SettingsButton.setBounds(20, 20, 180, 24);
		p_MuteButton.setBounds(20, 56, 180, 24);
		p_BirdsEyeButton.setBounds(20, 92, 180, 24);
	}

	// Frees any OpenGL objects that were created during rendering. 
	void shutdown() override
	{
		// Delete shader components
		p_Shader = nullptr;
		p_Attributes = nullptr;
		p_Uniforms = nullptr;

		// Stop the timer
		stopTimer();

		// Dispose model shapes and textures
		for (int i = 0; i < p_Models.size(); i++)
		{
			p_Models[i]->DisposeShape();
			p_Models[i]->DisposeTexture();
		}

		// Dispose road shapes and textures
		for (int i = 0; i < p_Roads.size(); i++)
		{
			p_Roads[i]->DisposeShape();
			p_Roads[i]->DisposeTexture();
		}

		// Dispose sphere shape and texture
		p_Sphere->DisposeShape();
		p_Sphere->DisposeTexture();
	}

	// This event occurs each time the timer ticks (once per-second).
	void timerCallback()
	{
		if (p_Countdown > 0)
		{
			// Decrease countdown value by one
			p_Countdown--;

			// Re-draw the window
			repaint();
		}
		else
		{
			// End the current game
			StopGame();
		}
	}
#pragma endregion
#pragma region Public Methods
	// Creates a matrix that represents the 3D view frustum.
	Matrix3D<float> CreateProjectionMatrix() const
	{
		// Calculate frustum size based on the window size
		float w = 1.0f / (0.5f + 0.1f);
		float h = w * getLocalBounds().toFloat().getAspectRatio(false);

		return Matrix3D<float>::fromFrustum(-w, w, -h, h, 4.0f, 100.0f);
	}

	// Compiles the GLSL vertex and fragment shaders.
	void CreateShaders()
	{
		p_VertexShader =
			"attribute vec4 position;\n"
			"attribute vec3 normal;\n"
			"attribute vec2 texureCoordIn;\n"
			"\n"
			"uniform mat4 projectionMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 modelMatrix;\n"
			"\n"
			"varying vec3 f_normal;\n"
			"varying vec3 f_position;\n"
			"varying vec2 f_texcoord;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;\n"
			"    mat3 normalMatrix = mat3(mvp);\n"
			"    normalMatrix = inverse(normalMatrix);\n"
			"\n"
			"    f_normal = normalize(normal * normalMatrix);\n"
			"    f_position = vec3(mvp * position);\n"
			"    f_texcoord = texureCoordIn;\n"
			"\n"
			"    gl_Position = mvp * position;\n"
			"}\n";

		p_FragmentShader =
			"varying vec3 f_normal;\n"
			"varying vec3 f_position;\n"
			"varying vec2 f_texcoord;\n"
			"\n"
			"uniform sampler2D demoTexture;\n"
			"\n"
			"vec3 sun = vec3(0.5, 1.0, -5.0);\n"
			"\n"
			"void main()\n"
			"{\n"
			"    vec3 light = normalize(sun - f_position);\n"
			"    light = max(dot(f_normal, light), 0.0) * vec3(1.0, 1.0, 1.0);\n"
			"    gl_FragColor = texture(demoTexture, f_texcoord) * vec4(light, 1.0);\n"
			"}\n";

		//
		ScopedPointer<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));

		//
		if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(p_VertexShader))
			&& newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(p_FragmentShader))
			&& newShader->link())
		{
			// Dispose model shapes
			for (int i = 0; i < p_Models.size(); i++)
			{
				p_Models[i]->DisposeShape();
			}

			// Dispose road shapes
			for (int i = 0; i < p_Roads.size(); i++)
			{
				p_Roads[i]->DisposeShape();
			}

			// Dispose sphere shape
			p_Sphere->DisposeShape();

			p_Attributes = nullptr;
			p_Uniforms = nullptr;

			p_Shader = newShader;
			p_Shader->use();

			// Re-load model shapes
			for (int i = 0; i < p_Models.size(); i++)
			{
				p_Models[i]->LoadShape(openGLContext, p_Models[i]->GetObjectFile());
			}

			// Re-load road shapes
			for (int i = 0; i < p_Roads.size(); i++)
			{
				p_Roads[i]->LoadShape(openGLContext, p_Roads[i]->GetObjectFile());
			}

			// Re-load sphere shape
			p_Sphere->LoadShape(openGLContext, p_Sphere->GetObjectFile());

			p_Attributes = new Attributes(openGLContext, *p_Shader);
			p_Uniforms = new Uniforms(openGLContext, *p_Shader);
		}
	}

	// Loads an audio file and attaches it to our transport source.
	void LoadFileIntoTransport(File file)
	{
		// Unload the previous file source and delete it
		p_TransportSource.stop();
		p_TransportSource.setSource(nullptr);
		p_CurrentAudioFileSource = nullptr;

		// Create a new audio format reader
		AudioFormatReader* reader = p_FormatManager.createReaderFor(file);

		if (reader != nullptr)
		{
			// Create a new audio file source
			p_CurrentAudioFileSource = new AudioFormatReaderSource(reader, true);

			// And plug it into our transport source
			p_TransportSource.setSource(p_CurrentAudioFileSource,
				32768, // Tells it to buffer this many samples ahead
				&p_Thread, // This is the background thread to use for reading-ahead
				reader->sampleRate); // Allows for sample rate correction
		}
	}

	// Updates the position and rotation of the sphere when the WASD keys are pressed.
	bool RollSphere()
	{
		// Local variables
		bool rotating = false;

		// Sphere position
		float pX = p_Sphere->GetPosition().x;
		float pY = p_Sphere->GetPosition().y;
		float pZ = p_Sphere->GetPosition().z;

		// Sphere size
		float sX = p_Sphere->GetSize().x;
		float sZ = p_Sphere->GetSize().z;

		// Determine sphere position
		if (p_Countdown > 0 && pZ > -100.0f)
		{
			// W key is being pressed, move the sphere forward (-Z axis)
			if (p_WKeyDown)
			{
				rotating = true;

				// Determine if rolling on road or grass and adjust speed
				if (pX > -1.0f && pX < 1.0f)
				{
					p_Speed = 0.1f;
					pZ -= p_Speed;
				}
				else
				{
					p_Speed = 0.05f;
					pZ -= p_Speed;
				}
			}

			// S key is being pressed, move the sphere backward (+Z axis)
			if (p_SKeyDown)
			{
				rotating = true;

				// Determine if rolling on road or grass and adjust speed
				if (pX > -1.0f && pX < 1.0f)
				{
					p_Speed = 0.1f;
					pZ += p_Speed;
				}
				else
				{
					p_Speed = 0.05f;
					pZ += p_Speed;
				}
			}

			// A key is being pressed, move the sphere left (-X axis)
			if (p_AKeyDown)
			{
				rotating = true;

				// Determine if rolling on road or grass and adjust speed
				if (pX > -1.0f && pX < 1.0f)
				{
					p_Speed = 0.1f;
					pX -= p_Speed;
				}
				else
				{
					p_Speed = 0.05f;
					pX -= p_Speed;
				}
			}

			// D key is being pressed, move the sphere right (+X axis)
			if (p_DKeyDown)
			{
				rotating = true;

				// Determine if rolling on road or grass and adjust speed
				if (pX > -1.0f && pX < 1.0f)
				{
					p_Speed = 0.1f;
					pX += p_Speed;
				}
				else
				{
					p_Speed = 0.05f;
					pX += p_Speed;
				}
			}

			if (pZ <= 0.0f && pZ > -100.0f) // Sphere has crossed the starting line
			{
				// Start a new game
				StartGame();
			}
			else if (pZ <= -100.0f) // Sphere has crossed the finish line
			{
				// End the current game
				StopGame();
			}

			// Loop through bench models and animate them
			for (int i = 0; i < p_Models.size(); i++)
			{
				if (p_Models[i]->GetObjectFile().getFileName() == "bench.obj")
				{
					float x = p_Models[i]->GetPosition().x;

					if (p_Models[i]->GetFlag()) // Move model left
					{
						x -= 0.05f;

						if (x < -0.75f) p_Models[i]->SetFlag(false);
					}
					else // Move model right
					{
						x += 0.05f;

						if (x > 0.75f) p_Models[i]->SetFlag(true);
					}

					// Update the model position
					p_Models[i]->SetPosition(Vector3D<float>(x, p_Models[i]->GetPosition().y, p_Models[i]->GetPosition().z));
				}
			}

			// Loop through all models and detect collision
			for (int i = 0; i < p_Models.size(); i++)
			{
				if (p_Models[i]->GetCollidable()) // Model can be collided with
				{
					// Determine if the sphere is contained within the bounding box of the model
					if (pX + sX > p_Models[i]->GetPosition().x - p_Models[i]->GetSize().x // Collision on -X axis
						&& pX - sX < p_Models[i]->GetPosition().x + p_Models[i]->GetSize().x // Collision on +X axis
						&& pZ + sZ > p_Models[i]->GetPosition().z - p_Models[i]->GetSize().z // Collision on -Z axis
						&& pZ - sZ < p_Models[i]->GetPosition().z + p_Models[i]->GetSize().z) // Collision on +Z axis
					{
						// Re-position the sphere to prevent clipping
						if (pZ > p_Models[i]->GetPosition().z) pZ = (p_Models[i]->GetPosition().z + p_Models[i]->GetSize().z) + sZ;
						else pZ = (p_Models[i]->GetPosition().z - p_Models[i]->GetSize().z) - sZ;

						// Allow the sphere to rotate if A or D keys are being pressed
						rotating = p_AKeyDown || p_DKeyDown;
						break;
					}
				}
			}

			// Update the sphere position
			p_Sphere->SetPosition(Vector3D<float>(pX, pY, pZ));

			// Move the camera to follow the sphere
			if (p_BirdsEyeButton.getToggleState())
			{
				// Move the camera above the sphere
				p_Camera->SetPosition(Vector3D<float>(-pX, pZ, -6.0f));
				p_Camera->SetRotation(Vector3D<float>(1.57f, 0.0f, 0.0f));
			}
			else
			{
				// Move the camera behind the sphere
				p_Camera->SetPosition(Vector3D<float>(-pX, -1.0f, -pZ - 5.0f));
				p_Camera->SetRotation(Vector3D<float>(0.0f, 0.0f, 0.0f));
			}
		}

		// Return a value indicating whether the sphere is rolling
		return rotating;
	}

	// Starts a new game.
	void StartGame()
	{
		// Determine if the timer is already running
		if (!isTimerRunning())
		{
			// Load wav file into transport
			LoadFileIntoTransport(File::getCurrentWorkingDirectory().getChildFile("Resources/background.wav"));

			// Start audio playback
			p_TransportSource.setPosition(0);
			p_TransportSource.start();

			// Start the timer
			startTimer(1000);

			// Re-draw the window
			repaint();
		}
	}

	// Ends the current game.
	void StopGame()
	{
		// Determine if the timer is already running
		if (isTimerRunning())
		{
			// Stop audio playback
			p_TransportSource.stop();

			// Dispose audio components
			p_TransportSource.setSource(nullptr);
			p_AudioSourcePlayer.setSource(nullptr);
			p_CurrentAudioFileSource = nullptr;

			// Stop the timer
			stopTimer();

			// Re-draw the window
			repaint();
		}
	}
#pragma endregion
private:
#pragma region Members
	// Member variables
	const char* p_VertexShader;
	const char* p_FragmentShader;
	ScopedPointer<OpenGLShaderProgram> p_Shader;
	ScopedPointer<Attributes> p_Attributes;
	ScopedPointer<Uniforms> p_Uniforms;

	ScopedPointer<AudioDeviceManager> p_DeviceManager;
	ScopedPointer<AudioDeviceSelectorComponent> p_DeviceSelectorComponent;

	ToggleButton p_SettingsButton;
	ToggleButton p_MuteButton;
	ToggleButton p_BirdsEyeButton;

	AudioSourcePlayer p_AudioSourcePlayer;
	AudioTransportSource p_TransportSource;
	ScopedPointer<AudioFormatReaderSource> p_CurrentAudioFileSource;
	AudioFormatManager p_FormatManager;
	TimeSliceThread p_Thread;

	Array<Model*> p_Models;
	Array<Model*> p_Roads;
	ScopedPointer<Model> p_Sphere;
	ScopedPointer<Camera> p_Camera;

	int p_Countdown;
	float p_Speed;

	bool p_WKeyDown;
	bool p_SKeyDown;
	bool p_AKeyDown;
	bool p_DKeyDown;
#pragma endregion
#pragma region Declares
	// Add this class to the Juce leak detector
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
#pragma endregion
};

Component* createMainContentComponent();

#endif  // MAINCOMPONENT_H_INCLUDED