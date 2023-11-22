#ifndef VERTEXBUFFER_H_
#define VERTEXBUFFER_H_

#include <GLEW/glew.h>

class VertexBuffer {
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
private:
	unsigned int m_RendererId;
};

#endif // !VERTEXBUFFER_H_
