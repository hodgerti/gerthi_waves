#ifndef TEXTURES_H
#define TEXTURES_H

/************************************
* Headers
*************************************/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h> 
#include <stb/stb_image.h>

/************************************
* Constants
*************************************/
// Texture params
#define	DFLT_PARAMETERI_S_SZ		4
#define EXTRA_PARAMETERI_S_SZ		1
#define NO_PARAMETER				-1

#define TEX_MIX_UNIFORM		"tex_mix"

// texture uniform naming
#define TEX_UNIFORM_BASE_NAME		"u_tex"
#define TEX_UNIFORM_0				TEX_UNIFORM_BASE_NAME"_0"
#define TEX_UNIFORM_1				TEX_UNIFORM_BASE_NAME"_1"
#define TEX_UNIFORM_2				TEX_UNIFORM_BASE_NAME"_2"
#define TEX_UNIFORM_3				TEX_UNIFORM_BASE_NAME"_3"
#define TEX_UNIFORM_4				TEX_UNIFORM_BASE_NAME"_4"
#define TEX_UNIFORM_5				TEX_UNIFORM_BASE_NAME"_5"
#define TEX_UNIFORM_6				TEX_UNIFORM_BASE_NAME"_6"
#define TEX_UNIFORM_7				TEX_UNIFORM_BASE_NAME"_7"
#define TEX_UNIFORM_8				TEX_UNIFORM_BASE_NAME"_8"
#define TEX_UNIFORM_9				TEX_UNIFORM_BASE_NAME"_9"
#define TEX_UNIFORM_10				TEX_UNIFORM_BASE_NAME"_10"
#define TEX_UNIFORM_11				TEX_UNIFORM_BASE_NAME"_11"
#define TEX_UNIFORM_12				TEX_UNIFORM_BASE_NAME"_12"
#define TEX_UNIFORM_13				TEX_UNIFORM_BASE_NAME"_13"
#define TEX_UNIFORM_14				TEX_UNIFORM_BASE_NAME"_14"
#define TEX_UNIFORM_15				TEX_UNIFORM_BASE_NAME"_15"


/************************************
* Macros
*************************************/
#define get_tex_unit_num(unit) unit - GL_TEXTURE0

//#define get_tex_uniform_name(unit) TEX_UNIFORM_BASE_NAME & (0xffffff00 + (char)get_tex_unit_num(unit))

//#define get_tex_uniform_name(unit) TEX_UNIFORM_ ## get_tex_unit_num(unit)

//#define STRINGIFY(x) #x
//#define LITERAL_STRINGIFY(x) STRINGIFY(x)
//#define get_tex_uniform_name(unit) TEX_UNIFORM_BASE_NAME LITERAL_STRINGIFY(get_tex_unit_num(unit))

#define GET_TEX_FORMAT(n) GL_RGB - (3 - n)

/************************************
* Types
*************************************/
struct tex_parameteri
{
	int	filter;
	int	wrapping;
};

class Texture
{
	private:
		unsigned int	texture;
		int				target;
		int				unit;

		tex_parameteri	**parameteri_s;
		int				  tex_parameteri_sz;

		// check if parameter's are using mipmaps
		bool check_mipmap();
		void init_parameteri(tex_parameteri *parameteri, int filter, int wrapping);

	public:
		Texture();
		Texture(int targ);
		~Texture();

		void set_target(int targ);
		void set_unit(int un);
		int get_target();
		int get_unit();

		void add_parameteri(int filter, int wrapping);

		void load(const char *filename);
		void use();
};

/************************************
* Functions
*************************************/
glm::vec3 hex_to_normalized(unsigned int hex_color);


#endif