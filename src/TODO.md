===============================================================================
OpenGL
===============================================================================
- ResourceUsage � ���� �� ������ ����������. �� ����� ���� ��� ��� ���� ������ ��� DRAW/READ/COPY, ������ STATIC/DYNAMIC/STREAM

- ����������� �������
	glBindBuffer(GL_COPY_READ_BUFFER, SourceBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, DestBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, SrcOffset, DstOffset, NumBytes);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);