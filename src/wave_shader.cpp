/************************************
* Headers
*************************************/
#include <wave_shader.h>

/************************************
* Static members
*************************************/
const GLchar * WaveShader::u_model_mat4						= "u_model";
const GLchar * WaveShader::u_view_mat4						= "u_view";
const GLchar * WaveShader::u_projection_mat4				= "u_projection";
const GLchar * WaveShader::u_orthographic_mat4				= "u_orthographic";
const GLchar * WaveShader::u_tex_mix_float					= "u_tex_mix";
const GLchar * WaveShader::u_all_color_vec3					= "u_all_color";
const GLchar * WaveShader::u_num_waves						= "u_num_waves";
const GLchar * WaveShader::u_wave_time_float				= "u_wave_time";
const GLchar * WaveShader::u_camera_position_vec3			= "u_camera_position";
const GLchar * WaveShader::u_wave_start_float				= "u_wave_start";
const GLchar * WaveShader::u_viewing_mode_bool				= "u_viewing_mode";
const GLchar * WaveShader::u_noise_freq_float				= "u_noise_freq";
const GLchar * WaveShader::u_noise_amp_float				= "u_noise_amp";
const GLchar * WaveShader::u_noise_normal_amp_float			= "u_noise_normal_amp";


const GLchar * WaveShader::u_texture0_int			= "u_tex_0";

const GLuint WaveShader::a_pos						= 0;
const GLuint WaveShader::a_normal					= 1;
const GLuint WaveShader::a_tex_coord				= 2;
const GLuint WaveShader::a_color					= 3;

/************************************
* Functions
*************************************/
WaveShader::WaveShader()
{
	for( int pdx = 0; pdx < NUM_POINT_LIGHTS; pdx++ )
	{
		point_lights[ pdx ] = 0;
	}
	for( int sdx = 0; sdx < NUM_SPOT_LIGHTS; sdx++ )
	{
		spot_lights[ sdx ] = 0;
	}
	for( int ddx = 0; ddx < NUM_DIRECTIONAL_LIGHTS; ddx++ )
	{
		directional_lights[ ddx ] = 0;
	}
	for( int wdx = 0; wdx < NUM_WAVES; wdx++ )
	{
		waves_infos[ wdx ] = 0;
	}

	viewing_mode = false;
	wave_start = 0.0f;
}

WaveShader::WaveShader(const char* vertex_path, const char* fragment_path)
{
	WaveShader( );
	set_shaders(vertex_path, fragment_path);
}

WaveShader::~WaveShader()
{
	delete [] vert_path;
	delete [] frag_path;
}

void WaveShader::set_shaders(const char* vertex_path, const char* fragment_path)
{
	int vert_len = (int)strlen(vertex_path) + 1;
	int frag_len = (int)strlen(fragment_path) + 1;
	vert_path = new char[vert_len];
	frag_path = new char[frag_len];
	strcpy_s(vert_path, vert_len, vertex_path);
	strcpy_s(frag_path, frag_len, fragment_path);

	compile_shaders();
	link_shaders();

	glDeleteShader(vert);
	glDeleteShader(frag); 
}

void WaveShader::read_shader_file(char *path, char **source)
{
	FILE *fp;

	fopen_s(&fp, path, "r");
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	int offset = 0;
	int num_lines = 0;
	fseek(fp, 0L, SEEK_SET);
	*source = new char[sz];
	while( offset < sz - num_lines)
	{
		fgets(*source + offset, sz, (FILE*)fp);
		num_lines++;
		offset += (int)(strlen(*source) - offset);
	}
	fclose(fp);
}

void WaveShader::compile_shaders()
{
	char *vert_src = NULL;
	char *frag_src = NULL;

	read_shader_file(vert_path, &vert_src);
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vert_src, NULL);
	glCompileShader(vert);
	check_shader_compile(vert);
	delete [] vert_src;

	read_shader_file(frag_path, &frag_src);
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &frag_src, NULL);
	glCompileShader(frag);
	check_shader_compile(frag);	
	delete [] frag_src;
}

void WaveShader::link_shaders()
{
	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	check_shader_program(program);
}

void WaveShader::check_shader_compile(unsigned int shader)
{
	int  success;
	char info_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		printf( info_log );
		#ifdef GEN_ERROR_FILE
		  FILE *fp;
		  fopen_s(&fp, SHADER_DEBUG_FILE_LOCATION, "a");
		  fputs("\nERROR::SHADER::COMPILATION_FAILED\n", fp);
		  fputs(info_log, fp);
		  fclose(fp);
		#endif
	}
}

void WaveShader::check_shader_program(unsigned int program)
{
	int  success;
	char info_log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(program, 512, NULL, info_log);
		printf( info_log );
		#ifdef GEN_ERROR_FILE
		  FILE *fp;
		  fopen_s(&fp, SHADER_DEBUG_FILE_LOCATION, "a");
		  fputs("\nERROR::SHADER_PROGRAM::LINK_FAILED\n", fp);
		  fputs(info_log, fp);
		  fclose(fp);
		#endif
	}
}

