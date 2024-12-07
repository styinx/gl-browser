#version 330 core

in vec2 in_tex_coord;

out vec4 FragColor;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    FragColor = mix(
        texture(u_texture1, in_tex_coord),
        texture(u_texture2, in_tex_coord),
        0.2
    );
}

