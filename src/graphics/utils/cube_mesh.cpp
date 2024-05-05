#include "cube_mesh.hpp"

#include <glm/ext.hpp>

#include <algorithm>
#include <iostream>

using point4 = glm::vec4;

namespace graphics::utilities {

struct vertex_data {
  std::vector<float> v_positions;
  std::vector<float> v_normals;
  std::vector<float> v_tex_coords;

  vertex_data(size_t n_vertices) {
    v_positions.reserve(4 * n_vertices);
    v_normals.reserve(3 * n_vertices);
    v_tex_coords.reserve(2 * n_vertices);
  }

  void append_quad(const point4* vertices, int a, int b, int c, int d) {
    // Initialize temporary vectors along the quad's edge to
    // compute its face normal
    glm::vec3 u(vertices[b] - vertices[a]);
    glm::vec3 v(vertices[c] - vertices[b]);
    glm::vec3 normal = glm::normalize(glm::cross(u, v));

    const glm::vec2 coords[] = {
        glm::vec2(0.0, 0.0), glm::vec2(0.0, 1.0), glm::vec2(1.0, 1.0),
        glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0), glm::vec2(1.0, 0.0),
    };

    const point4 vert[] = {
        vertices[a], vertices[b], vertices[c], vertices[a], vertices[c], vertices[d],
    };

    for (int i = 0; i < 6; ++i) {
      v_positions.push_back(vert[i].x);
      v_positions.push_back(vert[i].y);
      v_positions.push_back(vert[i].z);
      v_positions.push_back(vert[i].w);

      v_normals.push_back(normal.x);
      v_normals.push_back(normal.y);
      v_normals.push_back(normal.z);

      v_tex_coords.push_back(coords[i].x);
      v_tex_coords.push_back(coords[i].y);
    }
  }
};

constexpr size_t k_max_instances = 1000;

mesh_data make_cube_mesh_arrays(float width, float height, float depth) {
  float hW = width / 2;
  float hH = height / 2;
  float hD = depth / 2;

  point4 vertices[] = {point4(-hW, -hH, hD, 1.0),  point4(-hW, hH, hD, 1.0),
                       point4(hW, hH, hD, 1.0),    point4(hW, -hH, hD, 1.0),
                       point4(-hW, -hH, -hD, 1.0), point4(-hW, hH, -hD, 1.0),
                       point4(hW, hH, -hD, 1.0),   point4(hW, -hH, -hD, 1.0)};

  vertex_data data(36);
  data.append_quad(vertices, 1, 0, 3, 2);
  data.append_quad(vertices, 2, 3, 7, 6);
  data.append_quad(vertices, 3, 0, 4, 7);
  data.append_quad(vertices, 6, 5, 1, 2);
  data.append_quad(vertices, 4, 5, 6, 7);
  data.append_quad(vertices, 5, 4, 0, 1);

  // set up vertex buffer object for vertex positions, normals, and texture
  // coordinates
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLuint size_vertices = data.v_positions.size() * sizeof(float);
  GLuint size_normals = data.v_normals.size() * sizeof(float);
  GLuint size_tex_coords = data.v_tex_coords.size() * sizeof(float);
  GLuint data_size = size_vertices + size_normals + size_tex_coords;

  GLuint offset_vertices = 0;
  GLuint offset_normals = size_vertices;
  GLuint offset_tex_coords = size_vertices + size_normals;

  glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, offset_vertices, size_vertices,
                  data.v_positions.data());
  glBufferSubData(GL_ARRAY_BUFFER, offset_normals, size_normals, data.v_normals.data());
  glBufferSubData(GL_ARRAY_BUFFER, offset_tex_coords, size_tex_coords,
                  data.v_tex_coords.data());

  // buffer for instanced drawing with model matrices
  GLuint model_matrix_buffer;
  glGenBuffers(1, &model_matrix_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float) * k_max_instances, NULL,
               GL_STATIC_DRAW);

  // set up vertex array object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // setup the attributes of the vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)(offset_vertices));

  // normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset_normals));

  // texture coordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(offset_tex_coords));

  // setup the attributes of the model-matrix buffer
  // TODO: consider using a uniform buffer or shader storage buffer instead
  glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);

  size_t model_matrix_location = 3;
  for (size_t i = 0; i < 4; ++i) {
    glEnableVertexAttribArray(model_matrix_location + i);
    glVertexAttribPointer(model_matrix_location + i, 4, GL_FLOAT, GL_FALSE, 64,
                          (void*)(i * 16));
    glVertexAttribDivisor(model_matrix_location + i,
                          1);  // for instanced drawing
  }

  return mesh_data{vao, model_matrix_buffer};
}

GLint make_cube_mesh_elements() {
  return 0;
}

void update_matrix_buffer(GLuint buffer, std::vector<glm::mat4>& matrices) {
  size_t len = matrices.size();
  size_t stride = 16;

  if (len > k_max_instances) {
    std::cout << "Error: too many model matrices for array buffer.";
    return;
  }

  // update scatter position data
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  auto p_data =
      (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, stride * sizeof(float) * len,
                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

  if (p_data == nullptr) {
    std::cout << "Erros: failed to map position data.\n";
    glUnmapBuffer(GL_ARRAY_BUFFER);  // unmap buffer necessary?
    return;
  }

  // copy each marix's data
  size_t offset = 0;

  for (const auto& matrix : matrices) {
    const float* p_source = (const float*)glm::value_ptr(matrix);
    std::copy(p_source, p_source + stride, p_data + offset);

    offset += stride;
  }

  glUnmapBuffer(GL_ARRAY_BUFFER);
}

}  // namespace graphics::utilities
