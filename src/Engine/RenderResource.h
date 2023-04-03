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

enum class PrimitiveDraw : uint8_t
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

enum class TextureWrapping : uint8_t
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
	//...
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
	TextureWrapping wrapS = TextureWrapping::Repeat;
	TextureWrapping wrapT = TextureWrapping::Repeat;

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

class ShaderProgram
{ 
public:
	ShaderProgram() { id = glCreateProgram(); }
	ShaderProgram(ShaderProgram&&) = default;
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram() { glDeleteProgram(id); id = 0; }
	ShaderProgram& operator=(ShaderProgram&&) = default;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	unsigned id = 0; 
};

struct Uniform { int location = -1; unsigned programId = 0; };
struct VertexBuffer { unsigned id = 0; ResourceUsage usage = ResourceUsage::Static; unsigned count = 0; unsigned size = 0; };
struct IndexBuffer { unsigned id = 0; ResourceUsage usage = ResourceUsage::Static; unsigned count = 0; unsigned size = 0; };
struct VertexArray { unsigned id = 0; VertexBuffer *vbo = nullptr; IndexBuffer *ibo = nullptr; unsigned attribsCount = 0; };
struct Texture2D { unsigned id = 0; unsigned width = 0; unsigned height = 0; TexelsFormat format = TexelsFormat::RGBA_U8; };

struct BlendState { /*...*/ };
struct RasterState { /*...*/ };

bool operator==(const ShaderProgram& Left, const ShaderProgram& Right) noexcept;
bool operator==(const Uniform& Left, const Uniform& Right) noexcept;
bool operator==(const VertexBuffer& Left, const VertexBuffer& Right) noexcept;
bool operator==(const IndexBuffer& Left, const IndexBuffer& Right) noexcept;
bool operator==(const VertexArray& Left, const VertexArray& Right) noexcept;
bool operator==(const Texture2D& Left, const Texture2D& Right) noexcept;