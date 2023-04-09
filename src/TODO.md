===============================================================================
OpenGL
===============================================================================
- ResourceUsage у меня не совсем правильный. На самом деле это две вещи первая это DRAW/READ/COPY, вторая STATIC/DYNAMIC/STREAM

- Удалить GeometryBuffer - хватит VertexArray - то есть добавить метод который его создает по всем данным

- Storage Buffer - http://steps3d.narod.ru/tutorials/buffer-storage-tutorial.html

- копирование буферов
	glBindBuffer(GL_COPY_READ_BUFFER, SourceBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, DestBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, SrcOffset, DstOffset, NumBytes);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);