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

//List of flags. Some of these are no longer implemented, they caused too much of a performance problem. I do not recommend you enable them.
const uint GK_RENDER = uint(1); // Do we render it? This is perhaps the most important flag.
const uint GK_TEXTURED = uint(2); // Do we texture it? if disabled, only the texture will be used. if both this and colored are disabled, the object will be black.
const uint GK_COLORED = uint(4);// Do we color it? if disabled, only the texture will be used. if both this and textured are disabled, the object will be black.
const uint GK_FLAT_NORMAL = uint(8); // Do we use flat normals? If this is set, then the normals output to the fragment shader in the initial opaque pass will use the flat layout qualifier. 
const uint GK_FLAT_COLOR = uint(16); // Do we render flat colors? the final, provoking vertex will be used as the color for the entire triangle.
const uint GK_COLOR_IS_BASE = uint(32); //Use the color as the primary. Uses texture as primary if disabled.
const uint GK_TINT = uint(64); //Does secondary add to primary?
const uint GK_DARKEN = uint(128);//Does secondary subtract from primary?
const uint GK_AVERAGE = uint(256);//Do secondary and primary just get averaged?
const uint GK_COLOR_INVERSE = uint(512);//Do we use the inverse of the color?
const uint GK_TEXTURE_INVERSE = uint(1024);//Do we use the inverse of the texture color? DOES NOT invert alpha.
const uint GK_TEXTURE_ALPHA_MULTIPLY = uint(2048);//Do we multiply the color from the texture by the alpha before doing whatever it is we're doing? I do not recommend enabling this and alpha culling, especially if you're trying to create a texture-on-a-flat-color-model effect (Think sega saturn models)
const uint GK_ENABLE_ALPHA_CULLING = uint(4096); //Do we use the texture alpha to cull alpha fragments
const uint GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR = uint(8192); //if the alpha from the texture is <0.5 then the secondary color will replace the primary color.


struct dirlight {
	vec3 direction;
	vec3 color;
	vec4 sphere1;
	vec4 sphere2;
	vec4 sphere3;
	vec4 sphere4;
	vec3 AABBp1;
	vec3 AABBp2;
	vec3 AABBp3;
	vec3 AABBp4;
	uint isblacklist;
};

struct pointlight{
	vec3 position;
	vec3 color;
	float range;
	float dropoff;
	vec4 sphere1;
	vec4 sphere2;
	vec4 sphere3;
	vec4 sphere4;
	vec3 AABBp1;
	vec3 AABBp2;
	vec3 AABBp3;
	vec3 AABBp4;
	uint isblacklist;
};

struct amblight{
	vec3 position;
	vec3 color;
	float range;
	vec4 sphere1;
	vec4 sphere2;
	vec4 sphere3;
	vec4 sphere4;
	vec3 AABBp1;
	vec3 AABBp2;
	vec3 AABBp3;
	vec3 AABBp4;
	uint isblacklist;
};

struct cameralight{
	mat4 viewproj;
	vec3 position;//Is Direction if range < 0
	vec3 color;
	float solidColor;
	float range;
	float shadows; //If this is enabled, solidcolor will be used and the texture sample will be treated as a sample into a shadowmap
	vec2 radii; //inner and outer radii
};





uniform sampler2D diffuse; //0
uniform sampler2D CameraTex1; //3
uniform sampler2D CameraTex2;//4
uniform sampler2D CameraTex3; //5
uniform sampler2D CameraTex4; //6
uniform sampler2D CameraTex5; //7
uniform sampler2D CameraTex6; //8
uniform sampler2D CameraTex7; //9
uniform sampler2D CameraTex8; //10
uniform sampler2D CameraTex9; //11
uniform sampler2D CameraTex10; //12
vec4 CameraTexSamples[10]; //Grab the samples from the Sampler2Ds
uniform samplerCube worldaroundme; //1
uniform samplerCube SkyboxCubemap; //2
uniform vec4 backgroundColor;
uniform vec2 fogRange;

in vec2 texcoord;
in vec3 normout;
//flat in vec3 flatnormout;

in float isFlatNormal;
in float isTextured;
in float isColored;
in float isFlatColor;
in float ColorisBase;
in float AlphaReplaces;
in float isTinted;
in float isDarkened;
in float isAveraged;
in float isNotAnyofThose;

flat in vec3 Flat_Vert_Color;
flat in float whichVertColor;
flat in float alphareplace;
flat in float colorbase;
in vec3 Smooth_Vert_Color;
// in vec3 vert_to_camera;
//Logic from the vertex level
in vec3 worldout;


