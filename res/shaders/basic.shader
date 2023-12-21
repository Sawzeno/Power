#shader vertex
#version 440 core

layout (location = 0) in vec4 position;

void main()
{
    gl_Position = position;
};

#shader fragment
#version 440 core

layout(location = 0) out vec4 FragColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color;
};
