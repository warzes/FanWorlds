#pragma once

class CameraUtils : public BaseClass
{
public:
	void Init();
	void Close();
	void Update(float deltaTime);

	glm::mat4 GetPerspective() const { return m_perspective; }
	glm::mat4 GetView() const { return m_camera.GetViewMatrix(); }

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;
	Camera m_camera;
	glm::mat4 m_perspective;
};