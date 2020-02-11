/************************************
* Headers
*************************************/
#include <skybox.h>

/************************************
* Functions
*************************************/

Skybox::Skybox( )
{

}

Skybox::~Skybox( )
{

}

void Skybox::load( struct cubemap_textures textures )
{
	load
	( 
		textures.right,
		textures.left,
		textures.top,
		textures.bottom,
		textures.front,
		textures.back
	);
}

void Skybox::load
( 
	const char *right, 
	const char *left, 
	const char *top, 
	const char *bottom, 
	const char *back, 
	const char *front 
)
{
	// assign texture unit
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	// load textures
	int width, height, nr_channels;
	stbi_uc *buff;
	const char *textures[] = { right, left, top, bottom, back, front };
	for( GLuint idx = 0; idx < 6; idx++ )
	{
		buff = stbi_load( textures[idx], &width, &height, &nr_channels, 0 );
		if( buff )
		{
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx,
						  0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buff );
		}
		else
		{
			printf( "WARNING: CUBEMAP TEXTURE(%i) FAILED TO LOAD\n", idx );
		}
		stbi_image_free( buff );
	}

	// assign parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}