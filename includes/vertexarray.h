#pragma once

#include "vertexbuffer.h"

class VertexBufferLayout;

class VertexArray{

private:

	unsigned m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb , const VertexBufferLayout& layout);
	void Bind() const;
	void Unbind() const;
};
