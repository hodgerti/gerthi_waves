/************************************
* Include
*************************************/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <math.h>

#include <wave_shader.h>
#include <shaders.h>
#include <glfw_helper.h>
#include <camera.h>
#include <delta_time.h>
#include <geometry.h>

#include <glad/glad.h>
#include <glfw3.h>

/************************************
* Definitions
*************************************/
#define CONTEXT_TITLE		"LearnOpenGL"
#define WINDOW_WIDTH		1024
#define WINDOW_HEIGHT		1024

/************************************
* Defines
*************************************/
#define PI 3.14159265358979323846264338327950288

/************************************
* Macros
*************************************/
#define min(a,b)		(((a)<(b))?(a):(b))
#define max(a,b)		(((a)>(b))?(a):(b))
#define clamp(c,a,b)	(max((a),min((c),(b))))

#define epsilon									0.00001f 
#define approximatelyEqual(a,b)					(fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon))
#define essentiallyEqual(a,b)					(fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon))
#define definitelyGreaterThan(a,b)				((a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon))
#define definitelyLessThan(a,b)					((b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon))
#define definitelyGreaterThanOrEqual(a,b)		(essentiallyEqual(a,b) || definitelyGreaterThan(a,b))
#define definitelyLessThanOrEqual(a,b)			(essentiallyEqual(a,b) || definitelyLessThan(a,b))


/************************************
* Globals
*************************************/
// stuff and things
GLFWInputHandler	input_handler;
Camera				camera;
DeltaTime			delta_time( glfwGetTime );

// WAVES:
glm::vec3 wave_dimensions			= glm::vec3( 40.0f, 3.0f, 40.0f );
int									wave_xoops = 1000, wave_yoops = 0, wave_zoops = 1000;
int num_waves = NUM_WAVES;
int current_wave = 0;
struct wave_info waves_infos[ NUM_WAVES ] = 
{
	// wave 0 (big)
	{
		0.135502f,	// frequency
		1.617319f,	// amplitude
		1.079493f,	// phase constant (speed)
		0.385999f,	// crest constant ( 0.0 -> 1.0/(frequency*amplitude) )
		glm::vec2( 0.8f, 0.2f )	// direction
	},
	// wave 1 (small)
	{
		0.326000f,	// frequency
		0.888002f,	// amplitude
		0.105000f,	// phase constant (speed)
		0.108000f,	// crest constant
		glm::vec2( 0.75f, 0.25f )	// direction
	},
	// wave 2 (very cool)
	{
		1.770008f,	// frequency
		0.125400f,	// amplitude
		0.792002f,	// phase constant (speed)
		1.212008f,	// crest constant
		glm::vec2( -0.161228f, 0.808723f )	// direction
	},
	// wave 3 (crashing)
	{
		0.056203f,	// frequency
		0.078200f,	// amplitude
		5.025150f,	// phase constant (speed)
		7.393781f,	// crest constant ( 0.0 -> 1.0/(frequency*amplitude) )
		glm::vec2( 0.464056f, -0.681703f )	// direction
	},
	// wave 4
	{
		0.000799f,	// frequency
		0.108000f,	// amplitude
		2.397013f,	// phase constant (speed)
		0.052800f,	// crest constant ( 0.0 -> 1.0/(frequency*amplitude) )
		glm::vec2( 0.499604f, 0.656083f )	// direction
	},
	// wave 5
	{
		0.0f,	// frequency
		0.0f,	// amplitude
		0.0f,	// phase constant (speed)
		0.0f,	// crest constant ( 0.0 -> 1.0/(frequency*amplitude) )
		glm::vec2( 0.8f, 0.2f )	// direction
	},
	// wave 6
	{
		0.0f,	// frequency
		0.0f,	// amplitude
		0.0f,	// phase constant (speed)
		0.0f,	// crest constant ( 0.0 -> 1.0/(frequency*amplitude) )
		glm::vec2( 0.8f, 0.2f )	// direction
	}
};

// shaders
WaveShader	wave_shader;
Shader		light_shader;

// buffers
GLuint vbo_box, vao_box, box_vert_count;
GLuint vbo_rectprism, vao_rectprism, ebo_rectprism, rectprism_vert_count, rectprism_tri_count;

// colors
glm::vec4 DAY_BACKCOLOR = glm::vec4( 0.525f, 0.6375f, 0.6125f, 1.0f );  // light blue gray
glm::vec4 NIGHT_BACKCOLOR = DAY_BACKCOLOR / 3.0f;

