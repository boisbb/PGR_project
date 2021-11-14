#shader vertex
#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;


// Output data ; will be interpolated for each fragment.
out vec2 UV;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;

void main(){
  gl_Position = u_CameraMatrix * vec4(u_ModelMatrix * vec4(a_Pos, 1.0f));
  UV = a_UV;
};

#shader fragment
#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D u_Texture;

void main(){

  vec4 texColor = texture(u_Texture, UV);
  color = texColor;
}
