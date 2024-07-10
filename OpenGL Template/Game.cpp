#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Helper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow* Window = NULL;
unsigned int VBO, VAO, EBO, LightCubeVAO, CubeVAO;
unsigned int VertexShader;
unsigned int FragmentShader;
unsigned int ShaderProgram;
unsigned int LightShader;
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
glm::vec3 LightPosition(1.2f, 1.0f, 2.0f);

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
	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RectangleVertices), RectangleVertices, GL_STATIC_DRAW);

	glBindVertexArray(CubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glGenVertexArrays(1, &LightCubeVAO);
	glBindVertexArray(LightCubeVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	return true;
}

unsigned int CreateShader(const char* Name, const char* Source, int Type = GL_VERTEX_SHADER)
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
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED::" << Name << std::endl << InfoLog << std::endl;
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

	unsigned int ColoursVertexShader = CreateShader("ColoursVertexShader", ColoursVertexShaderCode, GL_VERTEX_SHADER);
	unsigned int ColoursFragmentShader = CreateShader("ColoursFragmentShader", ColoursFragmentShaderCode, GL_FRAGMENT_SHADER);
	unsigned int LightShaderProgram = CreateShaderProgram(std::vector<unsigned int> { ColoursVertexShaderCode, ColoursFragmentShaderCode });

	unsigned int LightCubeVertexShader = CreateShader("LightCubeVertexShader", LightCubeVertexShadersCode, GL_VERTEX_SHADER);
	unsigned int LightCubeFragmentShader = CreateShader("LightCubeFragmentShader", LightCubeFragmentShaderCode, GL_FRAGMENT_SHADER);
	unsigned int LightCubeShaderProgram = CreateShaderProgram(std::vector<unsigned int> { LightCubeVertexShader, LightCubeFragmentShader });

	CreateBuffers();

	while (!glfwWindowShouldClose(Window))
	{
		float CurrentFrame = glfwGetTime();
		DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		HandleInput();

		glUseProgram(LightShaderProgram);
		glm::vec3 ObjectColour = glm::vec3(1.0f, 0.5f, 0.31f);
		int ObjectColourPos = glGetUniformLocation(LightShaderProgram, "objectColour");
		glUniformMatrix3fv(ObjectColourPos, 1, GL_FALSE, glm::value_ptr(ObjectColour));
		glm::vec3 LightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		int LightColourPos = glGetUniformLocation(LightShaderProgram, "lightColour");
		glUniformMatrix3fv(LightColourPos, 1, GL_FALSE, glm::value_ptr(LightColour));

		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
		glm::mat4 View		 = glm::lookAt(CameraPosition, CameraPosition + CameraFront, CameraUp);
		int ProjectionPosition = glGetUniformLocation(LightShaderProgram, "projection");
		glUniformMatrix4fv(ProjectionPosition, 1, GL_FALSE, &View[0][0]);
		int ViewPosition = glGetUniformLocation(LightShaderProgram, "view");
		glUniformMatrix4fv(ViewPosition, 1, GL_FALSE, glm::value_ptr(Projection));

		glm::mat4 Model = glm::mat4(1.0f);
		int ModelPosition = glGetUniformLocation(LightShaderProgram, "model");
		glUniformMatrix4fv(ModelPosition, 1, GL_FALSE, glm::value_ptr(Model));

		glBindVertexArray(CubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(LightCubeShaderProgram);
		ProjectionPosition = glGetUniformLocation(LightShaderProgram, "projection");
		glUniformMatrix4fv(ProjectionPosition, 1, GL_FALSE, &View[0][0]);
		ViewPosition = glGetUniformLocation(LightShaderProgram, "view");
		glUniformMatrix4fv(ViewPosition, 1, GL_FALSE, glm::value_ptr(Projection));
		Model = glm::mat4(1.0f);
		Model = glm::translate(Model, LightPosition);
		Model = glm::scale(Model, glm::vec3(0.2f)); // a smaller cube
		ModelPosition = glGetUniformLocation(LightCubeShaderProgram, "model");
		glUniformMatrix4fv(ModelPosition, 1, GL_FALSE, glm::value_ptr(Model));

		glBindVertexArray(LightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	glfwTerminate();
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	return ExitCodes::Success;
}
