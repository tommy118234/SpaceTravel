#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;

uniform mat4 MVP;

out vec2 UV;

void main()
{			
	vec4 v = vec4(position,1.0);	
	gl_Position = MVP * v; 
	UV = vertexUV;
}