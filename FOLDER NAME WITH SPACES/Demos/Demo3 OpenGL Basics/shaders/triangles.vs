
#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec3 vColor; 

out vec3 color;

void
main()
{
    //gl_Position = vec4(vPosition.x,vPosition.y,0,1);
	color = vColor;
	gl_Position = vPosition;
}
