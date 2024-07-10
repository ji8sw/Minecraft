#pragma once

enum ExitCodes {
	Success = 0,
	Error = 1
};

float TriangleVertices[] =
{
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

const char* StandardVertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 VertexPosition;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);\n"
"}\n";

const char* StandardFragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n";