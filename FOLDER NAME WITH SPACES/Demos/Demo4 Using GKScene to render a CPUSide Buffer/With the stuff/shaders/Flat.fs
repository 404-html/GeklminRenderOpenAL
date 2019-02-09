#version 400

vec3 lightDirectionTest = -vec3(0.0,0.0,1.0);
out vec4 fColor;

in vec2 texcoord;

uniform sampler2D diffuse; //texture unit 0: Texture
uniform vec3 LightColor; //Color of our light
uniform float ambient;
uniform float specreflectivity;
uniform float specdamp;
uniform float emissivity;
uniform float diffusivity;

void main()
{

    fColor = texture2D(diffuse, texcoord)
}
