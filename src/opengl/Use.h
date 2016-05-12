#pragma once

#include "Program.h"
#include "VertexArrayObject.h"

namespace narwhal {

class UseProgram {
public:
    UseProgram(Program& program)
        : program_{ program } {
        program_.use();
    }

    ~UseProgram() {
        program_.stopUsing();
    }
private:
    Program&    program_;
};

class UseArray {
public:
    UseArray(VertexArrayObject& array)
        :array_{ array } {
        array.bind();
    }

    ~UseArray() {
        array_.unbind();
    }
private:
    VertexArrayObject&  array_;
};

}
