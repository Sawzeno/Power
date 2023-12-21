#pragma once

#include<vector>
#include"GL/glew.h"

struct VertexBufferElement{

	unsigned int type;
	unsigned int count;

	bool normalized;
};

class VertexBufferLayout{

	private:
		std::vector<VertexBufferElement> m_Elements;
	public:
		VertexBufferLayout();

		void Push(int count){
			static_assert(false);
		}

		template<>
			void Push<float>(int count){

				m_Elements.push_back({GL_FLOAT , count , false});
			}
};
