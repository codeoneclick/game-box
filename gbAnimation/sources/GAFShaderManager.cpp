#include "GAFPrecompiled.h"
#include "GAFShaderManager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
namespace gps
{
#include "ShadersPrecompiled/GAFPrecompiledShaders.h"
}
#include "CCPrecompiledShaders.h"
#endif

using std::string;
USING_NS_CC;

#define STRINGIFY(A)  #A

#include "Shaders/GaussianBlurFragmentShader.frag"
#include "Shaders/GlowFragmentShader.frag"
#include "Shaders/pcShader_PositionTextureAlpha_frag.frag"

NS_GAF_BEGIN

    gaf::GAFShaderManager* gaf::GAFShaderManager::m_instance = nullptr;

    gaf::GAFShaderManager::GAFShaderManager()
    {
        m_instance = this;

        s_initialized = false;

        s_fragmentShaders[EFragmentShader::GaussBlur] = GaussianBlurFragmentShader_fs;
        s_fragmentShaders[EFragmentShader::Glow] = GlowFragmentShader_fs;
        s_fragmentShaders[EFragmentShader::Alpha] = pcShader_PositionTextureAlpha_frag_fs;

        s_programs[EPrograms::Alpha] = nullptr;
        s_programs[EPrograms::Blur] = nullptr;
        s_programs[EPrograms::Glow] = nullptr;

        
        s_uniformNames[EUniforms::ColorTransformMult] = "colorTransformMult";
        s_uniformNames[EUniforms::ColorTransformOffset] = "colorTransformOffsets";

        s_uniformNames[EUniforms::BlurTexelOffset] = "u_step";

        s_uniformNames[EUniforms::GlowTexelOffset] = "u_step";
        s_uniformNames[EUniforms::GlowColor] = "u_glowColor";

        s_uniformNames[EUniforms::Strength] = "u_strength";

        s_uniformLocations[EUniforms::ColorTransformMult] = s_uniformLocations[EUniforms::ColorTransformOffset] = s_uniformLocations[EUniforms::BlurTexelOffset] =

        s_uniformLocations[EUniforms::GlowTexelOffset] = s_uniformLocations[EUniforms::GlowColor] =
        s_uniformLocations[EUniforms::Strength] = -1;
        
    }

    gaf::GAFShaderManager::~GAFShaderManager()
    {
        m_instance = nullptr;

        CC_SAFE_RELEASE(s_programs[EPrograms::Alpha]);
        CC_SAFE_RELEASE(s_programs[EPrograms::Blur]);
        CC_SAFE_RELEASE(s_programs[EPrograms::Glow]);

    }

    void GAFShaderManager::renderRecreate(EventCustom*)
    {
        GAFShaderManager::getRef().Initialize(true);
        CCLOG("RENDER recreated");
    }

    void GAFShaderManager::Initialize(bool force /*= false*/)
    {
        if (!s_initialized)
        {
#if COCOS2D_VERSION < 0x00030200
            const std::string eventName = EVENT_COME_TO_FOREGROUND;
#else
            const std::string eventName = EVENT_RENDERER_RECREATED;
#endif
            Director::getInstance()->getEventDispatcher()->addCustomEventListener(eventName, GAFShaderManager::renderRecreate);
        }

        bool skip = !force && s_initialized;
        bool reinit = force && s_initialized;

        if (!skip)
        {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
            for (unsigned int i = 0; i < gps::s_numPrograms; ++i)
            {
                cocos2d::
                    CCPrecompiledShaders::getInstance()->addPrecompiledProgram(
                    gps::s_programKeys[i],
                    gps::s_programs[i],
                    gps::s_programLengths[i]);
            }
#endif

            // Alpha
            {
                const char* fragmentShader = getShader(EFragmentShader::Alpha);
                GLProgram* program = nullptr;
                if (reinit)
                {
                    program = s_programs[EPrograms::Alpha];
                    program->reset();
                    CCASSERT(program->initWithByteArrays(cocos2d::ccPositionTextureColor_vert, fragmentShader), "`Alpha` shader init error");
                    CCASSERT(program->link(), "`Alpha` shader linking error");
                    program->updateUniforms();
                    CHECK_GL_ERROR_DEBUG();
                }
                else
                {
                    program = GLProgram::createWithByteArrays(cocos2d::ccPositionTextureColor_vert, fragmentShader);

                    CC_SAFE_RELEASE(s_programs[EPrograms::Alpha]);
                    s_programs[EPrograms::Alpha] = program;
                    CC_SAFE_RETAIN(s_programs[EPrograms::Alpha]);
                    CHECK_GL_ERROR_DEBUG();
                }
                CCASSERT(program, "`Alpha` shader not loaded.");

                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_POSITION, cocos2d::GLProgram::VERTEX_ATTRIB_POSITION);
                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_COLOR, cocos2d::GLProgram::VERTEX_ATTRIB_COLOR);
                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_TEX_COORD, cocos2d::GLProgram::VERTEX_ATTRIB_TEX_COORDS);
                s_uniformLocations[EUniforms::ColorTransformMult] =     glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::ColorTransformMult]);
                s_uniformLocations[EUniforms::ColorTransformOffset] =   glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::ColorTransformOffset]);
                CHECK_GL_ERROR_DEBUG();
            }
            
            // Blur
            {
                const char* fs = getShader(EFragmentShader::GaussBlur);
                GLProgram* program = nullptr;
                if (reinit)
                {
                    program = s_programs[EPrograms::Blur];
                    program->reset();
                    CCASSERT(program->initWithByteArrays(cocos2d::ccPositionTextureColor_vert, fs), "`Blur` shader init error");
                    CCASSERT(program->link(), "`Blur` shader linking error");
                    program->updateUniforms();
                    CHECK_GL_ERROR_DEBUG();
                }
                else
                {
                    program = GLProgram::createWithByteArrays(cocos2d::ccPositionTextureColor_vert, fs);
                    CC_SAFE_RELEASE(s_programs[EPrograms::Blur]);
                    s_programs[EPrograms::Blur] = program;
                    CC_SAFE_RETAIN(s_programs[EPrograms::Blur]);
                    CHECK_GL_ERROR_DEBUG();
                }

                CCASSERT(program, "`Blur` shader not loaded.");

                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_POSITION, cocos2d::GLProgram::VERTEX_ATTRIB_POSITION);
                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_COLOR, cocos2d::GLProgram::VERTEX_ATTRIB_COLOR);
                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_TEX_COORD, cocos2d::GLProgram::VERTEX_ATTRIB_TEX_COORDS);
                s_uniformLocations[EUniforms::BlurTexelOffset] = glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::BlurTexelOffset]);
                CHECK_GL_ERROR_DEBUG();
            }

            // Glow
            {
                const char* fs = GAFShaderManager::getShader(GAFShaderManager::EFragmentShader::Glow);
                GLProgram* program = nullptr;
                if (reinit)
                {
                    program = s_programs[EPrograms::Glow];
                    program->reset();
                    CCASSERT(program->initWithByteArrays(cocos2d::ccPositionTextureColor_vert, fs), "`Glow` shader init error");
                    CCASSERT(program->link(), "`Glow` shader linking error");
                    program->updateUniforms();
                    CHECK_GL_ERROR_DEBUG();
                }
                else
                {
                    program = GLProgram::createWithByteArrays(cocos2d::ccPositionTextureColor_vert, fs);
                    CC_SAFE_RELEASE(s_programs[EPrograms::Glow]);
                    s_programs[EPrograms::Glow] = program;
                    CC_SAFE_RETAIN(s_programs[EPrograms::Glow]);
                    CHECK_GL_ERROR_DEBUG();
                }

                CCASSERT(program, "`Glow` shader not loaded.");

                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_POSITION, cocos2d::GLProgram::VERTEX_ATTRIB_POSITION);
                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_COLOR, cocos2d::GLProgram::VERTEX_ATTRIB_COLOR);
                program->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_TEX_COORD, cocos2d::GLProgram::VERTEX_ATTRIB_TEX_COORDS);
                s_uniformLocations[EUniforms::GlowTexelOffset] = glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::GlowTexelOffset]);
                s_uniformLocations[EUniforms::GlowColor] = glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::GlowColor]);
                s_uniformLocations[EUniforms::Strength] = glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::Strength]);
                CHECK_GL_ERROR_DEBUG();

            }
            s_initialized = true;
        }
    }
NS_GAF_END
