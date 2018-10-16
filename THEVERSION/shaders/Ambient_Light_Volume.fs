#version 330

#define MAX_LIGHTS 100
out vec4 fColor[2];

in vec2 texcoord;
in vec2 ScreenPosition;
// uniform sampler2D diffuse; //This is actually the texture unit. limit 32. This one happens to be for the literal object's texture.

struct AmbientLight {
	vec3 pos;
	vec3 color;
	float range;
};

uniform AmbientLight LightArray[MAX_LIGHTS]; //MAX_LIGHTS
//NOTE: We are doing this:
/*
https://stackoverflow.com/questions/16039515/glsl-for-loop-array-index#26480937
*/

uniform sampler2D tex1; //Unit 0
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4; //Unit 3
uniform vec3 camerapos;
uniform int numlights; //How many lights do we have?

void main()
{
	//Grab the values from the initial opaque pass buffers
	vec4 tex1_value = texture2D(tex1, texcoord);
	vec4 tex2_value = texture2D(tex2, texcoord);
	vec4 tex3_value = texture2D(tex3, texcoord);
	vec4 tex4_value = texture2D(tex4, texcoord);
	
	vec3 world_pos = tex3_value.xyz;
	float diffusivity = tex4_value.x;
	
	vec3 betterdiffuse;
	//Our main for loop.
	for (int i = 0; i < numlights && i < MAX_LIGHTS; i++){
		vec3 frag_to_light = LightArray[i].pos - world_pos;
		float dist_to_light = length(frag_to_light);
		//DIFFUSE LIGHTING CALCULATION
		float distr = 1 - dist_to_light/LightArray[i].range;
		betterdiffuse += distr * LightArray[i].color * diffusivity * float(distr > 0);
		
	}
	
	fColor[0] = vec4(betterdiffuse,1.0);
	fColor[1] = vec4(0);
}