// LIGHTS:
// point lights:
int num_point_lights = 2;
struct point_light point_lights[ 2 ] = 
{
	// light 0
	{
		glm::vec3( 4.0f, 5.0f, 4.0f ),	// position
		glm::vec3( 1.0f, 1.0f, 1.0f ),  // color
		0.1f, 0.8f, 0.8f,				// ambient_strength, diffuse_strength, specular_strength
		1.0f, 0.09f, 0.032f			// constant_attenuation, linear_attenuation, quadratic_attenuation
	},
	// light 1
	{
		glm::vec3( 8.0f, 7.0f, 10.0f ),	// position
		glm::vec3( 1.0f, 0.2f, 0.2f ),  // color
		0.3f, 0.9f, 0.9f,				// ambient_strength, diffuse_strength, specular_strength
		1.0f, 0.09f, 0.032f				// constant_attenuation, linear_attenuation, quadratic_attenuation
	}
};
// spot lights:
int num_spot_lights = 2;
struct spot_light spot_lights[ 2 ] = 
{
	// light 0
	{
		glm::vec3( 15.0f, 5.5f, 15.0f ),	// position
		glm::vec3( 0.1f, -1.0f, -1.0f ),	// direction
		glm::cos( glm::radians(8.0f) ),		// dim start angle
		glm::cos( glm::radians(30.5f) ),	// dim end angle
		glm::vec3( 1.0f, 1.0f, 1.0f ),		// color
		0.4f, 0.8f, 0.8f,					// ambient_strength, diffuse_strength, specular_strength
		1.0f, 0.014f, 0.0007f				// constant_attenuation, linear_attenuation, quadratic_attenuation
	},
	// light 1
	{
		glm::vec3( 3.0f, 5.5f, 16.0f ),		// position
		glm::vec3( 0.0f, -1.0f, 0.0f ),		// direction
		glm::cos( glm::radians(15.0f) ),	// dim start angle
		glm::cos( glm::radians(18.0f) ),	// dim end angle
		glm::vec3( 0.2f, 0.2f, 1.0f ),		// color
		0.4f, 0.8f, 0.8f,					// ambient_strength, diffuse_strength, specular_strength
		1.0f, 0.014f, 0.0007f				// constant_attenuation, linear_attenuation, quadratic_attenuation
	}
};
// the sun:
const int SUN_IDX = 0;
bool sun_on = true;
int num_directional_lights = 1;
struct directional_light directional_lights[ 1 ] = 
{
	// light 0
	{
		glm::vec3( 0.5f, -1.0f, -0.5f ),		// direction
		glm::vec3( 0.988f, 0.792f, 0.149f ),	// color
		0.1f, 0.1f, 0.2f,						// ambient_strength, diffuse_strength, specular_strength
	}
};
glm::vec3 sun_position = glm::vec3( -100.0f, 100.0f, 100.0f );


/************************************
* Functions
*************************************/
int			init( );
int			loop( );
void		display( );
int			stop( );
void		process_inputs( );
void		generate_rectangluar_prism( GLuint * , GLuint * , GLfloat ** , GLuint ** , float , float , float , int , int , int );
void		generate_rectangle( GLuint * , GLuint * , GLfloat ** , GLuint ** , float , float , int , int );
float		angle_between( glm::vec3, glm::vec3, glm::vec3 = glm::vec3(0.0f, 0.0f, 0.0f)) ;
glm::mat4	rotate_to_direction( glm::mat4, glm::vec3 );
void		mod_wave( struct wave_info **, float , float , float , float , glm::vec2 );
void		print_wave( struct wave_info * );
glm::vec2	rotate( glm::vec2, float );


int main( )
{
	init( ); 
	return loop( );
} 


