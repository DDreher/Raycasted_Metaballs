#version 330 core


layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 P; // projection matrix

void main(){

	vec4 center = gl_in[0].gl_Position;
	float edge_length = 2.0f;
	vec4 offset = vec4(0.0f, 0.0f, 0.5f * edge_length, 0.0f); // offset the billboard to the camera

    gl_Position = P * (center + vec4(edge_length, edge_length, 0.0f, 0.0f) + offset);
    EmitVertex();
 
    gl_Position = P * (center + vec4(edge_length, -edge_length, 0.0f, 0.0f) + offset);
    EmitVertex();
 
    gl_Position = P * (center + vec4(-edge_length, edge_length, 0.0f, 0.0f) + offset);
    EmitVertex();
 
    gl_Position = P * (center + vec4(-edge_length, -edge_length, 0.0f, 0.0f) + offset);
    EmitVertex();
}