#version 330 core

in vec2 tex_cord;
out vec4 color;

void main()
{ 
	color = vec4(tex_cord.x, tex_cord.y, 1.0, 1.0);
}