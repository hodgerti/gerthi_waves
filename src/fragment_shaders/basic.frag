#version 330 core
in vec4 v_color;
in vec3 v_normal;
in vec2 v_tex_coord;

uniform sampler2D u_tex_0;

void main()
{
	gl_FragColor = v_color;
} 