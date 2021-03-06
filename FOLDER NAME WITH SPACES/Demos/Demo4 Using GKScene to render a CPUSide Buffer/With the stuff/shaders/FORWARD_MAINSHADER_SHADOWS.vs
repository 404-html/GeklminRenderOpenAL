#version 330

//FORWARD_MAINSHADER.VS
//(C) DMHSW 2018

//List of flags. Some of these are no longer implemented, they DID NOT caus too much of a performance problem. I do not recommend you enable them. (Remember: Mat4 changing lag?)
const uint GK_RENDER = uint(1); // Do we render it? This is perhaps the most important flag.
const uint GK_TEXTURED = uint(2); // Do we texture it? if disabled, only the texture will be used. if both this and colored are disabled, the object will be black.
const uint GK_COLORED = uint(4);// Do we color it? if disabled, only the texture will be used. if both this and textured are disabled, the object will be black.
const uint GK_FLAT_NORMAL = uint(8); // Do we use flat normals? If this is set, then the normals output to the fragment shader in the initial opaque pass will use the flat layout qualifier. 
const uint GK_FLAT_COLOR = uint(16); // Do we render flat colors? the final, provoking vertex will be used as the color for the entire triangle.
const uint GK_COLOR_IS_BASE = uint(32); //Use the color as the primary. Uses texture as primary if disabled.
const uint GK_TEXTURE_ALPHA_MULTIPLY = uint(2048);//Do we multiply the color from the texture by the alpha before doing whatever it is we're doing? I do not recommend enabling this and alpha culling, especially if you're trying to create a texture-on-a-flat-color-model effect (Think sega saturn models)
const uint GK_ENABLE_ALPHA_CULLING = uint(4096); //Do we use the texture alpha to cull alpha fragments
const uint GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR = uint(8192); //if the alpha from the texture is <0.5 then the secondary color will replace the primary color.


layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec2 intexcoord;
layout( location = 2 ) in vec3 Normal;
layout( location = 3 ) in vec3 VertexColor;
layout( location = 4 ) in mat4 instanced_model_matrix; //used for instancing! 4,5,6,7

out vec2 texcoord;
out vec3 normout;
flat out float whichVertColor;
flat out float alphareplace;
flat out float colorbase;
out vec3 Smooth_Vert_Color;
flat out vec3 Flat_Vert_Color;
// out vec3 vert_to_camera;
out vec3 worldout;

out float isFlatNormal;
out float isTextured;
out float isColored;
out float isFlatColor;
out float ColorisBase;
out float AlphaReplaces;
out float isTinted;
out float isDarkened;
out float isAveraged;
out float isNotAnyofThose;
vec3 worldpos; //Position of the fragment in the world!


uniform uint renderflags;
uniform float windowsize_x;
uniform float windowsize_y;
uniform mat4 World2Camera; //the world to camera transform. I figure this is faster than calculating VP seperately per vertex.
uniform mat4 Model2World; //Model->World
uniform vec3 CameraPos; //Camera position in world space
uniform float is_instanced; //used for determining whether or not we should use instanced_model_matrix or Model2World

void
main()
{
	mat4 worldtrans = float(is_instanced > 0) * instanced_model_matrix + (1 - float(is_instanced > 0)) * Model2World;
	//The position of this vertex in the world coordinate system.
	worldpos = (worldtrans * vec4(vPosition,1.0)).xyz;
	gl_Position = World2Camera * worldtrans * vec4(vPosition,1.0);
	
	// vert_to_camera = CameraPos  - worldpos;
	worldout = worldpos;
}
