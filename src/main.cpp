#include <Geode/Geode.hpp>
#include <Geode/modify/CCNode.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CCLayer.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Utils.hpp"
#include "ShaderScene.hpp"

using namespace geode::prelude;
using namespace Blur;

std::vector<CCNode*> blurredChildren = {};
CCNode* lastChild = nullptr;


void refreshAllSceneNodes(CCTransitionScene* self, CCScene* scene, bool useTrans) {
    return;
    blurredChildren.clear();
    CCArray* allChildrens = nullptr;
    if (useTrans) {
        allChildrens = public_cast(self, m_pInScene)->getChildren();
    }
    else {
        allChildrens = scene->getChildren();
    }
    CCObject* obj = nullptr;
    CCARRAY_FOREACH(allChildrens, obj) {
        auto existingChild = dynamic_cast<CCNode*>(obj);
        if (existingChild) {
            blurredChildren.push_back(existingChild);
            std::cout << typeid(*existingChild).name() + 6 << std::endl;
            lastChild = existingChild;
        }
    }
}

void CCTransitionScene_finish(CCTransitionScene* self) {
    auto utilScene = public_cast(self, m_pInScene);

    self->finish();
}

$execute{
    Mod::get()->hook(
        reinterpret_cast<void*>(
            geode::addresser::getNonVirtual(
                geode::modifier::Resolve<>::func(&cocos2d::CCTransitionScene::finish)
            )
        ),
        &CCTransitionScene_finish,
        "cocos2d::CCTransitionScene::finish",
        tulip::hook::TulipConvention::Thiscall
    );
}

class $modify(CCEGLView) {
    void toggleFullScreen(bool value, bool borderless) {
        gdRenderTexture = nullptr;
        CCEGLView::toggleFullScreen(value, borderless);
    }

    void resizeWindow(int width, int height) {
        CCEGLView::resizeWindow(width, height);
        if (gdRenderTexture)
            gdRenderTexture->resize({ static_cast<float>(width), static_cast<float>(height) });
    }
};

class $modify(CCNode) {

    bool changerNotEffect = false;

    void addChild(CCNode * child, int zOrder, int tag) {

        if ((typeinfo_cast<GJDropDownLayer*>(child) || typeinfo_cast<DialogLayer*>(child)
            || typeinfo_cast<CharacterColorPage*>(child) || typeinfo_cast<ShardsPage*>(child)
            || typeinfo_cast<ShardsPage*>(child) || typeinfo_cast<CommunityCreditsPage*>(child)
            || typeinfo_cast<FLAlertLayer*>(child)) && !exact_cast<CCScene*>(this)) {
            CCTransitionScene::get()->addChild(child);
            if (auto shaderScene_ = typeinfo_cast<ShaderScene*>(CCTransitionScene::get()->getChildByID("ShaderScene-node")); shaderScene_) {
                if (!shaderScene_->inChanged) {
                    shaderScene_->updateChilds();
                }
            }
            return;
        }

            CCNode::addChild(child, zOrder, tag);



        if (((exact_cast<CCScene*>(this) && !typeinfo_cast<CCTransitionScene*>(this))) && !m_fields->changerNotEffect) {
            if (auto shaderScene_ = typeinfo_cast<ShaderScene*>(this->getChildByID("ShaderScene-node")); shaderScene_) {
                if (!shaderScene_->inChanged) {
                    shaderScene_->updateChilds();
                }
            }
        }
    }

    void removeChild(CCNode * child, bool cleanup) {
        CCNode::removeChild(child, cleanup);

        if ((exact_cast<CCScene*>(this) && !typeinfo_cast<CCTransitionScene*>(this) && !m_fields->changerNotEffect)) {
            if (auto shaderScene_ = typeinfo_cast<ShaderScene*>(this->getChildByID("ShaderScene-node")); shaderScene_) {
               if (!shaderScene_->inChanged) {
                    shaderScene_->updateChilds();
               }
            }
        }
    }
};

class $modify(CCLayer) {

    bool oneAttempt = false;


    void keyDown(enumKeyCodes key) {
        /*
        if (m_fields->oneAttempt) {
            m_fields->oneAttempt = false;
            CCLayer::keyDown(key);
            return;
        }

        auto e = Utils::hasParentWithID(this, "ShaderScene-node");

        std::cout << e << std::endl;

        if (e) {

            if (CCTransitionScene::get() && !CCDirector::get()->getIsTransitioning()) {
                auto lay = typeinfo_cast<CCLayer*>(CCTransitionScene::get()->getChildren()->lastObject());
                if (lay) {
                    std::cout << Utils::getNameObj(lay) << std::endl;
                    if (auto shaderScene_ = typeinfo_cast<ShaderScene*>(CCTransitionScene::get()->getChildByID("ShaderScene-node")); shaderScene_) {
                        lay->retain();
                        lay->keyDown(key);
                        lay->release();
                    }
                    m_fields->oneAttempt = true;
                }
            }

            return;
        }
        /*
        if (!CCDirector::get()->getIsTransitioning()) {

            auto childrens = CCTransitionScene::get()->getChildByID("ShaderScene-node");
            CCObject* obj = nullptr;

            }
        }
        */
        CCLayer::keyDown(key);
    }

};

static void createShaderScene_(CCScene* pScene) {
    auto utilScene = pScene;

    if (auto transScene = typeinfo_cast<CCTransitionScene*>(pScene); transScene) {
        utilScene = public_cast(transScene, m_pInScene);
    }

    if (utilScene->getChildByID("ShaderScene-node")) {
        return;
    }

    std::cout << "ShaderScene-node" << std::endl;

    auto shaderScene = ShaderScene::create(utilScene);
    shaderScene->setID("ShaderScene-node");
    utilScene->addChild(shaderScene, INT_MIN);
}

class $modify(CCDirector) {
    bool replaceScene(CCScene * pScene) {
        auto ret = CCDirector::replaceScene(pScene);
        createShaderScene_(pScene);
        return ret;
    }

    bool pushScene(CCScene* pScene) {
        auto ret = CCDirector::pushScene(pScene);
        createShaderScene_(pScene);
        return ret;
    }
};

$on_mod(Loaded) {
    Blur::compileBlurShader();
    matdash::create_console();
}
