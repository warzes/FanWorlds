#pragma once

#include "Texture.h"

struct Material
{
	Texture *Diffuse = nullptr;
	Texture *Emissive = nullptr;

	void Bind(Texture &diffuseFallback, Texture &emissiveFallback) const
	{
		(Diffuse ? *Diffuse : diffuseFallback).Bind(0);
		(Emissive ? *Emissive : emissiveFallback).Bind(1);
	}
};