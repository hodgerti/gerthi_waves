/************************************
* Headers
*************************************/
#include <textures.h>

/************************************
* Functions
*************************************/

Texture::Texture()
{
	/* parameteri_s has a default size that will increase by DFLT_PARAMETERI_S_SZ
	   each time it is surpassed	*/
	parameteri_s = new tex_parameteri*[DFLT_PARAMETERI_S_SZ];
	tex_parameteri_sz = DFLT_PARAMETERI_S_SZ;
	for( int i = 0; i < tex_parameteri_sz; i++)
	{
		parameteri_s[i] = new tex_parameteri;
		init_parameteri(parameteri_s[i], NO_PARAMETER, NO_PARAMETER);
	}

	// default target is 2D
	target = GL_TEXTURE_2D;
}

Texture::Texture(int targ)
{
	Texture();
	target = targ;
}

Texture::~Texture()
{
	for( int i = 0; i < tex_parameteri_sz; i++)
	{
		delete [] parameteri_s[i];
	}
	delete [] parameteri_s;
}

void Texture::set_target(int targ)
{
	target = targ;
}

void Texture::set_unit(int un)
{
	unit = un;
}

void Texture::init_parameteri
(
	tex_parameteri *parameteri,
	int filter,
	int wrapping
)
{
	parameteri->filter = filter;
	parameteri->wrapping = wrapping;
}

void Texture::add_parameteri
(
	int filter, 
	int wrapping
)
{
	for( int i = 0; i < tex_parameteri_sz; i++ )
	{
		// insert parameteri into first open spot on parameteri_s
		if (parameteri_s[i]->filter == NO_PARAMETER
			|| parameteri_s[i]->wrapping == NO_PARAMETER)
		{
			init_parameteri(parameteri_s[i], filter, wrapping);
			return;
		}
	}
	// if an open spot wasn't found then increase the size of parameteri_s 
	tex_parameteri **new_handle = new tex_parameteri*[tex_parameteri_sz + EXTRA_PARAMETERI_S_SZ];
	for( int i = 0; i < tex_parameteri_sz; i++ )
	{
		new_handle[i] = parameteri_s[i];
	}
	for( int i = 0; i < EXTRA_PARAMETERI_S_SZ; i++ )
	{
		new_handle[tex_parameteri_sz + i] = new tex_parameteri;
		init_parameteri(new_handle[EXTRA_PARAMETERI_S_SZ + i], filter, wrapping);
	}
	delete [] parameteri_s;
	tex_parameteri_sz += EXTRA_PARAMETERI_S_SZ;
	parameteri_s = new_handle;
}

bool Texture::check_mipmap()
{
	for( int i = 0; i < tex_parameteri_sz; i++ )
	{
		if( parameteri_s[i]->wrapping >= GL_NEAREST_MIPMAP_NEAREST
			&& parameteri_s[i]->wrapping <= GL_LINEAR_MIPMAP_LINEAR )
		{
			return true;
		}
	}
	return false;
}
void Texture::load
(
	const char *filename
)
{
	int tex_width, tex_height, nr_channels, format;
	unsigned char *data = stbi_load(filename, &tex_width, &tex_height, &nr_channels, 0);

	format = GET_TEX_FORMAT(nr_channels);

	// make and bind texture
	glGenTextures(1, &texture);
	glBindTexture(target, texture);

	// assign parameteri's
	for( int i = 0; i < tex_parameteri_sz; i++ )
	{
		if (parameteri_s[i]->filter != NO_PARAMETER
			|| parameteri_s[i]->wrapping != NO_PARAMETER)
		{
			glTexParameteri(target, parameteri_s[i]->filter, parameteri_s[i]->wrapping);
		}
	}

	// load texture to GPU
	if (data)
	{
		glTexImage2D(target, 0, format, tex_width, tex_height, 0, format, GL_UNSIGNED_BYTE, data);
		if(check_mipmap()) 
		{
			glGenerateMipmap(target);
		}
	}
	else
	{

	}

	// free texture data
	stbi_image_free(data);
}

void Texture::use()
{
	glActiveTexture(unit);
    glBindTexture(target, texture);
}

int Texture::get_target()
{
	return target;
}

int Texture::get_unit()
{
	int askdhjaslkdj = get_tex_unit_num(unit);
	return unit;
}


glm::vec3 hex_to_normalized(unsigned int hex_color)
{
	unsigned int red = (hex_color & 0x00FF0000) >> 16;
	unsigned int green = (hex_color & 0x0000FF00) >> 8;
	unsigned int blue = (hex_color & 0x000000FF) >> 0;

	return glm::vec3((float)red/255.0f, (float)green/255.0f, (float)blue/255.0f);
}