#pragma once

#include "vertexbuffer.h"

class VertexArray{

private:

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb , const VertexBufferLayout& layout);
};