// uniform float ambient;
uniform float specreflectivity;
uniform float specdamp;
uniform float emissivity;
uniform float enableCubeMapReflections;
uniform float enableCubeMapDiffusivity;
uniform float diffusivity;
uniform vec3 CameraPos; //Camera position in world space
uniform float EnableTransparency;


//The lights
uniform dirlight dir_lightArray[MAX_DIR_LIGHTS];
uniform pointlight point_lightArray[MAX_POINT_LIGHTS];
uniform amblight amb_lightArray[MAX_AMB_LIGHTS];
uniform cameralight camera_lightArray[MAX_CAM_LIGHTS];
//how many are actually being sent in?
uniform int numpointlights;
uniform int numdirlights;
uniform int numamblights;
uniform int numcamlights;


vec2 bettertexcoord;
vec4 texture_value;
vec3 color_value;
vec3 primary_color;
vec3 secondary_color;
uniform uint renderflags;

float length2vec3(vec3 getmylength){
	return dot(getmylength, getmylength);
}

void main()
{
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
	 
	
	color_value = whichVertColor * Flat_Vert_Color + (1-whichVertColor) * Smooth_Vert_Color;
	

	
	 primary_color = colorbase * color_value + (1-colorbase) * texture_value.xyz;
	 secondary_color = (1-colorbase) * color_value + colorbase * texture_value.xyz;
	
	
	
	 primary_color = primary_color * (1-alphareplace) + ((primary_color * (1-texture_value.w)) + (texture_value.xyz * texture_value.w)) * alphareplace;
	//primary_color = texture_value.xyz;
	
	
	vec3 frag_to_camera = CameraPos - worldout;
	vec3 unit_frag_to_camera = normalize(frag_to_camera);
	
	vec3 diffuseffect = vec3(0);
	vec3 speceffect = vec3(0);
	float shouldRenderSpecEffect = float(dot(UnitNormal, unit_frag_to_camera) > 0);
	
	
	//FOG CALCULATIONS
	/*
		uniform samplerCube SkyboxCubemap;
		uniform vec4 backgroundColor; //.a is considered the same as boolean shouldUseBackgroundColor
		uniform vec2 fogRange;
	*/
	vec4 fogColor = backgroundColor.w * backgroundColor + (1-backgroundColor.w) * texture(SkyboxCubemap,-unit_frag_to_camera);
	float fogPercentage = clamp(max(length(frag_to_camera) - fogRange.x, 0) / (fogRange.y - fogRange.x), 0.0, 1.0);//Linear Fog Dropoff... I know
	//NOTE: BREAK UP THESE FOR LOOPS, THEY LAG ON AMD
		//AMD is smart enough to break them up now... at least on an RX-480
	//Directional Lights
	for (int i = 0; i < MAX_DIR_LIGHTS && i < numdirlights; i++)
	{
	
		float inthelist = float( //Comment out everything in here and make it just "false" if you dont want sphere and AABB light volume culling
								// (length2vec3(dir_lightArray[i].sphere1.xyz - worldout) < dir_lightArray[i].sphere1.w) || 
								// (length2vec3(dir_lightArray[i].sphere2.xyz - worldout) < dir_lightArray[i].sphere2.w) || 
								// (length2vec3(dir_lightArray[i].sphere3.xyz - worldout) < dir_lightArray[i].sphere3.w) || 
								// (length2vec3(dir_lightArray[i].sphere4.xyz - worldout) < dir_lightArray[i].sphere4.w) ||
								( //AABB 1&2
									worldout.x < dir_lightArray[i].AABBp2.x && worldout.x > dir_lightArray[i].AABBp1.x &&
									worldout.y < dir_lightArray[i].AABBp2.y && worldout.y > dir_lightArray[i].AABBp1.y &&
									worldout.z < dir_lightArray[i].AABBp2.z && worldout.z > dir_lightArray[i].AABBp1.z
								) ||
								( //AABB 3&4
									worldout.x < dir_lightArray[i].AABBp3.x && worldout.x > dir_lightArray[i].AABBp4.x &&
									worldout.y < dir_lightArray[i].AABBp3.y && worldout.y > dir_lightArray[i].AABBp4.y &&
									worldout.z < dir_lightArray[i].AABBp3.z && worldout.z > dir_lightArray[i].AABBp4.z
								) //||
								// false //Uncomment
							);
		float renderthislight = float(inthelist == 1 && dir_lightArray[i].isblacklist == uint(1) || inthelist == 0 && dir_lightArray[i].isblacklist == uint(0));
		float nDotl = dot(UnitNormal, -dir_lightArray[i].direction);
		nDotl = max(nDotl, 0.0);

		diffuseffect += renderthislight * nDotl * dir_lightArray[i].color * diffusivity;
		//specular
		vec3 reflectedLightDir = reflect(dir_lightArray[i].direction, UnitNormal);
		
		float specFactor = max(
			dot(reflectedLightDir, unit_frag_to_camera),
			0.0 
		);
		float specDampFactor = pow(specFactor,specdamp);
		speceffect += shouldRenderSpecEffect * renderthislight * specDampFactor * specreflectivity * dir_lightArray[i].color;
	}
	//Point Lights
	for (int i = 0; i < MAX_POINT_LIGHTS && i < numpointlights; i++)
	{
	
		float inthelist = float( //Comment out everything in here and make it just "false" if you dont want sphere and AABB light volume culling
								// (length2vec3(point_lightArray[i].sphere1.xyz - worldout) < point_lightArray[i].sphere1.w) || 
								// (length2vec3(point_lightArray[i].sphere2.xyz - worldout) < point_lightArray[i].sphere2.w) || 
								// (length2vec3(point_lightArray[i].sphere3.xyz - worldout) < point_lightArray[i].sphere3.w) || 
								// (length2vec3(point_lightArray[i].sphere4.xyz - worldout) < point_lightArray[i].sphere4.w) ||
								// ( //AABB 1&2
									// worldout.x < point_lightArray[i].AABBp2.x && worldout.x > point_lightArray[i].AABBp1.x &&
									// worldout.y < point_lightArray[i].AABBp2.y && worldout.y > point_lightArray[i].AABBp1.y &&
									// worldout.z < point_lightArray[i].AABBp2.z && worldout.z > point_lightArray[i].AABBp1.z
								// ) ||
								// ( //AABB 1&2
									// worldout.x < point_lightArray[i].AABBp3.x && worldout.x > point_lightArray[i].AABBp4.x &&
									// worldout.y < point_lightArray[i].AABBp3.y && worldout.y > point_lightArray[i].AABBp4.y &&
									// worldout.z < point_lightArray[i].AABBp3.z && worldout.z > point_lightArray[i].AABBp4.z
								// ) ||
								false
							);
		float renderthislight = float(inthelist == 1 && point_lightArray[i].isblacklist == uint(1) || inthelist == 0 && point_lightArray[i].isblacklist == uint(0));
		
		
		vec3 frag_to_light = point_lightArray[i].position - worldout;
		vec3 unit_frag_to_light = normalize(frag_to_light);
		vec3 lightDir = -unit_frag_to_light;
		float lightdist = length2vec3(frag_to_light); //Can never be negative
		float nDotl = dot(UnitNormal, unit_frag_to_light);
		float rangevar = pow(max(1-(lightdist/(point_lightArray[i].range * point_lightArray[i].range)),0.0), point_lightArray[i].dropoff);
		nDotl = max(nDotl * rangevar,0.0);
		diffuseffect += renderthislight * nDotl * point_lightArray[i].color * diffusivity;
		//diffuseffect += vec3(1);
		//diffuseffect += vec3(1);
		//specular
		vec3 reflectedLightDir = reflect(lightDir , UnitNormal);
		
		float specFactor = max(
			dot(reflectedLightDir, unit_frag_to_camera),
			0.0 
		);
		float specDampFactor = pow(specFactor,specdamp);
		
		speceffect += shouldRenderSpecEffect * renderthislight * specDampFactor * specreflectivity * point_lightArray[i].color * rangevar;
	}
	//Ambient Lights
	for (int i = 0; i < MAX_AMB_LIGHTS && i < numamblights; i++){
		float inthelist = float( //Comment out everything in here and make it just "false" if you dont want sphere and AABB light volume culling
								// (length2vec3(amb_lightArray[i].sphere1.xyz - worldout) < amb_lightArray[i].sphere1.w) || 
								// (length2vec3(amb_lightArray[i].sphere2.xyz - worldout) < amb_lightArray[i].sphere2.w) || 
								// (length2vec3(amb_lightArray[i].sphere3.xyz - worldout) < amb_lightArray[i].sphere3.w) || 
								// (length2vec3(amb_lightArray[i].sphere4.xyz - worldout) < amb_lightArray[i].sphere4.w) ||
								( //AABB 1&2
									worldout.x < amb_lightArray[i].AABBp2.x && worldout.x > amb_lightArray[i].AABBp1.x &&
									worldout.y < amb_lightArray[i].AABBp2.y && worldout.y > amb_lightArray[i].AABBp1.y &&
									worldout.z < amb_lightArray[i].AABBp2.z && worldout.z > amb_lightArray[i].AABBp1.z
								) ||
								( //AABB 1&2
									worldout.x < amb_lightArray[i].AABBp3.x && worldout.x > amb_lightArray[i].AABBp4.x &&
									worldout.y < amb_lightArray[i].AABBp3.y && worldout.y > amb_lightArray[i].AABBp4.y &&
									worldout.z < amb_lightArray[i].AABBp3.z && worldout.z > amb_lightArray[i].AABBp4.z
								) ||
								false
							);
		float renderthislight = float(inthelist == 1 && amb_lightArray[i].isblacklist == uint(1) || inthelist == 0 && amb_lightArray[i].isblacklist == uint(0));
	
		//add the diffuseeffect
		vec3 fragtolight = amb_lightArray[i].position - worldout;
		float inrange = float(length2vec3(fragtolight) < amb_lightArray[i].range * amb_lightArray[i].range);
		diffuseffect += renderthislight * amb_lightArray[i].color * inrange;
	}
	
	
	//TO DISABLE CAMERALIGHTS (FOR PERFORMANCE) START COMMENTING HERE
	//ON GEANY SELECT IT ALL AND HIT CTRL+E
	
	//Grab camera light samples
	//CameraTex1
	vec4 samplecoord1 = (camera_lightArray[0].viewproj * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	CameraTexSamples[0] = texture2D(CameraTex1, vec2(samplecoord1.x, (camera_lightArray[0].shadows > 0.0)?1.0:-1.0 *samplecoord1.y));
	//CameraTex2
	samplecoord1 = (camera_lightArray[1].viewproj * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	CameraTexSamples[1] = texture2D(CameraTex2, vec2(samplecoord1.x, (camera_lightArray[1].shadows > 0.0)?1.0:-1.0 *samplecoord1.y));
	//CameraTex3
	samplecoord1 = (camera_lightArray[2].viewproj * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	CameraTexSamples[2] = texture2D(CameraTex3, vec2(samplecoord1.x, (camera_lightArray[2].shadows > 0.0)?1.0:-1.0 *samplecoord1.y));
	//CameraTex4
	samplecoord1 = (camera_lightArray[3].viewproj * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	CameraTexSamples[3] = texture2D(CameraTex4, vec2(samplecoord1.x, (camera_lightArray[3].shadows > 0.0)?1.0:-1.0 *samplecoord1.y));
	//CameraTex5
	samplecoord1 = (camera_lightArray[4].viewproj * vec4(worldout,1.0));
	samplecoord1 = samplecoord1/samplecoord1.w; //NDC
	samplecoord1.xy = (samplecoord1.xy/2.0) + 0.5;
	CameraTexSamples[4] = texture2D(CameraTex5, vec2(samplecoord1.x, (camera_lightArray[4].shadows > 0.0)?1.0:-1.0 * samplecoord1.y));
	//Camera Lights
	for (int i = 0; i < MAX_CAM_LIGHTS; i++)
	{
		//mat4 viewproj = camera_lightArray[i].viewproj;
		vec3 lightpos = camera_lightArray[i].position;
		
		float shouldUseFlatColor = camera_lightArray[i].solidColor;
		vec3 lightcolor = shouldUseFlatColor * camera_lightArray[i].color + CameraTexSamples[i].xyz * (1-shouldUseFlatColor);
		vec4 samplecoord = (camera_lightArray[i].viewproj * vec4(worldout,1.0));
		vec3 screenspace_light = (samplecoord.xyz/samplecoord.w * 0.5) + vec3(0.5);
		float shouldRenderAtAll = float(((screenspace_light.x >= 0 && screenspace_light.x <= 1 && screenspace_light.y >= 0 && screenspace_light.y <= 1 && screenspace_light.z >= 0 && screenspace_light.z <= 1) || (camera_lightArray[i].range < 0 && screenspace_light.z >= 0 && screenspace_light.z <= 1)) && (i < numcamlights)); // add the shadowed test here
		
		//Copied from Point light code
		
		vec3 frag_to_light = lightpos - worldout;
		frag_to_light = frag_to_light * float(camera_lightArray[i].range >= 0) + float(camera_lightArray[i].range < 0) * -lightpos; //If range < 0 then we use position AS the direction. This allows for the correct caustics effect
		vec3 unit_frag_to_light = normalize(frag_to_light);
		vec3 lightDir = -unit_frag_to_light;
		
		//handling shadows
		float shouldRenderCaseShadow = float((camera_lightArray[i].shadows == 0) || (camera_lightArray[i].shadows == 1) && (CameraTexSamples[i].x + 0.001 > screenspace_light.z)); //Avoid shadow banding 
		float radial_distance = length(vec2(0.5,0.5) - screenspace_light.xy);
		float shouldRenderCaseRadii = 1.0-smoothstep(camera_lightArray[i].radii.x, camera_lightArray[i].radii.y, radial_distance);//float(camera_lightArray[i].radii.y > radial_distance);
		//~ float shouldRenderCaseShadow = float(CameraTexSamples[i].x > screenspace_light.z); //Avoid shadow banding 
		//float lightdist = length2vec3(frag_to_light); //Can never be negative
		float nDotl = dot(UnitNormal, unit_frag_to_light);
		float rangevar = 1.0 - clamp(dot(unit_frag_to_light,frag_to_light)/camera_lightArray[i].range, 0.0 , 1);
		rangevar = rangevar * float(camera_lightArray[i].range >= 0) + 1.0 * float(camera_lightArray[i].range < 0);
		nDotl = max(nDotl,0.0);
		diffuseffect += shouldRenderAtAll * shouldRenderCaseShadow * shouldRenderCaseRadii * nDotl * lightcolor * diffusivity * rangevar;
		//diffuseffect += vec3(1);
		//diffuseffect += vec3(1);
		//specular
		vec3 reflectedLightDir = reflect(lightDir , UnitNormal);
		
		float specFactor = max(
			dot(reflectedLightDir, unit_frag_to_camera),
			0.0 
		);
		float specDampFactor = pow(specFactor,specdamp);
		
		speceffect += shouldRenderAtAll * shouldRenderCaseShadow * shouldRenderCaseRadii * shouldRenderSpecEffect * specDampFactor * specreflectivity * lightcolor * rangevar;
		
	}
	
	
	//STOP COMMENTING HERE
	
	
	// float UnitNormalBad = float(UnitNormal == vec3(0));
	// UnitNormal = UnitNormalBad * vec3(0,0,1) + (1-UnitNormalBad) * UnitNormal;
	vec4 cubemapData = texture(worldaroundme,reflect(-unit_frag_to_camera, UnitNormal));
	vec4 cubemapData2 = texture(worldaroundme,UnitNormal);
	diffuseffect += enableCubeMapDiffusivity * cubemapData2.xyz;//enableCubeMapDiffusivity
	//clamp
	// speceffect = min(speceffect, vec3(1));
	diffuseffect = min(diffuseffect, vec3(1));
	diffuseffect = EnableTransparency * ((shouldRenderSpecEffect * diffuseffect) + ((1-shouldRenderSpecEffect) * diffuseffect * texture_value.a)) + (1-EnableTransparency) * diffuseffect; //Simulate light passing losing energy as it passes through transparent surfaces
	primary_color = clamp(diffuseffect * primary_color * diffusivity + emissivity * primary_color, vec3(0), primary_color);
	
	
	
	//vec3 diffusecolorval;
	vec3 specularcolorval = speceffect + shouldRenderSpecEffect * cubemapData.xyz * enableCubeMapReflections * specreflectivity * specdamp/128.0;
	specularcolorval = min(specularcolorval, vec3(1));
	vec3 NewfragColor = primary_color + specularcolorval;
	NewfragColor = mix(clamp(NewfragColor, vec3(0), vec3(1)), fogColor.xyz, fogPercentage);
	
			// Output linear (not gamma encoded!), unmultiplied color from
		// the rest of the shader.
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
		 
		// Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA
		//vec4 transparent_color_1.a = color.a;
			
			
	// Blend Func: GL_ONE, GL_ONE for transparent objects
		// Switch to premultiplied alpha and weight		
	gl_FragData[0] = vec4(NewfragColor, 1) * (1-EnableTransparency) + transparent_color_0 * EnableTransparency;
	// Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA for transparaent objects
	gl_FragData[1] = vec4(color.a, color.a, color.a, color.a);

}
