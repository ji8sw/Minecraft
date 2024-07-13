#include "Helper.h"

const char* StandardVertexShaderSource =
R"(
#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 TextureCoordinates;

out vec3 FragmentPosition;
out vec2 OutTextureCoordinates;

layout(location = 2) uniform mat4 Model;
layout(location = 3) uniform mat4 View;
layout(location = 4) uniform mat4 Projection;

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
#extension GL_ARB_explicit_uniform_location : enable
out vec4 FragColor;

in vec3 FragmentPosition;
in vec2 OutTextureCoordinates;

layout(location = 100) uniform sampler2D Top;
layout(location = 101) uniform sampler2D Bottom;
layout(location = 102) uniform sampler2D Front;
layout(location = 103) uniform sampler2D Back;
layout(location = 104) uniform sampler2D Left;
layout(location = 105) uniform sampler2D Right;

layout(location = 106) uniform int Discards;

const float EPSILON = 0.01;

void main()
{
    bool DiscardTop = (Discards & 1) != 0;
    bool DiscardBottom = (Discards & 2) != 0;
    bool DiscardFront = (Discards & 4) != 0;
    bool DiscardBack = (Discards & 8) != 0;
    bool DiscardLeft = (Discards & 16) != 0;
    bool DiscardRight = (Discards & 32) != 0;

    if (abs(FragmentPosition.y - 0.5) < EPSILON) { // Top
        if (DiscardTop) discard;
        FragColor = texture(Top, OutTextureCoordinates);
    } else if (abs(FragmentPosition.y + 0.5) < EPSILON) { // Bottom
        if (DiscardBottom) discard;
        FragColor = texture(Bottom, OutTextureCoordinates);
    } else if (abs(FragmentPosition.z - 0.5) < EPSILON) { // Front
        if (DiscardFront) discard;
        FragColor = texture(Front, OutTextureCoordinates);
    } else if (abs(FragmentPosition.z + 0.5) < EPSILON) { // Back
        if (DiscardBack) discard;
        FragColor = texture(Back, OutTextureCoordinates);
    } else if (abs(FragmentPosition.x + 0.5) < EPSILON) { // Left
        if (DiscardLeft) discard;
        FragColor = texture(Right, vec2(OutTextureCoordinates.y, OutTextureCoordinates.x));
    } else if (abs(FragmentPosition.x - 0.5) < EPSILON) { // Right
        if (DiscardRight) discard;
        FragColor = texture(Right, vec2(OutTextureCoordinates.y, OutTextureCoordinates.x));
    } else {
        FragColor = vec4(vec3(255, 0, 255), 1.0); // Pink
    }
}
)";