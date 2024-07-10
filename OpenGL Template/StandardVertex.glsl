// UNUSED

#version 330 core
layout(location = 0) in vec3 VertexPosition;

void main()
{
	gl_Position = vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);
}