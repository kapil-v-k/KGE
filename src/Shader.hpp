#pragma once
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Gui {

    class Shader {
    private:
        uint32_t m_RendererID{ 0 };
        mutable std::unordered_map<std::string, int32_t> m_UniformLocationCache;

        std::string ReadFile(const std::string& filepath);
        uint32_t CompileShader(GLenum type, const std::string& source);

    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        void Bind() const { glUseProgram(m_RendererID); }
        void Unbind() const { glUseProgram(0); }

        void SetUniformMat4(const std::string& name, const glm::mat4& matrix) const;
        void SetUniformVec4(const std::string& name, const glm::vec4& vector) const;
        void SetUniformInt(const std::string& name, int value) const;
        void SetUniformFloat(const std::string& name, float value) const;
        void SetUniformVec2(const std::string& name, const glm::vec2& vector) const;
        int32_t GetUniformLocation(const std::string& name) const;
    };

} // namespace Gui