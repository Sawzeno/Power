#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "renderer.h"
#include "vertexbufferlayout.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "vertexarray.h"
#include "shader.h"
#include "texture.h"

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
		float positions[] = {
			-0.5f,-0.5f, 0.0f, 0.0f,
			 0.5f,-0.5f, 1.0f, 0.0f,
			 0.5f, 0.5f, 1.0f, 1.1f,
			-0.5f, 0.5f, 0.0f, 1.0f, 
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA));

		//----------------------VERTEXBUFFERS------------------//

		VertexArray va;
		VertexBuffer vb(positions , 4*4*sizeof(float));

		VertexBufferLayout layout;
		layout.PushFloat(2);
		layout.PushFloat(2);

		va.AddBuffer(vb, layout);
	
		IndexBuffer ib(indices , 6 );

		//----------------------SHADER------------------------//

		Shader shader("res/shaders/basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color" , 0.8f  ,0.3f , 0.8f , 1.0f);

		//----------------------TEXTURE-----------------------//

		Texture texture("D:/Power/res/textures/adamkhor.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture" , 0);

		//----------------------RENDERER----------------------//

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		
		Renderer renderer;

		float r = 0.0f;
		float inc = 0.05f;

		while (!glfwWindowShouldClose(window))
		{

			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			shader.Bind();
			shader.SetUniform4f("u_Color" , r , 0.3f , 0.8f , 1.0f);

			renderer.Draw(va , ib , shader);

			if(r > 1.0f){

				inc = -0.05f;
			}else if(r < 0.05f){

				inc = 0.05f;
			}

			r += inc;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}

