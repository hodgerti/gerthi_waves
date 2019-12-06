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
	vec2 direction;
};
#define NUM_WAVES		2
uniform int				u_num_waves;
uniform wave_info 		u_waves_infos[ NUM_WAVES ];
uniform float			u_wave_time;

float wave_height( float frequency, float amplitude, float phase_constant, float roll_constant, vec2 direction, vec2 position, float time )
{
	return 2.0 * amplitude * pow( (sin((dot(direction, position) * frequency) + (time * phase_constant)) + 1.0) / 2.0, roll_constant );
}

float wave_heights( vec2 position, float time )
{
	float height = 0.0;
	for( int wdx = 0; wdx < u_num_waves; wdx++ )
	{
		height += wave_height( u_waves_infos[wdx].frequency, u_waves_infos[wdx].amplitude, u_waves_infos[wdx].phase_constant, u_waves_infos[wdx].roll_constant, u_waves_infos[wdx].direction, position, time );
	}
	return height;
}

float dFdx_mine( float frequency, float amplitude, float phase_constant, float roll_constant, vec2 direction, vec2 position, float time )
{
	return roll_constant*direction.x*frequency*amplitude*pow((sin((dot(direction,position)*frequency)+(time*phase_constant))+1.0)/2.0,roll_constant-1.0)*cos((dot(direction,position)*frequency)+(time*phase_constant)); 
}

float dFdy_mine( float frequency, float amplitude, float phase_constant, float roll_constant, vec2 direction, vec2 position, float time )
{
	return roll_constant*direction.y*frequency*amplitude*pow((sin((dot(direction,position)*frequency)+(time*phase_constant))+1.0)/2.0,roll_constant-1.0)*cos((dot(direction,position)*frequency)+(time*phase_constant)); 
}

float dFdx_all( vec2 position, float time )
{
	float value = 0.0;
	for( int wdx = 0; wdx < u_num_waves; wdx++ )
	{
		value += dFdx_mine( u_waves_infos[wdx].frequency, u_waves_infos[wdx].amplitude, u_waves_infos[wdx].phase_constant, u_waves_infos[wdx].roll_constant, u_waves_infos[wdx].direction, position, time );
	}
	return value;	
}

float dFdy_all( vec2 position, float time )
{
	float value = 0.0;
	for( int wdx = 0; wdx < u_num_waves; wdx++ )
	{
		value += dFdy_mine( u_waves_infos[wdx].frequency, u_waves_infos[wdx].amplitude, u_waves_infos[wdx].phase_constant, u_waves_infos[wdx].roll_constant, u_waves_infos[wdx].direction, position, time );
	}
	return value;	
}

void main()
{
	vec3 new_pos = a_pos;
	vec3 new_normal = a_normal;
	
	if( a_pos.y >= 0.5f )
	{	
		// find wave height:
		vec2 flat_pos = vec2(a_pos.x, a_pos.z);
		float height = wave_heights(flat_pos, u_wave_time);
		new_pos = vec3(a_pos.x, height + a_pos.y, a_pos.z);
		vec3 binormal = vec3(1.0, dFdx_all(flat_pos, u_wave_time), 0.0);  // for x
		vec3 tangent = vec3(0.0, dFdy_all(flat_pos, u_wave_time), 1.0);  // for z
		new_normal = cross(binormal, tangent);
		
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
	v_normal = mat3( transpose( inverse( u_model ) ) ) * a_normal;
	// v_normal = mat3( transpose( inverse( u_model ) ) ) * new_normal;
	
	// pipe out texture coords
	v_tex_coord = a_tex_coord;
	
	
}