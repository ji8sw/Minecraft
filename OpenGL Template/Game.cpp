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

void WindowSizeChanged(GLFWwindow* window, int NewWidth, int NewHeight)
{
	glViewport(0, 0, NewWidth, NewHeight);
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
	return true;
}

bool CreateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RectangleIndices), RectangleIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

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

void HandleInput()
{
	if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS)
	{
		WireframeMode = !WireframeMode;
		glPolygonMode(GL_FRONT_AND_BACK, WireframeMode ? GL_LINE : GL_FILL);
	}
}

int main()
{
	if (!InitializeGL() || !MakeWindow())
		return ExitCodes::Error;

	int Width, Height, ChannelCount;
	unsigned char* Data = stbi_load("container.jpg", &Width, &Height, &ChannelCount, 0);
	unsigned int Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(Data);

	VertexShader = CreateShader(StandardVertexShaderSource, GL_VERTEX_SHADER);
	FragmentShader = CreateShader(StandardFragmentShaderSource, GL_FRAGMENT_SHADER);
	ShaderProgram = CreateShaderProgram(std::vector<unsigned int> { VertexShader, FragmentShader });

	CreateBuffers();

	while (!glfwWindowShouldClose(Window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		HandleInput();

		float Time = glfwGetTime();
		float GreenValue = (sin(Time) / 2.0f) + 0.5f;
		int VertexColourLocation = glGetUniformLocation(ShaderProgram, "ObjectColour");

		glUseProgram(ShaderProgram);

		glUniform4f(VertexColourLocation, 0.0f, GreenValue, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	glfwTerminate();
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	return ExitCodes::Success;
}
