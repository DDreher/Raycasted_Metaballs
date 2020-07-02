#version 330 core

// Ouput data
out vec4 color;

void main(){
	vec2 window_size = vec2(1024.0f, 768.0f);

	float red = gl_FragCoord.x/window_size.x;	
	float green = gl_FragCoord.y/window_size.y;	

	color = vec4(red, green, 0.0f, 1.0f);
}
