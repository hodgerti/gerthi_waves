/************************************
* Headers
*************************************/
#include <camera.h>

/************************************
* Functions
*************************************/
void Camera::move_forward( double delta )
{
	float speed = (float)(move_speed * delta);
	pos += speed * front;
}

void Camera::move_backwards( double delta )
{
	float speed = (float)(move_speed * delta);
	pos -= speed * front;
}

void Camera::move_left( double delta )
{
	float speed = (float)(move_speed * delta);
	pos -= glm::normalize(glm::cross(front, up_view)) * speed;
}

void Camera::move_right( double delta )
{
	float speed = (float)(move_speed * delta);
	pos += glm::normalize(glm::cross(front, up_view)) * speed;
}

void Camera::set_up_view(glm::vec3 up)
{
	up_view = up;
}

void Camera::set_mouse(glm::vec2 mouse_pos)
{
	float x_offset = (float)(mouse_pos.x - last_x);
	float y_offset = (float)(last_y - mouse_pos.y);
	last_x = mouse_pos.x;
	last_y = mouse_pos.y;

	yaw   += x_offset * sensitivity;
	pitch += y_offset * sensitivity; 	
	
	if( first_mouse )
	{
		yaw = -90.0;
		pitch = 0.0;
	}

	pitch = clamp(pitch, MIN_PITCH, MAX_PITCH);

	front.x = (float)(cos(glm::radians(pitch)) * cos(glm::radians(yaw)));
	front.y = (float)(sin(glm::radians(pitch)));
	front.z = (float)(cos(glm::radians(pitch)) * sin(glm::radians(yaw)));
	front = glm::normalize(front);

	first_mouse = false;
}

void Camera::set_clip_perspective(double fov, double aspect, double near_plane, double far_plane)
{
	clip = glm::perspective(glm::radians((float)fov), (float)aspect, (float)near_plane, (float)far_plane);
}
void Camera::set_clip_orthographic(double width, double height, double near_plane, double far_plane)
{
	clip = glm::ortho(0.0f, (float)width, 0.0f, (float)height, (float)near_plane, (float)far_plane);
}

void Camera::use_world()
{
	GLint uniform_loc = glGetUniformLocation(shader_id, Shader::u_model_mat4);
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(model));
}

void Camera::use_view()
{
	view = glm::lookAt(pos, pos + front, up_view);
	GLint uniform_loc = glGetUniformLocation(shader_id, Shader::u_view_mat4);
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::use_clip()
{
	GLint uniform_loc = glGetUniformLocation(shader_id, Shader::u_projection_mat4);
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(clip));
}


void Camera::use()
{
	use_world();
	use_view();
	use_clip();
}

void Camera::use_world( glm::mat4 temp_world )
{
	GLint uniform_loc = glGetUniformLocation(shader_id, Shader::u_model_mat4);
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(temp_world));
}

void Camera::use_view( glm::mat4 temp_view )
{
	GLint uniform_loc = glGetUniformLocation(shader_id, Shader::u_view_mat4);
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(temp_view));
}

void Camera::use_clip( glm::mat4 temp_clip )
{
	GLint uniform_loc = glGetUniformLocation(shader_id, Shader::u_projection_mat4);
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(temp_clip));
}

void Camera::use( glm::mat4 temp_world, glm::mat4 temp_view, glm::mat4 temp_clip)
{
	use_world( temp_world );
	use_view( temp_view );
	use_clip( temp_clip );
}


void Camera::set_world(glm::mat4 model)
{
	this->model = model;
}

void Camera::set_view(glm::mat4 view)
{
	this->view = view;
}

void Camera::set_clip(glm::mat4 clip)
{
	this->clip = clip;
}

glm::mat4 Camera::get_world()
{
	return model;
}

glm::mat4 Camera::get_view()
{
	return view;
}

glm::mat4 Camera::get_clip()
{
	return clip;
}

glm::mat4 Camera::remove_translation( glm::mat4 mat )
{
	return glm::mat4(glm::mat3(mat));  
}