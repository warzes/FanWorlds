#include "stdafx.h"
#include "002_Quad.h"

тут доделать против часовой стрелки


https ://www.materialmaker.org
https://www.dhpoware.com/demos/glslAttenuation.html

как делать

три части
обточка движка - продолжаю
обточка тайлового редактора - пока что хватит базы - тайлы из рпг мейкера
обточка геймплея - под это сделать арену где тестить разные механики
идея - сначала реализовать базовую часть механик, потом уже собирать уровень






по тайловости
можно не делать под него редактор.А по другому - тайлы уже готовы, разных форм - я выбираю нужные и ставлю по сетке

сетка
- чтобы было проще - сетка примораживается к оси y.первоначально на 0 координату.с помощью колеса мыши можно поднять высоту по y.а если зажать контр и колесо - то можно менять размер ячеек.Тайлы ставятся строго к ячейке - центрируя на указатель
если я выбрал тайл - то с помощью колеса я могу вращать(если зажать XYZ - то вращать по соот оси)


первые тайлы взять из rpg in box

Вот в блендере рисование данжа
https ://www.youtube.com/watch?v=rIatfICEzDE
(подходит к тайловости)


В блендере можно юзать тот модуль для тайловой карты


вода
https ://www.youtube.com/watch?v=qgDPSnZPGMA&t=16s




текстуры можно рисовать тут - https ://www.materialmaker.org


	здесь есть советы по блендеру а также интересный атлас из цветов для текстуринга лоуполи
	https ://www.youtube.com/watch?v=2Kzo68liq78

пример воксельной местности
https ://www.youtube.com/watch?v=yKO1Q5QRZic
https://assetstore.unity.com/packages/3d/environments/dungeons/voxel-dungeons-212790




https://github.com/OpenGL-Graphics/first-person-shooter
RuinIsland_GLSL_Demo


в тредбуме сделать шаблонную карту котоую потом юзать как основу
то есть она будет указывать все границы.при этом сетка с нее будет игнорироваться движком(удалять не надо)

примеры тайлового редактора

https ://github.com/TheTophatDemon/Total-Editor-3
https://steamcommunity.com/sharedfiles/filedetails/?l=norwegian&id=2075248129
https://github.com/jdah/microcraft - визуал (то есть такое же в 3д


роцесс такой - первоначально мир пуст.игрок встает на точку и создает первую зону.от нее и строит остальное

мир разделен на клетки.клетка равна 1 метру.
система координат по x и z начинается от 0 и до бесконечности.по y может быть отрицательно, а ноль - это уровень моря(отрицательно - это подземелье, положительно - небо)

клетки объединены в зоны.например по 100 на 100 клеток.

редактор работает в режимах
- режим зоны - позволяет добавлять зону(это нужно чтобы делать летающие острова или данжи - то есть несвязанное в пустоте)
- режим клетки - можно добавлять / удалять клетки в текущей зоне(если добавить клетку на границе - создаст новую зону)
выглядит так - клетка, на которую смотрит камера - подсвечивается.если там нет реальной клетки то рисуется полупрозрачная и вот ее можно ставить
- также в режиме клеток можно менять высоту вверха и низа клетки
- режим вершин - позволяет выбирать одну из 8 вершин клетки и модифицировать ее высоту


Чтобы экономить память
- регионы сейчас могут быть равны 100 х 100 х 100. и всего десяток регионов.
но внутри себя регионы не содержат трехмерный массив.а хранят массив клеток у которых своя позиция.
Тогда будет не 100х100х100 клеток.а много меньше.
Когда добавляется новая клетка - то нужно обходить каждую из имеющихся.
также будут оптимизиации - например клетка окруженная другими будет удаляться


https ://docs.gl/gl3/glActiveTexture
Выбрать список - пройти по каждому

http ://www.hyzgame.org.cn/OpenGL/man3/bottom.php
https://registry.khronos.org/OpenGL/specs/gl/glspec33.core.pdf
https://github.com/mini3d/mini3d_physics



//-----------------------------------------------------------------------------
bool _002Quad::Create()
{
	Print("002_Quad Create");

	const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 projectionMatrix;

out vec3 fragmentColor;

void main()
{
	gl_Position   = projectionMatrix * vec4(position, 1.0);
	fragmentColor = color;
}
)";

	const char* fragmentShaderText = R"(
#version 330 core

in vec3 fragmentColor;
out vec4 color;

void main()
{
	color = vec4(fragmentColor, 1.0);
}
)";

	struct testVertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	}
	vert[] =
	{
		{{  0.5f,  0.5f, 4.0f}, {1.0f, 0.0f, 0.0f}}, // top right
		{{  0.5f, -0.5f, 4.0f}, {0.0f, 1.0f, 0.0f}}, // bottom right
		{{ -0.5f, -0.5f, 4.0f}, {0.0f, 0.0f, 1.0f}}, // bottom left
		{{ -0.5f,  0.5f, 4.0f}, {0.0f, 1.0f, 1.0f}}, // top left
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	//glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");

	m_geom = renderSystem.CreateGeometryBuffer(ResourceUsage::Static, Countof(vert), sizeof(testVertex), vert, Countof(indices), IndexBufferFormat::Uint32, indices, m_shader);

	return true;
}
//-----------------------------------------------------------------------------
void _002Quad::Destroy()
{
	m_shader.reset();
	m_geom.reset();

	Print("002_Quad Destroy");
}
//-----------------------------------------------------------------------------
void _002Quad::Render()
{
	auto& renderSystem = GetRenderSystem();

	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	renderSystem.Clear();
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------
void _002Quad::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}
}
//-----------------------------------------------------------------------------