#pragma once

#include <Geode/Geode.hpp>
#include "RenderTexture.hpp"
#include "Blur.hpp"

using namespace geode::prelude;

class ShaderScene : public CCLayer {
public:

    std::vector<CCNode*> backgroundNodes = {};
    CCScene* refScene = nullptr;
    bool inChanged = false;

    static ShaderScene* create(CCScene* pScene);
    bool init(CCScene* pScene);

    void updateChilds();
    virtual void visit();
};