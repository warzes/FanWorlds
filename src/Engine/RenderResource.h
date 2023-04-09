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

//=============================================================================
// Blend enum
//=============================================================================

enum class BlendOp : uint8_t
{
	Add,
	Subrtact,
	ReverseSubtract,
	Min,
	Max
};

enum class BlendFactor : uint8_t
{
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	DstColor,
	OneMinusDstColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	ConstantColor,
	OneMinusConstantColor,
	ConstantAlpha,
	OneMinusConstantAlpha,
	SrcAlphaSaturate,
	Src1Color,
	OneMinusSrc1Color,
	Src1Alpha,
	OneMinusSrc1Alpha
};

//=============================================================================
// Rasterizer enum
//=============================================================================

enum class RasterizerCullMode : uint8_t
{
	Back,
	Front,
	FrontAndBack
};

enum class FaceOrientation : uint8_t
{
	Cw,
	Ccw,
};

enum class RasterizerFillMode : uint8_t // TODO: нет функции TranslateToGL
{
	Solid,
	Wireframe,
	Point
};

// ERasterizerFillMode

//=============================================================================
// DepthStencil enum
//=============================================================================

enum class StencilOp : uint8_t
{
	Keep,
	Zero,
	Replace,
	IncrSat,
	DecrSat,
	Invert,
	IncrWrap,
	DecrWrap
};

enum class ComparisonFunc : uint8_t
{
	Lequal,
	Gequal,
	Less,
	Greater,
	Equal,
	Notequal,
	Always,
	Never
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

//=============================================================================
// GPUBuffer enum
//=============================================================================

// TODO: использовать
enum class BufferType : uint8_t
{
	ArrayBuffer, // aka VertexBuffer
	AtomicCounterBuffer,
	CopyReadBuffer,
	CopyWriteBuffer,
	DispatchIndirectBuffer,
	DrawIndirectBuffer,
	ElementArrayBuffer, // aka Index Buffer
	PixelPackBuffer,
	PixelUnpackBuffer,
	ShaderStorageBuffer,
	TextureBuffer,
	TransformFeedbackBuffer,
	UniformBuffer
};

enum class BufferUsage : uint8_t
{
	StaticDraw,
	DynamicDraw,
	StreamDraw,

	StaticRead,
	DynamicRead,
	StreamRead,

	StaticCopy,
	StreamCopy,
	DynamicCopy
};

enum class BufferMapAccess : uint8_t
{
	Read,
	Write,
	ReadWrite
};

enum class BufferBinding : uint8_t
{
	ArrayBufferBinding,
	AtomicCounterBufferBinding,
	CopyReadBuffer,
	CopyWriteBuffer,
	DispatchIndirectBufferBinding,
	DrawIndirectBufferBinding,
	ElementArrayBufferBinding,
	PixelPackBufferBinding,
	PixelUnpackBufferBinding,
	ShaderStorageBufferBinding,
	TextureBindingBuffer,
	TransformFeedbackBufferBinding,
	UniformBufferBinding
};

enum class BufferMapAccessFlags : uint8_t
{
	MapReadBit,
	MapWriteBit,
	MapInvalidateRangeBit,
	MapInvalidateBufferBit,
	MapFlushExplicitBit,
	MapUnsynchronizedBit,
	MapPersistentBit,
	MapCoherentBit
};

//=============================================================================
// IndexBuffer enum
//=============================================================================

enum class IndexType : uint8_t
{
	Uint8,
	Uint16,
	Uint32
};

//=============================================================================
// VertexArray enum
//=============================================================================
enum class PrimitiveTopology : uint8_t
{
	Points,
	Lines,
	LineStrip,
	LineLoop,
	Triangles,
	TriangleStrip,
	TriangleFan,
	LinesAdjacency,
	LineStripAdjacency,
	TrianglesAdjacency,
	TriangleStripAdjacency,
	Patches
};

//=============================================================================
// Texture enum
//=============================================================================

enum class TextureType : uint8_t // TODO: использовать
{
	Texture1D,
	Texture2D,
	Texture3D,
	TextureRectangle,
	Texture1DArray,
	Texture2DArray,
	TextureCubeMap,
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

enum class TextureCubeTarget : uint8_t
{
	TextureCubeMapPositiveX,
	TextureCubeMapNegativeX,
	TextureCubeMapPositiveY,
	TextureCubeMapNegativeY,
	TextureCubeMapPositiveZ,
	TextureCubeMapNegativeZ
};

//=============================================================================
// Framebuffer enum
//=============================================================================

enum class FramebufferAttachment : uint8_t
{
	ColorAttachment0,
	ColorAttachment1,
	ColorAttachment2,
	ColorAttachment3,
	ColorAttachment4,
	ColorAttachment5,
	ColorAttachment6,
	ColorAttachment7,
	ColorAttachment8,
	ColorAttachment9,
	ColorAttachment10,
	ColorAttachment11,
	ColorAttachment12,
	ColorAttachment13,
	ColorAttachment14,
	ColorAttachment15,
	DepthAttachment,
	StencilAttachment,
	DepthStencilAttachment
};

enum class FramebufferType : uint8_t
{
	ReadFramebuffer,
	DrawFramebuffer,
	Framebuffer
};

enum class FramebufferStatus : uint8_t
{
	FramebufferComplete,
	FramebufferUndefined,
	FramebufferIncompleteAttachment,
	FramebufferIncompleteMissingAttachment,
	FramebufferIncompleteDrawBuffer,
	FramebufferIncompleteReadBuffer,
	FramebufferUnsupported,
	FramebufferIncompleteMultisample,
	FramebufferIncompleteLayerTargets
};

enum class FramebufferBinding : uint8_t
{
	ReadFramebufferBinding,
	DrawFramebufferBinding,
	FramebufferBinding
};

//=============================================================================
// Shader Core
//=============================================================================

// A class that can load shader sources in from files, and do some preprocessing on them.
class ShaderSource
{
public:
	ShaderSource() = default;
	// Loads in the shader from a memory data.
	ShaderSource(const std::string& src) : m_src(src) {};
	ShaderSource(ShaderSource&&) = default;
	ShaderSource(const ShaderSource&) = default;