int init( )
{
	// make the window
	input_handler.start_glfw( WINDOW_WIDTH, WINDOW_HEIGHT, CONTEXT_TITLE, (GLADloadproc)glfwGetProcAddress);
	
	// setup HIDs
	input_handler.add_key(GLFW_KEY_ESCAPE, GLFW_KEY_ESCAPE);
	input_handler.add_key(GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_SHIFT);
	input_handler.add_key(GLFW_KEY_W, GLFW_KEY_W);
	input_handler.add_key(GLFW_KEY_A, GLFW_KEY_A);
	input_handler.add_key(GLFW_KEY_S, GLFW_KEY_S);
	input_handler.add_key(GLFW_KEY_D, GLFW_KEY_D);
	input_handler.add_key(GLFW_KEY_Q, GLFW_KEY_Q);
	input_handler.add_key(GLFW_KEY_T, GLFW_KEY_T);
	input_handler.add_key(GLFW_KEY_G, GLFW_KEY_G);
	input_handler.add_key(GLFW_KEY_Y, GLFW_KEY_Y);
	input_handler.add_key(GLFW_KEY_H, GLFW_KEY_H);
	input_handler.add_key(GLFW_KEY_U, GLFW_KEY_U);
	input_handler.add_key(GLFW_KEY_J, GLFW_KEY_J);
	input_handler.add_key(GLFW_KEY_I, GLFW_KEY_I);
	input_handler.add_key(GLFW_KEY_K, GLFW_KEY_K);
	input_handler.add_key(GLFW_KEY_M, GLFW_KEY_M);
	input_handler.add_key(GLFW_KEY_O, GLFW_KEY_O);
	input_handler.add_key(GLFW_KEY_L, GLFW_KEY_L);

	input_handler.start_mouse();
	
	// set up camera
	camera.set_clip_perspective( 45.0, WINDOW_WIDTH/WINDOW_HEIGHT, 0.01, 1000.0 );
	camera.move_speed	= 25.0 * DFLT_MOVE_SPEED_INCR;
	camera.strafe_speed = 25.0 * DFLT_STRAFE_SPEED_INCR;
	camera.pos = glm::vec3(0.0f, 10.0f, 5.0f);

	// set OGL context
	glEnable( GL_DEPTH_TEST );

	// Wave Shader:
	wave_shader.set_shaders( VERT_SHADERS"wave_light.vert", FRAG_SHADERS"wave_light.frag" );
    wave_shader.use( );
	// setup lights
	for( int pdx = 0; pdx < num_point_lights; pdx++ )
	{
		wave_shader.add_point_light( &point_lights[pdx] );
	}
	for( int sdx = 0; sdx < num_spot_lights; sdx++ )
	{
		wave_shader.add_spot_light( &spot_lights[sdx] );
	}
	for( int ddx = 0; ddx < num_spot_lights; ddx++ )
	{
		wave_shader.add_directional_light( &directional_lights[ddx] );
	}
	wave_shader.use_lights( );
	// base wave color
	wave_shader.set_base_color( glm::vec3( 0.2f, 0.3f, 1.0f ) );
	// set wave values
	for( int wdx = 0; wdx < num_waves; wdx++ )
	{
		wave_shader.add_wave( &waves_infos[wdx] );
	}

	// Light Shader:
	light_shader.set_shaders( VERT_SHADERS"basic.vert", FRAG_SHADERS"basic.frag" );

	// Load Geometry:
	// prism:
	GLfloat * rectprism_data;
	GLuint * rectprism_ind;
	generate_rectangluar_prism( &rectprism_vert_count, &rectprism_tri_count, &rectprism_data, &rectprism_ind, wave_dimensions.x, wave_dimensions.y, wave_dimensions.z, wave_xoops, wave_yoops, wave_zoops );
    glGenVertexArrays(1, &vao_rectprism);
    glGenBuffers(1, &vbo_rectprism);
    glGenBuffers(1, &ebo_rectprism);
    glBindVertexArray(vao_rectprism);
	  // vert
      glBindBuffer(GL_ARRAY_BUFFER, vbo_rectprism);
      glBufferData(GL_ARRAY_BUFFER, rectprism_vert_count * 8 * sizeof(GLfloat), rectprism_data, GL_STATIC_DRAW);
	  // indices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_rectprism);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, rectprism_tri_count * 3 * sizeof(GLuint), rectprism_ind, GL_STATIC_DRAW);
      // position attribute
      glVertexAttribPointer(Shader::a_pos, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(Shader::a_pos);
      // normal attribute
      glVertexAttribPointer(Shader::a_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(Shader::a_normal);
      // texture attribute
      glVertexAttribPointer(Shader::a_tex_coord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
      glEnableVertexAttribArray(Shader::a_tex_coord);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete rectprism_data;
	delete rectprism_ind;

	// box:
	box_vert_count = sizeof(cube_vertices)/(sizeof(float)*6);
    glGenVertexArrays(1, &vao_box);
    glGenBuffers(1, &vbo_box);
    glBindVertexArray(vao_box);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_box);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
      // position attribute
      glVertexAttribPointer(Shader::a_pos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(Shader::a_pos);
      // normal attribute
      glVertexAttribPointer(Shader::a_normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(Shader::a_normal);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	return 0;

}  //  init  /


int loop( )
{
	while(!glfwWindowShouldClose(input_handler.window))
	{
		delta_time.update( );
		process_inputs( );

		display( );

		glfwPollEvents( );
	}

	glDeleteVertexArrays(1, &vao_box);
    glDeleteBuffers(1, &vbo_box);

	glDeleteVertexArrays(1, &vao_rectprism);
    glDeleteBuffers(1, &vbo_rectprism);
    glDeleteBuffers(1, &ebo_rectprism);

	return stop();
};  //  loop  /


void display( )
{
	// clear
	if( sun_on )	glClearColor( DAY_BACKCOLOR.r, DAY_BACKCOLOR.g, DAY_BACKCOLOR.b, DAY_BACKCOLOR.a );
	else			glClearColor( NIGHT_BACKCOLOR.r, NIGHT_BACKCOLOR.g, NIGHT_BACKCOLOR.b, NIGHT_BACKCOLOR.a );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// render lights:
	light_shader.use( );

    glm::mat4 model;
	// point lights
	for( int pdx = 0; pdx < num_point_lights; pdx++ )
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, point_lights[pdx].position);
		model = glm::scale(model, glm::vec3(0.5f) );
		camera.set_world( model );
		camera.shader_id = light_shader.get_program( );
		camera.use( );

		light_shader.set_uniform3f( Shader::u_all_color_vec3, point_lights[pdx].color.x, point_lights[pdx].color.y, point_lights[pdx].color.z ); 
		glBindVertexArray( vao_box );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
	}
	// spot lights
	for( int sdx = 0; sdx < num_spot_lights; sdx++ )
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, spot_lights[sdx].position);
		model = rotate_to_direction( model, spot_lights[sdx].direction );
		model = glm::scale(model, glm::vec3(0.5f) );
		camera.set_world( model );
		camera.shader_id = light_shader.get_program( );
		camera.use( );

		light_shader.set_uniform3f( Shader::u_all_color_vec3, spot_lights[sdx].color.x, spot_lights[sdx].color.y, spot_lights[sdx].color.z ); 
		glBindVertexArray( vao_box );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
	}
	// the sun
	if( sun_on )
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, sun_position);
		model = glm::scale(model, glm::vec3(15.0f) );
		camera.set_world( model );
		camera.shader_id = light_shader.get_program( );
		camera.use( );
		light_shader.set_uniform3f( Shader::u_all_color_vec3, directional_lights[SUN_IDX].color.x, directional_lights[SUN_IDX].color.y, directional_lights[SUN_IDX].color.z ); 
		glBindVertexArray( vao_box );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
	}

	// render waves:
    wave_shader.use( );

	wave_shader.use_waves( );
	wave_shader.use_lights( );
	wave_shader.set_camera_position( camera.pos );
	wave_shader.set_uniform1f( WaveShader::u_wave_time_float, delta_time.current_time );

    model = glm::mat4(1.0f);
	camera.set_world( model );
	camera.shader_id = wave_shader.get_program( );
	camera.use( );
	
    glBindVertexArray( vao_rectprism );
	glDrawElements(GL_TRIANGLES, rectprism_tri_count * 3, GL_UNSIGNED_INT, (void*)0);

	// swap color buffer
	glfwSwapBuffers( input_handler.window );
}  //  display  /


