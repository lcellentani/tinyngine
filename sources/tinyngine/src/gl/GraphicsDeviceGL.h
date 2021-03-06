#pragma once

#include "GraphicsDevice.h"

namespace tinyngine
{

class GraphicsDeviceGL : public GraphicsDevice {
public:
	GraphicsDeviceGL();
	virtual ~GraphicsDeviceGL();

	void Commit() override;

	void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

	void Clear(uint8_t flags, Color color, float depth = 1.0f, uint8_t stencil = 0) override;
	void SetColorMake(bool red, bool green, bool blue, bool alpha) override;
	void SetDepthMask(bool flag) override;
	void SetStencilMask(uint32_t mask) override;

	void SetState(RendererStateType::Enum type, bool value) override;
	void SetCullMode(CullFaceModes::Enum mode) override;
	void SetWinding(WindingModes::Enum mode) override;
	void SetBlendFunc(BlendFuncs::Enum sfactor, BlendFuncs::Enum dfactor) override;
	void SetDepthFunc(DepthFuncs::Enum func) override;
	void SetStencilFunc(StencilFuncs::Enum func, int32_t ref, uint32_t mask) override;
	void SetStencilOp(StencilOpTypes::Enum sfail, StencilOpTypes::Enum dpfail, StencilOpTypes::Enum dppass) override;
	void SetBlendColor(Color color) override;
	void SetPolygonffset(float factor, float units) override;

	void DrawArray(PrimitiveType::Enum primitive, uint32_t first, uint32_t count) override;
	void DrawElements(PrimitiveType::Enum primitive, uint32_t count) override;

	VertexBufferHandle CreateVertexBuffer(const void* data, uint32_t size, const VertexFormat& vertexFormat) override;
	void SetVertexBuffer(const VertexBufferHandle& handle, Attributes::Enum attribute) override;

	IndexBufferHandle CreateIndexBuffer(const void* data, uint32_t size) override;
	void SetIndexBuffer(const IndexBufferHandle& handle) override;

	ShaderHandle CreateShader(ShaderType::Enum type, const char* source) override;

	ProgramHandle CreateProgram(ShaderHandle& vertexShaderHandle, ShaderHandle& fragmentShaderHandle, bool destroyShaders) override;
	void SetProgram(const ProgramHandle& handle, const VertexFormat& vertexFormat) override;

	UniformHandle GetUniform(const ProgramHandle& handle, const char* uniformName) const override;
	void setUniform1i(const ProgramHandle& programHandle, UniformHandle& uniformHandle, int32_t data) override;
	void SetUniformFloat(const ProgramHandle& programHandle, UniformHandle& uniformHandle, float data) override;
	void SetUniformFloat3(const ProgramHandle& programHandle, UniformHandle& uniformHandle, const float* data) override;
	void SetUniformMat4(const ProgramHandle& handle, const UniformHandle& uniform, const float* data, bool transpose) override;
	
	TextureHandle CreateTexture2D(const ImageHandle& imageHandle, ImageManager& imageManager, TextureFormats::Enum format, TextureFilteringMode::Enum filtering, bool useMipmaps) override;
	void SetTexture(uint32_t stage, const TextureHandle& textureHandle) override;

private:
	struct Impl;
	Impl* mImpl;
};

/*
enum class ClearFlags : uint8_t {
	None = 0,
	Color = 1 << 0,
	Depth = 1 << 1,
	Stencil = 1 << 2
};

inline constexpr ClearFlags operator&(ClearFlags x, ClearFlags y) {
	return static_cast<ClearFlags>(static_cast<int>(x) & static_cast<int>(y));
}

inline constexpr ClearFlags operator|(ClearFlags x, ClearFlags y) {
	return static_cast<ClearFlags>(static_cast<int>(x) | static_cast<int>(y));
}

inline constexpr ClearFlags operator^(ClearFlags x, ClearFlags y) {
	return static_cast<ClearFlags>(static_cast<int>(x) ^ static_cast<int>(y));
}

inline constexpr ClearFlags operator~(ClearFlags x) {
	return static_cast<ClearFlags>(~static_cast<int>(x));
}

inline ClearFlags& operator&=(ClearFlags & x, ClearFlags y) {
	x = x & y;
	return x;
}

inline ClearFlags& operator|=(ClearFlags & x, ClearFlags y) {
	x = x | y;
	return x;
}

inline ClearFlags& operator^=(ClearFlags & x, ClearFlags y) {
	x = x ^ y;
	return x;
}

struct ResourceHandle {
	ResourceHandle() {};
	ResourceHandle(uint32_t handle) : mHandle(handle) {}
	inline const bool IsValid() const { return mHandle != 0; }
	uint32_t mHandle = 0;
};

using ShaderHandle = ResourceHandle;
using ProgramHandle = ResourceHandle;
using VertexBufferHandle = ResourceHandle;

class Renderer {
public:
	static Renderer* GetRenderer();

public:
	Renderer();
	~Renderer();

	Renderer(Renderer& rhs);
	Renderer& operator=(Renderer& rhs);

	Renderer(Renderer&& rhs) noexcept = default;
	Renderer& operator= (Renderer&& rhs) noexcept = default;

public:
	void BeginFrame();
	void EndFrame();

	void SetViewClear(ClearFlags flags, Color rgba, float depth = 1.0f, uint8_t stencil = 0);
	void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	ShaderHandle CreateShader(uint32_t type, const char* source);
	ProgramHandle CreateProgram(const ShaderHandle& vsh, const ShaderHandle& fsh, bool destroyShaders);
	
	void SetProgramUniform(const ProgramHandle& handle);

	VertexBufferHandle CreateVertexBuffer(const void* data, uint32_t size);

	void SetProgram(const ProgramHandle& handle);
	void SetVertexBuffer(const VertexBufferHandle& handle);

	void DrawArray(uint32_t first, uint32_t count);

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};
*/
} // namespace tinyngine