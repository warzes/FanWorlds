===============================================================================
OpenGL
===============================================================================
- ResourceUsage � ���� �� ������ ����������. �� ����� ���� ��� ��� ���� ������ ��� DRAW/READ/COPY, ������ STATIC/DYNAMIC/STREAM

- ������� GeometryBuffer - ������ VertexArray - �� ���� �������� ����� ������� ��� ������� �� ���� ������

- Storage Buffer - http://steps3d.narod.ru/tutorials/buffer-storage-tutorial.html

- ����������� �������
	glBindBuffer(GL_COPY_READ_BUFFER, SourceBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, DestBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, SrcOffset, DstOffset, NumBytes);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

===============================================================================
OpenGL
===============================================================================
Material � Mesh ������ ����������, ������ ������. �������� ����� �� �� ������ ���� ������� ����� ������, �������� MeshLibrary - � ���� ������ ����������� �� ��������� ������� ��� � �����, ��� ���� ������ ���������� ���������� - ��� ����� ���� �������.
������� ����� Copy ��� ����������� ���� ��������