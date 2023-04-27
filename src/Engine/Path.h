#pragma once

#include "Error.h"

class Path
{
public:
	enum class Format
	{
		generic,
		native
	};

#if defined(_WIN32)
	using Char = wchar_t;
	using String = std::wstring;
	static constexpr Char preferredSeparator = '\\';
	static constexpr const Char* parentDirectory = L"..";
	static constexpr const Char* currentDirectory = L".";
#elif defined(__unix__) || defined(__APPLE__)
	using Char = char;
	using String = std::string;
	static constexpr Char preferredSeparator = '/';
	static constexpr const Char* parentDirectory = "..";
	static constexpr const Char* currentDirectory = ".";
#else
#  error "Unsupported platform"
#endif

	Path() = default;
	Path(const Path& p) : m_path{ p.m_path } {}
	Path(Path&& p) noexcept : m_path{ std::move(p.m_path) } {}
	Path(String&& p) noexcept : m_path{ std::move(p) } {}
	template <class Source>
	Path(const Source& p, Format format = Format::generic) : m_path{ format == Format::generic ? convertToNative(p) : encode(p) } {}

	Path& operator=(const Path& other)
	{
		if( &other == this ) return *this;
		m_path = other.m_path;
		return *this;
	}

	Path& operator=(Path&& other) noexcept
	{
		if( &other == this ) return *this;
		m_path = std::move(other.m_path);
		return *this;
	}

	[[nodiscard]] operator std::string() const
	{
		return convertToUtf8(m_path);
	}

	[[nodiscard]] bool operator==(const Path& p) const noexcept
	{
		return m_path == p.m_path;
	}

	template <class Source>
	[[nodiscard]] bool operator==(const Source& p) const
	{
		return m_path == convertToNative(p);
	}

	[[nodiscard]] bool operator!=(const Path& p) const noexcept
	{
		return m_path != p.m_path;
	}

	template <class Source>
	[[nodiscard]] bool operator!=(const Source& p) const
	{
		return m_path != convertToNative(p);
	}

	Path& operator+=(const Path& p)
	{
		m_path += p.m_path;
		return *this;
	}

	template <class Source>
	Path& operator+=(const Source& p)
	{
		m_path += convertToNative(p);
		return *this;
	}

	Path& operator/=(const Path& p)
	{
		if( !m_path.empty() && !isDirectorySeparator(m_path.back()) )
			m_path += preferredSeparator;
		m_path += p.m_path;
		return *this;
	}

	template <class Source>
	Path& operator/=(const Source& p)
	{
		if( !m_path.empty() && !isDirectorySeparator(m_path.back()) )
			m_path += preferredSeparator;
		m_path += convertToNative(p);
		return *this;
	}

	[[nodiscard]] Path operator+(const Path& p) const
	{
		Path result = *this;
		result.m_path += p.m_path;
		return result;
	}

	template <class Source>
	[[nodiscard]] Path operator+(const Source& p) const
	{
		Path result = *this;
		result.m_path += convertToNative(p);
		return result;
	}

	[[nodiscard]] Path operator/(const Path& p) const
	{
		Path result = *this;
		if( !result.m_path.empty() && !isDirectorySeparator(result.m_path.back()) )
			result.m_path += preferredSeparator;
		result.m_path += p.m_path;
		return result;
	}

	template <class Source>
	[[nodiscard]] Path operator/(const Source& p) const
	{
		Path result = *this;
		if( !result.m_path.empty() && !isDirectorySeparator(result.m_path.back()) )
			result.m_path += preferredSeparator;
		result.m_path += convertToNative(p);
		return result;
	}

	[[nodiscard]] bool IsEmpty() const noexcept
	{
		return m_path.empty();
	}

	[[nodiscard]] const String& GetNative() const noexcept
	{
		return m_path;
	}

	[[nodiscard]] std::string GetGeneric() const
	{
		return convertToGeneric(m_path);
	}

	[[nodiscard]] Path GetExtension() const
	{
		if( const std::size_t position = m_path.rfind(Char('.')); position != std::string::npos )
			return Path{ m_path.substr(position + 1) };
		else
			return Path{};
	}

	template <class Source>
	Path& ReplaceExtension(const Source& extension)
	{
		if( const std::size_t position = m_path.rfind(Char('.')); position != std::string::npos )
			m_path.resize(position + 1);
		m_path += convertToNative(extension);
		return *this;
	}

	Path& RemoveExtension()
	{
		if( const std::size_t position = m_path.rfind(Char('.')); position != std::string::npos )
			m_path.resize(position);
		return *this;
	}

	[[nodiscard]] Path GetFilename() const
	{
		if( const std::size_t position = findLastDirectorySeparator(m_path); position != String::npos )
			return Path{ m_path.substr(position + 1) };
		else
			return Path{ m_path };
	}

	template <class Source>
	Path& ReplaceFilename(const Source& filename)
	{
		if( const std::size_t position = findLastDirectorySeparator(m_path); position != std::string::npos )
			m_path.resize(position + 1);
		else
			m_path.clear();
		m_path += convertToNative(filename);
		return *this;
	}

