#shader vertex
#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;


// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;

void main(){
  gl_Position = u_CameraMatrix * vec4(u_ModelMatrix * vec4(a_Pos, 1.0f));
  FragPos = vec3(u_ModelMatrix * vec4(a_Pos, 1.0));
  UV = a_UV;
  Normal = mat3(transpose(inverse(u_ModelMatrix))) * a_Normal;
};

#shader fragment
#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 FragPos;
in vec3 Normal;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D u_Texture;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform float u_LightStrength;
uniform vec3 u_LightColor;
uniform vec3 u_AmbientColor;
uniform vec3 u_SpecularColor;
uniform vec3 u_DiffuseColor;
// texture(u_Texture, UV)

void main(){

    vec3 result = vec3(0.0, 0.0, 0.0);

    //vec3 u_LightColor = vec3(1.0f, 1.0f, 1.0f);
    //vec3 u_LightPos = vec3(4,4,4);

    //float ambientStrength = 0.33;
    vec3 ambient = u_LightStrength * u_LightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    float specularStrength = 0.5;

    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_LightColor;


    result = (ambient * u_AmbientColor + diffuse * u_DiffuseColor) * vec3(1, 1, 1);

    //result = (ambient + diffuse + specular) * vec3(0.0, 0.0, 0.0);

    color = vec4(u_DiffuseColor * (diffuse + ambient + specular), 1.0f);
}
