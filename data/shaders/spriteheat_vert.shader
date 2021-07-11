#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out VS_OUT {
    vec2 TexCoords;
} vs_out;

uniform mat4  projection;
uniform mat4  view;
uniform mat4  model;
uniform float time;

void main()
{
    vs_out.TexCoords = aTexCoords;

    mat4 ModelView = view * model;

    // remove rotaion in x
    // Column 0:
    ModelView[0][0] = 1;
    ModelView[0][1] = 0;
    ModelView[0][2] = 0;

    // remove rotaion in z
    // Column 2:
    ModelView[2][0] = 0;
    ModelView[2][1] = 0;
    ModelView[2][2] = 1;


    gl_Position = projection * ModelView * vec4(aPos, 1.0);
}

