#version 330
#define MAX_POINT_LIGHTS 32
#define MAX_DIR_LIGHTS 2
#define MAX_AMB_LIGHTS 3
#define MAX_CAM_LIGHTS 5
#extension GL_ARB_conservative_depth : enable
// out vec4 fColor[2];
// FORWARD_MAINSHADER.FS
// (C) DMHSW 2018
// layout (depth_greater) out float gl_FragDepth;
// ^ should probably re-enable that later

//List of flags. Some of these are no longer implemented in the default shader configuration, but it is easy to figure out how to make them work
const uint GK_RENDER = uint(1); // Do we render it? This is perhaps the most important flag.
const uint GK_TEXTURED = uint(2); // Do we texture it? if disabled, only the texture will be used. if both this and colored are disabled, the object will be black.
const uint GK_COLORED = uint(4);// Do we color it? if disabled, only the texture will be used. if both this and textured are disabled, the object will be black.
const uint GK_FLAT_NORMAL = uint(8); // Do we use flat normals? If this is set, then the normals output to the fragment shader in the initial opaque pass will use the flat layout qualifier. 
const uint GK_FLAT_COLOR = uint(16); // Do we render flat colors? the final, provoking vertex will be used as the color for the entire triangle.
const uint GK_COLOR_IS_BASE = uint(32); //Use the color as the primary. Uses texture as primary if disabled.
const uint GK_TINT = uint(64); //Does secondary add to primary? Makes a nice glowing effect
const uint GK_DARKEN = uint(128);//Does secondary subtract from primary?
const uint GK_AVERAGE = uint(256);//Do secondary and primary just get averaged?
const uint GK_COLOR_INVERSE = uint(512);//Do we use the inverse of the color?
const uint GK_TEXTURE_INVERSE = uint(1024);//Do we use the inverse of the texture color? DOES NOT invert alpha.
const uint GK_TEXTURE_ALPHA_MULTIPLY = uint(2048);//Do we multiply the color from the texture by the alpha before doing whatever it is we're doing? I do not recommend enabling this and alpha culling, especially if you're trying to create a texture-on-a-flat-color-model effect (Think sega saturn models)
const uint GK_ENABLE_ALPHA_CULLING = uint(4096); //Do we use the texture alpha to cull alpha fragments
const uint GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR = uint(8192); //if the alpha from the texture is <0.5 then the secondary color will replace the primary color.


//~ struct dirlight {
	//~ vec4 direction;
	//~ vec4 color;
	//~ vec4 sphere1;
	//~ vec4 sphere2;
	//~ vec4 sphere3;
	//~ vec4 sphere4;
	//~ vec4 AABBp1;
	//~ vec4 AABBp2;
	//~ vec4 AABBp3;
	//~ vec4 AABBp4;
	//~ uint isblacklist;
//~ }; //LAYOUT IN MEMORY HAS uint FIRST, THEN ALL THE VEC4s IN ORDER

//~ struct pointlight{
	//~ vec4 position;
	//~ vec4 color;
	//~ float range;
	//~ float dropoff;
	//~ vec4 sphere1;
	//~ vec4 sphere2;
	//~ vec4 sphere3;
	//~ vec4 sphere4;
	//~ vec4 AABBp1;
	//~ vec4 AABBp2;
	//~ vec4 AABBp3;
	//~ vec4 AABBp4;
	//~ uint isblacklist;
//~ };

//~ struct amblight{
	//~ vec4 position;
	//~ vec4 color;
	//~ float range;
	//~ vec4 sphere1;
	//~ vec4 sphere2;
	//~ vec4 sphere3;
	//~ vec4 sphere4;
	//~ vec4 AABBp1;
	//~ vec4 AABBp2;
	//~ vec4 AABBp3;
	//~ vec4 AABBp4;
	//~ uint isblacklist;
//~ };

//~ struct cameralight{
	//~ mat4 viewproj;
	//~ vec4 position;//Is Direction if range < 0
	//~ vec4 color;
	//~ float solidColor;
	//~ float range;
	//~ float shadows; //If this is enabled, solidcolor will be used and the texture sample will be treated as a sample into a shadowmap
	//~ vec2 radii; //inner and outer radii
