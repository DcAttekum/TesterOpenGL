#include "IndexBuffer.h"
#include "DebugGL.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : m_Count(count) {
    GLDebug(glGenBuffers(1, &m_RendererId));
    GLDebug(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId));
    GLDebug(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer() {
    GLDebug(glDeleteBuffers(1, &m_RendererId));
}

void IndexBuffer::Bind() const {
    GLDebug(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId));
}

void IndexBuffer::Unbind() const {
    GLDebug(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
