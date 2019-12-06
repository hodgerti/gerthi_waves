#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out vec4 v_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform vec3 u_all_color;

void main()
{
	v_color = vec4( u_all_color, 1.0 );
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
}