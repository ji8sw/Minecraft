#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Helper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow* Window = NULL;
unsigned int VBO, VAO, EBO;
unsigned int VertexShader;
unsigned int FragmentShader;
unsigned int ShaderProgram;
bool WireframeMode = false;
int ExistingTextures = 0;
glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float DeltaTime = 0.0f;
float LastFrame = 0.0f;
float CursorX = 1920 / 2, CursorY = 1080 / 2;
bool FirstMouse = true;
float CameraYaw = -90.0f, CameraPitch = 0.0f;

void WindowSizeChanged(GLFWwindow* Window, int NewWidth, int NewHeight)
{
	glViewport(0, 0, NewWidth, NewHeight);
}

void MouseMoved(GLFWwindow* Window, double NewXPosition, double NewYPosition)
{
	if (FirstMouse)
	{
		CursorX = NewXPosition;
		CursorY = NewYPosition;
		FirstMouse = false;
	}

	float XOffset = NewXPosition - CursorX;
	float YOffset = CursorY - NewYPosition; // reversed since y-coordinates range from bottom to top
	CursorX = NewXPosition;
	CursorY = NewYPosition;

	const float Sensitivity = 0.05f;
	XOffset *= Sensitivity;
	YOffset *= Sensitivity;

	CameraYaw += XOffset;
	CameraPitch += YOffset;

	if (CameraPitch > 89.0f)
		CameraPitch = 89.0f;
	if (CameraPitch < -89.0f)
		CameraPitch = -89.0f;

	glm::vec3 Front;
	Front.x = cos(glm::radians(CameraYaw)) * cos(glm::radians(CameraPitch));
	Front.y = sin(glm::radians(CameraPitch));
	Front.z = sin(glm::radians(CameraYaw)) * cos(glm::radians(CameraPitch));
	CameraFront = glm::normalize(Front);
}

bool InitializeGL(int Major = 3, int Minor = 3)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return true;
}

bool MakeWindow(int Width = 1920, int Height = 1080, std::string Title = "Minecraft", bool MakeContextCurrent = true, bool DefViewport = true)
{
	Window = glfwCreateWindow(Width, Height, Title.c_str(), NULL, NULL);
	if (Window == NULL)
		return false;
	if (MakeContextCurrent) glfwMakeContextCurrent(Window);
	glewInit();
	glViewport(0, 0, Width, Height);
	glfwSetFramebufferSizeCallback(Window, WindowSizeChanged);
	glfwSetCursorPosCallback(Window, MouseMoved);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return true;
}

bool CreateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RectangleVertices), RectangleVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// colour coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return true;
}

unsigned int CreateShader(const char* Source, int Type = GL_VERTEX_SHADER)
{
	unsigned int NewShader = glCreateShader(Type);
	glShaderSource(NewShader, 1, &Source, NULL);
	glCompileShader(NewShader);

	int Success;
	char InfoLog[512];

	glGetShaderiv(NewShader, GL_COMPILE_STATUS, &Success);

	if (!Success)
	{
		glGetShaderInfoLog(NewShader, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << InfoLog << std::endl;
	}

	return NewShader;
}

unsigned int CreateShaderProgram(std::vector<unsigned int> Shaders)
{
	unsigned int NewShaderProgram = glCreateProgram();
	for (unsigned int Shader : Shaders)
		glAttachShader(NewShaderProgram, Shader);
	glLinkProgram(NewShaderProgram);

	return NewShaderProgram;
}

float CameraSpeed = 0.05f;
void HandleInput()
{
	if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS)
	{
		WireframeMode = !WireframeMode;
		glPolygonMode(GL_FRONT_AND_BACK, WireframeMode ? GL_LINE : GL_FILL);
	}

	CameraSpeed = 5.0f * DeltaTime;

	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		CameraPosition += CameraSpeed * CameraFront;
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		CameraPosition -= CameraSpeed * CameraFront;
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		CameraPosition -= glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		CameraPosition += glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
}

unsigned int CreateTexture(const char* Path, int Type = GL_RGB, int TextureRepeat = GL_REPEAT, int MipmapSmoothing = GL_LINEAR, int Smoothing = GL_LINEAR, bool FlipVertically = true)
{
	stbi_set_flip_vertically_on_load(FlipVertically);
	unsigned int NewTexture;
	glGenTextures(1, &NewTexture);
	glBindTexture(GL_TEXTURE_2D, NewTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureRepeat);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureRepeat);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MipmapSmoothing);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Smoothing);

	int Width, Height, ChannelCount;
	unsigned char* Data = stbi_load(Path, &Width, &Height, &ChannelCount, 0);

	if (!Data)
		return -1;

	glTexImage2D(GL_TEXTURE_2D, 0, Type, Width, Height, 0, Type, GL_UNSIGNED_BYTE, Data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(Data);
	
	return NewTexture;
}

int main()
{
	if (!InitializeGL() || !MakeWindow())
		return ExitCodes::Error;

	VertexShader = CreateShader(StandardVertexShaderSource, GL_VERTEX_SHADER);
	FragmentShader = CreateShader(StandardFragmentShaderSource, GL_FRAGMENT_SHADER);
	ShaderProgram = CreateShaderProgram(std::vector<unsigned int> { VertexShader, FragmentShader });

	CreateBuffers();

	unsigned int Texture = CreateTexture("container.jpg");
	if (Texture == -1)
		return ExitCodes::Error;

	unsigned int FaceTexture = CreateTexture("awesomeface.png", GL_RGBA);
	if (FaceTexture == -1)
		return ExitCodes::Error;

	glUseProgram(ShaderProgram);
	glUniform1i(glGetUniformLocation(ShaderProgram, "Top"), 0);
	glUniform1i(glGetUniformLocation(ShaderProgram, "Bottom"), 1);

	while (!glfwWindowShouldClose(Window))
	{
		float CurrentFrame = glfwGetTime();
		DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		HandleInput();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FaceTexture);

		glUseProgram(ShaderProgram);

		glm::mat4 View		 = glm::mat4(1.0f);
		glm::mat4 Projection = glm::mat4(1.0f);
		View = glm::lookAt(CameraPosition, CameraPosition + CameraFront, CameraUp);
		Projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

		int ViewLocation = glGetUniformLocation(ShaderProgram, "View");
		glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, &View[0][0]);
		int ProjectionLocation = glGetUniformLocation(ShaderProgram, "Projection");
		glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(Projection));

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(VAO);
		for (unsigned int X = 0; X < 10; X++)
		{
			for (unsigned int Y = 0; Y < 10; Y++)
			{
				for (unsigned int Z = 0; Z < 10; Z++)
				{
					glm::mat4 Model = glm::mat4(1.0f);
					Model = glm::translate(Model, glm::vec3(X, Y, Z));
					int ModelLocation = glGetUniformLocation(ShaderProgram, "Model");
					glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(Model));

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	glfwTerminate();
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	return ExitCodes::Success;
}
