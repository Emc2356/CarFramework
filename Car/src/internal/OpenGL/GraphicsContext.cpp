#include "Car/Core/Core.hpp"

#include "Car/internal/OpenGL/GraphicsContext.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

void APIENTRY crGlDebugCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
    (void) length;
    (void) userParam;
    char* _source;
    char* _type;
    char* _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = (char*)"API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = (char*)"WINDOW SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = (char*)"SHADER COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = (char*)"THIRD PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        _source = (char*)"APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        _source = (char*)"UNKNOWN";
        break;
    default:
        _source = (char*)"UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:               _type = (char*)"ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: _type = (char*)"DEPRECATED BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  _type = (char*)"UDEFINED BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         _type = (char*)"PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         _type = (char*)"PERFORMANCE"; break;
    case GL_DEBUG_TYPE_OTHER:               _type = (char*)"OTHER"; break;
    case GL_DEBUG_TYPE_MARKER:              _type = (char*)"MARKER"; break;
    default:                                _type = (char*)"UNKNOWN"; break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         _severity = (char*)"HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       _severity = (char*)"MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW:          _severity = (char*)"LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: _severity = (char*)"NOTIFICATION"; break;
        default:                             _severity = (char*)"UNKNOWN"; break;
    }

    CR_CORE_ERROR("{0}: {1} of {2} severity, raised from {3}: {4}",
                 id, _type, _severity, _source, message);
}


namespace Car {
	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle) : mWindowHandle(windowHandle) {
        CR_ASSERT(windowHandle, "Interal Error: null window handle sent to gl graphics context");
	}

	void OpenGLGraphicsContext::init() {
		glfwMakeContextCurrent(mWindowHandle);
		// not 100% sure if i will keep using GLFW so i use the glad
		// built-in loader 
        if (!gladLoaderLoadGL()) {
            throw std::runtime_error("Car: Failed to initialize glad");
        }
		CR_CORE_TRACE("OpenGL Context Initialized");

		#if defined(CR_DEBUG)
        	glEnable(GL_DEBUG_OUTPUT);
        	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        	glDebugMessageCallback(crGlDebugCallback, 0);
		#endif
	}

	void OpenGLGraphicsContext::swapBuffers() {
		glfwSwapBuffers(mWindowHandle);
	}

}