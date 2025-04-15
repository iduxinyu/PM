#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;


void main()
{
    FragColor=vec4(0.5f,0.5f,0.5f,1.0f);
}