//~ };





uniform sampler2D diffuse; //0
uniform sampler2D CameraTex1; //3
uniform sampler2D CameraTex2;//4
uniform sampler2D CameraTex3; //5
uniform sampler2D CameraTex4; //6
uniform sampler2D CameraTex5; //7
//~ uniform sampler2D CameraTex6; //8
//~ uniform sampler2D CameraTex7; //9
//~ uniform sampler2D CameraTex8; //10
//~ uniform sampler2D CameraTex9; //11
//~ uniform sampler2D CameraTex10; //12
vec4 CameraTexSamples[20]; //Grab the samples from the Sampler2Ds
uniform samplerCube worldaroundme; //1
uniform samplerCube SkyboxCubemap; //2
uniform vec4 backgroundColor;
uniform vec2 fogRange;

in vec2 texcoord;
in vec3 normout;
//flat in vec3 flatnormout;

//in float isFlatNormal;
//in float isTextured;
in float isColored;
//in float isFlatColor;
//in float ColorisBase;
//in float AlphaReplaces;
//in float isTinted;
//in float isDarkened;
//in float isAveraged;
//in float isNotAnyofThose;

//in vec3 Flat_Vert_Color;
//in float whichVertColor;
in float alphareplace;
in float colorbase;
in vec3 Smooth_Vert_Color;
in vec3 worldout;


// uniform float ambient;
uniform float specreflectivity; //IF THIS IS NEGATIVE, FRESNEL IS USED
uniform float specdamp;
uniform float emissivity; //IF FRESNEL IS USED, EMISSIVITY IS DISABLED
uniform float enableCubeMapReflections;
uniform float enableCubeMapDiffusivity;
uniform float diffusivity;
uniform vec3 CameraPos; //Camera position in world space
uniform float EnableTransparency;


//The lights

