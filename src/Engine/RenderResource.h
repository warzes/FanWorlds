#pragma once

#include "OpenGLUtil.h"

//=============================================================================
// Core enum
//=============================================================================

enum class ResourceType : uint8_t
{
	Unknown,
	Shader,
	ShaderProgram,
	VertexBuffer,
	IndexBuffer,
	VertexArray,
	Texture2D,
	Framebuffer
};

enum class ResourceUsage : uint8_t
{
	Static,
	Dynamic,
	Stream,
};

//=============================================================================
// Shader enum
//=============================================================================

enum class ShaderType : uint8_t
{
	Vertex,
	Fragment,
	Geometry,
	Compute,
	TessControl,
	Evaluation
};




enum class PrimitiveTopology : uint8_t
{
	Lines,
	Triangles,
	Points,
};

enum class TextureMinFilter : uint8_t
{
	Nearest,
	Linear,
	NearestMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapNearest,
	LinearMipmapLinear,
};

enum class TextureMagFilter : uint8_t
{
	Nearest,
	Linear,
};

enum class TextureAddressMode : uint8_t
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
#if !defined(__EMSCRIPTEN__) // not support in webGL
	ClampToBorder
#endif
};

enum class TexelsFormat : uint8_t
{
	None = 0,
	R_U8,
	RG_U8,
	RGB_U8,
	RGBA_U8,

	RG_F32,

	Depth_U16,
	DepthStencil_U16,
	Depth_U24,
	DepthStencil_U24,
};

enum class BlendFactor : uint8_t
{
	Zero,
	One
	//...
};

enum class BlendOp : uint8_t
{
	Add = 1,
	Subrtact = 2,
	//...
};

enum class RasterFillMode : uint8_t
{
	Solid,
	Wireframe,
};

enum class RasterCullMode : uint8_t
{
	Back,
	Front,
	Full
};

enum class StencilOp : uint8_t
{
	Keep,
	Zero,
	Replace,
	IncrSat,
	DecrSat,
	Invert,
	Incr,
	Decr
};

enum class ComparisonFunc : uint8_t
{
	Never,
	Less = 2,
	Equal = 3,
	LessOrEqual = 4,
	Greater = 5,
	NotEqual = 6,
	GreaterOrEqual = 7,
	Always = 8
};

enum class IndexBufferFormat : uint8_t
{
	Uint16,
	Uint32
};

struct VertexAttribute
{
	unsigned location/* = -1*/;  // ���� -1, �� ������� ������ ������� ���������
	int size;
	//unsigned type;
	bool normalized;
	int stride;         // sizeof Vertex
	const void* offset; // (void*)offsetof(Vertex, TexCoord)}
};

struct ShaderAttributeInfo
{
	unsigned typeId;
	unsigned type;
	int numType;
	std::string name;
	int location;
};

struct Texture2DInfo
{
	ResourceUsage usage = ResourceUsage::Static;

	TextureMinFilter minFilter = TextureMinFilter::NearestMipmapNearest;
	TextureMagFilter magFilter = TextureMagFilter::Nearest;
	TextureAddressMode wrapS = TextureAddressMode::Repeat;
	TextureAddressMode wrapT = TextureAddressMode::Repeat;

	bool mipmap = true;
};

struct Texture2DCreateInfo
{
	TexelsFormat format = TexelsFormat::RGBA_U8;
	uint16_t width = 1;
	uint16_t height = 1;
	uint8_t* pixelData = nullptr;
	unsigned mipMapCount = 1; // TODO: only compressed
};

struct Uniform { int location = -1; unsigned programId = 0; };
bool operator==(const Uniform& Left, const Uniform& Right) noexcept;

class glObject
{
public:
	glObject(const glObject&) = delete;
	glObject& operator=(const glObject&) = delete;

	glObject(glObject&& other) noexcept
		: m_handle{ other.m_handle }
		, m_ownership{ other.m_ownership }
	{
		other.m_handle = 0;
		other.m_ownership = false;
	}
	glObject& operator=(glObject&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_ownership, other.m_ownership);
		return *this;
	}

	operator GLuint() const { return m_handle; }
	GLuint Id() const { return m_handle; }
	bool HasOwnership() const { return m_ownership; }
	bool IsValid() const { return m_handle > 0; }

protected:
	glObject() = default;
	GLuint m_handle = 0;
	// Specifies if this object has the ownership of the entity behind the handle
	bool m_ownership = false;
};

class Shader : public glObject
{
public:
	explicit Shader(GLuint handle) { m_handle = handle; m_ownership = false; }
	explicit Shader(ShaderType type);
	~Shader() { if( m_ownership ) glDeleteShader(m_handle); }

	Shader(Shader&&) noexcept = default;
	Shader& operator=(Shader&&) noexcept = default;
};
using ShaderRef = std::shared_ptr<Shader>;

