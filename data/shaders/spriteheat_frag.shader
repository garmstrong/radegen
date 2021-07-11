#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
} fs_in;

uniform float time;

uniform sampler2D diffuseTexture;

void main()
{
  vec2 tc = fs_in.TexCoords;
  vec2 p = -1.0 + (2.0 * tc);
  float len = length(p);
  float falloff = 1.0-len;

  vec2 uv = tc + (p/len)*cos(len*3.0-time*5.0)*0.03;
  
  vec4 color = texture(diffuseTexture, uv).rgba;
  if(color.a < 0.1)
      discard;
  FragColor = color;
}