void WaveShader::add_point_light( struct point_light * light )
{
	if( num_point_lights < NUM_POINT_LIGHTS )
	{
		for( int idx = 0; idx < NUM_POINT_LIGHTS; idx++ )
		{
			if( point_lights[ idx ] == 0 )
			{
				point_lights[ idx ] = light;
				num_point_lights++;
				break;
			}
		}
	}
	else
	{
		printf( "WARNING: POINT LIGHT SLOTS FULL\n" );
	}
}

void WaveShader::add_spot_light( struct spot_light * light )
{
	if( num_spot_lights < NUM_SPOT_LIGHTS )
	{
		for( int idx = 0; idx < NUM_SPOT_LIGHTS; idx++ )
		{
			if( spot_lights[ idx ] == 0 )
			{
				spot_lights[ idx ] = light;
				num_spot_lights++;
				break;
			}
		}
	}
	else
	{
		printf( "WARNING: SPOT LIGHT SLOTS FULL\n" );
	}
}

void WaveShader::add_directional_light( struct directional_light * light )
{
	if( num_directional_lights < NUM_DIRECTIONAL_LIGHTS )
	{
		for( int idx = 0; idx < NUM_DIRECTIONAL_LIGHTS; idx++ )
		{
			if( directional_lights[ idx ] == 0 )
			{
				directional_lights[ idx ] = light;
				num_directional_lights++;
				break;
			}
		}
	}
	else
	{
		printf( "WARNING: DIRECTIONAL LIGHT SLOTS FULL\n" );
	}
}


void WaveShader::add_wave( struct wave_info *wave )
{
	if( num_waves < NUM_WAVES )
	{
		for( int idx = 0; idx < NUM_WAVES; idx++ )
		{
			if( waves_infos[ idx ] == 0 )
			{
				waves_infos[ idx ] = wave;
				num_waves++;
				break;
			}
		}
	}
	else
	{
		num_waves = NUM_WAVES;
		printf( "WARNING: WAVE SLOTS FULL\n" );
	}	
}

void WaveShader::remove_point_light( struct point_light * light )
{
	bool cleared = false;

	for( int idx = 0; idx < num_point_lights; idx++ )
	{
		if( point_lights[ idx ] == light )
		{
			point_lights[ idx ] = 0;
			num_point_lights--;
			cleared = true;
			break;
		}
	}

	if( !cleared )
	{
		printf( "WARNING: POINT LIGHT NOT FOUND\n" );
	}
}

void WaveShader::remove_spot_light( struct spot_light * light )
{
	bool cleared = false;

	for( int idx = 0; idx < NUM_SPOT_LIGHTS; idx++ )
	{
		if( spot_lights[ idx ] == light )
		{
			spot_lights[ idx ] = 0;
			num_spot_lights--;
			cleared = true;
			break;
		}
	}

	if( !cleared )
	{
		printf( "WARNING: SPOT LIGHT NOT FOUND\n" );
	}
}

void WaveShader::remove_directional_light( struct directional_light * light )
{
	bool cleared = false;

	for( int idx = 0; idx < NUM_DIRECTIONAL_LIGHTS; idx++ )
	{
		if( directional_lights[ idx ] == light )
		{
			directional_lights[ idx ] = 0;
			num_directional_lights--;
			cleared = true;
			break;
		}
	}

	if( !cleared )
	{
		printf( "WARNING: DIRECTIONAL LIGHT NOT FOUND\n" );
	}
}

void WaveShader::remove_wave( struct wave_info * wave )
{
	bool cleared = false;

	for( int idx = 0; idx < NUM_DIRECTIONAL_LIGHTS; idx++ )
	{
		if( waves_infos[ idx ] == wave )
		{
			waves_infos[ idx ] = 0;
			num_waves--;
			cleared = true;
			break;
		}
	}

	if( !cleared )
	{
		printf( "WARNING: WAVES NOT FOUND\n" );
	}
}

void WaveShader::set_camera_position( glm::vec3 position )
{
	camera_position = position;
	set_uniform3f( WaveShader::u_camera_position_vec3, camera_position.x, camera_position.y, camera_position.z );
}

void WaveShader::set_base_color( glm::vec3 color )
{	
	set_uniform3f( WaveShader::u_all_color_vec3, 0.2f, 0.3f, 1.0f );
}

glm::vec3 WaveShader::get_point_light_position( struct point_light * light )
{
	for( int idx = 0; idx < num_point_lights; idx++ )
	{
		if( point_lights[idx] == light )
		{
			return point_lights[idx]->position;
		}
	}

	printf( "WARNING: POINT LIGHT NOT FOUND\n" );
	return glm::vec3( 0.0f, 0.0f, 0.0f );
}

