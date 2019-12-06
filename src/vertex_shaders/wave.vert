#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

out v_MATERIAL
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} v_material_out;
out vec3 v_model_position;
out vec3 v_world_position;
out vec3 v_normal;
out vec4 v_object_color;
out vec2 v_tex_coord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform vec3 u_all_color;

uniform float u_wave_x_t;
uniform float u_wave_x_amp;
uniform float u_wave_x_freq;
uniform float u_wave_x_phase;
uniform float u_wave_x_vert;
uniform float u_wave_z_t;
uniform float u_wave_z_amp;
uniform float u_wave_z_freq;
uniform float u_wave_z_phase;
uniform float u_wave_z_vert;


void main()
{
	vec3 new_pos = a_pos;
	if( a_pos.y >= 3.0 )
	{	
		// find wave height:
		// z wave
		new_pos.y = u_wave_x_amp * sin( u_wave_x_freq*(u_wave_x_t + a_pos.z + u_wave_x_phase) ) + u_wave_x_vert + a_pos.y;
		// x wave
		new_pos.y = u_wave_z_amp * cos( u_wave_z_freq*(u_wave_z_t + a_pos.x + u_wave_z_phase) ) + u_wave_z_vert + new_pos.y;
		// find wave color:
		float saturation = 0.3 + ( (new_pos.y - a_pos.y) + (u_wave_x_amp + u_wave_z_amp) ) / (2 * (u_wave_x_amp + u_wave_z_amp) );
		v_material_out.ambient = vec4( u_all_color * saturation / 10.0, 1.0 );
		v_material_out.diffuse = vec4( u_all_color * saturation, 1.0 );
		v_material_out.specular = vec4( u_all_color * saturation, 1.0 );
		v_material_out.shininess = 16.0f;
	}
	else
	{
		// color
		v_material_out.ambient = vec4( u_all_color / 10.0, 1.0 );
		v_material_out.diffuse = vec4( u_all_color, 1.0 );
		v_material_out.specular = vec4( u_all_color, 1.0 );
		v_material_out.shininess = 16.0f;
	}
	
	// apply position and pipe out
	v_model_position.xyz = new_pos.xyz;
	v_world_position.xyz = u_model * new_pos.xyz;
    gl_Position = u_projection * u_view * u_model * vec4(new_pos, 1.0);
	
	// apply normal matrix to normal and pipe out
	v_normal = mat3( transpose( inverse( u_model ) ) ) * a_normal;
	
	// pipe out texture coords
	v_tex_coord = a_tex_coord;
	
	
}