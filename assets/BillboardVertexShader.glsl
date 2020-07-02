#version 330 core

layout(location = 0) in vec4 position; // Position of the center of the particle

uniform mat4 M; // model matrix
uniform mat4 V; // view matrix

void main()
{
	// Output position of the vertex in camera space
	gl_Position = V * M * position; 
}
