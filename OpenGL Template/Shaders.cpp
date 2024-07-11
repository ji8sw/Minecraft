#include "Helper.h"

const char* StandardVertexShaderSource =
R"(
#version 330 core
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 TextureCoordinates;

out vec3 FragmentPosition;
out vec2 OutTextureCoordinates;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

const float EPSILON = 0.01;

void main()
{
    FragmentPosition = VertexPosition;
	gl_Position = Projection * View * Model * vec4(VertexPosition, 1.0);

    OutTextureCoordinates = TextureCoordinates;
};
)";

const char* StandardFragmentShaderSource =
R"(
#version 330 core
out vec4 FragColor;

in vec3 FragmentPosition;
in vec2 OutTextureCoordinates;

uniform sampler2D Top;
uniform sampler2D Bottom;
uniform sampler2D Front;
uniform sampler2D Back;
uniform sampler2D Left;
uniform sampler2D Right;

const float EPSILON = 0.01;

void main()
{
    if (abs(FragmentPosition.y - 0.5) < EPSILON) { // Top
        FragColor = texture(Top, OutTextureCoordinates);
    } else if (abs(FragmentPosition.y + 0.5) < EPSILON) { // Bottom
        FragColor = texture(Bottom, OutTextureCoordinates);
    } else if (abs(FragmentPosition.z - 0.5) < EPSILON) { // Front
        FragColor = texture(Front, OutTextureCoordinates);
    } else if (abs(FragmentPosition.z + 0.5) < EPSILON) { // Back
        FragColor = texture(Back, OutTextureCoordinates);
    } else if (abs(FragmentPosition.x + 0.5) < EPSILON) { // Left
        FragColor = texture(Right, vec2(OutTextureCoordinates.y, OutTextureCoordinates.x));
    } else if (abs(FragmentPosition.x - 0.5) < EPSILON) { // Right
        FragColor = texture(Right, vec2(OutTextureCoordinates.y, OutTextureCoordinates.x));
    } else {
        FragColor = vec4(vec3(255, 0, 255), 1.0); // Pink
    }
}
)";