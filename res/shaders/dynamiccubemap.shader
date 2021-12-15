#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_Transform;
uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.FragPos = vec3(u_ModelMatrix * vec4(a_Pos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(u_ModelMatrix))) * a_Normal;
    vs_out.TexCoords = a_UV;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    gl_Position = u_CameraMatrix * vec4(u_Transform * u_ModelMatrix * vec4(a_Pos, 1.0f));
}  

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform int u_ReflectionsBool = 1;
uniform int u_ShadowsBool = 1;

uniform int u_HasTexture;
uniform float u_Opacity;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform float u_LightStrength;
uniform vec3 u_LightColor;
uniform samplerCube skybox;
uniform sampler2D u_Texture;

uniform vec3 u_AmbientColor;
uniform vec3 u_DiffuseColor;


void main()
{    
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    vec3 lighting = vec3(0.0f, 0.0f, 0.0f);

    vec3 n = normalize(fs_in.Normal);

    vec3 I = normalize(u_ViewPos - fs_in.FragPos);
    vec3 R = reflect(-I, n);



    if(u_HasTexture == 1){
        color = texture(u_Texture, fs_in.TexCoords).rgb;
    }
    else{
        color = u_DiffuseColor;
    }
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = u_LightStrength * u_LightColor;
    // diffuse
    vec3 lightDir = normalize(u_LightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * u_LightColor;
    // specular
    vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * u_LightColor;    
    // calculate shadow

    lighting = (ambient + diffuse + specular) * color * texture(skybox, R).rgb;



    FragColor = vec4(lighting, u_Opacity);
}