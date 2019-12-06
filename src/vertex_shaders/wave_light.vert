#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

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

struct wave_info
{
	float frequency;
	float amplitude;
	float phase_constant;
	float roll_constant;
	float crest_constant;
	vec2 direction;
};
#define NUM_WAVES		3
uniform int				u_num_waves;
uniform wave_info 		u_waves_infos[ NUM_WAVES ];
uniform float			u_wave_time;

vec3 gerstner_wave( vec3 start_position, float time )
{
	vec2 position = vec2(start_position.x, start_position.z);
	
	float x_value = start_position.x;
	for( int wdx = 0; wdx < u_num_waves; wdx++ )
	{
		x_value += u_waves_infos[wdx].crest_constant*u_waves_infos[wdx].amplitude * u_waves_infos[wdx].direction.x*cos(dot(u_waves_infos[wdx].frequency*u_waves_infos[wdx].direction, position) + u_waves_infos[wdx].phase_constant*time);
	}
	float z_value = start_position.z;
	for( int wdx = 0; wdx < u_num_waves; wdx++ )
	{
		z_value += u_waves_infos[wdx].crest_constant*u_waves_infos[wdx].amplitude * u_waves_infos[wdx].direction.y*cos(dot(u_waves_infos[wdx].frequency*u_waves_infos[wdx].direction, position) + u_waves_infos[wdx].phase_constant*time);
	}
	float height = start_position.y;
	for( int wdx = 0; wdx < u_num_waves; wdx++ )
	{
		height += u_waves_infos[wdx].amplitude*sin(dot(u_waves_infos[wdx].frequency*u_waves_infos[wdx].direction, position) + u_waves_infos[wdx].phase_constant*time);
	}
	
	return vec3(x_value, height, z_value);
	
}

void main()
{
	vec3 new_pos = a_pos;
	vec3 new_normal = a_normal;
	
	if( a_pos.y >= 0.5f )
	{	
		// find wave height:
		new_pos = gerstner_wave( a_pos, u_wave_time );
		// vec3 binormal = vec3(1.0, dFdx_all(flat_pos, u_wave_time), 0.0);  // for x
		// vec3 tangent = vec3(0.0, dFdy_all(flat_pos, u_wave_time), 1.0);  // for z
		// new_normal = cross(binormal, tangent);
		
		// find wave color:
		v_material_out.ambient = vec3( 0.01, 0.1, 0.06 );
		v_material_out.diffuse = vec3( 0.01, 0.50980392, 0.50980392 );
		v_material_out.specular = vec3( 0.50196078, 0.50196078, 0.50196078 );
		v_material_out.shininess = 51.2;
	}
	else
	{
		// black out
		v_material_out.ambient = vec3( 0.0, 0.0, 0.0 );
		v_material_out.diffuse = vec3( 0.0, 0.0, 0.0 );
		v_material_out.specular = vec3( 0.0, 0.0, 0.0 );
		v_material_out.shininess = 1.0;
	}
	
	
	// apply position and pipe out
	v_model_position = new_pos;
	v_world_position.xyz = ( u_model * vec4(new_pos, 1.0) ).xyz;
    gl_Position = u_projection * u_view * u_model * vec4(new_pos, 1.0);
	
	// apply normal matrix to normal and pipe out
	v_normal = mat3( transpose( inverse( u_model ) ) ) * new_normal;
	
	// pipe out texture coords
	v_tex_coord = a_tex_coord;
	
	
}