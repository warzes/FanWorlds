#pragma once

class GameApp final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float elapsedTimeSeconds) final;
};