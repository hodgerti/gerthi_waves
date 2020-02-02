#version 330 core

// inputs
in v_MATERIAL
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} v_material_in;
in vec3 v_model_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_tex_coord;

// uniforms
uniform vec3 u_camera_position;

// textures
uniform sampler2D u_tex_0;  // noise

// view as noise texture
uniform bool u_viewing_mode;

// lights:
// 10 point lights
struct point_light 
{
    vec3 position;
	
	vec3 color;
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
    vec3 position;
	vec3 direction;
	float dim_start_angle;
	float dim_end_angle;
	
	vec3 color;
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
	vec3 direction;
	
	vec3 color;
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
};

#define NUM_POINT_LIGHTS			10
#define NUM_SPOT_LIGHTS				4
#define NUM_DIRECTIONAL_LIGHTS		2
uniform int					u_num_point_lights;
uniform int					u_num_spot_lights;
uniform int					u_num_directional_lights;
uniform point_light 		u_point_lights[ NUM_POINT_LIGHTS ];
uniform spot_light 			u_spot_lights[ NUM_SPOT_LIGHTS ];
uniform directional_light 	u_directional_lights[ NUM_DIRECTIONAL_LIGHTS ];

vec3 calculate_point_light( point_light );
vec3 calculate_spot_light( spot_light );
vec3 calculate_directional_light( directional_light );

void main()
{
	if( u_viewing_mode )
	{
		gl_FragColor = texture2D( u_tex_0, v_tex_coord );
		return;
	}

	vec3 result = vec3( 0.0, 0.0, 0.0 );
	
	for( int pdx = 0; pdx < u_num_point_lights; pdx++ )
	{
		result += calculate_point_light( u_point_lights[ pdx ] );
	}
	for( int sdx = 0; sdx < u_num_spot_lights; sdx++ )
	{
		result += calculate_spot_light( u_spot_lights[ sdx ] );
	}
	for( int ddx = 0; ddx < u_num_directional_lights; ddx++ )
	{
		result += calculate_directional_light( u_directional_lights[ ddx ] );
	}
	
	gl_FragColor = vec4( result, 1.0 );
} 

vec3 calculate_point_light( point_light light )
{
	// globals
	vec3 normal = normalize( v_normal );
	vec3 light_direction = normalize( light.position - v_world_position );
	vec3 view_direction = normalize( u_camera_position - v_world_position );
	vec3 reflect_direction = reflect( -light_direction, normal );
	float light_distance = length( v_world_position - light.position );
		
	// attenuation
	float attenuation = 1.0 / ( light.constant_attenuation + light.linear_attenuation*light_distance + light.quadratic_attenuation*light_distance*light_distance );

	// ambient
	vec3 ambient = v_material_in.ambient * ( light.color * light.ambient_strength ) * attenuation;
	
	// diffuse
	float diffuse_strength = max( dot( normal, light_direction ), 0.0 );
	vec3 diffuse = ( diffuse_strength * v_material_in.diffuse ) * ( light.color * light.diffuse_strength ) * attenuation;
	
	// specular
	vec3 specular = vec3(0.0);
	float phi = dot( reflect_direction, view_direction );
	if( phi > 0.0 )
	{
		float specular_amount = pow( max( phi, 0.0 ), v_material_in.shininess );
		specular = ( specular_amount * v_material_in.specular ) * ( light.color * light.specular_strength ) * attenuation;
	}
	
	return ambient + diffuse + specular;
}

vec3 calculate_spot_light( spot_light light )
{
	// globals
	vec3 normal = normalize( v_normal );
	vec3 light_direction = normalize( light.position - v_world_position );
	vec3 view_direction = normalize( u_camera_position - v_world_position );
	vec3 reflect_direction = reflect( -light_direction, normal );
	float light_distance = length( v_world_position - light.position );
	float fragment_stray_angle = dot( light_direction, normalize( -light.direction ) );
	
	// attenuation
	float attenuation = 1.0 / ( light.constant_attenuation + light.linear_attenuation*light_distance + light.quadratic_attenuation*light_distance*light_distance );

	// ambient
	vec3 ambient = v_material_in.ambient * ( light.color * light.ambient_strength ) * attenuation;
		
	if( fragment_stray_angle > light.dim_end_angle )
	{		
		// intensity
		float intensity = ( fragment_stray_angle - light.dim_end_angle ) / ( light.dim_start_angle - light.dim_end_angle );
		
		// diffuse
		float diffuse_strength = max( dot( normal, light_direction ), 0.0 );
		vec3 diffuse = ( diffuse_strength * v_material_in.diffuse ) * ( light.color * light.diffuse_strength ) * attenuation * intensity;
		
		// specular
		float specular_amount = pow( max( dot( reflect_direction, view_direction ), 0.0 ), v_material_in.shininess );
		vec3 specular = ( specular_amount * v_material_in.specular ) * ( light.color * light.specular_strength ) * attenuation * intensity;
		
		return ambient + diffuse + specular;
	}
	else if( fragment_stray_angle > light.dim_start_angle )
	{		
		// diffuse
		float diffuse_strength = max( dot( normal, light_direction ), 0.0 );
		vec3 diffuse = ( diffuse_strength * v_material_in.diffuse ) * ( light.color * light.diffuse_strength ) * attenuation;
		
		// specular
		vec3 specular = vec3(0.0);
		float phi = dot( reflect_direction, view_direction );
		if( phi > 0.0 )
		{
			float specular_amount = pow( max( phi, 0.0 ), v_material_in.shininess );
			specular = ( specular_amount * v_material_in.specular ) * ( light.color * light.specular_strength ) * attenuation;
		}
		
		return ambient + diffuse + specular;
	}
	else
	{
		return ambient;
	}
}

vec3 calculate_directional_light( directional_light light )
{
	// globals
	vec3 normal = normalize( v_normal );
	vec3 light_direction = normalize( -light.direction );
	vec3 view_direction = normalize( u_camera_position - v_world_position );
	vec3 reflect_direction = reflect( -light_direction, normal );

	// ambient
	vec3 ambient = v_material_in.ambient * ( light.color * light.ambient_strength );
	
	// diffuse
	float diffuse_strength = max( dot( normal, light_direction ), 0.0 );
	vec3 diffuse = ( diffuse_strength * v_material_in.diffuse ) * ( light.color * light.diffuse_strength );
	
	// specular
	vec3 specular = vec3(0.0);
	float phi = dot( reflect_direction, view_direction );
	if( phi > 0.0 )
	{
		float specular_amount = pow( max( phi, 0.0 ), v_material_in.shininess );
		specular = ( specular_amount * v_material_in.specular ) * ( light.color * light.specular_strength );
	}
	
	return ambient + diffuse + specular;
	
}