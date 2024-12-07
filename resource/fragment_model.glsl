#version 330 core

in vec2 in_tex_coords;

out vec4 out_frag_color;

uniform sampler2D u_texture_diffuse;

void main()
{    
    out_frag_color = texture(u_texture_diffuse, in_tex_coords);
}