#pragma once

class _001Triangle final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float elapsedTimeSeconds) final;

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;
};