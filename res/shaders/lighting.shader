#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;

out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_Scale;
void main()
{
    TexCoords = a_UV;
    Normal = mat3(transpose(inverse(u_Scale *u_ModelMatrix))) * a_Normal;
    Position = vec3(u_Scale * u_ModelMatrix * vec4(a_Pos, 1.0));
    gl_Position = u_CameraMatrix * vec4(u_Scale * u_ModelMatrix * vec4(a_Pos, 1.0f));
}  

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 u_CameraPos;
uniform int u_HasTexture;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform float u_LightStrength;
uniform vec3 u_LightColor;
uniform sampler2D u_Texture;

void main()
{    

    vec3 ambient = u_LightStrength * u_LightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - Position);
    //vec3 lightDir = normalize(vec3(-0.2f, -1.0f, -0.3f));


    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    float specularStrength = 0.5;

    vec3 viewDir = normalize(u_ViewPos - Position);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_LightColor * vec3(1,1,1);

    vec3 I = normalize(Position - u_CameraPos);
    vec3 R = reflect(I, normalize(Normal));


    if(u_HasTexture == 1){
        FragColor = vec4((ambient + diffuse + specular) * texture(u_Texture, TexCoords).rgb, 1.0f);

    }
    else{
        FragColor = vec4((ambient + diffuse + specular), 1.0f);
        //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}