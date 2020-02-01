#ifndef WAVE_SHADER_H
#define WAVE_SHADER_H

/************************************
* Headers
*************************************/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <glad/glad.h> 
#include <glfw3.h>
#include <iostream>
#include <stdio.h>

/************************************
* Defines
*************************************/
#define NUM_POINT_LIGHTS			10
#define NUM_SPOT_LIGHTS				4
#define NUM_DIRECTIONAL_LIGHTS		2
#define NUM_WAVES					7

/************************************
* Types
*************************************/
// 10 point lights
struct point_light 
{
    glm::vec3 position;
	
	glm::vec3 color;
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
	
	float constant_attenuation;
	float linear_attenuation;
	float quadratic_attenuation;
};  
// 4 spot lights
struct spot_light 
{
    glm::vec3 position;
	glm::vec3 direction;
	float dim_start_angle;
	float dim_end_angle;
	
	glm::vec3 color;
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
	
	float constant_attenuation;
	float linear_attenuation;
	float quadratic_attenuation;
};  
// 2 directional lights
struct directional_light 
{
	glm::vec3 direction;
	
	glm::vec3 color;
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
};
// 2 waves
struct wave_info
{
	float frequency;
	float amplitude;
	float phase_constant;
	float crest_constant;
	glm::vec2 direction;
};

class WaveShader
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

		struct point_light			*point_lights[ NUM_POINT_LIGHTS ];
		struct spot_light			*spot_lights[ NUM_SPOT_LIGHTS ];
		struct directional_light	*directional_lights[ NUM_DIRECTIONAL_LIGHTS ];
		struct wave_info			*waves_infos[ NUM_WAVES ];

		glm::vec3 camera_position;

	public:
		// static members:
		static const GLchar *u_model_mat4;		
		static const GLchar *u_view_mat4;
		static const GLchar *u_projection_mat4;
		static const GLchar *u_orthographic_mat4;
		static const GLchar *u_tex_mix_float;
		static const GLchar *u_all_color_vec3;
		static const GLchar *u_num_waves;
		static const GLchar *u_wave_time_float;
		static const GLchar *u_camera_position_vec3;

		static const GLchar *u_texture0_int;

		static const GLuint a_pos;
		static const GLuint a_normal;
		static const GLuint a_color;
		static const GLuint a_tex_coord;

		WaveShader();
		WaveShader(const char* vertex_path, const char* fragment_path);

		~WaveShader();

		void set_shaders(const char* vertex_path, const char* fragment_path);

		void check_shader_compile(unsigned int shader);
		void check_shader_program(unsigned int program);

		int num_point_lights = 0;
		int	num_spot_lights = 0;
		int	num_directional_lights = 0;
		int num_waves = 0;

		// user fills out own struct
		// shader keeps track of it and uses info in it to do everything
		void add_point_light( struct point_light * );
		void add_spot_light( struct spot_light * );
		void add_directional_light( struct directional_light * );
		void add_wave( struct wave_info * );
		void remove_point_light( struct point_light * );
		void remove_spot_light( struct spot_light * );
		void remove_directional_light( struct directional_light * );
		void remove_wave( struct wave_info * );

		void set_camera_position( glm::vec3 );
		void set_base_color( glm::vec3 );

		glm::vec3 get_point_light_position( struct point_light * );
		glm::vec3 get_point_light_color( struct point_light * );
		glm::vec3 get_spot_light_position( struct spot_light * );
		glm::vec3 get_spot_light_direction( struct spot_light * );
		glm::vec3 get_spot_light_color( struct spot_light * );

		void use_waves( );
		void use_lights( );
		void use();
		void stop();

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