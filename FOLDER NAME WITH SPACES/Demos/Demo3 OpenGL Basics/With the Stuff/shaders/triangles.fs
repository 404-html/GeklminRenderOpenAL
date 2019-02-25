#version 450 core

out vec4 fColor;
in vec3 color;
void main()
{
    fColor = vec4(color, 0.0);
}