void process_inputs( )
{
	input_handler.update( );
	camera.set_mouse( input_handler.mouse_pos );

	if(input_handler.check_pressed(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(input_handler.window, true);
	}

	if(input_handler.check_pressed(GLFW_KEY_W))
	{
		camera.move_forward( delta_time.delta );
	}
	if(input_handler.check_pressed(GLFW_KEY_A))
	{
		camera.move_left( delta_time.delta );
	}
	if(input_handler.check_pressed(GLFW_KEY_S))
	{
		camera.move_backwards( delta_time.delta );
	}
	if(input_handler.check_pressed(GLFW_KEY_D))
	{
		camera.move_right( delta_time.delta );
	}

	struct wave_info *wave = &waves_infos[current_wave];
	// speed
	float adj_speed = 0.0001f;
	if(input_handler.check_pressed(GLFW_KEY_LEFT_SHIFT))
	{
		adj_speed = 0.003f;
	}
	// freq
	if(input_handler.check_pressed(GLFW_KEY_T))
	{
		mod_wave( &wave, wave->frequency + adj_speed, wave->amplitude, wave->phase_constant, wave->crest_constant, wave->direction );
	}
	if(input_handler.check_pressed(GLFW_KEY_G))
	{
		mod_wave( &wave, wave->frequency - adj_speed, wave->amplitude, wave->phase_constant, wave->crest_constant, wave->direction );
	}
	// amp
	if(input_handler.check_pressed(GLFW_KEY_Y))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude + adj_speed, wave->phase_constant, wave->crest_constant, wave->direction );
	}
	if(input_handler.check_pressed(GLFW_KEY_H))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude - adj_speed, wave->phase_constant, wave->crest_constant, wave->direction );
	}
	// phase
	if(input_handler.check_pressed(GLFW_KEY_U))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude, wave->phase_constant + adj_speed, wave->crest_constant, wave->direction );
	}
	if(input_handler.check_pressed(GLFW_KEY_J))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude, wave->phase_constant - adj_speed, wave->crest_constant, wave->direction );
	}
	// crest
	if(input_handler.check_pressed(GLFW_KEY_I))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude, wave->phase_constant, wave->crest_constant + adj_speed, wave->direction );
	}
	if(input_handler.check_pressed(GLFW_KEY_K))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude, wave->phase_constant, wave->crest_constant - adj_speed, wave->direction );
	}
	// rotate
	if(input_handler.check_pressed(GLFW_KEY_O))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude, wave->phase_constant, wave->crest_constant, rotate( wave->direction, adj_speed ) );
	}
	if(input_handler.check_pressed(GLFW_KEY_L))
	{
		mod_wave( &wave, wave->frequency, wave->amplitude, wave->phase_constant, wave->crest_constant, rotate( wave->direction, -adj_speed ) );
	}

	// change wave
	if(input_handler.pop_click(GLFW_KEY_Q))
	{
		current_wave++;
		current_wave %= NUM_WAVES;
		print_wave( wave );
	}

	if(input_handler.pop_click(GLFW_KEY_M))
	{
		sun_on = !sun_on;
		if( sun_on )
		{
			wave_shader.add_directional_light( &directional_lights[SUN_IDX] );
		}
		else
		{
			wave_shader.remove_directional_light( &directional_lights[SUN_IDX] );
		}
	}

	camera.move_speed	= clamp( camera.move_speed,   0.0, 50.0 * DFLT_MOVE_SPEED_INCR );
	camera.strafe_speed	= clamp( camera.strafe_speed, 0.0, 50.0 * DFLT_STRAFE_SPEED_INCR );
}  //  process_inputs  /