	Path& RemoveFilename()
	{
		if( const std::size_t position = findLastDirectorySeparator(m_path); position != std::string::npos )
			m_path.resize(position + 1);
		else
			m_path.clear();
		return *this;
	}

	[[nodiscard]] Path GetStem() const
	{
		const std::size_t directoryPosition = findLastDirectorySeparator(m_path);
		const std::size_t startPosition = directoryPosition == String::npos ? 0 : directoryPosition + 1;
		const std::size_t extensionPosition = m_path.find(Char('.'), startPosition);
		const std::size_t endPosition = extensionPosition == String::npos ? m_path.size() : extensionPosition;

		Path result;
		result.m_path = m_path.substr(startPosition, endPosition - startPosition);
		return result;
	}

	[[nodiscard]] Path GetDirectory() const
	{
		if( const std::size_t position = findLastDirectorySeparator(m_path); position != String::npos )
			return Path{ m_path.substr(0, position) };
		else
			return Path{};
	}

	[[nodiscard]] Path GetRoot() const
	{
		Path result;
#if defined(_WIN32)
		if( m_path.size() >= 2 &&
			((m_path[0] >= L'a' && m_path[0] <= L'z') || (m_path[0] >= L'A' && m_path[0] <= L'Z')) &&
			m_path[1] == L':' )
			result.m_path = { m_path[0], L':' };
#elif defined(__unix__) || defined(__APPLE__)
		if( path.size() >= 1 && path[0] == '/' ) result.path = '/';
#else
#  error "Unsupported platform"
#endif
		return result;
	}

	void Normalize()
	{
		std::vector<String> parts;
		String newPath;
		String::size_type previousPosition = 0;

#if defined(_WIN32)
		if( m_path.size() >= 2 &&
			((m_path[0] >= L'a' && m_path[0] <= L'z') || (m_path[0] >= L'A' && m_path[0] <= L'Z')) &&
			m_path[1] == L':' )
		{
			parts.push_back({ m_path[0], L':' });
			previousPosition = 2;

			if( m_path.size() >= 3 )
				parts.push_back(L"\\");
		}
#elif defined(__unix__) || defined(__APPLE__)
		if( path.size() >= 1 && path[0] == '/' )
		{
			parts.push_back("/");
			previousPosition = 1;
		}
#else
#  error "Unsupported platform"
#endif

		while( previousPosition < m_path.length() )
		{
			if( isDirectorySeparator(m_path[previousPosition]) ) ++previousPosition;

			const auto position = findFirstDirectorySeparator(m_path, previousPosition);
			const auto endPosition = position != String::npos ? position : m_path.length();
			const auto currentPart = m_path.substr(previousPosition, endPosition - previousPosition);

			if( currentPart == parentDirectory )
			{
				if( parts.empty() )
					parts.push_back(currentPart);
				else
				{
					const auto& previousPart = parts.back();
					if( previousPart != parentDirectory &&
						previousPart.length() != 1 && !isDirectorySeparator(previousPart.back()) )
						parts.pop_back();
				}
			}
			else if( currentPart.empty() )
			{
				if( position == String::npos ) // the last part
					parts.push_back(currentPart);
			}
			else if( currentPart != currentDirectory )
				parts.push_back(currentPart);

			previousPosition = endPosition;
		}

		for( const auto& part : parts )
		{
			if( !newPath.empty() && !isDirectorySeparator(newPath.back()) &&
				(part.length() != 1 || !isDirectorySeparator(part.back())) )
				newPath += preferredSeparator;

			newPath += part;
		}

		m_path = newPath;
	}

	[[nodiscard]] Path GetNormal() const
	{
		Path result = *this;
		result.Normalize();
		return result;
	}

	[[nodiscard]] bool IsAbsolute() const noexcept
	{
#if defined(_WIN32)
		return m_path.size() >= 2 &&
			((m_path[0] >= L'a' && m_path[0] <= L'z') || (m_path[0] >= L'A' && m_path[0] <= L'Z')) &&
			m_path[1] == L':';
#elif defined(__unix__) || defined(__APPLE__)
		return path.size() >= 1 && path[0] == '/';
#else
#  error "Unsupported platform"
#endif
	}

	[[nodiscard]] bool IsRelative() const noexcept
	{
		return !IsAbsolute();
	}

private:
	static constexpr bool isDirectorySeparator(Char c) noexcept
	{
#if defined(_WIN32)
		return c == L'\\' || c == L'/';
#elif defined(__unix__) || defined(__APPLE__)
		return c == '/';
#else
#  error "Unsupported platform"
#endif
	}

	static std::size_t findLastDirectorySeparator(const String& str, std::size_t position = String::npos) noexcept
	{
#if defined(_WIN32)
		return str.find_last_of(L"\\/", position);
#elif defined(__unix__) || defined(__APPLE__)
		return str.rfind('/', position);
#else
#  error "Unsupported platform"
#endif
	}

	static std::size_t findFirstDirectorySeparator(const String& str, std::size_t position = 0) noexcept
	{
#if defined(_WIN32)
		return str.find_first_of(L"\\/", position);
#elif defined(__unix__) || defined(__APPLE__)
		return str.find('/', position);
#else
#  error "Unsupported platform"
#endif
	}

