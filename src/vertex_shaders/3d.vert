#version 330 core		

// navigating meshes								
layout (location = 0) in vec3 aPos;

// space transforming
uniform mat4 view;
uniform mat4 proj;
		
// pipeline	
out vec3 TexCoords;
							
void main()											
{	
	TexCoords = aPos;													
	vec4 pos = proj * view * vec4(aPos, 1.0);	
	gl_Position = pos.xyww; // becomes < x, y, w, w >

}