#pragma once

class GameApp final : public IApp
{
	bool Create() final;
	void Destroy() final;
};