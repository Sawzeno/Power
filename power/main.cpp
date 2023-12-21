#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <test/test.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "renderer.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "EXPLOSION", NULL, NULL);

	if (!window){
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	{
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

		unsigned int vao;
		GLCall(glGenVertexArrays(1 , &vao));
		GLCall(glBindVertexArray(vao));

		VertexBuffer vb(positions , 4*2*sizeof(float));

		GLCall(glEnableVertexAttribArray(0));                                          // enables the generic vertex attribute array
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0)); // how the data in buffer should be read

		IndexBuffer ib(indices , 6 );

		//----------------------SHADER------------------------//

		ShaderProgramSource source = parseShader("res/shaders/basic.shader");

		unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
		GLCall(glUseProgram(shader)); // binding the program 

		GLCall(int location = glGetUniformLocation(shader, "u_Color"));
		ASSERT(location != -1);
		GLCall(glUniform4f(location , 0.0f , 0.4f , 0.8f , 1.0f));

		//----------------------DRIVER------------------------//
		//openGL uses indices to draw the last array buffer bound

		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER , 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , 0))

			float r = 0.0f;
		float inc = 0.05f;
		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			//BIND SHADER
			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location , r , 0.3f , 0.8f , 1.0f));

			/*
				NOT REQUIRED ANYMORE AFTER SETTING UP VAO !
				BIND BUFFER
				GLCall(glBindBuffer(GL_ARRAY_BUFFER , buffer));
				BIND VERTEX ARRAY
				GLCall(glEnableVertexAttribArray(0));
				BIND VERTEX LAYOUT
				GLCall(glVertexAttribPointer(0,2,GL_FLOAT , GL_FALSE , sizeof(float) * 2 , 0));
			 */

			//BIND VAO
			GLCall(glBindVertexArray(vao));
			//BIND INDEX
			ib.Bind();
			//DRAWCALL
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // drawcall

			if(r > 1.0f){

				inc = -0.05f;
			}else if(r < 0.05f){

				inc = 0.05f;
			}

			r += inc;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteProgram(shader);
	}
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
