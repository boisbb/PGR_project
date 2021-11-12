#include "Shader.h"
#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource src = ParseShader(filepath);
	m_RendererID = CreateShader(src.VertexSource, src.FragmentSource);

}

Shader::~Shader(){
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1f(const std::string& name, float value) 
{
  GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string& name, int value) 
{
  GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) 
{
  GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const string& name, float v0, float v1, float v2, float v3){
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat1i(const std::string& name, const int* value) 
{
  GLCall(glUniform1iv(GetUniformLocation(name), 2, value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) 
{
  GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const string& name){
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1) {
		cout << "Warning: uniform " << name << " - doesn't exist!" << endl;
	}
	m_UniformLocationCache[name] = location;

	return location;
}


unsigned int Shader::CreateShader(const string& vertexShader, const string& fragmentShader){
  GLCall(unsigned int program = glCreateProgram());
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  GLCall(glAttachShader(program, vs));
  GLCall(glAttachShader(program, fs));
  GLCall(glLinkProgram(program));
  GLCall(glValidateProgram(program));

  GLCall(glDeleteShader(vs));
  GLCall(glDeleteShader(fs));

  return program;
}

unsigned int Shader::CompileShader(unsigned int type, const string& source){
  GLCall(unsigned int id = glCreateShader(type));
  const char* src = source.c_str();
  GLCall(glShaderSource(id, 1, &src, NULL));
  GLCall(glCompileShader(id));

  int res;
  GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &res));
  if (res == GL_FALSE) {
    int length;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
    char* message = (char*)alloca(length * sizeof(char));
    GLCall(glGetShaderInfoLog(id, length, &length, message));
    cout << "Failed to compile shader!" << endl;
    cout << message << endl;
    GLCall(glDeleteShader(id));
    return 0;
  }

  // TODO: error handling

  return id;
}

ShaderProgramSource Shader::ParseShader(const string& filepath){

  enum class ShaderType{
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
  };

  ifstream stream(filepath);
  string line;
  stringstream ss[2];
  ShaderType type = ShaderType::NONE;



  while (getline(stream, line)) {
    if (line.find("#shader") != string::npos ) {
      if (line.find("vertex") != string::npos) {
        type = ShaderType::VERTEX;
      }
      else if (line.find("fragment") != string::npos) {
        type = ShaderType::FRAGMENT;
      }
    }
    else{
      ss[(int)type] << line << endl;
    }
  }

  return { ss[0].str(), ss[1].str() };
}
