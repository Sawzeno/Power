#shader vertex
#version 440 core

layout (location = 0) in vec4 position;

void main()
{
    gl_Position = position;
};

#shader fragment
#version 440 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
};