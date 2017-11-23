#include "Application.h"
#include "GraphicsDevice.h"
#include "ImageLoader.h"
#include "MeshLoader.h"
#include "VertexFormat.h"
#include "TransformHelper.h"
#include "StringUtils.h"
#include "Log.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

using namespace tinyngine;

class MultiTexture : public Application {
public:
	MultiTexture() = default;
	virtual ~MultiTexture() = default;

	ContextAttribs& GetContextAttribs() override {
		static ContextAttribs sAttributes;

		sAttributes.mRequiredApi = Api::OpenGLES2;
		sAttributes.mDepthBPP = 32;
		sAttributes.mStencilBPP = 0;
		sAttributes.mRedBits = 8;
		sAttributes.mGreenBits = 8;
		sAttributes.mBlueBits = 8;
		sAttributes.mAlphaBits = 8;

		return sAttributes;
	}

	void InitApplication() override {

	}

	void InitView(Engine& engine, uint32_t windowWidth, uint32_t windowHeight) override {
		GraphicsDevice& graphicsDevice = engine.GetSystem<GraphicsDevice>();
		MeshLoader& meshLoader = engine.GetSystem<MeshLoader>();
		std::vector<MeshInfo> allMeshes = meshLoader.LoadObj("models/Cube.obj");
		if (allMeshes.size() == 0) {
			return;
		}
		auto& mesh = allMeshes[0];
		uint32_t numVertices = mesh.mNumVertices;
		mNumIndices = mesh.mNumIndices;

		mColors.reserve(numVertices * 4);
		for (uint32_t i = 0; i < numVertices * 4; i += 4) {
			mColors.push_back(255);
			mColors.push_back(255);
			mColors.push_back(255);
			mColors.push_back(255);
		}

		mPosVertexFormat.Add(Attributes::Position, AttributeType::Float, 3, false);
		//mPosVertexFormat.Add(Attributes::Normal, AttributeType::Float, 3, false);
		mPosVertexFormat.Add(Attributes::Color0, AttributeType::Uint8, 4, true);

		mPositionsHandle = graphicsDevice.CreateVertexBuffer(&mesh.mPositions[0], sizeof(mesh.mPositions[0]) * numVertices * 3, mPosVertexFormat);
		//mNornalsHandle = graphicsDevice.CreateVertexBuffer(&mesh.mNormals[0], sizeof(mesh.mNormals[0]) * numVertices * 3, mPosVertexFormat);
		mColorsHandle = graphicsDevice.CreateVertexBuffer(&mColors[0], sizeof(mColors[0]) * mColors.size() * 4, mPosVertexFormat);
		mIndexesBufferHandle = graphicsDevice.CreateIndexBuffer(&mesh.mIndices[0], sizeof(mesh.mIndices[0]) * mNumIndices);

		std::string vertexShaderSource;
		StringUtils::ReadFileToString("shaders/multitexture_vert_2.glsl", vertexShaderSource);
		std::string fragmentShaderSource;
		StringUtils::ReadFileToString("shaders/multitexture_frag_2.glsl", fragmentShaderSource);

		ShaderHandle vsHandle = graphicsDevice.CreateShader(ShaderType::VertexProgram, vertexShaderSource.c_str());
		ShaderHandle fsHandle = graphicsDevice.CreateShader(ShaderType::FragmentProgram, fragmentShaderSource.c_str());
		mProgramHandle = graphicsDevice.CreateProgram(vsHandle, fsHandle, true);
		mModelViewProjHandle = graphicsDevice.GetUniform(mProgramHandle, "u_modelViewProj");

		ImageLoader& imageLoader = engine.GetSystem<ImageLoader>();
		ImageData imageData;
		imageLoader.Load("textures/woodenbox.png", imageData);

		float ratio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
		mProj = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 100.0f);
		mView = glm::lookAt(glm::vec3(-2.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mUp = glm::vec3(0.0f, 1.0f, 0.0f);
		mRight = glm::vec3(1.0f, 0.0f, 0.0f);
		mAngles = glm::vec3(0.0f, 0.0f, 0.0f);

		mTransformHelper.SetMatrixMode(TransformHelper::MatrixMode::Projection);;
		mTransformHelper.LoadMatrix(mProj);
		mTransformHelper.SetMatrixMode(TransformHelper::MatrixMode::View);
		mTransformHelper.LoadMatrix(mView);

		graphicsDevice.SetState(RendererStateType::CullFace, true);
		graphicsDevice.SetState(RendererStateType::DepthTest, true);

		mInitialized = true;
	}

	void RenderFrame(Engine& engine) override {
		if (!mInitialized) { return; }

		GraphicsDevice& graphicsDevice = engine.GetSystem<GraphicsDevice>();
		mAngles.x += mSpeed.x;
		if (mAngles.x > 360.0f) {
			mAngles.x -= 360.0f;
		}
		mAngles.y += mSpeed.y;
		if (mAngles.y > 360.0f) {
			mAngles.y -= 360.0f;
		}

		mTransformHelper.SetMatrixMode(TransformHelper::MatrixMode::Model);
		mTransformHelper.LoadIdentity();
		mTransformHelper.Rotate(mAngles.y, mUp);
		mTransformHelper.Rotate(-mAngles.x, mRight);

		graphicsDevice.Clear(GraphicsDevice::ColorBuffer | GraphicsDevice::DepthBuffer, Color(92, 92, 92), 1.0f);

		graphicsDevice.SetVertexBuffer(mPositionsHandle, Attributes::Position);
		//graphicsDevice.SetVertexBuffer(mNornalsHandle, Attributes::Normal);
		graphicsDevice.SetVertexBuffer(mColorsHandle, Attributes::Color0);

		graphicsDevice.SetProgram(mProgramHandle, mPosVertexFormat);
		graphicsDevice.SetUniformMat4(mProgramHandle, mModelViewProjHandle, &mTransformHelper.GetModelViewProjectionMatrix()[0][0], false);

		graphicsDevice.SetIndexBuffer(mIndexesBufferHandle);
		graphicsDevice.DrawElements(PrimitiveType::Triangles, mNumIndices);

		graphicsDevice.Commit();
	}

	void ReleaseView(Engine&) override {
	}

	void ReleaseApplication() override {

	}

private:
	/*bool CreateSimpleTexture() {
		const GLubyte pixels[4 * 3]{
			255, 0, 0,
			0, 255, 0,
			0, 0, 255,
			255, 255, 0
		};

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &mTextureId);
		glBindTexture(GL_TEXTURE_2D, mTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		return true;
	}

	bool InitializeShaders() {
		const char* fragmentShaderSource = SHADER_SOURCE
		(
			precision mediump float;
			varying vec2 v_texcoord;
			uniform sampler2D u_texture;
			void main (void)
			{
				
				gl_FragColor = texture2D(u_texture, v_texcoord);
			}
		);
		const char* vertexShaderSource = SHADER_SOURCE
		(
			attribute highp vec4 a_position;
			attribute lowp vec2 a_texcoord;
			varying lowp vec2 v_texcoord;
			void main(void)
			{
				gl_Position = a_position;
				v_texcoord = a_texcoord;
			}
		);
		mShaderProgram = CompileProgram(vertexShaderSource, fragmentShaderSource, [](GLenum type, const char * errorMessage) {
			if (errorMessage) {
				if (type == GL_VERTEX_SHADER) {
					Log(Logger::Error, "Failed compile vertex shader : %s", errorMessage);
				}
				else if (type == GL_FRAGMENT_SHADER) {
					Log(Logger::Error, "Failed compile fragment shader : %s", errorMessage);
				}
				else {
					Log(Logger::Error, "Failed compile ling program : %s", errorMessage);
				}
			}
		}); 
		if (mShaderProgram == 0) {
			return false;
		}

		glUseProgram(mShaderProgram);
		GLenum lastError = glGetError();
		if (lastError != GL_NO_ERROR) { return false; }
		mPositionAttributePos = glGetAttribLocation(mShaderProgram, "a_position");
		mTexcoordAttributePos = glGetAttribLocation(mShaderProgram, "a_texcoord");
		mSamplerPos = glGetUniformLocation(mShaderProgram, "s_texture");

		return true;
	}*/

private:
	VertexFormat mPosVertexFormat;
	ProgramHandle mProgramHandle;

	VertexBufferHandle mPositionsHandle;
	VertexBufferHandle mNornalsHandle;
	VertexBufferHandle mColorsHandle;
	IndexBufferHandle mIndexesBufferHandle;
	TextureHandle mPrimaryTextureHandle;
	TextureHandle mSecondaryTextureHandle;

	UniformHandle mModelViewProjHandle;

	bool mInitialized = false;
	uint32_t mNumIndices = 0;
	std::vector<uint8_t> mColors;

	TransformHelper mTransformHelper;

	glm::vec3 mSpeed{ 2.f, 1.f, 0.0f };
	glm::vec3 mAngles;
	glm::mat4 mProj;
	glm::mat4 mView;
	glm::vec3 mRight;
	glm::vec3 mUp;
};


extern "C" tinyngine::Application * CreateApplication() {
	return new MultiTexture();
}