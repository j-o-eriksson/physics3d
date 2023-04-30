
#include "phong_shader.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <iostream>

#include <shader/shader.hpp>

namespace graphics::shader {

const std::string vert_src = R"(

#version 420

// attributes
layout(location = 0) in vec4 i_position; // xyz - position
layout(location = 1) in vec3 i_normal; // xyz - normal
layout(location = 2) in vec2 i_texcoord0; // xy - texture coords
layout(location = 3) in mat4 instance_model_mat;

// TODO: add model matrix and color buffers to input

// matrices
// uniform mat4 u_modelMat; // TODO: buffer instead
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

// The camera's position in world coordinates
uniform vec3 u_cameraPosition;

// data for fragment shader
out vec3 o_normal;
out vec3 o_toCamera;
out vec2 o_texcoords;

out vec3 o_worldPos;
///////////////////////////////////////////////////////////////////

void main(void)
{
   // position in world space
   vec4 worldPosition = instance_model_mat * i_position; // vec4(i_position, 1);

   //
   o_worldPos = worldPosition.xyz;
   
   // normal in world space
   o_normal = normalize( (instance_model_mat * vec4(i_normal, 0.0)).xyz );

   // direction to camera
   o_toCamera = normalize(u_cameraPosition - worldPosition.xyz);

   // texture coordinates to fragment shader
   o_texcoords = i_texcoord0; 

   // screen space coordinates of the vertex
   gl_Position = u_projMat * u_viewMat * worldPosition;
} 

)";

const std::string frag_src = R"(

#version 420

// data from vertex shader
in vec3 o_normal;
in vec3 o_toCamera;
in vec2 o_texcoords;
in vec3 o_worldPos;

// color for framebuffer
out vec4 resultingColor;

/////////////////////////////////////////////////////////

// texture with diffuse color of the object
uniform vec3 u_diffuse_color;
// uniform sampler2D u_diffuseTexture; // TODO: put colors in buffer object instead

uniform vec3 u_AmbientProduct;
uniform vec3 u_DiffuseProduct;
uniform vec3 u_SpecularProduct;

uniform float u_matShininess; // = 64;

#define MAX_NUM_LIGHTS 12
uniform vec3 u_lightPositions[MAX_NUM_LIGHTS];
uniform int u_numLights;

/////////////////////////////////////////////////////////

// returns intensity of reflected ambient lighting
vec3 ambientLighting()
{
   return u_AmbientProduct;
}

// returns intensity of diffuse reflection
vec3 diffuseLighting(in vec3 N, in vec3 L)
{
   // calculation as for Lambertian reflection
   float diffuseTerm = max(dot(N, L), 0.0) ;
   return u_DiffuseProduct * diffuseTerm;
}

// returns intensity of specular reflection
vec3 specularLighting(in vec3 N, in vec3 L, in vec3 V)
{
	vec3 H = normalize(L + V);
	
   	float Ks = pow(max(dot(N, H), 0.0), u_matShininess);   
	vec3 specular = Ks*u_SpecularProduct;

	// discard the specular highlight if the light's behind the vertex    
	if( dot(L, N) < 0.0 )	
	{		
		specular = vec3(0.0, 0.0, 0.0);
	}	
   
	return specular;
}



void main(void)
{
   // normalize vectors after interpolation
   vec3 V = normalize(o_toCamera); 
   vec3 N = normalize(o_normal);

   // get Blinn-Phong reflectance components
   vec3 Iamb = ambientLighting();
   
   // diffuse color of the object from texture
   // vec3 diffuseColor = texture(u_diffuseTexture, o_texcoords).rgb;
   
   // Apply light from all lights
   vec3 colorSum = vec3(0.0, 0.0, 0.0);
   for(int i = 0; i < u_numLights; i++)
   {
		vec3 lightDir = u_lightPositions[i] - o_worldPos;
		
		float Kdi = 2 / max(length(lightDir), 2.0);

		vec3 L = normalize(lightDir);
		
		vec3 Idif = diffuseLighting(N, L);
		vec3 Ispe = specularLighting(N, L, V);
		
		// combination of all components and diffuse color of the object
		// colorSum += diffuseColor * (Iamb + Kdi * (Idif + Ispe));
        colorSum += u_diffuse_color * (Iamb + Kdi * (Idif + Ispe));
   }
   
   resultingColor = vec4(colorSum, 1.0);
}

)";

const std::string simple_vert_src = R"(

#version 420

// attributes
layout(location = 0) in vec4 i_position; // xyz - position
layout(location = 1) in vec3 i_normal; // xyz - normal

// matrices
uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

out vec3 o_normal;

void main(void)
{
   o_normal = normalize( (vec4(i_normal, 0.0)).xyz );
   gl_Position = u_projMat * u_viewMat * u_modelMat * i_position;
} 

)";

const std::string simple_frag_src = R"(

#version 420

uniform vec3 u_diffuse_color;

in vec3 o_normal;

out vec4 fragment;

void main(void)
{
    fragment = vec4(o_normal, 1.0);
    // fragment = vec4(0.75, 0.75, 0.0, 1.0);
}

)";

