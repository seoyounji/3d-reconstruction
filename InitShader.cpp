#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>

using namespace std;

static string ReadFile(const char* pFileName)
{
	string outFile;

	ifstream f(pFileName);

	if (!f.is_open()) return "";

	string line;
	while (getline(f, line)) {
		outFile.append(line);
		outFile.append("\n");
	}
	return outFile;
}

GLuint InitShader(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader {
		const char*  filename;
		GLenum       type;
		string		 source;
	}  shaders[2] = {
	{ vShaderFile, GL_VERTEX_SHADER, "" },
	{ fShaderFile, GL_FRAGMENT_SHADER, "" }
	};

	GLuint program = glCreateProgram();

	for (int i = 0; i < 2; ++i) {
		Shader& s = shaders[i];
		s.source = ReadFile(s.filename);
		if (shaders[i].source == "") {
			std::cerr << "Failed to read \n" << s.filename << std::endl;
			exit(EXIT_FAILURE);
		}

		GLuint shader = glCreateShader(s.type);
		const char* shader_src = s.source.c_str();
		glShaderSource(shader, 1, (const GLchar**)&shader_src, NULL);
		glCompileShader(shader);

		GLint  compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			std::cerr << s.filename << " failed to compile: \n" << std::endl;
			GLint  logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, NULL, logMsg);
			std::cerr << logMsg << std::endl;
			delete[] logMsg;

			exit(EXIT_FAILURE);
		}

		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		std::cerr << "Shader program failed to link \n" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	glUseProgram(program);

	return program;
}

