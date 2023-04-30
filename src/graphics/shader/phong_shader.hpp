#ifndef PHONG_SHADER_HPP
#define PHONG_SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

namespace graphics::shader {

/* With point lighting. Does not render the lights themselves. */
class phong_shader {
 public:
  phong_shader();

  ~phong_shader();

  /* bind must be called before the renderer's draw call */
  void bind();

  /* bind must be called before calling setters */

  // setters for the vertex shader uniforms
  void set_model_matrices(const std::vector<glm::mat4>& matrices);

  void set_model_matrix(const glm::mat4& m);

  void set_view_matrix(const glm::mat4& m);

  void set_projection_matrix(const glm::mat4& m);

  void set_camera_pos(const glm::vec3& pos);

  // setters for the fragment shader uniforms
  void set_ambient_product(const glm::vec3& v);

  void set_diffuse_product(const glm::vec3& v);

  void set_specular_product(const glm::vec3& v);

  void set_material_shininess(float value);

  void set_light_position(size_t index, const glm::vec3& v);

  void set_num_lights(int n);

 private:
  // vertex uniforms
  GLint u_model_mat;  // TODO: put in buffer for instanced drawing
  GLint u_view_mat;
  GLint u_proj_mat;
  GLint u_camera_pos;

  // fragment uniforms
  GLint u_ambient_prod;
  GLint u_diffuse_prod;
  GLint u_specular_prod;
  GLint u_material_shininess;

  GLint u_light_positions;
  GLint u_num_lights;

  GLuint m_program;

  void print_uniform_locations();
};

}  // namespace graphics::shader

#endif  // PHONG_SHADER_HPP