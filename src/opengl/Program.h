#pragma once 

#include "Shader.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "common/Array.h"

namespace narwhal {

/// @brief A wrapper around an OpenGL shader object.
/// 
/// The shader can be linked at construction time from a vector of individually compiled shader objects,
/// or at a later time by calling the link-method.
class Program {
public:
    /// @brief Construct the program from a vector of shader objects
    Program(const DynamicArray<Shader>& shaders);
    ~Program();
    Program()                           = default;
    Program(const Program&)             = delete;
    Program(Program&&)                  = delete;
    Program& operator=(const Program&)  = delete;
    Program& operator=(Program&&)       = delete;

    void link(const DynamicArray<Shader>& shaders);

    /// @brief Get the OpenGL handle for this shader program.
    GLuint object() const;

    /**
     * @brief Get the location of a shader attribute within a shader.
     * @param attribName
     */
    GLint attribute(const GLchar* attribName) const;
    /**
     * @brief Get the location of a uniform variable within shader.
     * @param uniformName The null-terminated string containing the name of the uniform variable whose location is to be queries.
     */
    GLint uniform(const GLchar* uniformName) const;

    GLint subroutineUniform(const GLchar* uniformName, GLenum shaderType) const;

    GLuint subroutineIndex(const GLchar* functionName, GLenum shaderType) const;

    /// @brief Use this shader.
    void use() const;
    bool isInUse() const;
    void stopUsing() const;

    /// A wrapper for glUniform*i
    void setUniform(const GLchar*, GLint) const;
    void setUniform(const GLchar*, GLint, GLint) const;
    void setUniform(const GLchar*, GLint, GLint, GLint) const;
    void setUniform(const GLchar*, GLint, GLint, GLint, GLint) const;
    /// A wrapper for glUniform*f
    void setUniform(const GLchar*, GLfloat) const;
    void setUniform(const GLchar*, GLfloat, GLfloat) const;
    void setUniform(const GLchar*, GLfloat, GLfloat, GLfloat) const;
    void setUniform(const GLchar*, GLfloat, GLfloat, GLfloat, GLfloat) const;
    /// A wrapper for glUniform*fv
    void setUniform(const GLchar*, const Vec2f&) const;
    void setUniform(const GLchar*, const Vec3f&) const;
    void setUniform(const GLchar*, const Vec4f&) const;

    /// A wrapper for glUniformMatrix*fv
    void setUniform(const GLchar*, const Mat3f&) const;
    void setUniform(const GLchar*, const Mat4f&) const;

private:
    GLuint  object_{ 0u };
};


}