int stop( )
{
	glfwTerminate();
	return 0;
}  //  stop  /

void generate_rectangle
( 
	GLuint *vert_count, 
	GLuint *tri_count, 
	GLfloat **vert_data, 
	GLuint **index_data, 
	float width, 
	float depth, 
	int xoops, 
	int zoops
)
{
	float x_spacing = depth / (1 + xoops);
	float z_spacing = width / (1 + zoops);

	(*vert_count) = (2 + xoops) * (2 + zoops);
	(*vert_data) = new GLfloat[ (*vert_count) * 8 ];

	(*tri_count) = 2 * (1 + xoops) * (1 + zoops);
	(*index_data) = new GLuint[ (*tri_count) * 3 ]; 

	// top
	int idx = 0;
	for( float xdx = 0.0f; xdx <= depth; xdx += x_spacing )
	{
		for( float zdx = 0.0f; zdx <= width; zdx += z_spacing )
		{
			(*vert_data)[idx++]=xdx;	(*vert_data)[idx++]=0.0f;	(*vert_data)[idx++]=zdx; 
			(*vert_data)[idx++]=0.0f; 	(*vert_data)[idx++]=1.0f; 	(*vert_data)[idx++]=0.0f;
			(*vert_data)[idx++]=0.0f; 	(*vert_data)[idx++]=0.0f;

			// printf("<%f, %f, %f>    <%f, %f, %f>     <%f, %f>\n", (*vert_data)[idx-8], (*vert_data)[idx-7], (*vert_data)[idx-6], (*vert_data)[idx-5], (*vert_data)[idx-4], (*vert_data)[idx-3], (*vert_data)[idx-2], (*vert_data)[idx-1] ); 
		}
	}
	
	// elements
	idx = 0;
	int skip_ctr = zoops + 1;
	for( int tdx = 0; tdx < ((*vert_count) - (2+zoops)); tdx++ )
	{
		if( tdx == skip_ctr )
		{
			skip_ctr += 2+zoops;
			continue;
		}

		(*index_data)[idx++]=tdx;		(*index_data)[idx++]=tdx+1;				(*index_data)[idx++]=tdx+2+zoops;
		(*index_data)[idx++]=tdx+1;		(*index_data)[idx++]=tdx+3+zoops;		(*index_data)[idx++]=tdx+2+zoops;

		// printf("<%i, %i, %i>\n<%i, %i, %i>\n\n", tdx, tdx+1, tdx+2+zoops, tdx+1, tdx+3+zoops, tdx+2+zoops);
	}

}

