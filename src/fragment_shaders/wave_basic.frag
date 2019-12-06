#version 330 core
in vec4 v_our_color;
in vec3 v_pos;
in vec2 v_tex_coord;

uniform float u_circle_inner_radius;
uniform float u_circle_outer_radius;
uniform vec2 u_circle_origin;

uniform sampler2D u_tex_0;

void main()
{
	if( v_pos.y > 0.5 )
	{
		vec2 compare = vec2( v_pos.x, v_pos.z ) - u_circle_origin;
		float compare_length = length( compare );
		if( compare_length >= u_circle_inner_radius && compare_length <= u_circle_outer_radius )
		{
			float percent = compare_length / u_circle_outer_radius - 0.1;
			vec4 color = vec4( percent, 0.02, 1.0 - percent, 1.0 );
			gl_FragColor = color;
			// gl_FragColor = texture( u_tex_0, v_tex_coord );
			return;
		}
	}
	
	gl_FragColor = v_our_color;
} 