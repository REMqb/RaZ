#pragma once

#ifndef RAZ_GRAPHICOBJECTS_HPP
#define RAZ_GRAPHICOBJECTS_HPP

#include <vector>

#include "GL/glew.h"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

struct Vertex {
  Vec3f position {};
  Vec2f texcoords {};
  Vec3f normal {};
  Vec3f tangent {};

  std::size_t operator()() const { return tangent.hash(normal.hash(texcoords.hash(position.hash(0)))); }
  bool operator==(const Vertex& vert) const { return (position == vert.position)
                                                  && (texcoords == vert.texcoords)
                                                  && (normal == vert.normal)
                                                  && (tangent == vert.tangent); }
};

class ElementBuffer {
public:
  ElementBuffer() { glGenBuffers(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const std::vector<unsigned int>& getIndices() const { return m_indices; }
  std::vector<unsigned int>& getIndices() { return m_indices; }

  void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index); }
  void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

  ~ElementBuffer() { glDeleteBuffers(1, &m_index); }

private:
  GLuint m_index {};
  std::vector<unsigned int> m_indices {};
};

class VertexBuffer {
public:
  VertexBuffer() { glGenBuffers(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const std::vector<Vertex>& getVertices() const { return m_vertices; }
  std::vector<Vertex>& getVertices() { return m_vertices; }

  void bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_index); }
  void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

  ~VertexBuffer() { glDeleteBuffers(1, &m_index); }

private:
  GLuint m_index {};
  std::vector<Vertex> m_vertices {};
};

class VertexArray {
public:
  VertexArray() { glGenVertexArrays(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const ElementBuffer& getEbo() const { return m_ebo; }
  ElementBuffer& getEbo() { return m_ebo; }

  void bind() const { glBindVertexArray(m_index); m_ebo.bind(); }
  void unbind() const { glBindVertexArray(0); m_ebo.unbind(); }

  ~VertexArray() { glDeleteVertexArrays(1, &m_index); }

private:
  GLuint m_index {};
  ElementBuffer m_ebo {};
};

} // namespace Raz

#endif // RAZ_GRAPHICOBJECTS_HPP