phong_shader::phong_shader() {
  // compile shader program
//   m_program = shader::compile_program(simple_vert_src.c_str(), simple_frag_src.c_str());
  m_program = shader::compile_program(vert_src.c_str(), frag_src.c_str());

  // get uniform locations
  u_model_mat = glGetUniformLocation(m_program, "u_modelMat");
  u_view_mat = glGetUniformLocation(m_program, "u_viewMat");
  u_proj_mat = glGetUniformLocation(m_program, "u_projMat");
  u_camera_pos = glGetUniformLocation(m_program, "u_cameraPosition");

  u_ambient_prod = glGetUniformLocation(m_program, "u_AmbientProduct");
  u_diffuse_prod = glGetUniformLocation(m_program, "u_DiffuseProduct");
  u_specular_prod = glGetUniformLocation(m_program, "u_SpecularProduct");
  u_material_shininess = glGetUniformLocation(m_program, "u_matShininess");

  u_light_positions = glGetUniformLocation(m_program, "u_lightPositions[0]");
  u_num_lights = glGetUniformLocation(m_program, "u_numLights");

  // initialize uniforms with default values
  bind();

  // model matrix
  glm::mat4 model_matrix(1.f);
  set_model_matrix(model_matrix);

  // camera position
  glm::vec3 camera_position(0.f, 0.f, 30.f);
  set_camera_pos(camera_position);

  // view matrix
  glm::mat4 view_matrix =
      glm::lookAt(camera_position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
  set_view_matrix(view_matrix);

  // projection matrix

  glm::mat4 proj_matrix = glm::perspective(glm::pi<float>() * 60.f / 180.f,
                                           640 / (float)480, 0.1f, 100.f);
  set_projection_matrix(proj_matrix);

  // light products
  glm::vec3 light_ambient(0.01f, 0.01f, 0.01f);
  glm::vec3 ambient_product = light_ambient;
  set_ambient_product(ambient_product);

  glm::vec3 light_diffuse(1.f, 1.f, 1.f);
  glm::vec3 diffuse_product = light_diffuse;
  set_diffuse_product(diffuse_product);

  glm::vec3 light_specular(1.f, 1.f, 1.f);
  glm::vec3 material_specular(1.f, 1.f, 1.f);
  glm::vec3 specular_product = light_specular * material_specular;
  set_specular_product(specular_product);

  float material_shininess = 100.0;
  set_material_shininess(material_shininess);

  // lights
  set_num_lights(2);
  set_light_position(0, glm::vec3(-2.5f, 2.5f, 1.f));
  set_light_position(1, glm::vec3( 2.5f, 2.5f, 1.f));

  // uniform cube color
  glUniform3f(glGetUniformLocation(m_program, "u_diffuse_color"), 0.75f, 0.75f, 0.f);

  print_uniform_locations();
}

phong_shader::~phong_shader() {
  glUseProgram(0);

  // destroy program?
}

void phong_shader::bind() { glUseProgram(m_program); }

void phong_shader::set_model_matrix(const glm::mat4& m) {
  glUniformMatrix4fv(u_model_mat, 1, GL_FALSE, glm::value_ptr(m));
}

void phong_shader::set_view_matrix(const glm::mat4& m) {
  glUniformMatrix4fv(u_view_mat, 1, GL_FALSE, glm::value_ptr(m));
}

void phong_shader::set_projection_matrix(const glm::mat4& m) {
  glUniformMatrix4fv(u_proj_mat, 1, GL_FALSE, glm::value_ptr(m));
}

void phong_shader::set_camera_pos(const glm::vec3& v) {
  glUniform3f(u_camera_pos, v.x, v.y, v.z);
}

void phong_shader::set_ambient_product(const glm::vec3& v) {
  glUniform3f(u_ambient_prod, v.x, v.y, v.z);
}

void phong_shader::set_diffuse_product(const glm::vec3& v) {
  glUniform3f(u_diffuse_prod, v.x, v.y, v.z);
}

void phong_shader::set_specular_product(const glm::vec3& v) {
  glUniform3f(u_specular_prod, v.x, v.y, v.z);
}

void phong_shader::set_material_shininess(float value) {
  glUniform1f(u_material_shininess, value);
}

void phong_shader::set_light_position(size_t index, const glm::vec3& v) {
  if (index < u_num_lights) {
    glUniform3f(u_light_positions + index, v.x, v.y, v.z);
  }
}

void phong_shader::set_num_lights(int n) {
  glUniform1i(u_num_lights, n);
  // glUniform1ui(u_num_lights, GLuint ui);
}

void phong_shader::print_uniform_locations() {
  std::cout << "u_model_mat: " << u_model_mat << "\n"
            << "u_view_mat: " << u_view_mat << "\n"
            << "u_proj_mat: " << u_proj_mat << "\n"
            << "u_camera_pos: " << u_camera_pos << "\n"
            << "u_ambient_prod: " << u_ambient_prod << "\n"
            << "u_diffuse_prod: " << u_diffuse_prod << "\n"
            << "u_specular_prod: " << u_specular_prod << "\n"
            << "u_material_shininess: " << u_material_shininess << "\n"
            << "u_light_positions: " << u_light_positions << "\n"
            << "u_num_lights: " << u_num_lights << "\n"
            << "u_diffuse_color: "
            << glGetUniformLocation(m_program, "u_diffuse_color") << "\n";
}

}  // namespace graphics::shader