glm::vec3 WaveShader::get_point_light_color( struct point_light * light )
{
	for( int idx = 0; idx < num_spot_lights; idx++ )
	{
		if( point_lights[idx] == light )
		{
			return point_lights[idx]->color;
		}
	}

	printf( "WARNING: POINT LIGHT NOT FOUND\n" );
	return glm::vec3( 0.0f, 0.0f, 0.0f );
}

glm::vec3 WaveShader::get_spot_light_position( struct spot_light * light )
{
	for( int idx = 0; idx < num_spot_lights; idx++ )
	{
		if( spot_lights[idx] == light )
		{
			return spot_lights[idx]->position;
		}
	}

	printf( "WARNING: SPOT LIGHT NOT FOUND\n" );
	return glm::vec3( 0.0f, 0.0f, 0.0f );
}

glm::vec3 WaveShader::get_spot_light_direction( struct spot_light * light )
{
	for( int idx = 0; idx < num_spot_lights; idx++ )
	{
		if( spot_lights[idx] == light )
		{
			return spot_lights[idx]->direction;
		}
	}

	printf( "WARNING: SPOT LIGHT NOT FOUND\n" );
	return glm::vec3( 0.0f, 0.0f, 0.0f );
}

glm::vec3 WaveShader::get_spot_light_color( struct spot_light * light )
{
	for( int idx = 0; idx < num_spot_lights; idx++ )
	{
		if( spot_lights[idx] == light )
		{
			return spot_lights[idx]->color;
		}
	}

	printf( "WARNING: SPOT LIGHT NOT FOUND\n" );
	return glm::vec3( 0.0f, 0.0f, 0.0f );
}

