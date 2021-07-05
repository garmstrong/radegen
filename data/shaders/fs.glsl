#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;

void main()
{
    vec4 diffuseColor = texture(diffuseTexture, fs_in.TexCoords).rgba;

    FragColor = diffuseColor;
}