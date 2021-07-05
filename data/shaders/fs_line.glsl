#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    //vec2 TexCoords;
} fs_in;

//uniform sampler2D diffuseTexture;

void main()
{
    vec4 diffuseColor =  vec4(1.0f, 0.0f, 0.0f, 0.5f);

    FragColor = diffuseColor;
}
