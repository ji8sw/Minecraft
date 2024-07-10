#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <format>

enum ExitCodes {
	Success = 0,
	Error = 1
};

float TriangleVertices[] =
{
	// positions         // colors
	0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
   -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
	0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

float RectangleVertices[] =
{
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};

unsigned int RectangleIndices[] =
{
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

inline const char* ReadFile(const char* Path)
{
    std::ifstream File(Path);
    if (!File.is_open()) 
		throw std::runtime_error(std::format("Could not open file: {}", Path).c_str());
    std::stringstream Buffer;
    Buffer << File.rdbuf();
    File.close();
	return Buffer.str().c_str();;
}

const char* StandardVertexShaderSource =
R"(
#version 330 core
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColour;

out vec3 OutColour;

void main()
{
	gl_Position = vec4(VertexPosition, 1.0);
	OutColour = VertexColour;
};
)";

const char* StandardFragmentShaderSource =
R"(
#version 330 core
out vec4 FragColor;
in vec3 OutColour;

void main()
{
	FragColor = vec4(OutColour, 1.0);
}
)";