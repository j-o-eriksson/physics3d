#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>

namespace graphics::shader {

/* Compile and return program from vertex and fragments shader sources. Prints
 * error if compilation fails. */
GLuint compile_program(const char* vert_src, const char* frag_src);


} // namespace graphics::shader

#endif // SHADER_HPP