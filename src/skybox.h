#ifndef SKYBOX_H
#define SKYBOX_H

/************************************
* Headers
*************************************/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <glad/glad.h> 
#include <glfw3.h>
#include <stdio.h>
#include <stb_image.h>

/************************************
* Defines
*************************************/

/************************************
* Types
*************************************/
struct cubemap_textures
{
	const char *right;
	const char *left;
	const char *top;
	const char *bottom;
	const char *front;
	const char *back;
};

class Skybox
{
	private:

	public:
		Skybox( );
		~Skybox( );
		
		void load( struct cubemap_textures );
		void load( const char *, const char *, const char *, const char *, const char *, const char * );

		GLuint texture_id; 
};

#endif