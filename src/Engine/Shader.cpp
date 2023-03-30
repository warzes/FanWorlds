#include "stdafx.h"
#include "Shader.h"

такой стиль - https://sketchfab.com/3d-models/disciples-2-diorama-9410ae51d5654ece968b9fcd21b4a84b

dwSampleFramework
unrimp
haru
https ://open.gl
https://github.com/OpenGL-Graphics/opengl-utils
https://github.com/OpenGL-Graphics/first-person-shooter
PawsForAdventure
https ://discord.com/channels/794280341149712404/1042555400844750858

https://github.com/KTStephano/StratusGFX
https://ktstephano.github.io/




fixed grid

мир разделен на клетки
центры объектов прописываются в ячейки
проверки идут внутри сетки и по границам.перебор каждой клетки
при этом можно не проверять ячейки на границах(нулевые)


for( int x = 1; x < gridWidth - 1; x++ )
{
	for( int y = 1; y < gridHeight - 1; y++ )
	{
		auto& currentCell = grid[x][y];
		for( int dx = -1; dx <= 1; dx++ )
		{
			for( int dy = -1; dy <= 1; dy++ )
			{
				auto& otherCell = grid[x + dx][y + dy];
				CheckCells(currentCell, otherCell);
			}
		}
	}
}

void CheckCells(Cell & cell1, Cell & cell2)
{
	for( auto& objIdx1 : cell1.objects )
	{
		for( auto& objIdx2 : cell2.objects )
		{
			if( objIdx1 != objIdx2 )
			{
				...
			}
		}
	}
}

А еще эту сетку можно разделить на поток.То есть для двух потоков будет две сетки, для четырех - четыре
Чтобы не было конфликтов потоков, можно счделать так

пусть у нас два потока.делим сетку на две сетки
теперь берем первый поток, и снова делим стеку на две сетки(тоже и для второго)
теперь в обоих потоках сначала обрабатываем первую часть сетки.а потом вторую.тогда одновременно между двумя потоками не будет общих ячеек(видео 10.50)


идеи
https ://www.youtube.com/watch?v=9IULfQH7E90


как генерить острова
алгоритм пьяницы(animated drunken umber hulks)

берется точка, ей дается кол - во шагов.и она начинает ходить в любых направлениях.когда закончит - берется еще одна точка внутри открытой зоны - повторяется.
в итоге получаются неплохие острова

