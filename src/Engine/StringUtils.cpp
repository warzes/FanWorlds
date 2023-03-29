#include "stdafx.h"
#include "StringUtils.h"

namespace StringUtils
{
	template <> long sto_number(std::string const& s)
	{
		return std::stol(s, nullptr, 0);
	}

	template <> float sto_number(std::string const& s)
	{
		return std::stof(s);
	}

	template <> double sto_number(std::string const& s)
	{
		return std::stod(s);
	}

	template <> std::optional<bool> from_string(std::string const& s)
	{
		return stob(s);
	}

	template <> std::optional<float> parse(std::string_view s)
	{
		trim(s);
		trim(s, '+');

		char buf[32];
		buf[sizeof(buf) - 1] = 0;
		strncpy_s(buf, s.data(), std::min(s.size(), sizeof(buf) - 1));
		char* endptr = buf;
		float value = strtof(buf, &endptr);

		if( endptr == buf )
			return std::optional<float>();

		return value;
	}

	template <> std::optional<double> parse(std::string_view s)
	{
		trim(s);
		trim(s, '+');

		char buf[32];
		buf[sizeof(buf) - 1] = 0;
		strncpy_s(buf, s.data(), std::min(s.size(), sizeof(buf) - 1));
		char* endptr = buf;
		double value = strtod(buf, &endptr);

		if( endptr == buf )
			return std::optional<double>();

		return value;
	}

	template <> std::optional<bool> parse<bool>(std::string_view s)
	{
		return stob(s);
	}

	template <> std::optional<std::string_view> parse<std::string_view>(std::string_view s)
	{
		trim(s);
		trim(s, '"');
		return s;
	}

	template <> std::optional<std::string> parse<std::string>(std::string_view s)
	{
		if( auto r = parse<std::string_view>(s) )
			return std::string(*r);
		return std::nullopt;
	}

	template <> std::optional<glm::ivec2> parse(std::string_view s) { return parse_vector<glm::ivec2>(s); }
	template <> std::optional<glm::ivec3> parse(std::string_view s) { return parse_vector<glm::ivec3>(s); }
	template <> std::optional<glm::ivec4> parse(std::string_view s) { return parse_vector<glm::ivec4>(s); }

	template <> std::optional<glm::uvec2> parse(std::string_view s) { return parse_vector<glm::uvec2>(s); }
	template <> std::optional<glm::uvec3> parse(std::string_view s) { return parse_vector<glm::uvec3>(s); }
	template <> std::optional<glm::uvec4> parse(std::string_view s) { return parse_vector<glm::uvec4>(s); }

	template <> std::optional<glm::vec2> parse(std::string_view s) { return parse_vector<glm::vec2>(s); }
	template <> std::optional<glm::vec3> parse(std::string_view s) { return parse_vector<glm::vec3>(s); }
	template <> std::optional<glm::vec4> parse(std::string_view s) { return parse_vector<glm::vec4>(s); }

} // namespace StringUtils