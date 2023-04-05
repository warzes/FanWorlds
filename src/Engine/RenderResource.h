#pragma once

enum class ResourceType : uint8_t
{
	Unknown,
	ShaderProgram,
	VertexBuffer,
	IndexBuffer,
	VertexArray,
	Texture2D
};

enum class ResourceUsage : uint8_t
{
	Static,
	Dynamic,
	Stream,
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
	unsigned location/* = -1*/;  // если -1, то берется индекс массива атрибутов
	int size;
	//unsigned type;
	bool normalized;
	int stride;         // sizeof Vertex
	const void* offset; // (void*)offsetof(Vertex, TexCoord)}
};

struct ShaderAttribInfo
{
	unsigned typeId;
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

class ShaderProgram
{
public:
	ShaderProgram() { id = glCreateProgram(); }
	ShaderProgram(ShaderProgram&&) = default;
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram() { glDeleteProgram(id); }
	ShaderProgram& operator=(ShaderProgram&&) = default;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	unsigned id = 0; 
};

using ShaderProgramRef = std::shared_ptr<ShaderProgram>;

// GPUBuffer

class VertexBuffer
{
public:
	VertexBuffer() = delete;
	VertexBuffer(ResourceUsage _usage, unsigned _count, unsigned _size) : usage(_usage), count(_count), size(_size) { glGenBuffers(1, &id); }
	VertexBuffer(VertexBuffer&&) = default;
	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer() { glDeleteBuffers(1, &id); }
	VertexBuffer& operator=(VertexBuffer&&) = default;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	unsigned id = 0;
	ResourceUsage usage = ResourceUsage::Static;
	unsigned count = 0;
	unsigned size = 0; 
};

using VertexBufferRef = std::shared_ptr<VertexBuffer>;

class IndexBuffer
{
public:
	IndexBuffer() = delete;
	IndexBuffer(ResourceUsage _usage, unsigned _count, unsigned _size) : usage(_usage), count(_count), size(_size) { glGenBuffers(1, &id); }
	IndexBuffer(IndexBuffer&&) = default;
	IndexBuffer(const IndexBuffer&) = delete;
	~IndexBuffer() { glDeleteBuffers(1, &id); }
	IndexBuffer& operator=(IndexBuffer&&) = default;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	unsigned id = 0;
	ResourceUsage usage = ResourceUsage::Static;
	unsigned count = 0;
	unsigned size = 0;
};

using IndexBufferRef = std::shared_ptr<IndexBuffer>;

class VertexArray 
{
public:
	VertexArray() = delete;
	VertexArray(std::shared_ptr<VertexBuffer> _vbo, std::shared_ptr<IndexBuffer> _ibo, unsigned _attribsCount) : vbo(_vbo), ibo(_ibo), attribsCount(_attribsCount) { glGenVertexArrays(1, &id); }
	VertexArray(VertexArray&&) = default;
	VertexArray(const VertexArray&) = delete;
	~VertexArray() { glDeleteVertexArrays(1, &id); }
	VertexArray& operator=(VertexArray&&) = default;
	VertexArray& operator=(const VertexArray&) = delete;

	unsigned id = 0;
	std::shared_ptr<VertexBuffer> vbo = nullptr;
	std::shared_ptr<IndexBuffer> ibo = nullptr;
	unsigned attribsCount = 0;
};

using VertexArrayRef = std::shared_ptr<VertexArray>;

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

class Texture2D 
{
public:
	Texture2D() = delete;
	Texture2D(unsigned _width, unsigned _height, TexelsFormat _format) : width(_width), height(_height), format(_format) { glGenTextures(1, &id); }
	Texture2D(Texture2D&&) = default;
	Texture2D(const Texture2D&) = delete;
	~Texture2D() { glDeleteTextures(1, &id); }
	Texture2D& operator=(Texture2D&&) = default;
	Texture2D& operator=(const Texture2D&) = delete;

	unsigned id = 0;
	unsigned width = 0;
	unsigned height = 0;
	TexelsFormat format = TexelsFormat::RGBA_U8; 
};

using Texture2DRef = std::shared_ptr<Texture2D>;

class Framebuffer
{
public:
	Framebuffer() { glGenFramebuffers(1, &id); }
	Framebuffer(Framebuffer&&) = default;
	Framebuffer(const Framebuffer&) = delete;
	~Framebuffer() { glDeleteFramebuffers(1, &id); }
	Framebuffer& operator=(Framebuffer&&) = default;
	Framebuffer& operator=(const Framebuffer&) = delete;

	unsigned id = 0;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

struct BlendState { /*...*/ };
struct DepthStencilState { /*...*/ };
struct RasterizerState { /*...*/ };

bool operator==(ShaderProgramRef Left, ShaderProgramRef Right) noexcept;
bool operator==(const Uniform& Left, const Uniform& Right) noexcept;
bool operator==(VertexBufferRef Left, VertexBufferRef Right) noexcept;
bool operator==(IndexBufferRef Left, IndexBufferRef Right) noexcept;
bool operator==(VertexArrayRef Left, VertexArrayRef Right) noexcept;
bool operator==(Texture2DRef Left, Texture2DRef Right) noexcept;