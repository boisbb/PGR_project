#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Pos;

uniform mat4 lightSpaceMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * u_ModelMatrix * vec4(a_Pos, 1.0);
}

#shader fragment
#version 330 core

void main()
{             
    gl_FragDepth = gl_FragCoord.z;
}