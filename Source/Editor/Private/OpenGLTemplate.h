#include "CameraComponent.h"
#include "ECS.h"
#include "IndexBuffer.h"
#include "Logging/LoggingMacros.h"
#include "MathLibrary.h"
#include "OpenGL/OpenGLHeader.h"
#include "OpenGL/OpenGLRenderer.h"
#include "OpenGL/OpenGLShader.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <iostream>

void onKeyPressed(GLFWwindow* window, int key, int scanCode, int action, int mode)
{
	std::cout << "Key pressed [" << key << "]" << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		std::cout << "Escape pressed. Window should close." << std::endl;
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

bool isGLError()
{
	while (GLenum error = glGetError())
	{
		std::cout << "GL error [" << error << "]" << std::endl;
		return true;
	}
	return false;
}

void OpenGLTemplate()
{
	std::cout << "Initializing GLFW" << std::endl;
	if (!glfwInit())
	{
		fprintf(stderr, "Could not initialize GLFW\n");
		return;
	}

	std::cout << "Setting GLFW window hints" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	std::cout << "Creating GLFW window" << std::endl;
	GLFWwindow* window = glfwCreateWindow(1080, 720, "Untitled", nullptr, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Could not create GLFW window\n");
		glfwTerminate();
		return;
	}

	std::cout << "Configuring GLFW window" << std::endl;
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, onKeyPressed);

	std::cout << "Initializing GLAD" << std::endl;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Could not initialize GLAD\n");
		return;
	}

	std::cout << "Graphics context:" << std::endl;
	std::cout << "-- Vendor " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "-- Renderer " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "-- Version " << glGetString(GL_VERSION) << std::endl;
	//Declaring buffer parameters
	GLuint	vao, vbo, ebo;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Declares the Vertex Array, where the coordinates of all the 8 cube vertices are stored
	static GLfloat vertices[] = {
		1.0,	1.0,	1.0,
		0.0f,	1.0,	1.0,
		1.0,	1.0,	0.0f,
		0.0f,	1.0,	0.0f,
		1.0,	0.0f,	1.0,
		0.0f,	0.0f,	1.0,
		0.0f,	0.0f,	0.0f,
		1.0,	0.0f,	0.0f
	};

	// Declares the Elements Array, where the indexs to be drawn are stored
	static GLuint elements[] = {
		3, 2, 6, 7, 4, 2, 0,
		3, 1, 6, 5, 4, 1, 0
	};

	GLuint elementCount = sizeof(elements) / sizeof(GLuint);

	// OpenGL set up:
	// Creates and binds Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Creates and binds Vertex Buffer Object (24 is the elements array size)
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Creates and bind Elements Buffer Object (14 is the elements array size)
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 14 * sizeof(GLuint), elements, GL_STATIC_DRAW);

	std::string vertexSource = R"(
			#version 330 core

			uniform mat4 MVP;

			layout(location = 0) in vec3 in_position;

			out Vertex {
				vec4 color;
			} out_vertex;

			void main()
			{
				out_vertex.color = vec4(0.2f, 0.5f, 1.0f, 1.0f);
				gl_Position = MVP * vec4(in_position, 1.0);
			}
		)";

	std::string fragmentSource = R"(
			#version 330 core

			in Vertex {
				vec4 color;
			} in_vertex;

			out vec4 out_color;

			void main()
			{
				out_color = in_vertex.color;
			}
		)";

	Bloodshot::FOpenGLShader shader("Default", vertexSource, fragmentSource);
	shader.Bind();

	float angle = 0;

	glm::mat4 View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -1.5f));
	glm::mat4 Projection = glm::perspective(60.f, 1920.f / 1080.f, 0.1f, 100.f);


	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		angle++;
		glm::mat4 Model = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0, 1, 0));

		shader.SetUniformMat4("MVP", Model* View* Projection);

		// Call to OpenGL draw function. (14 is the elements array size)
		glDrawElements(GL_TRIANGLE_STRIP, elementCount, GL_UNSIGNED_INT, 0);

		if (isGLError())
		{
			fprintf(stderr, "Could not render\n");
			return;
		}

		glfwSwapBuffers(window);
	}

	std::cout << "Destroying window" << std::endl;
	glfwDestroyWindow(window);
	std::cout << "Terminating GLFW" << std::endl;
	glfwTerminate();
	return;
}