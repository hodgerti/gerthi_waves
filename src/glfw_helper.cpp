/*==================================================
= Description: This provides helper functions to
= facilitate GLFW
====================================================*/

/************************************
* Headers
*************************************/
#include <glfw_helper.h>

/************************************
* Global variables
*************************************/

/************************************
* Functions
*************************************/

int GLFWInputHandler::start_glfw(int w_width, int w_height, const char *monitor, GLADloadproc load_proc)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(w_width, w_height, monitor, NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	if( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) )
	{
		return -1;
	}

	// set window resizing callback
	glViewport(0, 0, w_width, w_height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

	return 1;
}


GLFWInputHandler::GLFWInputHandler()
{
	/* keys has a default size that will increase by DFLT_KEYS_SIZE
	   each time it is surpassed	*/
	keys = new key_handle*[DFLT_KEYS_SIZE];
	keys_sz = DFLT_KEYS_SIZE;
	for( int i = 0; i < keys_sz; i++)
	{
		keys[i] = new key_handle;
		init_key(keys[i], NO_KEY_ID, NO_KEY_ID);
	}
}

GLFWInputHandler::GLFWInputHandler(int w_width, int w_height, const char *monitor, GLADloadproc load_proc)
{
	GLFWInputHandler();
	start_glfw(w_width, w_height, monitor, load_proc);
}

GLFWInputHandler::~GLFWInputHandler()
{
	for( int i = 0; i < keys_sz; i++)
	{
		delete [] keys[i];
	}
	delete [] keys;
}

int GLFWInputHandler::clear_clicks(int key_id)
{
	for( int i = 0; i < keys_sz; i++ )
	{
		if (keys[i]->key_id == key_id)
		{
			int num_clicks = keys[i]->clicks;
			keys[i]->clicks = 0;
			return num_clicks;
		}
	}
	return NO_KEY_ID;
}

int GLFWInputHandler::get_clicks(int key_id)
{
	for( int i = 0; i < keys_sz; i++ )
	{
		if (keys[i]->key_id == key_id)
		{
			return keys[i]->clicks;
		}
	}
	return NO_KEY_ID;
}

bool GLFWInputHandler::pop_click(int key_id)
{
	for( int i = 0; i < keys_sz; i++ )
	{
		if (keys[i]->key_id == key_id)
		{
			if(keys[i]->clicks > 0)
			{
				keys[i]->clicks--;
				return true;
			}
			return false;
		}
	}
	return false;
}

bool GLFWInputHandler::check_pressed(int key_id)
{
	for( int i = 0; i < keys_sz; i++ )
	{
		if (keys[i]->key_id == key_id)
		{
			return keys[i]->pressed;
		}
	}
	return false;
}

bool GLFWInputHandler::check_unpressed(int key_id)
{
	for( int i = 0; i < keys_sz; i++ )
	{
		if (keys[i]->key_id == key_id)
		{
			return !keys[i]->pressed;
		}
	}
	return false;
}

void GLFWInputHandler::update()
{
	for(int i = 0; i < keys_sz; i++)
	{
		char press_result = glfwGetKey(window, keys[i]->key_name);
		// check for key click
		if( (press_result == GLFW_PRESS)
		    && !(keys[i]->pressed) )
		{
			keys[i]->clicks++;
		}
		keys[i]->pressed = press_result == GLFW_PRESS;
	}
	double x_pos, y_pos;
	glfwGetCursorPos(window, &x_pos, &y_pos);
	mouse_pos.x = (float)x_pos;
	mouse_pos.y = (float)y_pos;
}

void GLFWInputHandler::init_key(key_handle *key, int key_id, int key_name)
{
	key->key_id = key_id;
	key->key_name = key_name;
	key->clicks = 0;
	key->pressed = false;
}

void GLFWInputHandler::add_key(int key_id, int key_name)
{
	for( int i = 0; i < keys_sz; i++ )
	{
		// insert key into first open spot on keys
		if (keys[i]->key_id == NO_KEY_ID)
		{
			init_key(keys[i], key_id, key_name);
			return;
		}
	}
	// if an open spot wasn't found then increase the size of keys 
	key_handle **new_handle = new key_handle*[keys_sz + DFLT_KEYS_SIZE];
	for( int i = 0; i < keys_sz; i++ )
	{
		new_handle[i] = keys[i];
	}
	for( int i = 0; i < DFLT_KEYS_SIZE; i++ )
	{
		new_handle[keys_sz + i] = new key_handle;
		init_key(new_handle[keys_sz + i], NO_KEY_ID, NO_KEY_ID);
	}
	delete [] keys;
	keys_sz += DFLT_KEYS_SIZE;
	keys = new_handle;
}

void GLFWInputHandler::remove_key(int key_id)
{
	for( int i = 0; i < keys_sz; i++ )
	{
		if(keys[i]->key_id == key_id)
		{
			init_key(keys[i], NO_KEY_ID, NO_KEY_ID);
		}
	}
}

void GLFWInputHandler::start_mouse(void)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
	//glfwSetCursorPosCallback(window, mouse_callback);
}

// callbacks
void GLFWInputHandler::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
} 

void GLFWInputHandler::mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
	//mouse_pos = glm::vec2(x_pos, y_pos);
}

/************************************
* Initialize static class members
*************************************/
//glm::vec2 GLFWInputHandler::mouse_pos;