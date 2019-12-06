#ifndef SHADERS_H
#define SHADERS_H

/************************************
* Headers
*************************************/
#include <glad/glad.h> 
#include <glfw3.h>
#include <iostream>
#include <stdio.h>

/************************************
* Types
*************************************/
class Shader
{
	private:

		unsigned int vert;
		unsigned int frag;
		unsigned int program;

		char *vert_path;
		char *frag_path;

		void compile_shaders();

		void link_shaders();

		void read_shader_file(char *path, char **source);

	public:
		// static members:
		static const GLchar *u_model_mat4;		
		static const GLchar *u_view_mat4;
		static const GLchar *u_projection_mat4;
		static const GLchar *u_orthographic_mat4;
		static const GLchar *u_all_color_vec3;

		static const GLuint a_pos;
		static const GLuint a_normal;
		static const GLuint a_color;
		static const GLuint a_tex_coord;

		Shader();
		Shader(const char* vertex_path, const char* fragment_path);

		~Shader();

		void set_shaders(const char* vertex_path, const char* fragment_path);

		void check_shader_compile(unsigned int shader);
		void check_shader_program(unsigned int program);

		void use();

		unsigned int get_vert();
		unsigned int get_frag();
		unsigned int get_program();

		void set_uniform1f(const char* name, float x) const;
		void set_uniform2f(const char* name, float x, float y) const;
		void set_uniform3f(const char* name, float x, float y, float z) const;
		void set_uniform4f(const char* name, float x, float y, float z, float w) const;
		void set_uniform1i(const char* name, int x) const;

};

/************************************
* Functions
*************************************/

#endif