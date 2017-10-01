#include "Application.h"
#include "GeometryUtil.h"
#include "VertexFormat.h"
#include "TransformHelper.h"
#include "Log.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace tinyngine;

class VertexShader : public Application {
public:
	VertexShader() = default;
	virtual ~VertexShader() {

	}

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

	void InitView(std::unique_ptr<Renderer>& renderer) override {
		GenerateCube(1.0f, mCube);
		mColors.reserve(mCube.numVertices * 4);
		for (uint32_t i = 0; i < mCube.numVertices * 4; i += 4) {
			mColors.push_back(255);
			mColors.push_back(255);
			mColors.push_back(0);
			mColors.push_back(255);
		}

		mPosVertexFormat.Add(Attributes::Position, AttributeType::Float, 3, false);
		mPosVertexFormat.Add(Attributes::Color0, AttributeType::Uint8, 4, true);

		mPositionsHandle = renderer->CreateVertexBuffer(&mCube.positions[0], sizeof(mCube.positions[0]) * mCube.numVertices * 3, mPosVertexFormat);
		mColorsHandle = renderer->CreateVertexBuffer(&mColors[0], sizeof(mColors[0]) * mColors.size() * 4, mPosVertexFormat);
		mIndexesBufferHandle = renderer->CreateIndexBuffer(&mCube.indices[0], sizeof(mCube.indices[0]) * mCube.numIndices);

		const char* fragmentShaderSource = SHADER_SOURCE
		(
			varying lowp vec4 v_color;
			void main(void)
			{
				gl_FragColor = v_color;
			}
		);
		const char* vertexShaderSource = SHADER_SOURCE
		(
			attribute highp vec4 a_position;
			attribute lowp vec4 a_color0;
			uniform mediump mat4 u_modelViewProj;
			varying lowp vec4 v_color;
			void main(void)
			{
				gl_Position = u_modelViewProj * a_position;
				v_color = a_color0;
			}
		);
		ShaderHandle vsHandle = renderer->CreateShader(ShaderType::VertexProgram, vertexShaderSource);
		ShaderHandle fsHandle = renderer->CreateShader(ShaderType::FragmentProgram, fragmentShaderSource);
		mProgramHandle = renderer->CreateProgram(vsHandle, fsHandle, true);
		mModelViewProjHandle = renderer->GetUniform(mProgramHandle, "u_modelViewProj");

		mProj = glm::perspective(glm::radians(60.0f), mAspect, 0.1f, 100.0f);
		mView = glm::lookAt(glm::vec3(-2.0f, 2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mUp = glm::vec3(0.0f, 1.0f, 0.0f);
		mRight = glm::vec3(1.0f, 0.0f, 0.0f);
		mAngles = glm::vec3(0.0f, 0.0f, 0.0f);

		mTransformHelper.SetMatrixMode(TransformHelper::MatrixMode::Projection);;
		mTransformHelper.LoadMatrix(mProj);
		mTransformHelper.SetMatrixMode(TransformHelper::MatrixMode::View);
		mTransformHelper.LoadMatrix(mView);

		renderer->SetState(RendererStateType::CullFace, true);
	}

	void RenderFrame(std::unique_ptr<Renderer>& renderer, float deltaTime) override {
		mAngles.x -= mSpeed.x * deltaTime;
		if (mAngles.x < 0.0f) {
			mAngles.x += 360.0f;
		}
		mAngles.y += mSpeed.y * deltaTime;
		if (mAngles.y > 360.0f) {
			mAngles.y -= 360.0f;
		}
		
		mTransformHelper.SetMatrixMode(TransformHelper::MatrixMode::Model);
		mTransformHelper.LoadIdentity();
		mTransformHelper.Rotate(mAngles.y, mUp);
		mTransformHelper.Rotate(-mAngles.x, mRight);

		renderer->SetViewport(0, 0, mWindowWidth, mWindowHeight);
		renderer->Clear(Renderer::ColorBuffer | Renderer::DepthBuffer, Color(92, 92, 92));

		renderer->SetVertexBuffer(mPositionsHandle, Attributes::Position);
		renderer->SetVertexBuffer(mColorsHandle, Attributes::Color0);

		renderer->SetProgram(mProgramHandle, mPosVertexFormat);
		renderer->SetUniformMat4(mProgramHandle, mModelViewProjHandle, &mTransformHelper.GetModelViewProjectionMatrix()[0][0], false);

		renderer->SetIndexBuffer(mIndexesBufferHandle);
		renderer->DrawElements(PrimitiveType::Triangles, mCube.numIndices);

		renderer->Commit();
	}

	void ReleaseView(std::unique_ptr<Renderer>& renderer) override {
		TINYNGINE_UNUSED(renderer);
	}

	void ReleaseApplication() override {

	}

	void OnReshape(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override {
		TINYNGINE_UNUSED(x); TINYNGINE_UNUSED(y);
		mWindowWidth = width;
		mWindowHeight = height;
		mAspect = (float)mWindowWidth / (float)mWindowHeight;
	}

private:
	uint32_t mWindowWidth;
	uint32_t mWindowHeight;
	float mAspect;

	CubeGeometry mCube;
	std::vector<uint8_t> mColors;

	VertexFormat mPosVertexFormat;
	ProgramHandle mProgramHandle;
	UniformHandle mModelViewProjHandle;
	VertexBufferHandle mPositionsHandle;
	VertexBufferHandle mColorsHandle;
	IndexBufferHandle mIndexesBufferHandle;

	TransformHelper mTransformHelper;

	glm::vec3 mSpeed{ 2.f, 1.f, 0.0f };
	glm::vec3 mAngles;
	glm::mat4 mProj;
	glm::mat4 mView;
	glm::vec3 mRight;
	glm::vec3 mUp;
};


extern "C" tinyngine::Application * CreateApplication() {
	return new VertexShader();
}