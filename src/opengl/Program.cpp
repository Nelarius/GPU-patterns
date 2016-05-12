#include "Program.h"
#include "Assert.h"
#include "common/Log.h"
#include <cstdlib>  //for std::exit

namespace narwhal {

Program::Program(const DynamicArray<Shader>& shaders) {
    link(shaders);
}

Program::~Program() {
    glDeleteProgram(object_);
    object_ = 0;
}

void Program::link(const DynamicArray<Shader>& shaders) {
    if (shaders.size() == 0u) {
        std::cerr << "No shaders were provided to create the program.\n";
        std::exit(EXIT_SUCCESS);
    }

    object_ = glCreateProgram();
    if (object_ == 0) {
        LOG_ERROR << "Shader program creation error";
    }

    for (auto& shader : shaders) {
        glAttachShader(object_, shader.object());
    }

    glLinkProgram(object_);

    //check the linking status
    GLint status;
    glGetProgramiv(object_, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");

        GLint infoLogLength;
        glGetProgramiv(object_, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLogStr = new char[infoLogLength + 1];
        glGetProgramInfoLog(object_, infoLogLength, NULL, infoLogStr);
        msg += infoLogStr;
        delete[] infoLogStr;

        glDeleteProgram(object_);
        object_ = 0;
        LOG_ERROR << "Program linkage error";
    }
}

GLuint Program::object() const {
    return object_;
}

GLint Program::attribute(const GLchar* attribName) const {
    NARWHAL_ASSERT(attribName);

    GLint attrib = glGetAttribLocation(object_, attribName);
    NARWHAL_ASSERT(attrib != -1);

    return attrib;
}

GLint Program::uniform(const GLchar* uniformName) const {
    NARWHAL_ASSERT(uniformName);

    GLint uniform = glGetUniformLocation(object_, uniformName);
    NARWHAL_ASSERT(uniform != -1);
    return uniform;
}

GLint Program::subroutineUniform(const GLchar* uniformName, GLenum shaderType) const {
    NARWHAL_ASSERT(uniformName);
    GLint uniform = glGetSubroutineUniformLocation(object_, shaderType, uniformName);
    NARWHAL_ASSERT(uniform != -1);
    return uniform;
}

GLuint Program::subroutineIndex(const GLchar* functionName, GLenum shaderType) const {
    NARWHAL_ASSERT(functionName);
    GLuint index = glGetSubroutineIndex(object_, shaderType, functionName);
    NARWHAL_ASSERT(index != GL_INVALID_INDEX);
    return index;
}

void Program::use() const {
    glUseProgram(object_);
}

bool Program::isInUse() const {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    return (currentProgram == (GLint)object_);
}

void Program::stopUsing() const {
    NARWHAL_ASSERT(isInUse());
    glUseProgram(0);
}

void Program::setUniform(const GLchar* name, GLint v0) const {
    NARWHAL_ASSERT(isInUse());
    glUniform1i(uniform(name), v0);
}

void Program::setUniform(const GLchar* name, GLint v0, GLint v1) const {
    NARWHAL_ASSERT(isInUse());
    glUniform2i(uniform(name), v0, v1);
}

void Program::setUniform(const GLchar* name, GLint v0, GLint v1, GLint v2) const {
    NARWHAL_ASSERT(isInUse());
    glUniform3i(uniform(name), v0, v1, v2);
}

void Program::setUniform(const GLchar* name, GLint v0, GLint v1, GLint v2, GLint v3) const {
    NARWHAL_ASSERT(isInUse());
    glUniform4i(uniform(name), v0, v1, v2, v3);
}

void Program::setUniform(const GLchar* name, GLfloat v0) const {
    NARWHAL_ASSERT(isInUse());
    glUniform1f(uniform(name), v0);
}

void Program::setUniform(const GLchar* name, GLfloat v0, GLfloat v1) const {
    NARWHAL_ASSERT(isInUse());
    glUniform2f(uniform(name), v0, v1);
}

void Program::setUniform(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2) const {
    NARWHAL_ASSERT(isInUse());
    glUniform3f(uniform(name), v0, v1, v2);
}

void Program::setUniform(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const
{
    NARWHAL_ASSERT(isInUse());
    glUniform4f(uniform(name), v0, v1, v2, v3);
}

void Program::setUniform(const GLchar* name, const Vec2f& v) const {
    NARWHAL_ASSERT(isInUse());
    glUniform2fv(uniform(name), 1, v.data);
}

void Program::setUniform(const GLchar* name, const Vec3f& v) const {
    NARWHAL_ASSERT(isInUse());
    glUniform3fv(uniform(name), 1, v.data);
}

void Program::setUniform(const GLchar* name, const Vec4f& v) const {
    NARWHAL_ASSERT(isInUse());
    glUniform4fv(uniform(name), 1, v.data);
}

void Program::setUniform(const GLchar* name, const Mat3f& M) const {
    NARWHAL_ASSERT(isInUse());
    glUniformMatrix3fv(uniform(name), 1, GL_TRUE, M.data);
}

void Program::setUniform(const GLchar* name, const Mat4f& M) const {
    NARWHAL_ASSERT(isInUse());
    glUniformMatrix4fv(uniform(name), 1, GL_TRUE, M.data);
}

}
