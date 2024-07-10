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
	// positions          // colors           // texture coords
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
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
layout(location = 2) in vec2 TextureCoordinates;

out vec2 OutTextureCoordinates;

uniform mat4 Transform;

void main()
{
	gl_Position = Transform * vec4(VertexPosition, 1.0);
	OutTextureCoordinates = vec2(TextureCoordinates.x, TextureCoordinates.y);
};
)";

const char* StandardFragmentShaderSource =
R"(
#version 330 core
out vec4 FragColor;

in vec3 OutColour;
in vec2 OutTextureCoordinates;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{
	FragColor = mix(texture(Texture1, OutTextureCoordinates), texture(Texture2, OutTextureCoordinates), 0.2);
}
)";