#version 430
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 M;

void main()
{		
	vec4 pos = projection * view  * M * vec4(position, 1.0);
	gl_Position = pos;
	TexCoords = position;
}