class ShaderProgram : public glObject
{
public:
	ShaderProgram() { m_handle = glCreateProgram(); m_ownership = true; }
	explicit ShaderProgram(GLuint handle) { m_handle = handle; m_ownership = false; }
	ShaderProgram(ShaderProgram&&) noexcept = default;
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram() { if( m_ownership ) glDeleteProgram(m_handle); }
	ShaderProgram& operator=(ShaderProgram&&) noexcept = default;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	bool operator==(const ShaderProgram& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership; }
};
using ShaderProgramRef = std::shared_ptr<ShaderProgram>;

static_assert(sizeof(ShaderProgram) == 8, "ShaderProgram size changed!!!");
inline bool operator==(ShaderProgramRef Left, ShaderProgramRef Right) noexcept { return *Left == *Right; }

// TODO: GPUBuffer
class VertexBuffer : public glObject
{
public:
	VertexBuffer() = delete;
	VertexBuffer(GLuint handle, ResourceUsage Usage, unsigned Count, unsigned Size) : usage(Usage), count(Count), size(Size) { m_handle = handle; m_ownership = false; }
	VertexBuffer(ResourceUsage Usage, unsigned Count, unsigned Size) : usage(Usage), count(Count), size(Size) { glGenBuffers(1, &m_handle); m_ownership = true; }
	VertexBuffer(VertexBuffer&&) noexcept = default;
	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer() { if( m_ownership ) glDeleteBuffers(1, &m_handle); }
	VertexBuffer& operator=(VertexBuffer&&) noexcept = default;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	bool operator==(const VertexBuffer& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && usage == ref.usage && count == ref.count && size == ref.size; }

	ResourceUsage usage = ResourceUsage::Static;
	unsigned count = 0;
	unsigned size = 0; 
};
using VertexBufferRef = std::shared_ptr<VertexBuffer>;

static_assert(sizeof(VertexBuffer) == 20, "VertexBuffer size changed!!!");
inline bool operator==(VertexBufferRef Left, VertexBufferRef Right) noexcept { return *Left == *Right; }

class IndexBuffer : public VertexBuffer
{
public:
	IndexBuffer() = delete;
	IndexBuffer(GLuint handle, ResourceUsage Usage, unsigned Count, unsigned Size) : VertexBuffer(handle, Usage, Count, Size) { }
	IndexBuffer(ResourceUsage Usage, unsigned Count, unsigned Size) : VertexBuffer(Usage, Count, Size) {}
	IndexBuffer(IndexBuffer&&) = default;
	IndexBuffer(const IndexBuffer&) = delete;
	~IndexBuffer() { if( m_ownership ) glDeleteBuffers(1, &m_handle); }
	IndexBuffer& operator=(IndexBuffer&&) = default;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	bool operator==(const IndexBuffer& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && usage == ref.usage && count == ref.count && size == ref.size; }
};
using IndexBufferRef = std::shared_ptr<IndexBuffer>;

static_assert(sizeof(IndexBuffer) == 20, "IndexBuffer size changed!!!");
inline bool operator==(IndexBufferRef Left, IndexBufferRef Right) noexcept { return *Left == *Right; }

class VertexArray : public glObject
{
public:
	VertexArray() = delete;
	VertexArray(GLuint handle, VertexBufferRef Vbo, IndexBufferRef Ibo, unsigned AttribsCount) : vbo(Vbo), ibo(Ibo), attribsCount(AttribsCount) { m_handle = handle; m_ownership = false; }
	VertexArray(VertexBufferRef Vbo, IndexBufferRef Ibo, unsigned AttribsCount) : vbo(Vbo), ibo(Ibo), attribsCount(AttribsCount) { glGenVertexArrays(1, &m_handle); m_ownership = true; }
	VertexArray(VertexArray&&) = default;
	VertexArray(const VertexArray&) = delete;
	~VertexArray() { if( m_ownership ) glDeleteVertexArrays(1, &m_handle); }
	VertexArray& operator=(VertexArray&&) = default;
	VertexArray& operator=(const VertexArray&) = delete;

	bool operator==(const VertexArray& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && vbo == ref.vbo && ibo == ref.ibo && attribsCount == ref.attribsCount; }

	VertexBufferRef vbo = nullptr;
	IndexBufferRef ibo = nullptr;
	unsigned attribsCount = 0;
};
using VertexArrayRef = std::shared_ptr<VertexArray>;
static_assert(sizeof(VertexArray) == 48, "VertexArray size changed!!!");
inline bool operator==(VertexArrayRef Left, VertexArrayRef Right) noexcept { return *Left == *Right; }

class GeometryBuffer
{
public:
	GeometryBuffer() = default;
	GeometryBuffer(GeometryBuffer&&) = default;
	GeometryBuffer(const GeometryBuffer&) = delete;
	GeometryBuffer& operator=(GeometryBuffer&&) = default;
	GeometryBuffer& operator=(const GeometryBuffer&) = delete;