// printf("<%f, %f, %f>    <%f, %f, %f>     <%f, %f>\n", (*vert_data)[idx-8], (*vert_data)[idx-7], (*vert_data)[idx-6], (*vert_data)[idx-5], (*vert_data)[idx-4], (*vert_data)[idx-3], (*vert_data)[idx-2], (*vert_data)[idx-1] ); 

void generate_rectangluar_prism
( 
	GLuint *vert_count,
	GLuint *tri_count, 
	GLfloat **vert_data,
	GLuint **index_data,
	float width,
	float height, 
	float depth,
	int xoops, 
	int yoops, 
	int zoops
)
{
	float x_spacing = depth / (1 + xoops), x_space;
	float y_spacing = height / (1 + yoops), y_space;
	float z_spacing = width / (1 + zoops), z_space;

	int d_idx = 0;

	(*vert_count) = (2+xoops)*(2+zoops) + (1+yoops)*(2+zoops) + (1+xoops)*(2+zoops) + (yoops)*(2+zoops);
	(*vert_data) = new GLfloat[ (*vert_count) * 8 ];

	(*tri_count) = 2*(2*((1+xoops)*(1+zoops))) + 2*(2*((1+yoops)*(1+zoops)));
	(*index_data) = new GLuint[ (*tri_count) * 3 ]; 

	// BOTTOM:
	x_space = 0.0f;
	z_space = 0.0f;
	for( int xdx = 0; xdx <= (1 + xoops); xdx += 1 )
	{
		for( int zdx = 0; zdx <= (1 + zoops); zdx += 1 )
		{
			(*vert_data)[d_idx++]=x_space;			(*vert_data)[d_idx++]=0.0f;				(*vert_data)[d_idx++]=z_space; 
			(*vert_data)[d_idx++]=0.0f; 			(*vert_data)[d_idx++]=-1.0f; 			(*vert_data)[d_idx++]=0.0f;
			(*vert_data)[d_idx++]=x_space/depth; 	(*vert_data)[d_idx++]=z_space/width;
			z_space += z_spacing;
		}
		z_space = 0.0f;
		x_space += x_spacing;
	}
	// RIGHT:
	y_space = y_spacing;
	z_space = 0.0f;
	for( int ydx = 1; ydx <= (1 + yoops); ydx += 1 )
	{
		for( int zdx = 0; zdx <= (1 + zoops); zdx += 1 )
		{
		   (*vert_data)[d_idx++]=depth;				(*vert_data)[d_idx++]=y_space;			(*vert_data)[d_idx++]=z_space; 
		   (*vert_data)[d_idx++]=1.0f; 				(*vert_data)[d_idx++]=0.0f; 			(*vert_data)[d_idx++]=0.0f;
		   (*vert_data)[d_idx++]=y_space/height; 	(*vert_data)[d_idx++]=z_space/width;
			z_space += z_spacing;
		}
		z_space = 0.0f;
		y_space += y_spacing;
	}
	// TOP:
	x_space = depth - x_spacing;
	z_space = 0.0f;
	for( int xdx = xoops; xdx >= 0; xdx -= 1 )
	{
		for( int zdx = 0; zdx <= (1 + zoops); zdx += 1 )
		{
			(*vert_data)[d_idx++]=x_space;			(*vert_data)[d_idx++]=height;			(*vert_data)[d_idx++]=z_space; 
			(*vert_data)[d_idx++]=0.0f; 			(*vert_data)[d_idx++]=1.0f; 			(*vert_data)[d_idx++]=0.0f;
			(*vert_data)[d_idx++]=x_space/depth; 	(*vert_data)[d_idx++]=z_space/width;
			z_space += z_spacing;
		}
		z_space = 0.0f;
		x_space -= x_spacing;
	}
	// LEFT:
	y_space = height - y_spacing;
	z_space = 0.0f;
	for( int ydx = yoops; ydx > 1; ydx -= 1 )
	{
		for( int zdx = 0; zdx <= (1 + zoops); zdx += 1 )
		{
			(*vert_data)[d_idx++]=0.0f;				(*vert_data)[d_idx++]=y_space;			(*vert_data)[d_idx++]=z_space; 
			(*vert_data)[d_idx++]=-1.0f; 			(*vert_data)[d_idx++]=0.0f; 			(*vert_data)[d_idx++]=0.0f;
			(*vert_data)[d_idx++]=y_space/height; 	(*vert_data)[d_idx++]=z_space/width;
			z_space += z_spacing;
		}
		z_space = 0.0f;
		y_space -= y_spacing;
	}

	// elements
	int e_idx = 0;
	int skip_ctr = zoops + 1;
	for( int tdx = 0; tdx < (*vert_count); tdx++ )
	{
		if( tdx == skip_ctr )
		{
			skip_ctr += 2+zoops;
			continue;
		}

		(*index_data)[e_idx++]=(tdx+2+zoops)%(*vert_count);		(*index_data)[e_idx++]=(tdx+1)%(*vert_count);			(*index_data)[e_idx++]=tdx%(*vert_count);
		(*index_data)[e_idx++]=(tdx+2+zoops)%(*vert_count); 	(*index_data)[e_idx++]=(tdx+3+zoops)%(*vert_count);		(*index_data)[e_idx++]=(tdx+1)%(*vert_count);

		// printf("<%i, %i, %i>\n<%i, %i, %i>\n\n", (tdx+2+zoops)%(*vert_count), (tdx+1)%(*vert_count), tdx%(*vert_count), (tdx+2+zoops)%(*vert_count), (tdx+3+zoops)%(*vert_count), (tdx+1)%(*vert_count) );
	}
}

