#ifndef CUBE_MESH_HPP
#define CUBE_MESH_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>


namespace graphics::utilities {

struct mesh_data {
    GLuint vertex_array_object;
    GLuint matrix_buffer_object;
};

/* Generates a cube mesh with vertex positions, normals, and texture
 * coordinates intended to be used with glDrawArrays. */
mesh_data make_cube_mesh_arrays(float width, float height, float depth);

/* Generates a cube mesh with vertex positions, normals, and texture
 * coordinates intended to be used with glDrawElements. */
GLint make_cube_mesh_elements();

/* Overwrites the array buffer's data with matrices. Starts writing from the
 * beginning of the buffer. */
void update_matrix_buffer(GLuint buffer, std::vector<glm::mat4>& matrices);

} // namespace graphics::utilities

#endif // CUBE_MESH_HPP