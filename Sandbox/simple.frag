#version 330 core
in vec2 tex_cord;

//uniform sampler2D pokemonTexture;
uniform vec3 quad_color;

out vec4 fragcolor;
 


float diff_factor(vec3 norm, vec3 lightdir)
{
	norm = normalize(norm);
	lightdir = normalize(-lightdir);
	return (max(dot(norm,lightdir), 0.2));
}

float specular_factor(vec3 cam, vec3 frag,vec3 norm, vec3 lightdir)
{
	vec3 camdir = normalize(cam - frag);
	vec3 reflectdir = normalize(reflect(lightdir, norm));  
	return (pow(max(dot(camdir, reflectdir), 0.0), 32));
}


void main()
{
	//vec3 color_diff = vec3(tex_cord, 0);
    fragcolor = vec4(quad_color, 1);
} 