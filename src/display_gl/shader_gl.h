#pragma once

#include <string>

#ifdef __ANDROID__
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else


#endif


#include <map>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class CPoint3D;

typedef struct
{
    unsigned int prgID;
    unsigned int vs, fs;
} ShaderPrg;

class Shader
{
public:
    Shader();

    ~Shader();

    bool CreateShaderFromString(const std::string& vs, const std::string& fs);

    bool CreateShader(const char* vs, const char* fs);

    int GetAttribLoc(const char* name) const;

    int GetUniformLocation(const std::string& name);

    unsigned int GetShaderID() const
    {
        return m_prog.prgID;
    }

    void Use() const;

    void SetBool(const std::string& name, bool value);

    void SetInt(const std::string& name, int value);

    void SetFloat(const std::string& name, float value);

    void SetVec2(const std::string& name, const glm::vec2& value);

    void SetVec2(const std::string& name, float x, float y);

    void SetVec3(const std::string& name, const glm::vec3& value);

    void SetVec3(const std::string &name, const CPoint3D &value);

    void SetVec3(const std::string& name, float x, float y, float z);

    void SetVec4(const std::string& name, const glm::vec4& value);

    void SetVec4(const std::string& name, float x, float y, float z, float w);

    void SetMat2(const std::string& name, const glm::mat2& mat);

    void SetMat3(const std::string& name, const glm::mat3& mat);

    void SetMat4(const std::string& name, const glm::mat4& mat);

private:
    std::map<std::string, int> m_uniformLocations;

    ShaderPrg m_prog{};

    unsigned int CompileShader(const char* shader, unsigned int type, int num_bytes);

    bool LinkPrg();
};
