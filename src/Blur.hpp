#pragma once

#include <Geode/Geode.hpp>
#include "RenderTexture.hpp"

using namespace geode::prelude;

namespace Blur
{
    inline GLuint VAO, VBO;

    inline cocos2d::CCGLProgram* blurProgram;
    inline std::shared_ptr<RenderTexture> gdRenderTexture;

    inline GLint oldTexture = -1;
    inline GLint darknessUniform = -1;
    inline GLint stepsUniform = -1;
    inline GLint sizeUniform = -1;
    inline GLint amountUniform = -1;

    inline float blurAmount = 1.f;

    inline bool compiled = false;

    void compileBlurShader();


    void setBlurUniforms();

    void blurCallback();
    void resetCallback();
};