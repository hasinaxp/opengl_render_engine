#version 330 core
layout (location = 0) in vec3 position; // the position variable has attribute position 0
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 view_projection_matrix;
uniform mat4 model_matrix;

out vec2 tex_cord;


void main()
{ 
	vec4 world_position = model_matrix * vec4(position, 1.0);
	vec4 projected_position = view_projection_matrix * world_position;
    gl_Position =  projected_position;
	tex_cord = uv;
}