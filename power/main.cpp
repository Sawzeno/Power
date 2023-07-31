#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <test/test.h>
#include <iostream>

static unsigned int compileShader(unsigned int type, const std::string &source);

static unsigned int createShader(const std::string &vertexShader, const std::string &compileShader);

int main(void)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    GLFWmonitor *primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *videomode = glfwGetVideoMode(primary);
    const char *m_name = glfwGetMonitorName(primary);

    GLFWwindow *window;
    window = glfwCreateWindow(videomode->width, videomode->height, "EXPLOSION", primary, NULL);
    std::cout << "monitor name : " << m_name << std::endl;

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f};

    std::string vertexShader =
        "#version 120 \n"
        "\n"
        "attribute vec4 position;"
        "\n"
        "void main()\n"
        "{\n"
        "gl_Position = position;\n"
        "}\n";
    std::string fragmentShader =
        "#version 120 \n"
        "\n"
        "void main()\n"
        "{\n"
        "gl_FragColor = vec4(1.0 , 0.0, 0.0 , 1.0);\n"
        "}\n";

    unsigned int buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // selecting a buffer ,  this tells gpu , this is just an array
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);                                          // enables the generic vertex attribute array
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // how the data in buffer should be read

    unsigned int shader = createShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3); // drawcall
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}

static unsigned int compileShader(unsigned int type, const std::string &source)
{

    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);

        return 0;
    }
    return id;
}

static unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader)
{

    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}