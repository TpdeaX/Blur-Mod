#include "Utils.hpp"


namespace Utils
{
	const char* getNameObj(cocos2d::CCNode* obj)
	{
		return (typeid(*obj).name() + 6);
	}

	cocos2d::CCSize WinSize() {
		return CCDirector::get()->getWinSize();
	}

	bool hasParentWithID(cocos2d::CCNode* node, const std::string& parentID) {
		cocos2d::CCNode* parentNode = node->getParent();
		while (parentNode) {
			if (parentNode->getID() == parentID) {
				return true;
			}
			parentNode = parentNode->getParent();
		}
		return false;
	}

	cocos2d::CCNode* getParentWithID(cocos2d::CCNode* node, const std::string& parentID) {
		cocos2d::CCNode* parentNode = node->getParent();
		while (parentNode) {
			if (parentNode->getID() == parentID) {
				return parentNode;
			}
			parentNode = parentNode->getParent();
		}
		return nullptr;
	}

}