	static const std::string& convertToUtf8(const std::string& p) noexcept
	{
		return p;
	}

	static std::string convertToUtf8(wchar_t c)
	{
		std::string s;

		if( c <= 0x7F )
			s.push_back(static_cast<char>(c));
		else if( c <= 0x7FF )
		{
			s.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
			s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
		}
		else if( c <= 0xFFFF )
		{
			s.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
			s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
			s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
		}
#if WCHAR_MAX > 0xFFFF
		else
		{
			s.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
			s.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
			s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
			s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
		}
#endif
		return s;
	}

	static std::string convertToUtf8(const std::wstring& p)
	{
		std::string s;

		for( wchar_t c : p )
		{
			if( c <= 0x7F )
				s.push_back(static_cast<char>(c));
			else if( c <= 0x7FF )
			{
				s.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
				s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
			}
			else if( c <= 0xFFFF )
			{
				s.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
				s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
				s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
			}
#if WCHAR_MAX > 0xFFFF
			else
			{
				s.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
				s.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
				s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
				s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
			}
#endif
		}

		return s;
	}

#ifdef _WIN32
	static std::wstring convertToWchar(const std::string& p)
	{
		std::wstring s;

		for( auto i = p.begin(); i != p.end(); ++i )
		{
			char32_t cp = *i & 0xFF;

			if( cp <= 0x7F ) // length = 1
			{
				// do nothing
			}
			else if( (cp >> 5) == 0x6 ) // length = 2
			{
				if( ++i == p.end() )
					Error("Invalid UTF-8 string");
				cp = ((cp << 6) & 0x7FF) + (*i & 0x3F);
			}
			else if( (cp >> 4) == 0xE ) // length = 3
			{
				if( ++i == p.end() )
					Error("Invalid UTF-8 string");
				cp = ((cp << 12) & 0xFFFF) + (((*i & 0xFF) << 6) & 0x0FFF);
				if( ++i == p.end() )
					Error("Invalid UTF-8 string");
				cp += *i & 0x3F;
			}
			else if( (cp >> 3) == 0x1E ) // length = 4
			{
				if( ++i == p.end() )
					Error("Invalid UTF-8 string");
				cp = ((cp << 18) & 0x1FFFFF) + (((*i & 0xFF) << 12) & 0x3FFFF);
				if( ++i == p.end() )
					Error("Invalid UTF-8 string");
				cp += ((*i & 0xFF) << 6) & 0x0FFF;
				if( ++i == p.end() )
					Error("Invalid UTF-8 string");
				cp += (*i) & 0x3F;
			}

			if( cp > WCHAR_MAX )
				Error("Unsupported UTF-8 character");

			s.push_back(static_cast<wchar_t>(cp));
		}

		return s;
	}

	static std::wstring convertToNative(char c)
	{
		const wchar_t result = (c == '/') ? L'\\' : static_cast<wchar_t>(c);
		return std::wstring{ result };
	}

	static std::wstring convertToNative(const std::string& p)
	{
		std::wstring result = convertToWchar(p);

		for( auto& c : result ) if( c == L'/' ) c = L'\\';
		return result;
	}

	static std::wstring convertToNative(wchar_t c)
	{
		const wchar_t result = (c == L'/') ? L'\\' : c;
		return std::wstring{ result };
	}

	static std::wstring convertToNative(const std::wstring& p)
	{
		std::wstring result = p;

		for( auto& c : result ) if( c == L'/' ) c = L'\\';
		return result;
	}

	static std::string convertToGeneric(const std::wstring& p)
	{
		std::string result = convertToUtf8(p);

		for( auto& c : result ) if( c == '\\' ) c = '/';
		return result;
	}

	static std::wstring encode(char c)
	{
		return std::wstring{ static_cast<wchar_t>(c) };
	}

	static std::wstring encode(const std::string& p)
	{
		return convertToWchar(p);
	}

	static std::wstring encode(wchar_t c)
	{
		return std::wstring{ c };
	}

	static const std::wstring& encode(const std::wstring& p) noexcept
	{
		return p;
	}
#elif defined(__unix__) || defined(__APPLE__)
	static std::string convertToNative(char c)
	{
		return std::string{ c };
	}

	static const std::string& convertToNative(const std::string& p) noexcept
	{
		return p;
	}

	static std::string convertToNative(wchar_t c)
	{
		return convertToUtf8(c);
	}

	static std::string convertToNative(const std::wstring& p)
	{
		return convertToUtf8(p);
	}

	static const std::string& convertToGeneric(const std::string& p) noexcept
	{
		return p;
	}

	static std::string encode(char c)
	{
		return std::string{ c };
	}

	static const std::string& encode(const std::string& p) noexcept
	{
		return p;
	}

	static std::string encode(wchar_t c)
	{
		return convertToUtf8(c);
	}

	static std::string encode(const std::wstring& p)
	{
		return convertToUtf8(p);
	}
#else
#  error "Unsupported platform"
#endif

	String m_path;
};