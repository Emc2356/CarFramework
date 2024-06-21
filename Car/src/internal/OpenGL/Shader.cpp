#include "Car/internal/OpenGL/Shader.hpp"

#include <glad/gl.h>


namespace Car {
    OpenGLShader::OpenGLShader(const std::string& vertexBinary, const std::string& fragmeantBinary) {
        // Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader binary code to GL
		//TODO: make this work
		// glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexBinary.c_str(), vertexBinary.length());
		// glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);
		const char* vertexSource = vertexBinary.c_str();
		int vertexSourceLength = vertexBinary.length();
		glShaderSource(vertexShader, 1, &vertexSource, &vertexSourceLength);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

            std::string errorMessage;

            errorMessage += infoLog.data();
            errorMessage += "Vertex Shader compilation failure!";

            throw std::runtime_error(errorMessage);
		}

		// Create an empty fragment shader handle
		GLuint fragmeantShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader binary code to GL
		// glShaderBinary(1, &fragmeantShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmeantBinary.c_str(), fragmeantBinary.length());
		// glSpecializeShader(fragmeantShader, "main", 0, nullptr, nullptr);
		const char* fragmeantSource = fragmeantBinary.c_str();
		int fragmeantSourceLength = fragmeantBinary.length();
		glShaderSource(fragmeantShader, 1, &fragmeantSource, &fragmeantSourceLength);

		// Compile the fragment shader
		glCompileShader(fragmeantShader);

		glGetShaderiv(fragmeantShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(fragmeantShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmeantShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmeantShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

            std::string errorMessage;

            errorMessage += infoLog.data();
            errorMessage += "Fragmeant Shader compilation failure!";

            throw std::runtime_error(errorMessage);
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		mID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(mID, vertexShader);
		glAttachShader(mID, fragmeantShader);

		// Link our program
		glLinkProgram(mID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(mID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(mID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(mID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmeantShader);

            std::string errorMessage;

            errorMessage += infoLog.data();
            errorMessage += "Shader link failure!";

            throw std::runtime_error(errorMessage);
		}

		// Always detach shaders after a successful link.
		glDetachShader(mID, vertexShader);
		glDetachShader(mID, fragmeantShader);
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(mID);
    }

    void OpenGLShader::bind() const {
        glUseProgram(mID);
    }

    void OpenGLShader::unbind() const {
        glUseProgram(0);
    }

    Ref<Shader> Shader::CreateImpl(const std::string& vertexBinary, const std::string& fragmeantBinary) {
        return createRef<OpenGLShader>(vertexBinary, fragmeantBinary);
    }
}
