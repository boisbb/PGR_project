#pragma once

#include <array>
#include <vector>

#include "vendor/glm/glm.hpp"


struct Vertex
{
  glm::vec3 position;
  glm::vec2 texUV;
  glm::vec3 normal;
};

class VertexBuffer{
private:
  unsigned int m_RendererID;
public:
  VertexBuffer(const void* data, unsigned int count);
  VertexBuffer(std::vector<Vertex>& vertices);
  VertexBuffer(std::vector<glm::vec3>& vertices);
  VertexBuffer(std::vector<glm::vec2>& vertices);
  ~VertexBuffer();

  void Bind() const;
  void Unbind() const;
};


