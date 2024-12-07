#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coords;

out vec2 out_tex_coords;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    out_tex_coords = in_tex_coords;
    gl_Position = u_projection * u_view * u_model * vec4(in_pos, 1.0);
}
