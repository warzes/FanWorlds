#pragma once

class Uniform
{
	int location = -1;
	unsigned programId = 0;
};

class ShaderProgram
{
public:
	ShaderProgram() = default;
	ShaderProgram(
		const std::string &sharedSource,
		const std::string &vertexSource,
		const std::string &fragmentSource
	);
	ShaderProgram(
		const std::string &sharedSource,
		const std::string &vertexSource,
		const std::string &geometrySource,
		const std::string &fragmentSource
	);
	~ShaderProgram();

	static ShaderProgram FromFile(
		const std::string &sharedFilename,
		const std::string &vertexFilename,
		const std::string &fragmentFilename
	);

	static ShaderProgram FromFile(
		const std::string &sharedFilename,
		const std::string &vertexFilename,
		const std::string &geometryFilename,
		const std::string &fragmentFilename
	);

	void Bind() const;

private:
	unsigned id = 0;
};