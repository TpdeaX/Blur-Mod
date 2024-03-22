#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class RenderTexture
{
  private:
	GLuint framebuffer = 0;
	GLuint renderbuffer = 0;
	GLuint textureID = 0;

  public:
	 cocos2d::CCPoint resolution = {0, 0};

  public:

    static std::shared_ptr<RenderTexture> create(cocos2d::CCPoint resolution);
    bool init(cocos2d::CCPoint resolution);

    void resize(cocos2d::CCPoint resolution);

    void bind();
    void unbind();

    void clear();

	GLuint getTexture()
	{
		return textureID;
	}
};