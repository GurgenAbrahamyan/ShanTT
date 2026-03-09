#pragma once
#include <cstdint>
#include <glad/glad.h>
enum class RenderBufferFormat
{
	None = 0,
	Depth24,
	Depth32F,
	Depth24Stencil8,
	Stencil8
};

class RenderBuffer {

private:
	
	uint32_t m_ID=0;
	
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	RenderBufferFormat m_Format = RenderBufferFormat::Depth24;
	uint32_t m_Samples = 1;

public:

	RenderBuffer(uint32_t width = 0,
		uint32_t height =0,
		RenderBufferFormat format = RenderBufferFormat::Depth24,
		uint32_t samples = 1);

	~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &m_ID);
	}

	void resize(uint32_t width, uint32_t height);


	RenderBuffer(RenderBuffer&& other) noexcept;
	RenderBuffer& operator=(RenderBuffer&& other) noexcept;

	uint32_t getID() const;

	uint32_t getWidth() const;
	uint32_t getHeight() const;

	RenderBufferFormat getFormat() const;

	uint32_t getSamples() const;
};