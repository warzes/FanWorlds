===============================================================================
OpenGL
===============================================================================
- ResourceUsage у мен€ не совсем правильный. Ќа самом деле это две вещи перва€ это DRAW/READ/COPY, втора€ STATIC/DYNAMIC/STREAM

- копирование буферов
	glBindBuffer(GL_COPY_READ_BUFFER, SourceBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, DestBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, SrcOffset, DstOffset, NumBytes);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);