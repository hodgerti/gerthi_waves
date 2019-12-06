/************************************
* Headers
*************************************/
#include <shaders.h>

/************************************
* Static members
*************************************/
const GLchar * Shader::u_model_mat4						= "u_model";
const GLchar * Shader::u_view_mat4						= "u_view";
const GLchar * Shader::u_projection_mat4				= "u_projection";
const GLchar * Shader::u_orthographic_mat4				= "u_orthographic";
const GLchar * Shader::u_all_color_vec3					= "u_all_color";

const GLuint Shader::a_pos						= 0;
const GLuint Shader::a_normal					= 1;
const GLuint Shader::a_tex_coord				= 2;
const GLuint Shader::a_color					= 3;

/************************************
* Functions
*************************************/
Shader::Shader()
{

}

Shader::Shader(const char* vertex_path, const char* fragment_path)
{
	set_shaders(vertex_path, fragment_path);
}

Shader::~Shader()
{
	delete [] vert_path;
	delete [] frag_path;
}

void Shader::set_shaders(const char* vertex_path, const char* fragment_path)
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

void Shader::read_shader_file(char *path, char **source)
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

void Shader::compile_shaders()
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

void Shader::link_shaders()
{
	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	check_shader_program(program);
}

void Shader::check_shader_compile(unsigned int shader)
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

void Shader::check_shader_program(unsigned int program)
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

void Shader::use()
{
	glUseProgram(program);
}

unsigned int Shader::get_vert()
{
	return vert;
}

unsigned int Shader::get_frag()
{
	return frag;
}

unsigned int Shader::get_program()
{
	return program;
}

void Shader::set_uniform1f(const char* name, float x) const
{
	glUniform1f(glGetUniformLocation(program, name), x);
}
void Shader::set_uniform2f(const char* name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(program, name), x, y);
}
void Shader::set_uniform3f(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(program, name), x, y, z);
}
void Shader::set_uniform4f(const char* name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(program, name), x, y, z, w);
}
void Shader::set_uniform1i(const char* name, int x) const
{
	glUniform1i(glGetUniformLocation(program, name), x);
}
