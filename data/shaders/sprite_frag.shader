#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
void main()
{
    vec4 color = texture(diffuseTexture, fs_in.TexCoords).rgba;
    if(color.a < 0.1)
        discard;
    FragColor = color;
}
