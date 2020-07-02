#version 330 core

layout(location = 0) in vec4 position; // Position of the center of the particle and size of the square

uniform mat4 MVP;

void main()
{
	gl_PointSize = 5.0f; // size in pixels

	// Output position of the vertex
	gl_Position = MVP*position;
}

