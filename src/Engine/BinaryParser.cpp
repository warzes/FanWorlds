#include "stdafx.h"
#include "BinaryParser.h"
#include "FileSystem.h"

BinaryParser::BinaryParser(SystemRef& systemRef, const std::string &filename)
	: m_systemRef(systemRef)
	, m_binary(systemRef.GetFileSystem().Read(filename))
	, m_current(m_binary.data())
	, m_remainingBytes(m_binary.size())
{
}

bool BinaryParser::ReadBytes(size_t numBytes, void *output)
{
	if( m_remainingBytes < numBytes )
	{
		m_systemRef.Error("Not enough data in the binary file.");
		return false;
	}
	memcpy(output, m_current, numBytes);
	m_current += numBytes;
	m_remainingBytes -= numBytes;
	return true;
}