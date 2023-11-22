#include "VertexBuffer.h"
#include "DebugGL.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    GLDebug(glGenBuffers(1, &m_RendererId));
    GLDebug(glBindBuffer(GL_ARRAY_BUFFER, m_RendererId));
    GLDebug(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    GLDebug(glDeleteBuffers(1, &m_RendererId));
}

void VertexBuffer::Bind() const {
    GLDebug(glBindBuffer(GL_ARRAY_BUFFER, m_RendererId));
}

void VertexBuffer::Unbind() const {
    GLDebug(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
