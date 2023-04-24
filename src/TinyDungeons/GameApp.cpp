#include "stdafx.h"
#include "GameApp.h"


шутер в стиле
https ://youdoyoubuddy.itch.io/lead-haul

использовать тайл
https ://little-martian.itch.io/retro-texture-pack
https://merchant-shade.itch.io/16x16-mini-world-sprites
перерисовать:
https://brysiaa.itch.io/pixel-tileset-grass-island



повторить одну из этмх
https ://youdoyoubuddy.itch.io/lead-haul
https://freds72.itch.io/poom


Может это
https ://github.com/ScrappyCocco/ScrapEngine
физика - реакт

тайлы отсюда
https ://github.com/jdah/microcraft


повторить это
вода и трава - https ://github.com/leaf3d/leaf3d
	небо - https ://github.com/shff/opengl_sky

	https ://github.com/staniks/worship-mirror











//план
//- посмотреть редактор - https://www.youtube.com/watch?v=GukhptdHlPk
// по кубо-редактору
//	https://www.youtube.com/watch?v=GfInoloaQJs
// и сцена под него - https://lettier.github.io/3d-game-shaders-for-beginners/normal-mapping.html
//начало идеи
//	https://www.youtube.com/watch?v=tAr8zYDdI8s
//	процесс - https://www.youtube.com/watch?v=CZMXE2n0vNg&t=1399s
//
//визуал
//https://www.youtube.com/watch?v=SLs_TgisxI0
//https://rgsdev.itch.io/free-3d-modular-low-poly-assets-for-prototyping-by-rgsdev
//https://rgsdev.itch.io/free-isometric-dungeon-game-tileset-2d-asset-by-rgsdev
//https://kaylousberg.itch.io/kaykit-dungeon-remastered
//https://kaylousberg.itch.io/kaykit-dungeon
//https://kaylousberg.itch.io/kaykit-medieval-builder-pack
//https://rgsdev.itch.io/free-modular-low-poly-dungeon-pack-by-rgsdev - красиво
//https://elegantcrow.itch.io/retro-modular-house-pack
//https://rgsdev.itch.io/nature-pack-low-poly-by-rgsdev
//https://chilly-durango.itch.io/medieval-building-parts
//https://maxparata.itch.io/rocky-mountains
//https://free-game-assets.itch.io/free-environment-props-3d-low-poly-pack
//https://fertile-soil-productions.itch.io/modular-platformer
//https://devilsworkshop.itch.io/essential-low-poly-isometric-3d-block-and-hex-pack
//https://essssam.itch.io/3d-leap-land
//https://free-game-assets.itch.io/free-shrubs-flowers-and-mushrooms-3d-low-poly-pack
//https://maxparata.itch.io/monogon-dungeon
//https://devilsworkshop.itch.io/isometric-tiles-pixel-art
//https://jestan.itch.io/fantasy-textures
//https://jestan.itch.io/pixel-texture-pack
//https://devilsworkshop.itch.io/big-pixel-isometric-block-pack-free-2d-sprites
//https://devilsworkshop.itch.io/low-poly-3d-and-pixel-2d-rpg-game-assets
//https://www.kenney.nl/assets/tiny-dungeon
//https://www.kenney.nl/assets/isometric-miniature-prototype
//https://zeromatrix.itch.io/dungeoninmypocket
//
//разное
//	шейдер - https://www.youtube.com/watch?v=yi0uzdXcIIU
//	генерация данжа из тайлов - https://www.youtube.com/watch?v=DRJ1a9Xpb9Y
//	куболестница в блендере - https://www.youtube.com/watch?v=eO-e6jjfkmc
//	https://www.youtube.com/watch?v=hK5N_dTDMPw
//	стиль персонажа - https://www.youtube.com/playlist?list=PLhEFRhEnz05sfcqDU_stZR2gxoP2cTzj-
//	https://www.youtube.com/playlist?list=PLob6hbUtmU2PFqWMQr8AT10Qbk_7kiRVf
//	https://www.youtube.com/watch?v=NbzuHAtOrQw
//	https://www.youtube.com/watch?v=RQVAUaSUP-k&t=4s
//-----------------------------------------------------------------------------
bool GameApp::Create()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	m_gameStateTest.InitFromOther(this);
	m_editorState.InitFromOther(this);
	m_gameStateManager.InitFromOther(this);
	m_gameStateManager.SetActiveState(&m_editorState);

	return true;
}
//-----------------------------------------------------------------------------
void GameApp::Destroy()
{
	m_editorState.Destroy();
	m_gameStateTest.Destroy();
	GetInput().SetMouseLock(false);
}
//-----------------------------------------------------------------------------
void GameApp::Render()
{
	m_gameStateManager.Frame();
}
//-----------------------------------------------------------------------------
void GameApp::Update(float deltaTime)
{
	m_gameStateManager.Update(deltaTime);
}
//-----------------------------------------------------------------------------