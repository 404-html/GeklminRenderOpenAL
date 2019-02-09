#version 330

#define MAX_LIGHTS 200
#define MAX_DIR_LIGHTS 10
#define MAX_AMB_LIGHTS 50
#define MAX_SPOT_LIGHTS 100
//Cameralights will be rendered in seperate passes.

out vec4 fColor[2];

in vec2 texcoord;
in vec2 ScreenPosition;
// uniform sampler2D diffuse; //This is actually the texture unit. limit 32. This one happens to be for the literal object's texture.

struct PointLight {
	vec3 pos;
	vec3 color;
	float range;
	float dropoff;
};

struct AmbientLight {
	vec3 pos;
	vec3 color;
	float range;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;
};

struct Spotlight {
	vec3 pos;
	vec3 direction;
	vec3 color;
	float innerangle;
	float outerangle;
	float range;
	//Note: We are not doing any shadows here. No texture.
};

uniform PointLight LightArray[MAX_LIGHTS]; //MAX_LIGHTS
uniform Spotlight SpotlightArray[MAX_SPOT_LIGHTS];
uniform DirectionalLight DirectionalLightArray[MAX_DIR_LIGHTS];
uniform AmbientLight AmbientLightArray[MAX_AMB_LIGHTS];

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
uniform int numdirlights;
uniform int numspotlights;
uniform int numambientlights;
//far and near clip planes
/* DOES NOT work!
vec3 calculate_world_position(vec2 texture_coordinate, float depth_from_depth_buffer)
{
    vec4 clip_space_position = vec4(texture_coordinate * 2.0 - vec2(1.0), 2.0 * depth_from_depth_buffer - 1.0, 1.0);

    //vec4 position = inverse_projection_matrix * clip_space_position; // Use this for view space
    vec4 position = inverse_view_projection_matrix * clip_space_position; // Use this for world space

    return(position.xyz / position.w);
}
*/

//Another thing that should work

void main()
{
	//Grab the values from the initial opaque pass buffers
	vec4 tex1_value = texture2D(tex1, texcoord);
	vec4 tex2_value = texture2D(tex2, texcoord);
	vec4 tex3_value = texture2D(tex3, texcoord);
	vec4 tex4_value = texture2D(tex4, texcoord);
	
	vec3 world_pos = tex3_value.xyz;
	vec3 unit_surface_normal_world = tex2_value.xyz;
	float specular_dampening = tex2_value.w;
	float specular_reflectivity = tex1_value.w;
	float diffusivity = tex4_value.x;
	vec3 frag_to_camera = camerapos - world_pos;
	vec3 unit_frag_to_camera = normalize(frag_to_camera);
	
	vec3 betterdiffuse;
	vec3 finalSpecular;
	//Our main for loop.
	for (int i = 0; i < numlights && i < MAX_LIGHTS; i++){
		vec3 frag_to_light = LightArray[i].pos - world_pos;
		vec3 unit_frag_to_light = normalize(frag_to_light);
		vec3 lightDir = -unit_frag_to_light;
		
		//DIFFUSE LIGHTING CALCULATION
		float nDotl = dot(unit_surface_normal_world, unit_frag_to_light);
		float lightdist = length(frag_to_light); //Can never be negative
		nDotl = max(nDotl * pow(max(1-(lightdist/LightArray[i].range),0.0), LightArray[i].dropoff),0.0);
		betterdiffuse += nDotl * LightArray[i].color * diffusivity;
		
		//SPECULAR LIGHTING CALCULATION
		vec3 reflectedLightDir = reflect(lightDir,unit_surface_normal_world);
		
		float specFactor = max(
			dot(reflectedLightDir, unit_frag_to_camera),
			0.0 
		);
		float specDampFactor = pow(specFactor,specular_dampening);
		
		finalSpecular += specDampFactor * specular_reflectivity * LightArray[i].color * pow(max(1-(lightdist/LightArray[i].range),0.0), LightArray[i].dropoff);
	}
	
	for (int i = 0; i < MAX_DIR_LIGHTS && i < numdirlights; i++)
	{
		float nDotl = dot(unit_surface_normal_world, -DirectionalLightArray[i].direction);
		nDotl = max(nDotl, 0.0);
		betterdiffuse += nDotl * DirectionalLightArray[i].color * diffusivity;
	}
	
	fColor[0] = vec4(betterdiffuse,1.0);
	fColor[1] = vec4(finalSpecular,1.0);
}
