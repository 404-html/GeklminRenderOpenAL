#version 400

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec2 intexcoord;
layout( location = 2 ) in vec3 Normal;

out vec2 texcoord;

uniform mat4 MVP; //Model->World->Camera
uniform vec3 LightPosition; //The location of our light in WORLD SPACE!
uniform mat4 World2Camera; //the world to camera transform. I figure this is faster than calculating MVP seperately per vertex.
uniform mat4 Model2World; //Model->World
uniform mat4 viewMatrix; //Result of glm::lookAt();

void
main()
{
	gl_Position = MVP * vec4(vPosition,1.0);
	texcoord = intexcoord; //this is faster
}
