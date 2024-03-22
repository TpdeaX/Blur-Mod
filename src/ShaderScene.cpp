#include "ShaderScene.hpp"

ShaderScene* ShaderScene::create(CCScene* pScene) {
	auto ret = new ShaderScene();
	if (ret && ret->init(pScene)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool ShaderScene::init(CCScene* pScene) {
	if (!CCLayer::init()) {
		return false;
	}

	refScene = pScene;

    this->setKeyboardEnabled(false);
    this->setTouchEnabled(false);
    this->unregisterScriptKeypadHandler();

	return true;
}

void ShaderScene::updateChilds() {
    inChanged = true;

    if (refScene->getChildByID("PlayLayer")) {
        return;
    }

    if (refScene->getChildrenCount() > 2) {
        while (refScene->getChildrenCount() > 2) {
            auto node = typeinfo_cast<CCNode*>(refScene->getChildren()->objectAtIndex(1));
            node->retain();
            refScene->removeChild(node, false);
            this->addChild(node);/*
            auto laur = typeinfo_cast<CCLayer*>(node);
            if (laur) {
                cocos2d::CCKeyboardDispatcher::get()->forceRemoveDelegate(laur);
                laur->setKeyboardEnabled(false);
                laur->setKeypadEnabled(false);
                laur->unregisterScriptKeypadHandler();
            } 
            node->release();
        }

        auto node = typeinfo_cast<CCNode*>(refScene->getChildren()->objectAtIndex(1));
        if (node) {
            node->retain();
            auto laur = typeinfo_cast<CCLayer*>(node);
            if (laur) {
                cocos2d::CCKeyboardDispatcher::get()->forceAddDelegate(laur);
                laur->setKeyboardEnabled(true);
                laur->setKeypadEnabled(true);
                laur->registerScriptKeypadHandler(500);
            }
            */
            node->release();
        }
       
    } 
    else if (refScene->getChildrenCount() < 2 && this->getChildrenCount() > 0) {

            auto node = typeinfo_cast<CCNode*>(this->getChildren()->lastObject());
            node->retain();
            this->removeChild(node, false);
            refScene->addChild(node, node->getZOrder(), node->getTag());
            node->release();
    }

    inChanged = false;
}

void ShaderScene::visit() {
    if (!Blur::compiled)
        Blur::compileBlurShader();
    if (!Blur::gdRenderTexture) {
        auto winSize = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
        Blur::gdRenderTexture = RenderTexture::create({ winSize.size.width, winSize.size.height });
    }
    Blur::gdRenderTexture->bind();
    Blur::gdRenderTexture->clear();
    CCNode::visit();
    Blur::gdRenderTexture->unbind();
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &Blur::oldTexture);
    glBindTexture(GL_TEXTURE_2D, Blur::gdRenderTexture->getTexture());
    Blur::blurProgram->use();
    Blur::setBlurUniforms();
    glBindVertexArray(Blur::VAO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, Blur::oldTexture);
    auto* shader = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor);
    shader->use();
    shader->setUniformsForBuiltins();
}
