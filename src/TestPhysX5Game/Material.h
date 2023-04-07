#pragma once

#include "Texture.h"

struct Material2
{
	Texture2 *Diffuse = nullptr;
	Texture2 *Emissive = nullptr;

	void Bind(Texture2 &diffuseFallback, Texture2 &emissiveFallback) const
	{
		(Diffuse ? *Diffuse : diffuseFallback).Bind(0);
		(Emissive ? *Emissive : emissiveFallback).Bind(1);
	}
};