#pragma once

class _001MonkeyScenes final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	void renderScene(ShaderProgramRef shader);
	void renderQuad();

	int m_windowWidth = 0;
	int m_windowHeight = 0;

	Texture2DRef shadowMapTexture, shadowMapDepthTexture, blurTexture;
	FramebufferRef shadowMapFbo, blurFbo;
	ShaderProgramRef simpleDepthShader, shader, blurShader, debugQuadShader;
	ModelRef m, plane;
	Texture2DRef t1, t2;
	Camera camera;

	ShaderProgramRef m_tempShader;
	Uniform m_tempuniformProj;
	Uniform m_tempuniformView;
	ShaderProgramRef shaderModel;
	Uniform uniformModelWorldMatrix;
	Uniform uniformModelViewMatrix;
	Uniform uniformModelProjectionMatrix;
	GeometryBufferRef m_geom;
	Texture2DRef m_texture;
	ModelRef m_model;
};