// This is a bad function that uses way to much memory DO NOT compile it! :)
void WaveShader::use_lights( )
{
	char buffer[255];

	// point lights:
	int use_idx = 0;
	for( int adx = 0; adx < num_point_lights; adx++ )
	{
		if( point_lights[adx] != 0 )
		{
			sprintf( buffer, "u_point_lights[%i].position\0", use_idx );
			set_uniform3f( buffer, point_lights[adx]->position.x, point_lights[adx]->position.y, point_lights[adx]->position.z );
			sprintf( buffer, "u_point_lights[%i].color\0", use_idx );
			set_uniform3f( buffer, point_lights[adx]->color.x, point_lights[adx]->color.y, point_lights[adx]->color.z );
			sprintf( buffer, "u_point_lights[%i].ambient_strength\0", use_idx );
			set_uniform1f( buffer, point_lights[adx]->ambient_strength );
			sprintf( buffer, "u_point_lights[%i].diffuse_strength\0", use_idx );
			set_uniform1f( buffer, point_lights[adx]->diffuse_strength );
			sprintf( buffer, "u_point_lights[%i].specular_strength\0", use_idx );
			set_uniform1f( buffer, point_lights[adx]->specular_strength );
			sprintf( buffer, "u_point_lights[%i].constant_attenuation\0", use_idx );
			set_uniform1f( buffer, point_lights[adx]->constant_attenuation );
			sprintf( buffer, "u_point_lights[%i].linear_attenuation\0", use_idx );
			set_uniform1f( buffer, point_lights[adx]->linear_attenuation );
			sprintf( buffer, "u_point_lights[%i].quadratic_attenuation\0", use_idx );
			set_uniform1f( buffer, point_lights[adx]->quadratic_attenuation );
			use_idx++;
		}
	}
	set_uniform1i( "u_num_point_lights\0", use_idx );

	// spot lights:
	use_idx = 0;
	for( int adx = 0; adx < num_point_lights; adx++ )
	{
		if( spot_lights[adx] != 0 )
		{
			sprintf( buffer, "u_spot_lights[%i].position\0", use_idx );
			set_uniform3f( buffer, spot_lights[adx]->position.x, spot_lights[adx]->position.y, spot_lights[adx]->position.z );
			sprintf( buffer, "u_spot_lights[%i].direction\0", use_idx );
			set_uniform3f( buffer, spot_lights[adx]->direction.x, spot_lights[adx]->direction.y, spot_lights[adx]->direction.z );
			sprintf( buffer, "u_spot_lights[%i].dim_start_angle\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->dim_start_angle );
			sprintf( buffer, "u_spot_lights[%i].dim_end_angle\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->dim_end_angle );
			sprintf( buffer, "u_spot_lights[%i].color\0", use_idx );
			set_uniform3f( buffer, spot_lights[adx]->color.x, spot_lights[adx]->color.y, spot_lights[adx]->color.z );
			sprintf( buffer, "u_spot_lights[%i].ambient_strength\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->ambient_strength );
			sprintf( buffer, "u_spot_lights[%i].diffuse_strength\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->diffuse_strength );
			sprintf( buffer, "u_spot_lights[%i].specular_strength\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->specular_strength );
			sprintf( buffer, "u_spot_lights[%i].constant_attenuation\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->constant_attenuation );
			sprintf( buffer, "u_spot_lights[%i].linear_attenuation\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->linear_attenuation );
			sprintf( buffer, "u_spot_lights[%i].quadratic_attenuation\0", use_idx );
			set_uniform1f( buffer, spot_lights[adx]->quadratic_attenuation );
			use_idx++;
		}
	}
	set_uniform1i( "u_num_spot_lights\0", use_idx );

	// directional lights:
	use_idx = 0;
	for( int adx = 0; adx < num_point_lights; adx++ )
	{
		if( directional_lights[adx] != 0 )
		{
			sprintf( buffer, "u_directional_lights[%i].direction\0", use_idx );
			set_uniform3f( buffer, directional_lights[adx]->direction.x, directional_lights[adx]->direction.y, directional_lights[adx]->direction.z );
			sprintf( buffer, "u_directional_lights[%i].color\0", use_idx );
			set_uniform3f( buffer, directional_lights[adx]->color.x, directional_lights[adx]->color.y, directional_lights[adx]->color.z );
			sprintf( buffer, "u_directional_lights[%i].ambient_strength\0", use_idx );
			set_uniform1f( buffer, directional_lights[adx]->ambient_strength );
			sprintf( buffer, "u_directional_lights[%i].diffuse_strength\0", use_idx );
			set_uniform1f( buffer, directional_lights[adx]->diffuse_strength );
			sprintf( buffer, "u_directional_lights[%i].specular_strength\0", use_idx );
			set_uniform1f( buffer, directional_lights[adx]->specular_strength );
			use_idx++;
		}
	}
	set_uniform1i( "u_num_directional_lights\0", use_idx );
}

void WaveShader::use_waves( )
{
	char buffer[255];

	// point lights:
	int use_idx = 0;
	for( int adx = 0; adx < num_waves; adx++ )
	{
		if( waves_infos[adx] != 0 )
		{
			sprintf( buffer, "u_waves_infos[%i].frequency\0", use_idx );
			set_uniform1f( buffer, waves_infos[adx]->frequency );
			sprintf( buffer, "u_waves_infos[%i].amplitude\0", use_idx );
			set_uniform1f( buffer, waves_infos[adx]->amplitude );
			sprintf( buffer, "u_waves_infos[%i].phase_constant\0", use_idx );
			set_uniform1f( buffer, waves_infos[adx]->phase_constant );
			sprintf( buffer, "u_waves_infos[%i].crest_constant\0", use_idx );
			set_uniform1f( buffer, waves_infos[adx]->crest_constant );
			sprintf( buffer, "u_waves_infos[%i].direction\0", use_idx );
			set_uniform2f( buffer, waves_infos[adx]->direction.x, waves_infos[adx]->direction.y );
			use_idx++;
		}
	}
	set_uniform1i( WaveShader::u_num_waves, use_idx );
	set_uniform1f( WaveShader::u_wave_start_float, wave_start );
	set_uniform1b( WaveShader::u_viewing_mode_bool, viewing_mode );
	set_uniform1f( WaveShader::u_noise_freq_float, noise_freq );
	set_uniform1f( WaveShader::u_noise_amp_float, noise_amp );
	set_uniform1f( WaveShader::u_noise_normal_amp_float, noise_normal_amp );
}

void WaveShader::toggle_viewing_mode( )
{
	viewing_mode = !viewing_mode;
}

bool WaveShader::get_viewing_mode( )
{
	return viewing_mode;
}

void WaveShader::set_wave_start( float new_start )
{
	wave_start = new_start;
}

float WaveShader::get_wave_start( )
{
	return wave_start;
}

void WaveShader::use()
{
	glUseProgram(program);
}

void WaveShader::stop()
{
	glUseProgram(0);
}

unsigned int WaveShader::get_vert()
{
	return vert;
}

unsigned int WaveShader::get_frag()
{
	return frag;
}

unsigned int WaveShader::get_program()
{
	return program;
}

void WaveShader::set_uniform1f(const char* name, float x) const
{
	glUniform1f(glGetUniformLocation(program, name), x);
}
void WaveShader::set_uniform2f(const char* name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(program, name), x, y);
}
void WaveShader::set_uniform3f(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(program, name), x, y, z);
}
void WaveShader::set_uniform4f(const char* name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(program, name), x, y, z, w);
}
void WaveShader::set_uniform1i(const char* name, int x) const
{
	glUniform1i(glGetUniformLocation(program, name), x);
}
void WaveShader::set_uniform1b(const char* name, bool x) const
{
	glUniform1ui(glGetUniformLocation(program, name), x);
}
