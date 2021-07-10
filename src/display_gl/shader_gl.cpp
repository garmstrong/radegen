#include <string>
#include "shader_gl.h"
#include "osutils.h"
#include "point3d.h"

#include <glad/glad.h>

using namespace rade;

void Shader::Use() const
{
    glUseProgram(GetShaderID());
}

bool Shader::CreateShaderFromString(const std::string& key, const std::string& vs, const std::string& fs)
{
    m_name = key;
    m_prog.vs = CompileShader(vs.c_str(), GL_VERTEX_SHADER, vs.size());
    if(!m_prog.vs)
    {
        return false;
    }

    m_prog.fs = CompileShader(fs.c_str(), GL_FRAGMENT_SHADER, fs.size());
    if(!m_prog.fs)
    {
        return false;
    }

    if(!LinkPrg())
    {
        return false;
    }
    return true;
}

bool Shader::CreateShader(const std::string& key, const char *vs, const char *fs)
{
    m_name = key;
    long vs_size = 0;
    char *vsData = ReadPlatformAssetFile(vs, &vs_size);
    if(!vsData)
	{
    	return false;
	}

	long fs_size = 0;
    char *fsData = ReadPlatformAssetFile(fs, &fs_size);
	if(!fsData)
	{
		return false;
	}

    m_prog.vs = CompileShader(vsData, GL_VERTEX_SHADER, vs_size);
	if(!m_prog.vs)
	{
		return false;
	}

    m_prog.fs = CompileShader(fsData, GL_FRAGMENT_SHADER, fs_size);
	if(!m_prog.fs)
	{
		return false;
	}

    if(!LinkPrg())
	{
    	return false;
	}

    free(vsData);
    free(fsData);
    return true;
}

unsigned int Shader::CompileShader(const char *shader, unsigned int type, int num_bytes)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &shader, &num_bytes);
    glCompileShader(s);

	GLint isCompiled = 0;
	glGetShaderiv(s, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(s, maxLength, &maxLength, errorLog.data());
		std::string errorStr(begin(errorLog), end(errorLog));
		Log("Shader compile error: %s\n", errorStr.c_str());
		glDeleteShader(s);
		return 0;
	}
	return s;
}

bool Shader::LinkPrg()
{
    m_prog.prgID = glCreateProgram();
    glAttachShader(m_prog.prgID, m_prog.vs);
    glAttachShader(m_prog.prgID, m_prog.fs);
    glLinkProgram(m_prog.prgID);

    // Check if program linked okay
    GLint okay = GL_FALSE;
    glGetProgramiv(m_prog.prgID, GL_LINK_STATUS, &okay);
    if(okay == GL_FALSE)
    {
		Log("Shader linking error:\n");
        glDeleteProgram(m_prog.prgID);
        m_prog.prgID = 0;
        return false;
    }
    return true;
}

int Shader::GetAttribLoc(const char *name) const
{
    int attrib = glGetAttribLocation(m_prog.prgID, name);
    if(attrib == -1)
    {
		Abort("Could not find vertex attribute: %s\n", name);
        return -1;
    }
    return attrib;
}

int Shader::GetUniformLocation(const std::string &name)
{
    //return glGetUniformLocation(m_prog.prgID, name.c_str());
	std::map<std::string, int>::iterator it;
	it = m_uniformLocations.find(name);
	if (it != m_uniformLocations.end())
	{
		return it->second;
	}

	GLint uniformLoc = glGetUniformLocation(m_prog.prgID, name.c_str());
	if(uniformLoc == -1)
    {
        Warning("shader: glGetUniformLocation %s failed\n", name.c_str());
    }
	m_uniformLocations[name] = uniformLoc;
	return uniformLoc;
}

void Shader::SetBool(const std::string &name, bool value)
{
	glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string &name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string &name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &value)
{
	glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec2(const std::string &name, float x, float y)
{
	glUniform2f(GetUniformLocation(name), x, y);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value)
{
	glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec3(const std::string &name, const rade::vector3 &value)
{
    float val[3];
    value.ToFloat3(val);
    glUniform3fv(GetUniformLocation(name), 1, val);
}

void Shader::SetVec3(const std::string &name, float x, float y, float z)
{
	glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value)
{
	glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void Shader::SetMat2(const std::string &name, const glm::mat2 &mat)
{
	glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &mat)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}
