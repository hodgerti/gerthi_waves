#version 330 core
layout (location = 0) in vec3 a_pos;

out vec3 v_tex_coord;

uniform mat4 u_view;
uniform mat4 u_projection;

void main( )
{
	v_tex_coord = a_pos;
	gl_Position = u_projection * u_view * vec4(a_pos, 1.0);
}