/************************************
* Headers
*************************************/
#include <transform.h>

/************************************
* Functions
*************************************/
glm::vec3 translate_around_point
(
	glm::vec3 object_pos,
	glm::vec3 origin_pos,
	glm::vec3 rotation_axis,
	float rotation
)
{
	// get difference
	glm::vec3 difference = object_pos - origin_pos;
	//rotate difference on rotation axis
	glm::mat4 model(1.0);
	model = glm::rotate(model, rotation, rotation_axis);
	glm::vec3 trans = model * glm::vec4(difference, 1.0);
	//add difference to object pos
	return origin_pos + trans;
}

glm::vec3 transpose_onto_plane
(
	glm::vec3 object_pos,
	glm::vec3 plane_pos,
	glm::vec3 plane_normal
)
{
	glm::vec3 v_1 = object_pos - plane_pos;
	float dist = glm::dot(v_1, plane_normal);
	return object_pos - dist*plane_normal;
}

glm::vec3 cart_to_spher
(
	glm::vec3 cart_coords
)
{
	double radius = length(cart_coords);
	double theta = atan(cart_coords.x/cart_coords.y);
	double phi	= acos(cart_coords.z/radius);
	
	// theta is yaw
	// phi is pitch
	return glm::vec3(radius, theta, phi);
}

glm::vec3 spher_to_cart
(
	glm::vec3 spher_coords
)
{
	float x = spher_coords.x * cos(spher_coords.y) * sin(spher_coords.z);
	float y = spher_coords.x * sin(spher_coords.y) * sin(spher_coords.z);
	float z = spher_coords.x * cos(spher_coords.z);

	return glm::vec3(x, y, z);
}

float difference
(
	glm::vec3 v_1, 
	glm::vec3 v_2
)
{
	return sqrtf(pow(v_1.x - v_2.x, 2.0) 
		       + pow(v_1.y - v_2.y, 2.0) 
		       + pow(v_1.z - v_2.z, 2.0));
}

float length
(
	glm::vec3 v_1
)
{
	return sqrtf(pow(v_1.x, 2.0) 
		       + pow(v_1.y, 2.0) 
		       + pow(v_1.z, 2.0));
}
