#include "stdafx.h"
#include "Shader.h"

����� ����� - https://sketchfab.com/3d-models/disciples-2-diorama-9410ae51d5654ece968b9fcd21b4a84b

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

��� �������� �� ������
������ �������� ������������� � ������
�������� ���� ������ ����� � �� ��������.������� ������ ������
��� ���� ����� �� ��������� ������ �� ��������(�������)


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

� ��� ��� ����� ����� ��������� �� �����.�� ���� ��� ���� ������� ����� ��� �����, ��� ������� - ������
����� �� ���� ���������� �������, ����� �������� ���

����� � ��� ��� ������.����� ����� �� ��� �����
������ ����� ������ �����, � ����� ����� ����� �� ��� �����(���� � ��� �������)
������ � ����� ������� ������� ������������ ������ ����� �����.� ����� ������.����� ������������ ����� ����� �������� �� ����� ����� �����(����� 10.50)


����
https ://www.youtube.com/watch?v=9IULfQH7E90


��� �������� �������
�������� �������(animated drunken umber hulks)

������� �����, �� ������ ��� - �� �����.� ��� �������� ������ � ����� ������������.����� �������� - ������� ��� ���� ����� ������ �������� ���� - �����������.
� ����� ���������� �������� �������

