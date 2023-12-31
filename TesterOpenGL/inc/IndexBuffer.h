#ifndef IndexBuffer_H_
#define IndexBuffer_H_

#include <GLEW/glew.h>

class IndexBuffer {
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
private:
	unsigned int m_RendererId, m_Count;
};

#endif // !IndexBuffer_H_
