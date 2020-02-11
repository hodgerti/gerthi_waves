#version 330 compatibility
in vec3 v_tex_coord;

uniform samplerCube u_skybox;

void main( )
{
	gl_FragColor = texture( u_skybox, v_tex_coord );
}