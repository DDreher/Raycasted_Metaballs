#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 P; // projection matrix

void main()
{
	vec4 center = gl_in[0].gl_Position;
	vec4 offset = vec4(0.0f, 0.0f, 0.5f, 0.0f); // offset the billboard to the camera

    gl_Position = P * (center + vec4(1.0f, 1.0f, 0.0f, 0.0f) + offset);
    EmitVertex();
 
    gl_Position = P * (center + vec4(1.0f, -1, 0.0f, 0.0f) + offset);
    EmitVertex();
 
    gl_Position = P * (center + vec4(-1.0f, 1.0f, 0.0f, 0.0f) + offset);
    EmitVertex();
 
    gl_Position = P * (center + vec4(-1.0f, -1.0f, 0.0f, 0.0f) + offset);
    EmitVertex();
}
