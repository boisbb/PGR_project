#shader vertex
#version 460 core
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;


out vec3 v_Color;
out vec3 v_CurrentPosition;
//out vec2 v_TexCoord;
//out vec3 v_Normal;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;

void main(){
  v_CurrentPosition = vec3(u_ModelMatrix * vec4(a_Pos, 1.0f));
  gl_Position = u_CameraMatrix * vec4(v_CurrentPosition, 1.0);
  v_Color = a_Color;
  //v_TexCoord = a_TexCoord;
  //v_Normal = a_Normal;
};

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 v_Color;
in vec3 v_CurrentPosition;
//in vec2 v_TexCoord;
//in vec3 v_Normal;

//uniform sampler2D diffuse0;
//uniform sampler2D specular0;
//uniform vec4 u_LightColor;
//uniform vec3 u_LightPosition;
//uniform vec3 u_CameraPosition;

void main(){
  color = vec4(v_Color, 1.0f);
};
