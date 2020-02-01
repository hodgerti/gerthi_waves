#ifndef GLFW_HELPER_H
#define GLFW_HELPER_H

/*==================================================
= Description: This provides helper functions to 
= facilitate GLFW
====================================================*/

/************************************
* Headers
*************************************/
#include <glm/glm.hpp>
#include <glad/glad.h> 
#include <glfw3.h>
#include <stdio.h>
#include <stdlib.h>


/************************************
* Definitions
*************************************/
#define DFLT_KEYS_SIZE		32
#define NO_KEY_ID			-1
#define DFLT_MOUSEPOS glm::vec2(0.0, 0.0);
#define MAX_YAW 
#define MIN_YAW

/************************************
* Macros
*************************************/
#define min(a,b)		(((a)<(b))?(a):(b))
#define max(a,b)		(((a)>(b))?(a):(b))
#define clamp(c,a,b)	(max((a),min((c),(b))))

/************************************
* Types
*************************************/
struct key_handle
{
	int				key_id;
	int				key_name;
	unsigned int	clicks;
	bool			pressed;
};

class GLFWInputHandler
{
	private:
		key_handle **keys;
		int keys_sz;

		void init_key(key_handle *key, int key_id, int key_name);

	public:
		GLFWwindow *window;

		// variables used in callbacks
		glm::vec2 mouse_pos;

		GLFWInputHandler();
		GLFWInputHandler(int w_width, int w_height, const char *monitor, GLADloadproc load_proc);
		~GLFWInputHandler();

		int start_glfw(int w_width, int w_height, const char *monitor, GLADloadproc load_proc);

		// get number of times button has been clicked. Non-destructive.
		int  get_clicks(int key_id);
		// pop a click off total clicks if one exists. Destructive.
		bool pop_click(int key_id);
		// erase all clicks.
		int  clear_clicks(int key_id);

		// check if button is pressed down
		bool check_pressed(int key_id);
		// check if button is not pressed down
		bool check_unpressed(int key_id);

		void update();
		// Add a key track
		void add_key(int key_id, int key_name);
		// Remove a key track. Doesn't unallocate any memory.
		void remove_key(int key_id);
		// enable mouse
		void start_mouse(void);

		// callbacks
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
};


/************************************
* Functions
*************************************/


#endif