	std::shared_ptr<VertexBuffer> vb = nullptr;
	std::shared_ptr<IndexBuffer> ib = nullptr;
	std::shared_ptr<VertexArray> vao = nullptr;
};
using GeometryBufferRef = std::shared_ptr<GeometryBuffer>;

class Texture : public glObject
{
public:
	Texture() { glGenTextures(1, &m_handle); m_ownership = true; }
	explicit Texture(GLuint handle) { m_handle = handle; m_ownership = false; }
	Texture(Texture&&) noexcept = default;
	Texture(const Texture&) = delete;
	~Texture() { if( m_ownership ) glDeleteTextures(1, &m_handle); }
	Texture& operator=(Texture&&) noexcept = default;
	Texture& operator=(const Texture&) = delete;
};

class Texture2D : public Texture
{
public:
	Texture2D() = delete;
	Texture2D(GLuint handle, unsigned Width, unsigned Height, TexelsFormat Format) : Texture(handle), width(Width), height(Height), format(Format) {}
	Texture2D(unsigned Width, unsigned Height, TexelsFormat Format) : Texture(), width(Width), height(Height), format(Format) { }
	Texture2D(Texture2D&&) = default;
	Texture2D(const Texture2D&) = delete;
	~Texture2D() { if( m_ownership ) glDeleteTextures(1, &m_handle); }
	Texture2D& operator=(Texture2D&&) = default;
	Texture2D& operator=(const Texture2D&) = delete;

	bool operator==(const Texture2D& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && width == ref.width && height == ref.height && format == ref.format; }

	unsigned width = 0;
	unsigned height = 0;
	TexelsFormat format = TexelsFormat::RGBA_U8; 
};

using Texture2DRef = std::shared_ptr<Texture2D>;

static_assert(sizeof(Texture2D) == 20, "Texture2D size changed!!!");
inline bool operator==(Texture2DRef Left, Texture2DRef Right) noexcept { return *Left == *Right; }

class Renderbuffer : public glObject
{
public:
	Renderbuffer() { glGenRenderbuffers(1, &m_handle); m_ownership = true; }
	explicit Renderbuffer(GLuint handle) { m_handle = handle; m_ownership = false; }
	Renderbuffer(Renderbuffer&&) noexcept = default;
	Renderbuffer(const Renderbuffer&) = delete;
	~Renderbuffer() { if( m_ownership ) glDeleteRenderbuffers(1, &m_handle); }
	Renderbuffer& operator=(Renderbuffer&&) noexcept = default;
	Renderbuffer& operator=(const Renderbuffer&) = delete;

	bool operator==(const Renderbuffer& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership; }
};
using RenderbufferRef = std::shared_ptr<Renderbuffer>;

static_assert(sizeof(Renderbuffer) == 8, "Renderbuffer size changed!!!");
inline bool operator==(RenderbufferRef Left, RenderbufferRef Right) noexcept { return *Left == *Right; }

class Framebuffer : public glObject
{
public:
	Framebuffer() { glGenFramebuffers(1, &m_handle); m_ownership = true; }
	explicit Framebuffer(GLuint handle) { m_handle = handle; m_ownership = false; }
	Framebuffer(Framebuffer&&) = default;
	Framebuffer(const Framebuffer&) = delete;
	~Framebuffer() { if( m_ownership ) glDeleteFramebuffers(1, &m_handle); }
	Framebuffer& operator=(Framebuffer&&) = default;
	Framebuffer& operator=(const Framebuffer&) = delete;

	bool operator==(const Framebuffer& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && texture == ref.texture; }

	Texture2DRef texture;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

static_assert(sizeof(Framebuffer) == 24, "Framebuffer size changed!!!");
inline bool operator==(FramebufferRef Left, FramebufferRef Right) noexcept { return *Left == *Right; }

#if USE_OPENGL_VERSION == OPENGL40
class TransformFeedback : public glObject
{
public:
	TransformFeedback() { glGenTransformFeedbacks(1, &m_handle); m_ownership = true; }
	explicit TransformFeedback(GLuint handle) { m_handle = handle; m_ownership = false; }
	TransformFeedback(TransformFeedback&&) noexcept = default;
	TransformFeedback(const TransformFeedback&) = delete;
	~TransformFeedback() { if( m_ownership ) glDeleteTransformFeedbacks(1, &m_handle); }
	TransformFeedback& operator=(TransformFeedback&&) noexcept = default;
	TransformFeedback& operator=(const TransformFeedback&) = delete;

	bool operator==(const TransformFeedback& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership; }
};
using TransformFeedbackRef = std::shared_ptr<TransformFeedback>;

static_assert(sizeof(TransformFeedback) == 8, "TransformFeedback size changed!!!");
inline bool operator==(TransformFeedbackRef Left, TransformFeedbackRef Right) noexcept { return *Left == *Right; }

#endif // OPENGL40

struct BlendState { /*...*/ };
struct DepthStencilState { /*...*/ };
struct RasterizerState { /*...*/ };