	ShaderSource& operator=(ShaderSource&&) = default;
	ShaderSource& operator=(const ShaderSource&) = default;
	ShaderSource& operator=(const std::string& src) { m_src = src; return *this; }

	bool LoadFromFile(const std::string& file);

	std::string& GetSource() { return m_src; }
	const std::string& GetSource() const { return m_src; }
	const std::string& GetPath() const { return m_path; }
	const std::string& GetFilename() const { return m_filename; }

	bool IsValid() const { return m_src.length() > 0; }

	template<typename T>
	void InsertMacroValue(const std::string& macroName, const T& value)
	{
		size_t macroPos = m_src.find("#define " + macroName);
#if defined(_DEBUG)
		if( macroPos == std::string::npos )
		{
			//Fatal("ShaderSource::insert_macro_value is called for '" + filename_ + "', but the shader doesn't have any macro named " + macroName);
			return;
		}
#endif
		size_t macroEnd = m_src.find('\n', macroPos);

		std::stringstream sstream;
		sstream << m_src.substr(0, macroPos + strlen("#define ") + macroName.length());
		sstream << ' ' << value << m_src.substr(macroEnd);
		m_src = sstream.str();
	}

private:
	std::string m_filename = "Unnamed shader";
	std::string m_path;
	std::string m_src;
};

struct ShaderAttributeInfo
{
	unsigned typeId;
	unsigned type;
	int numType;
	std::string name;
	int location;
};

struct Uniform 
{ 
	int location = -1; 
	unsigned programId = 0; 
};

static_assert(sizeof(Uniform) == 8, "Uniform size changed!!!");
inline bool operator==(const Uniform& Left, const Uniform& Right) noexcept { return Left.location == Right.location && Left.programId == Right.programId; }

//=============================================================================
// Buffer Core
//=============================================================================

struct VertexAttribute
{
	unsigned location/* = -1*/;  // если -1, то берется индекс массива атрибутов
	int size;
	//unsigned type;
	bool normalized;
	int stride;         // sizeof Vertex
	const void* offset; // (void*)offsetof(Vertex, TexCoord)}
};

//=============================================================================
// Texture Core
//=============================================================================

struct Texture2DInfo
{
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

//=============================================================================
// Render Resource OpenGL Object
//=============================================================================
// TODO: в будущем, если решу делать классы ресурсов, то вот эти ресурсы включать в них, как OpenGL обертки

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

class GPUBuffer : public glObject
{
public:
	GPUBuffer() = delete;
	GPUBuffer(GLuint handle, BufferType Type, BufferUsage Usage, unsigned Count, unsigned Size) : type(Type), usage(Usage), count(Count), size(Size) { m_handle = handle; m_ownership = false; }
	GPUBuffer(BufferType Type, BufferUsage Usage, unsigned Count, unsigned Size) : type(Type), usage(Usage), count(Count), size(Size) { glGenBuffers(1, &m_handle); m_ownership = true; }
	GPUBuffer(GPUBuffer&&) noexcept = default;
	GPUBuffer(const GPUBuffer&) = delete;
	~GPUBuffer() { if( m_ownership ) glDeleteBuffers(1, &m_handle); }
	GPUBuffer& operator=(GPUBuffer&&) noexcept = default;
	GPUBuffer& operator=(const GPUBuffer&) = delete;

	bool operator==(const GPUBuffer& ref) noexcept { return m_handle == ref.m_handle &&  m_ownership == ref.m_ownership &&  usage == ref.usage && count == ref.count && size == ref.size && type == ref.type; }

	BufferType type = BufferType::ArrayBuffer;
	BufferUsage usage = BufferUsage::StaticDraw;
	unsigned count = 0;
	unsigned size = 0; 
};
using GPUBufferRef = std::shared_ptr<GPUBuffer>;

static_assert(sizeof(GPUBuffer) == 20, "GPUBuffer size changed!!!");
inline bool operator==(GPUBufferRef Left, GPUBufferRef Right) noexcept { return *Left == *Right; }

class VertexArray : public glObject
{
public:
	VertexArray() = delete;
	VertexArray(GLuint handle, GPUBufferRef Vbo, GPUBufferRef Ibo, unsigned AttribsCount) : vbo(Vbo), ibo(Ibo), attribsCount(AttribsCount) { m_handle = handle; m_ownership = false; }
	VertexArray(GPUBufferRef Vbo, GPUBufferRef Ibo, unsigned AttribsCount) : vbo(Vbo), ibo(Ibo), attribsCount(AttribsCount) { glGenVertexArrays(1, &m_handle); m_ownership = true; }
	VertexArray(VertexArray&&) = default;
	VertexArray(const VertexArray&) = delete;
	~VertexArray() { if( m_ownership ) glDeleteVertexArrays(1, &m_handle); }
	VertexArray& operator=(VertexArray&&) = default;
	VertexArray& operator=(const VertexArray&) = delete;

	bool operator==(const VertexArray& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && vbo == ref.vbo && ibo == ref.ibo && attribsCount == ref.attribsCount; }

	GPUBufferRef vbo = nullptr;
	GPUBufferRef ibo = nullptr;
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

	GPUBufferRef GetVBO() { return vao->vbo; }
	GPUBufferRef GetIBO() { return vao->ibo; }

	VertexArrayRef vao = nullptr;
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