// ripped without shame from https://stackoverflow.com/questions/31064234/find-the-angle-between-two-vectors-from-an-arbitrary-origin
float angle_between
(
	glm::vec3 a,
	glm::vec3 b,
	glm::vec3 origin
)
{
	glm::vec3 da=glm::normalize(a-origin);
	glm::vec3 db=glm::normalize(b-origin);
	return glm::acos(glm::dot(da, db));
}

// transcribed with impunity from https://gamedev.stackexchange.com/questions/118960/convert-a-direction-vector-normalized-to-rotation
glm::mat4 rotate_to_direction( glm::mat4 matrix, glm::vec3 direction )
{
	float angle = std::atan2( direction.y, direction.x );
	glm::mat4 rotate_xy = glm::rotate( matrix, angle, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	float z_angle = -std::asin( direction.z );
	return glm::rotate( rotate_xy, z_angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
}

void mod_wave( struct wave_info **wave, float freq, float amp, float phi, float qi, glm::vec2 direction )
{
	(*wave)->frequency = max( freq, 0.0f );
	(*wave)->amplitude = max( amp, 0.0f );
	(*wave)->phase_constant = max( phi, 0.0f );
	(*wave)->crest_constant = clamp( qi, 0.0f, 1.0f/((*wave)->frequency*(*wave)->amplitude));
	(*wave)->direction = direction;
	print_wave( (*wave) );
}

void print_wave( struct wave_info *wave )
{
	printf( "CURRENT WAVE: %i\n", current_wave );
	printf( "FREQUENCY: %f\n", wave->frequency );
	printf( "AMPLITUDE: %f\n", wave->amplitude );
	printf( "PHASE CONSTANT: %f\n", wave->phase_constant );
	printf( "CREST CONSTANT: %f\n", wave->crest_constant );
	printf( "DIRECTION: < %f, %f >\n\n", wave->direction.x, wave->direction.y );
}

glm::vec2 rotate( glm::vec2 v_in, float rads )
{
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate( mat, rads, glm::vec3(0.0f, 0.0f, 1.0f) );
	glm::vec4 result = mat * glm::vec4( v_in.x, v_in.y, 1.0f, 1.0f );
	return glm::vec2( result.x, result.y );
}