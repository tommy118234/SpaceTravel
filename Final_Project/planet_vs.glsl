#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;
in layout(location=2) vec3 normal;

uniform mat4 Trans;
uniform mat4 MVP;
uniform vec3 ambientLight;

out vec2 UV;
out vec3 normalWorld;
out vec3 positionWorld;

void main()
{		
	vec4 v = vec4(position,1.0);	
	vec4 new_position =  Trans * v;
	gl_Position = MVP * v; 

	vec4 normal_temp = Trans * vec4(normal, 0);
	normalWorld = normal_temp.xyz;
	positionWorld = new_position.xyz; 

	UV = vertexUV;	
}