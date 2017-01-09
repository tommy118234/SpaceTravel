#version 430

uniform sampler2D sampler;
uniform sampler2D sampler2;
uniform vec3 ambientLight;
uniform vec3 lightposition;
uniform vec3 eyeposition;
uniform float diffuse_brightness_control;
uniform float specular_brightness_control;
uniform bool multiple_mapping_flag;
uniform bool normal_mapping_flag;

in vec3 positionWorld;
in vec3 normalWorld;
in vec2 UV;

out vec3 color;


void main()
{		
	vec3 normal = normalize(normalWorld);
	// Ambient
	vec3 MaterialAmbientColor = texture(sampler,UV).rgb;
	vec3 MaterialDiffuseColor = texture(sampler,UV).rgb;	
	vec3 MaterialSpecularColor = vec3(0.3+specular_brightness_control,0.3+specular_brightness_control,0.3+specular_brightness_control);	
	if (multiple_mapping_flag){
	// Ambient	
	MaterialAmbientColor = (0.3*texture(sampler,UV)+0.7*texture(sampler2,UV)).rgb;
	MaterialDiffuseColor = (0.3*texture(sampler,UV)+0.7*texture(sampler2,UV)).rgb;		
	}
	else if (normal_mapping_flag){
	normal = texture(sampler2,UV).rgb;
	normal = normalize(normal*2.0-1.0);	
	}
	// Diffuse
	vec3 lightVectorWorld = normalize(lightposition-positionWorld);
	float brightness = dot(lightVectorWorld, normal)+diffuse_brightness_control;
	vec3 diffuseLight = vec3(brightness,brightness,brightness);	
	// Specular
	vec3 reflectedlightVectorWorld = reflect(-lightVectorWorld,normalWorld);
	vec3 eyeVectorWorld = normalize(eyeposition-positionWorld);
	float s = clamp(dot(reflectedlightVectorWorld,eyeVectorWorld),0,1);
	s = pow(s,50);
	vec3 specularLight = vec3 (s,0,0);
	color = MaterialAmbientColor * ambientLight +
			MaterialDiffuseColor*clamp(diffuseLight,0,1)+
			MaterialSpecularColor*specularLight;

}