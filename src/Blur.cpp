#include "Blur.hpp"

using namespace geode::prelude;
using namespace Blur;

struct Vertex {
	float position[3];  // 12 bytes
	unsigned char color[4];     // 4 bytes
	float texCoords[2];  // 8 bytes
};


const char* const vertexShaderCode = R"(													
attribute vec4 a_position;							
attribute vec2 a_texCoord;							
attribute vec4 a_color;								
													
#ifdef GL_ES										
varying lowp vec4 v_fragmentColor;					
varying mediump vec2 v_texCoord;					
#else												
varying vec4 v_fragmentColor;						
varying vec2 v_texCoord;							
#endif												
													
void main()											
{													
    gl_Position = CC_MVPMatrix * a_position;		
	v_fragmentColor = a_color;						
	v_texCoord = a_texCoord;						
}													
)";

const char* const fragmentShaderCode = R"(
#ifdef GL_ES								
precision lowp float;						
#endif										
																					
varying vec4 v_fragmentColor;				
varying vec2 v_texCoord;					
uniform sampler2D CC_Texture0;
uniform float blurDarkness;		
uniform float blurSize;		
uniform float blurAmount;		
uniform int blurSteps;	
											
void main()									
{
	float pi = 3.1415926;
	float sigma = 5.;

	vec4 sum = vec4(0.);

	float scaledBlurSize = blurAmount * blurSize * 0.001;

	for(int x = -blurSteps; x <= blurSteps; x++){
        for(int y = -blurSteps; y <= blurSteps; y++){
            vec2 newUV = v_texCoord + vec2(float(x) * scaledBlurSize, float(y) * scaledBlurSize);
            sum += texture(CC_Texture0, newUV) * (exp(-(pow(float(x), 2.) + pow(float(y), 2.)) / (2. * pow(sigma, 2.))) / (2. * pi * pow(sigma, 2.)));
        }   
    }

	//sum *= vec4(0.1, 0.1, 0.1, 1);

	sum.a = 1.1f - blurDarkness;			
											
    gl_FragColor = sum;
})";

void Blur::compileBlurShader()
{
	if (compiled)
		return;

	blurProgram = new cocos2d::CCGLProgram();
	blurProgram->initWithVertexShaderByteArray(vertexShaderCode, fragmentShaderCode);
	blurProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	blurProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	blurProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
	blurProgram->link();
	blurProgram->updateUniforms();

	auto screenSize = CCDirector::sharedDirector()->getWinSize();

	Vertex vertices[] = {
		{ 0.f, 0, 0.0f, 255, 255, 255, 255, 0.0f, 0.0f },
		{ screenSize.width + 0.1, 0.f, 0.0f, 255, 255, 255, 255, 1.0f, 0.0f },
		{ screenSize.width + 0.1, screenSize.height, 0.0f, 255, 255, 255, 255, 1.0f, 1.0f },
		{ 0,  screenSize.height + 0, 0.0f, 255, 255, 255, 255, 0.0f, 1.0f },
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (GLvoid*)12);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)16);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	compiled = true;
}

void Blur::setBlurUniforms()
{
	blurProgram->setUniformsForBuiltins();
	//float blurDarkness = Settings::get<float>("menu/blur/darkness", 1.f);
	//float blurSize = Settings::get<float>("menu/blur/size", 1.f);
	//int blurSteps = Settings::get<int>("menu/blur/steps", 10);

	if (darknessUniform == -1)
		darknessUniform = blurProgram->getUniformLocationForName("blurDarkness");
	else
		blurProgram->setUniformLocationWith1f(darknessUniform, 1.f);

	if (stepsUniform == -1)
		stepsUniform = blurProgram->getUniformLocationForName("blurSteps");
	else
		blurProgram->setUniformLocationWith1i(stepsUniform, 10);

	if (sizeUniform == -1)
		sizeUniform = blurProgram->getUniformLocationForName("blurSize");
	else
		blurProgram->setUniformLocationWith1f(sizeUniform, 1.f);

	if (amountUniform == -1)
		amountUniform = blurProgram->getUniformLocationForName("blurAmount");
	else
		blurProgram->setUniformLocationWith1f(amountUniform, blurAmount);
}

void Blur::blurCallback()
{
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);
	blurProgram->use();
	glBindTexture(GL_TEXTURE_2D, gdRenderTexture->getTexture());
	setBlurUniforms();
}

void Blur::resetCallback()
{
	glBindTexture(GL_TEXTURE_2D, oldTexture);
	auto* shader = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor);
	shader->use();
	shader->setUniformsForBuiltins();
}