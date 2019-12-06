#ifndef TRANSFORM_H
#define TRANSFORM_H

/************************************
* Headers
*************************************/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

/************************************
* Constants
*************************************/

/************************************
* Macros
*************************************/
#define EXPAND(x) x
#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,MACRO_NAME,...) MACRO_NAME
#define transform_my(...) EXPAND(GET_MACRO(__VA_ARGS__, transform_7_4f_3f, none_6, transform_5_4f_vec3, transform_4_vec4_3f, none_3, transform_2_vec4_vec3)(__VA_ARGS__))
#define scale_my(...)     EXPAND(GET_MACRO(__VA_ARGS__, scale_7_4f_3f,     none_6, scale_5_4f_vec3,     scale_4_vec4_3f,     none_3, scale_2_vec4_vec3)(__VA_ARGS__))


#define transform_2_vec4_vec3(in, trans)										_transform(in, trans)
#define scale_2_vec4_vec3(in, trans)											_scale(in, trans)
#define none_3(...)					
#define transform_4_vec4_3f(in, trans_x, trans_y, trans_z)						_transform(in, glm::vec3(trans_x, trans_y, trans_z))
#define scale_4_vec4_3f(in, trans_x, trans_y, trans_z)							_scale(in, glm::vec3(trans_x, trans_y, trans_z))
#define transform_5_4f_vec3(in_x, in_y, in_z, in_w, trans)						_transform(glm::vec4(in_x, in_y, in_z, in_w), trans)
#define scale_5_4f_vec3(in_x, in_y, in_z, in_w, trans)							_scale(glm::vec4(in_x, in_y, in_z, in_w), trans)
#define none_6(...)					
#define transform_7_4f_3f(in_x, in_y, in_z, in_w, trans_x, trans_y, trans_z)	_transform(glm::vec4(in_x, in_y, in_z, in_w), glm::vec3(trans_x, trans_y, trans_z))
#define scale_7_4f_3f(in_x, in_y, in_z, in_w, trans_x, trans_y, trans_z)		_scale(glm::vec4(in_x, in_y, in_z, in_w), glm::vec3(trans_x, trans_y, trans_z))


/************************************
* Functions
*************************************/
glm::vec3 translate_around_point
(
	glm::vec3 object_pos,
	glm::vec3 origin_pos,
	glm::vec3 rotation_axis,
	float rotation
);

glm::vec3 transpose_onto_plane
(
	glm::vec3 object_pos,
	glm::vec3 plane_pos,
	glm::vec3 plane_normal
);

glm::vec3 cart_to_spher
(
	glm::vec3 cart_coords
);

glm::vec3 spher_to_cart
(
	glm::vec3 spher_coords
);

float difference
(
	glm::vec3 v_1, 
	glm::vec3 v_2
);

float length
(
	glm::vec3 v_1
);

#endif