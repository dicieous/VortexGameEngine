#include "Vxpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <fstream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>


namespace Vortex {
	static GLenum ShaderTypeFromString(std::string& type) {
		if (type == "vertex") return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		VX_CORE_ASSERT(false, "The Shader type is not Supported");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		VX_PROFILE_FUNCTION();

		std::string shaderSource = ReadFile(filePath);

		auto shaderSourcesList = Preprocess(shaderSource);
		Compile(shaderSourcesList);

		std::filesystem::path path = filePath;
		m_name = path.stem().string(); // Returns the file's name stripped of the extension.
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_RendererID(0), m_name(name)
	{
		VX_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;

		Compile(shaderSources);
	}


	OpenGLShader::~OpenGLShader()
	{
		VX_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filePath)
	{
		VX_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);

		if (in) {
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			VX_CORE_ERROR("Could not open file '{0}' ", filePath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::Preprocess(const std::string& source)
	{
		VX_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typetoken = "#type";
		size_t typeTokenLength = strlen(typetoken);
		size_t pos = source.find(typetoken, 0);

		while (pos != std::string::npos) {

			size_t eol = source.find_first_of("\r\n", pos);
			VX_CORE_ASSERT((eol != std::string::npos), "Syntax Error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			VX_CORE_ASSERT(ShaderTypeFromString(type), "Invalid Shader type Specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typetoken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& ShaderSources)
	{
		VX_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();

		VX_CORE_ASSERT((ShaderSources.size() <= 2), "We only support two shaders for now");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDsIndex = 0;
		for (auto kv : ShaderSources) {
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			// Compile the shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				VX_CORE_ERROR("{0}", infoLog.data());
				VX_CORE_ASSERT(false, "Shader Compilation Failed!");

				break;
			}
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDsIndex++] = (shader);
		}


		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.

			for (auto iD : glShaderIDs) {
				glDeleteShader(iD);
			}

			VX_CORE_ERROR("{0}", infoLog.data());
			VX_CORE_ASSERT(false, "Shader Linking Failed!");

			return;
		}

		// Always detach shaders after a successful link.
		for (auto iD : glShaderIDs) {
			glDetachShader(program, iD);
			glDeleteShader(iD);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Bind() const
	{
		VX_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}


	void OpenGLShader::Unbind() const
	{
		VX_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, const int value)
	{
		VX_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		VX_PROFILE_FUNCTION();

		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		VX_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		VX_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		VX_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}


	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}