layout (std140) uniform light_data{
	//Old implementation
	//dirlight dir_lightArray[MAX_DIR_LIGHTS];
	//pointlight point_lightArray[MAX_POINT_LIGHTS];
	//amblight amb_lightArray[MAX_AMB_LIGHTS];
	//cameralight camera_lightArray[MAX_CAM_LIGHTS];
	
	//I don't trust structs, so the values are being stored here directly
	//dir lights
	vec4 dir_direction[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 1 (STARTS AT 0)
	vec4 dir_color[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 2 (STARTS AT 16 * 2 * 1 + 0)
	
	vec4 dir_sphere1[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 3 (STARTS AT 16 * 2 * 2 + 0)
	vec4 dir_sphere2[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 4 (STARTS AT 16 * 2 * 3 + 0)
	vec4 dir_sphere3[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 5 (STARTS AT 16 * 2 * 4 + 0)
	vec4 dir_sphere4[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 6 (STARTS AT 16 * 2 * 5 + 0)
	
	vec4 dir_AABBp1[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 7 (STARTS AT 16 * 2 * 6 + 0)
	vec4 dir_AABBp2[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 8 (STARTS AT 16 * 2 * 7 + 0)
	vec4 dir_AABBp3[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 9 (STARTS AT 16 * 2 * 8 + 0)
	vec4 dir_AABBp4[MAX_DIR_LIGHTS]; // 4 bytes per float * 4 floats per element * 2 elements = 32 bytes 10 (STARTS AT 16 * 2 * 9 + 0)
	//320 subtotal
	//320 bytes so far
	
	//point lights
	vec4 point_position[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 1 (STARTS AT 16 * 32 * 0 + 320)
	vec4 point_color[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 2 (STARTS AT 16 * 32 * 1 + 320)
	//These currently go unused
	vec4 point_sphere1[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 3 (STARTS AT 16 * 32 * 2 + 320)
	vec4 point_sphere2[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 4 (STARTS AT 16 * 32 * 3 + 320)
	vec4 point_sphere3[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 5 (STARTS AT 16 * 32 * 4 + 320)
	vec4 point_sphere4[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 6 (STARTS AT 16 * 32 * 5 + 320)
	vec4 point_AABBp1[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 7 (STARTS AT 16 * 32 * 6 + 320)
	vec4 point_AABBp2[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 8 (STARTS AT 16 * 32 * 7 + 320)
	vec4 point_AABBp3[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 9 (STARTS AT 16 * 32 * 8 + 320)
	vec4 point_AABBp4[MAX_POINT_LIGHTS]; // 4 bytes per float * 4 floats per element * 32 elements = 512 bytes 10 (STARTS AT 16 * 32 * 9 + 320)
	//5120 subtotal
	//320 + 5120 bytes = 5440 bytes so far
	
	//amb lights
	vec4 amb_position[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 0 + 5440)
	vec4 amb_color[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 1 + 5440)
	
	vec4 amb_sphere1[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 2 + 5440)
	vec4 amb_sphere2[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 3 + 5440)
	vec4 amb_sphere3[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 4 + 5440)
	vec4 amb_sphere4[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 5 + 5440)
	vec4 amb_AABBp1[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 6 + 5440)
	vec4 amb_AABBp2[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 7 + 5440)
	vec4 amb_AABBp3[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes (STARTS AT 16 * 3 * 8 + 5440)
	vec4 amb_AABBp4[MAX_AMB_LIGHTS]; // 4 bytes per float * 4 floats per element * 3 elements = 48 bytes 10 (STARTS AT 16 * 3 * 9 + 5440)
	//480 subtotal
	
	//5440 + 480 = 5920 bytes so far
	
	
	//cam lights
	vec4 cam_position[MAX_CAM_LIGHTS];//Is Direction if range < 0 // 4 bytes per float * 4 floats per element * 5 elements = 80 bytes   (STARTS AT 16 * 5 * 0 + 5920)
	vec4 cam_color[MAX_CAM_LIGHTS]; // 4 bytes per float * 4 floats per element * 5 elements = 80 bytes 								(STARTS AT 16 * 5 * 1 + 5920)
	mat4 cam_viewproj[MAX_CAM_LIGHTS]; // 4 bytes per float * 16 floats per element * 5 elements = 320 bytes 							(STARTS AT 16 * 5 * 2 + 5920)
	//480 subtotal
	
	//5920 + 480 = 6400 bytes so far
	
	
	
	//All non-4-aligned things are down here, organized in this pattern: vec2s, floats, uints, ints
	vec2 cam_radii[MAX_CAM_LIGHTS]; 		// 4 bytes per float * 4 floats per element(2 float or 8 byte padding) * 5 elements = 80 bytes (STARTS AT 6400)
	//Arrays, no matter the type, every element is at least 4 bytes.
	float point_range[MAX_POINT_LIGHTS]; 	// 4 * 4(3 floats or 12 bytes of padding) * 32 = 512 (STARTS AT 6480)
	float point_dropoff[MAX_POINT_LIGHTS]; 	// 4 * 4(3 floats or 12 bytes of padding) * 32 = 512 (STARTS AT 6992)
	
	float amb_range[MAX_AMB_LIGHTS]; 		// 4 * 4(3 floats or 12 bytes of padding) * 3 = 48 (STARTS AT 7504)
	
	float cam_solidColor[MAX_CAM_LIGHTS]; 	// 4 * 4(3 floats or 12 bytes of padding) * 5 = 80 (STARTS AT 7552)
	float cam_range[MAX_CAM_LIGHTS]; 		// 4 * 4(3 floats or 12 bytes of padding) * 5 = 80 (STARTS AT 7632)
	float cam_shadows[MAX_CAM_LIGHTS]; 		// 4 * 4(3 floats or 12 bytes of padding) * 5 = 80 (STARTS AT 7712)
	//^If this is enabled, solidcolor will be used and the texture sample will be treated as a sample into a shadowmap
	//1392 subtotal
	//6400 + 1392 = 7792 bytes
	
	uint dir_isblacklist[MAX_DIR_LIGHTS];   	// 4 * 4(padded) * 2 = 32 bytes (STARTS AT 7792)
	uint point_isblacklist[MAX_POINT_LIGHTS]; 	// 4 * 4(padded) * 32 = 512 bytes (STARTS AT 7824)
	uint amb_isblacklist[MAX_AMB_LIGHTS];		// 4 * 4(padded) * 3 = 48 bytes (STARTS AT 8336)
	//592 byte subtotal
	//7792 + 592 = 8384 bytes so far
	
	//how many are actually being sent in?
	int numpointlights;						//AT 8384
	int numdirlights;						//AT 8388
	int numamblights;						//AT 8392
	int numcamlights;						//AT 8396
	//These 4 are all packed into a single sizeof(vec4)
	//TOTAL 8400
	//Almost certainly supported by all target machines (3.3+)
};

vec2 bettertexcoord;
vec4 texture_value;
vec3 color_value;
vec3 primary_color;
vec3 secondary_color;
uniform uint renderflags;

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float length2vec3(vec3 getmylength){
	return dot(getmylength, getmylength);
}

void main()
{
	
	//Configure Fresnel
	//float useShallowShiny = float(specreflectivity < 0); //max(this, ShallowShiny);
	//float Rspecreflectivity = abs(specreflectivity);
	//float Remissivity = (1.0 - useShallowShiny) * emissivity;
	
	
	bettertexcoord = vec2(texcoord.x, -texcoord.y); //Looks like blender
	vec3 UnitNormal;
	
	//Get the Renderflags sorted out
	
	
	
	UnitNormal = normalize(normout);
	

	
	texture_value = texture2D(diffuse, bettertexcoord);
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//UNCOMMENT THIS LINE IF YOU WANT ALPHA CULLING! It will slow down your application, be weary!
	//if ((renderflags & GK_ENABLE_ALPHA_CULLING) > uint(0))
		// if (texture_value.w < 0.9)
			// discard;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 
	
	color_value = Smooth_Vert_Color; //Change: Removed Flat Color
	

	
	 primary_color = colorbase * color_value + (1-colorbase) * texture_value.xyz;
	 secondary_color = (1-colorbase) * color_value + colorbase * texture_value.xyz;
	
	
	
	 primary_color = primary_color * (1-alphareplace) + ((primary_color * (1-texture_value.w)) + (secondary_color.xyz * texture_value.w)) * alphareplace;
	
	
	vec3 frag_to_camera = CameraPos - worldout;
	vec3 unit_frag_to_camera = normalize(frag_to_camera);
	vec3 diffuseffect = vec3(0);
	vec3 speceffect = vec3(0);
	float shouldRenderSpecEffect = float(dot(UnitNormal, unit_frag_to_camera) > 0) * specreflectivity; //Important for transparent objects
	
	
	//FOG CALCULATIONS
	vec4 fogColor = backgroundColor.w * backgroundColor + (1-backgroundColor.w) * texture(SkyboxCubemap,-unit_frag_to_camera);
	float fogPercentage = clamp(max(dot(frag_to_camera, unit_frag_to_camera) - fogRange.x, 0) / (fogRange.y - fogRange.x), 0.0, 1.0);//Linear Fog Dropoff
	for (int i = 0; i < MAX_DIR_LIGHTS && i < numdirlights; i++) //TODO: Replace the 1 with something better
	{
	
		bool inthelist =( //Comment out everything in here and make it just "false" if you dont want sphere and AABB light volume culling
								// (length2vec3(dir_lightArray[i].sphere1.xyz - worldout) < dir_lightArray[i].sphere1.w) || 
								// (length2vec3(dir_lightArray[i].sphere2.xyz - worldout) < dir_lightArray[i].sphere2.w) || 
								// (length2vec3(dir_lightArray[i].sphere3.xyz - worldout) < dir_lightArray[i].sphere3.w) || 
								// (length2vec3(dir_lightArray[i].sphere4.xyz - worldout) < dir_lightArray[i].sphere4.w) ||
								( //AABB 1&2
									worldout.x < dir_AABBp2[i].x && worldout.x > dir_AABBp1[i].x &&
									worldout.y < dir_AABBp2[i].y && worldout.y > dir_AABBp1[i].y &&
									worldout.z < dir_AABBp2[i].z && worldout.z > dir_AABBp1[i].z
								) ||
								( //AABB 3&4
									worldout.x < dir_AABBp3[i].x && worldout.x > dir_AABBp4[i].x &&
									worldout.y < dir_AABBp3[i].y && worldout.y > dir_AABBp4[i].y &&
									worldout.z < dir_AABBp3[i].z && worldout.z > dir_AABBp4[i].z
								) //||
								// false //Uncomment
							);
		float renderthislight = float(inthelist && dir_isblacklist[i] == uint(1) || !inthelist && dir_isblacklist[i] == uint(0));
		float nDotl = dot(UnitNormal, -vec3(dir_direction[i]));
		nDotl = max(nDotl, 0.0);

		diffuseffect += renderthislight * nDotl * vec3(dir_color[i]) * diffusivity;
		//specular
		vec3 reflectedLightDir = reflect(vec3(dir_direction[i]), UnitNormal);
		
		float specFactor = max(
			dot(reflectedLightDir, unit_frag_to_camera),
			0.0 
		);
		float specDampFactor = pow(specFactor,specdamp);
		speceffect += shouldRenderSpecEffect * renderthislight * specDampFactor * vec3(dir_color[i]);
	}
	//Point Lights
	for (int i = 0; i < 16 && i < numpointlights; i++) //Branching makes this FASTER believe it or not
	{
	
		//~ float inthelist = float( //Comment out everything in here and make it just "false" if you dont want sphere and AABB light volume culling
								//~ // (length2vec3(point_lightArray[i].sphere1.xyz - worldout) < point_lightArray[i].sphere1.w) || 
								//~ // (length2vec3(point_lightArray[i].sphere2.xyz - worldout) < point_lightArray[i].sphere2.w) || 
								//~ // (length2vec3(point_lightArray[i].sphere3.xyz - worldout) < point_lightArray[i].sphere3.w) || 
								//~ // (length2vec3(point_lightArray[i].sphere4.xyz - worldout) < point_lightArray[i].sphere4.w) ||
								//~ // ( //AABB 1&2
									//~ // worldout.x < point_lightArray[i].AABBp2.x && worldout.x > point_lightArray[i].AABBp1.x &&
									//~ // worldout.y < point_lightArray[i].AABBp2.y && worldout.y > point_lightArray[i].AABBp1.y &&
									//~ // worldout.z < point_lightArray[i].AABBp2.z && worldout.z > point_lightArray[i].AABBp1.z
								//~ // ) ||
								//~ // ( //AABB 1&2
									//~ // worldout.x < point_lightArray[i].AABBp3.x && worldout.x > point_lightArray[i].AABBp4.x &&
									//~ // worldout.y < point_lightArray[i].AABBp3.y && worldout.y > point_lightArray[i].AABBp4.y &&
									//~ // worldout.z < point_lightArray[i].AABBp3.z && worldout.z > point_lightArray[i].AABBp4.z
								//~ // ) ||
								//~ false
							//~ ); //UNUSED (too slow)
		//float renderthislight = 1;
		
		
		vec3 frag_to_light = point_position[i].xyz - worldout;
		vec3 unit_frag_to_light = normalize(frag_to_light);
		vec3 lightDir = -unit_frag_to_light;
		float lightdist = dot(frag_to_light, unit_frag_to_light); //NOT squared
		float nDotl = dot(UnitNormal, unit_frag_to_light);
		float rangevar = clamp(1 - (lightdist/(point_range[i])),0,1);
		nDotl = max(nDotl * rangevar,0.0);
		diffuseffect += nDotl * vec3(point_color[i]) * diffusivity;
		//specular
		//~ vec3 reflectedLightDir = reflect(lightDir , UnitNormal);
		
		float specFactor = max(
			dot(reflect(lightDir , UnitNormal), unit_frag_to_camera),
			0.0 
		);
		float specDampFactor = pow(specFactor,specdamp); //Unavoidable pow
		//The number of mult ops can be reduced in the following line
		speceffect += shouldRenderSpecEffect * specDampFactor * vec3(point_color[i]) * rangevar * float(i < numpointlights);
	}
	//Ambient Lights
	for (int i = 0; i < MAX_AMB_LIGHTS && i < numamblights; i++){
		float inthelist = float( //Comment out everything in here and make it just "false" if you dont want sphere and AABB light volume culling
								// (length2vec3(amb_lightArray[i].sphere1.xyz - worldout) < amb_lightArray[i].sphere1.w) || 
								// (length2vec3(amb_lightArray[i].sphere2.xyz - worldout) < amb_lightArray[i].sphere2.w) || 
								// (length2vec3(amb_lightArray[i].sphere3.xyz - worldout) < amb_lightArray[i].sphere3.w) || 
								// (length2vec3(amb_lightArray[i].sphere4.xyz - worldout) < amb_lightArray[i].sphere4.w) ||
								( //AABB 1&2
									worldout.x < amb_AABBp2[i].x && worldout.x > amb_AABBp1[i].x &&
									worldout.y < amb_AABBp2[i].y && worldout.y > amb_AABBp1[i].y &&
									worldout.z < amb_AABBp2[i].z && worldout.z > amb_AABBp1[i].z
								) ||
								( //AABB 1&2
									worldout.x < amb_AABBp3[i].x && worldout.x > amb_AABBp4[i].x &&
									worldout.y < amb_AABBp3[i].y && worldout.y > amb_AABBp4[i].y &&
									worldout.z < amb_AABBp3[i].z && worldout.z > amb_AABBp4[i].z
								) ||
								false
							);
		float renderthislight = float(inthelist == 1 && amb_isblacklist[i] == uint(1) || inthelist == 0 && amb_isblacklist[i] == uint(0));
	
		//add the diffuseeffect
		vec3 fragtolight = vec3(amb_position[i]) - worldout;
		float inrange = float(dot(fragtolight, fragtolight) < amb_range[i]);
		diffuseffect += renderthislight * vec3(amb_color[i]) * inrange;
	}
	
	
	//TO DISABLE CAMERALIGHTS (FOR PERFORMANCE) START COMMENTING HERE
	//ON GEANY SELECT IT ALL AND HIT CTRL+E
	
	//Grab camera light samples
	//CameraTex1
	vec4 samplecoord1 = (cam_viewproj[0] * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	//~ for(int i = 0; i < 4; i++)
		//~ CameraTexSamples[4 * 0 + i] = texture2D(CameraTex1, vec2(samplecoord1.x, float(cam_shadows[0] <= 0.0) * -samplecoord1.y + float(cam_shadows[0] > 0.0) * samplecoord1.y) + float(cam_shadows[0] > 0.0) * (poissonDisk[i]/700.0));
	CameraTexSamples[0] = texture2D(CameraTex1, vec2(samplecoord1.x, float(cam_shadows[0] <= 0.0) * -samplecoord1.y + float(cam_shadows[0] > 0.0) * samplecoord1.y));
	//CameraTex2
	samplecoord1 = (cam_viewproj[1] * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	//~ for(int i = 0; i < 4; i++)
		//~ CameraTexSamples[4 * 1 + i] = texture2D(CameraTex2, vec2(samplecoord1.x, float(cam_shadows[1] <= 0.0) * -samplecoord1.y + float(cam_shadows[1] > 0.0) * samplecoord1.y) + float(cam_shadows[1] > 0.0) * (poissonDisk[i]/1200.0));
	CameraTexSamples[1] = texture2D(CameraTex2, vec2(samplecoord1.x, float(cam_shadows[1] <= 0.0) * -samplecoord1.y + float(cam_shadows[1] > 0.0) * samplecoord1.y));
	//CameraTex3
	samplecoord1 = (cam_viewproj[2] * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	//~ for(int i = 0; i < 4; i++)
		//~ CameraTexSamples[4 * 2 + i] = texture2D(CameraTex3, vec2(samplecoord1.x, float(cam_shadows[2] <= 0.0) * -samplecoord1.y + float(cam_shadows[2] > 0.0) * samplecoord1.y) + float(cam_shadows[2] > 0.0) * (poissonDisk[i]/1200.0));
	CameraTexSamples[2] = texture2D(CameraTex3, vec2(samplecoord1.x, float(cam_shadows[2] <= 0.0) * -samplecoord1.y + float(cam_shadows[2] > 0.0) * samplecoord1.y));
	//CameraTex4
	samplecoord1 = (cam_viewproj[3] * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	//~ for(int i = 0; i < 4; i++)
		//~ CameraTexSamples[4 * 3 + i] = texture2D(CameraTex4, vec2(samplecoord1.x, float(cam_shadows[3] <= 0.0) * -samplecoord1.y + float(cam_shadows[3] > 0.0) * samplecoord1.y) + float(cam_shadows[3] > 0.0) * (poissonDisk[i]/1200.0));
	CameraTexSamples[3] = texture2D(CameraTex4, vec2(samplecoord1.x, float(cam_shadows[3] <= 0.0) * -samplecoord1.y + float(cam_shadows[3] > 0.0) * samplecoord1.y));
	//CameraTex5
	samplecoord1 = (cam_viewproj[4] * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	//~ for(int i = 0; i < 4; i++)
		//~ CameraTexSamples[4 * 4 + i] = texture2D(CameraTex5, vec2(samplecoord1.x, float(cam_shadows[4] <= 0.0) * -samplecoord1.y + float(cam_shadows[4] > 0.0) * samplecoord1.y) + float(cam_shadows[4] > 0.0) * (poissonDisk[i]/1200.0));
	CameraTexSamples[4] = texture2D(CameraTex5, vec2(samplecoord1.x, float(cam_shadows[4] <= 0.0) * -samplecoord1.y + float(cam_shadows[4] > 0.0) * samplecoord1.y));
	//Camera Lights
	for (int i = 0; i < MAX_CAM_LIGHTS && i < numcamlights; i++)
	{
		//mat4 viewproj = camera_lightArray[i].viewproj;
		vec3 lightpos = vec3(cam_position[i]);
		
		float shouldUseFlatColor = cam_solidColor[i];
		vec3 lightcolor = shouldUseFlatColor * vec3(cam_color[i]) + CameraTexSamples[i].xyz * (1-shouldUseFlatColor);
		vec4 samplecoord = (cam_viewproj[i] * vec4(worldout,1.0));
		vec3 screenspace_light = (samplecoord.xyz/samplecoord.w * 0.5) + vec3(0.5);
		float shouldRenderAtAll = float(((screenspace_light.x >= 0 && screenspace_light.x <= 1 && screenspace_light.y >= 0 && screenspace_light.y <= 1 && screenspace_light.z >= 0 && screenspace_light.z <= 1) || (cam_range[i] < 0 && screenspace_light.z >= 0 && screenspace_light.z <= 1)) && (i < numcamlights)); // add the shadowed test here
		
		//Copied from Point light code
		
		vec3 frag_to_light = lightpos - worldout;
		frag_to_light = frag_to_light * float(cam_range[i] >= 0) + float(cam_range[i] < 0) * -lightpos; //If range < 0 then we use position AS the direction. This allows for the correct caustics effect
		vec3 unit_frag_to_light = normalize(frag_to_light);
		vec3 lightDir = -unit_frag_to_light;
		
		//handling shadows, no filtering (aka sharp)
		float shouldRenderCaseShadow = float((cam_shadows[i] == 0) || (cam_shadows[i] == 1) && (CameraTexSamples[i].x + 0.001 > screenspace_light.z)); //Avoid shadow banding 
		//handling shadows, poisson sampling
		//~ float shouldRenderCaseShadow = 1;
		//~ for (int j=0;j < 4; j++){
			//~ shouldRenderCaseShadow -= 0.25 * float (CameraTexSamples[i * 4 + j].x  + 0.001 < screenspace_light.z && cam_shadows[i] > 0.0);
		//~ }
		
		float radial_distance = length(vec2(0.5,0.5) - screenspace_light.xy);
		float shouldRenderCaseRadii = 1.0-smoothstep(cam_radii[i].x, cam_radii[i].y, radial_distance);
		//float shouldRenderCaseShadow = float(CameraTexSamples[i].x > screenspace_light.z); //Avoid shadow banding 
		//float lightdist = length2vec3(frag_to_light); //Can never be negative
		float nDotl = dot(UnitNormal, unit_frag_to_light);
		float rangevar = 1.0 - clamp(dot(unit_frag_to_light,frag_to_light)/abs(cam_range[i]), 0.0 , 1);
		rangevar = rangevar * float(cam_range[i] >= 0) + float(cam_range[i] < 0);
		nDotl = max(nDotl,0.0);
		diffuseffect += shouldRenderAtAll * shouldRenderCaseShadow * shouldRenderCaseRadii * nDotl * lightcolor * diffusivity * rangevar;
		//specular
		vec3 reflectedLightDir = reflect(lightDir , UnitNormal);
		
		float specFactor = max(
			dot(reflectedLightDir, unit_frag_to_camera),
			0.0 
		);
		float specDampFactor = pow(specFactor,specdamp);
		
		speceffect += shouldRenderAtAll * shouldRenderCaseShadow * shouldRenderCaseRadii * shouldRenderSpecEffect * specDampFactor * lightcolor * rangevar;
		
	}
	
	
	//STOP COMMENTING HERE
	
	
	// float UnitNormalBad = float(UnitNormal == vec3(0));
	// UnitNormal = UnitNormalBad * vec3(0,0,1) + (1-UnitNormalBad) * UnitNormal;
	vec4 cubemapData = texture(worldaroundme,reflect(-unit_frag_to_camera, UnitNormal));
	//~ vec4 cubemapData2 = texture(worldaroundme,UnitNormal);
	//~ diffuseffect += enableCubeMapDiffusivity * cubemapData2.xyz;//enableCubeMapDiffusivity
	//clamp
	// speceffect = min(speceffect, vec3(1));
	diffuseffect = min(diffuseffect, vec3(1));
	//~ diffuseffect = EnableTransparency * ((shouldRenderSpecEffect * diffuseffect) + ((1-shouldRenderSpecEffect) * diffuseffect * texture_value.a)) + (1-EnableTransparency) * diffuseffect; //Simulate light passing losing energy as it passes through transparent surfaces
	primary_color = clamp(diffuseffect * primary_color * diffusivity + emissivity * primary_color, vec3(0), primary_color);
	
	
	
	//vec3 diffusecolorval;
	vec3 specularcolorval = speceffect + shouldRenderSpecEffect * cubemapData.xyz * enableCubeMapReflections;
	specularcolorval = min(specularcolorval, vec3(1));
	vec3 NewfragColor = primary_color + specularcolorval;
	NewfragColor = mix(clamp(NewfragColor, vec3(0), vec3(1)), fogColor.xyz, fogPercentage);
	
			// Output linear (not gamma encoded!), unmultiplied color from
		// the rest of the shader.
	//TO DISABLE WBOIT, START COMMENTING HERE!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		vec4 color = vec4(NewfragColor, texture_value.a); // regular shading code
		 
		 
		 
		// Insert your favorite weighting function here. The color-based factor
		// avoids color pollution from the edges of wispy clouds. The z-based
		// factor gives precedence to nearer surfaces.
		float weight = 
			  max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) *
			  clamp(0.03 / (1e-5 + pow(gl_FragCoord.z / 200, 4.0)), 1e-2, 3e3);
		 
		// Blend Func: GL_ONE, GL_ONE
		// Switch to premultiplied alpha and weight
		vec4 transparent_color_0 = vec4(color.rgb * color.a, color.a) * weight;
		 
		//~ // Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA
		//~ //vec4 transparent_color_1.a = color.a;
			
	//STOP COMMENTING!!! BUT... you have some more things to comment out~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Blend Func: GL_ONE, GL_ONE for transparent objects
		// Switch to premultiplied alpha and weight		
	gl_FragData[0] = vec4(NewfragColor, 1) * (1-EnableTransparency) + transparent_color_0 * EnableTransparency; //comment this out, use the below line
	//~ gl_FragData[0] = vec4(NewfragColor, texture_value.a);
	// Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA for transparaent objects
	gl_FragData[1] = vec4(color.a, color.a, color.a, color.a); //Comment this out to disable wboit too

}
