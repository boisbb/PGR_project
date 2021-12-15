#shader vertex

// This whole shader code was heavily influenced by code derived from this link:
// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// Author: Joe de Vriez (https://twitter.com/JoeyDeVriez)
// licensed under CC BY 4.0


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

uniform vec3 u_DiffuseColor;
uniform int u_HasTexture;
uniform float u_Opacity;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform float u_LightStrength;
uniform vec3 u_LightColor;

uniform samplerCube skybox;
uniform sampler2D shadowMap;
uniform sampler2D u_Texture;




float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Transform coords to NDC
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform NDC to range [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    
    // Retrieve the closest depth from the lights POV
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // Get depth of the current fragment
    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(u_LightPos - fs_in.FragPos);

    // Calculate bias based on the the surface angle towards the light
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // Percentage-closer filtering:
    // - sample more than once with slightly different coords
    // - each sample is checked whether it is in shadow or not
    // - all the sub-results are averaged and combined
    // --> VOILA - softer shadow
    // --> here -> sample the surrounding texels of depth map and average
    float shadow = 0.0;

    // textureSize -> return vec2(width, height) of texture
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


void main()
{    
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    vec3 lighting = vec3(0.0f, 0.0f, 0.0f);

    vec3 I = normalize(fs_in.FragPos - u_ViewPos);
    vec3 R = reflect(I, normalize(fs_in.Normal));

    if(u_HasTexture == 1){
        color = texture(u_Texture, fs_in.TexCoords).rgb;
    }
    else{
        color = u_DiffuseColor;
    }

    vec3 normal = normalize(fs_in.Normal);

    // Compute ambient lighting
    vec3 ambient = u_LightStrength * u_LightColor;

    // Compute light direction
    vec3 lightDir = normalize(u_LightPos - fs_in.FragPos);

    // Compute diffuse lighting
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * u_LightColor;

    // Compute view direction
    vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);

    // Compute reflectionDirection
    vec3 reflectDir = reflect(-lightDir, normal);

    // Compute specular lighting
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * u_LightColor;    

    // Calculate the shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    // Compute the final lighting based on uniforms
    if(u_ReflectionsBool == 1 && u_ShadowsBool == 1){
        lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color * texture(skybox, R).rgb;
    }
    else if(u_ReflectionsBool == 1 && u_ShadowsBool != 1){
        lighting = (ambient + diffuse + specular) * color * texture(skybox, R).rgb;
    }
    else if(u_ReflectionsBool != 1 && u_ShadowsBool == 1){
        lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    }
    else if(u_ReflectionsBool != 1 && u_ShadowsBool != 1){
        lighting = (ambient + diffuse + specular) * color;
    }


    FragColor = vec4(lighting, u_Opacity);
}