#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <test/test.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static unsigned int compileShader(unsigned int type, const std::string &source);

static unsigned int createShader(const std::string &vertexShader, const std::string &compileShader);

static ShaderProgramSource parseShader(const std::string &filepath);

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

    float positions[8] = {
        -0.5f,-0.5f,
        0.5f,-0.5f,
        0.5f,0.5f,
        -0.5f,0.5f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0

    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // selecting a buffer ,  this tells gpu , this is just an array
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);/// allocates memory for the buffer

    glEnableVertexAttribArray(0);                                          // enables the generic vertex attribute array
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // how the data in buffer should be read

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // GL_ELEMENT_ARRAY_BUFFER is used for indices 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    ShaderProgramSource source = parseShader("res/shaders/basic.shader");

    unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // drawcall
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

static ShaderProgramSource parseShader(const std::string &filepath)
{

    std::string fullFilePath = std::string(PROJECT_DIR) + "/" + filepath;
    std::ifstream stream(fullFilePath);

    if (!stream.is_open())
    {
        std::cerr << "Error opening shader file: " << PROJECT_DIR + fullFilePath << std::endl;
        return {}; // Return an empty ShaderProgramSource if the file cannot be opened.
    }

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }

            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}
