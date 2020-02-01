#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

out vec4 v_color;
out vec3 v_normal;
out vec2 v_tex_coord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform vec3 u_all_color;

void main()
{
	v_color = vec4( u_all_color, 1.0 );
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
	
	// apply normal matrix to normal and pipe out
	v_normal = mat3( transpose( inverse( u_model ) ) ) * a_normal;
	
	// pipe out texture coords
	v_tex_coord = a_tex_coord;
}