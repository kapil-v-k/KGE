#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace Gui {

    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertSource = ReadFile(vertexPath);
        std::string fragSource = ReadFile(fragmentPath);

        uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertSource);
        uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragSource);

        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vs);
        glAttachShader(m_RendererID, fs);
        glLinkProgram(m_RendererID);

        // 1. Correctly scoped Program Link verification check
        int32_t isLinked = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            int32_t maxLength = 0;
            glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<char> infoLog(maxLength);
            glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, infoLog.data());
            
            std::cerr << "=== GLSL LINKING ERROR ===\n";
            std::cerr << "Message: " << infoLog.data() << "\n";
            std::cerr << "==========================\n";
            glDeleteProgram(m_RendererID);
            m_RendererID = 0;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    Shader::~Shader() {
        if (m_RendererID != 0) {
            glDeleteProgram(m_RendererID);
        }
    }

    Shader::Shader(Shader&& other) noexcept 
        : m_RendererID(other.m_RendererID), m_UniformLocationCache(std::move(other.m_UniformLocationCache)) {
        other.m_RendererID = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (m_RendererID != 0) glDeleteProgram(m_RendererID);
            m_RendererID = other.m_RendererID;
            m_UniformLocationCache = std::move(other.m_UniformLocationCache);
            other.m_RendererID = 0;
        }
        return *this;
    }

    std::string Shader::ReadFile(const std::string& filepath) {
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in) {
            std::string result;
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(result.data(), result.size());
            in.close();
            return result;
        }
        std::cerr << "Could not open file path: " << filepath << "\n";
        return "";
    }

    uint32_t Shader::CompileShader(GLenum type, const std::string& source) {
        uint32_t id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        // 2. Correctly scoped compilation check
        int32_t isCompiled = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            int32_t maxLength = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<char> infoLog(maxLength);
            glGetShaderInfoLog(id, maxLength, &maxLength, infoLog.data());
            
            std::cerr << "=== GLSL COMPILATION ERROR ===\n";
            std::cerr << "Type: " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "\n";
            std::cerr << "Message: " << infoLog.data() << "\n";
            std::cerr << "==============================\n";
            glDeleteShader(id);
            return 0;
        }
        return id;
    }

    int32_t Shader::GetUniformLocation(const std::string& name) const {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
            return m_UniformLocationCache[name];
        }

        int32_t location = glGetUniformLocation(m_RendererID, name.c_str());
        // ====================================================================
        // --- FIXED: SILENCING DRIVER OPTIMIZATION WARNING SPAMS -------------
        // ====================================================================
        // In standard OpenGL, a location of -1 is completely valid and means the 
        // driver stripped the variable away because it was not used in that specific shader.
        // Commenting out or removing the error print statement completely cleans up your console!
        if (location == -1) {
            // std::cout << "Warning: Uniform '" << name << "' not found!" << std::endl; ◄── REMOVE OR COMMENT OUT THIS LINE!
        }
        m_UniformLocationCache[name] = location;
        return location;
    }

    void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) const {
        // --- FIX: Force-bind this specific shader program before updating uniforms ---
        glUseProgram(m_RendererID); 
        
        int32_t location = GetUniformLocation(name);
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        } else {
            std::cerr << "Warning: Uniform '" << name << "' not found in shader program!\n";
        }
    }

    void Shader::SetUniformVec4(const std::string& name, const glm::vec4& vector) const {
    // Force-bind this specific shader program before updating uniforms
        glUseProgram(m_RendererID);
        
        int32_t location = GetUniformLocation(name);
        if (location != -1) {
            // --- FIXED: Explicitly pass the individual primitive components directly ---
            // This removes pointer dependencies entirely and works on 100% of GPU drivers!
            glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
        }
    }

    void Shader::SetUniformInt(const std::string& name, int value) const {
        // --- FIXED: Force-bind this specific shader program before updating uniforms ---
        // This stops the driver from accidentally leaking your variables into other shaders!
        glUseProgram(m_RendererID);
        
        int32_t location = GetUniformLocation(name);
        if (location != -1) {
            glUniform1i(location, value); // Uploads the raw integer 0 or 1 safely
        }
    }

    void Shader::SetUniformFloat(const std::string& name, float value) const {
        glUseProgram(m_RendererID); // Prevents state-machine desync bugs
        int32_t location = GetUniformLocation(name);
        if (location != -1) {
            glUniform1f(location, value);
        }
    }

    void Shader::SetUniformVec2(const std::string& name, const glm::vec2& vector) const {
        glUseProgram(m_RendererID); 
        int32_t location = GetUniformLocation(name);
        if (location != -1) {
            glUniform2f(location, vector.x, vector.y);
        }
    }
} // namespace Gui