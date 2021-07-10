#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec2 TexCoordsLM;
    vec3 Normal;
} fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2D diffuseTexture;
uniform sampler2D lightmapTexture;

void main()
{
    vec4 diffuseTexColor = texture(diffuseTexture, fs_in.TexCoords).rgba;
    vec4 lightmapTexColor = texture(lightmapTexture, fs_in.TexCoordsLM).rgba;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec4 result = vec4(ambient + diffuse + specular, 1.0);

    diffuseTexColor = diffuseTexColor * result;

    lightmapTexColor *= result;
    //FragColor = lightmapTexColor * diffuseTexColor;
    //FragColor = lightmapTexColor;
    FragColor = diffuseTexColor;

//    vec4 lightmapTexColor = texture(lightmapTexture, fs_in.TexCoordsLM).rgba;
//    FragColor = lightmapTexColor;
}

