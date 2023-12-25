#pragma once

#include <GL/glew.h>

#include "vertexarray.h"
#include "indexbuffer.h"
#include "shader.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function , const char* file , int line);

class Renderer{

public:
	void Draw(const VertexArray& va , const IndexBuffer& vb , const Shader& shader) const;
};
