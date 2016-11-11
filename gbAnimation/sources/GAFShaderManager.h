#pragma once

NS_GAF_BEGIN

class GAFShaderManager
{
private:
    static GAFShaderManager* m_instance;

public:

    GAFShaderManager();
    ~GAFShaderManager();

    static GAFShaderManager& getRef()
    {
        assert(m_instance);

        return *m_instance;
    }

    static GAFShaderManager* getPtr()
    {
        return m_instance;
    }

    static void renderRecreate(cocos2d::EventCustom*);

    struct EFragmentShader {
        enum Name {
            GaussBlur = 0,
            Glow,
            Alpha,

            SIZE
        };
    };
    
    struct EPrograms {
        enum Name {
            Alpha = 0,
            Blur,
            Glow,

            SIZE
        };
    };

    struct EUniforms {
        enum Name {
            ColorTransformMult = 0,
            ColorTransformOffset,

            BlurTexelOffset,

            GlowTexelOffset,
            GlowColor,

            Strength,


            SIZE
        };
    };


    inline const char* getShader(EFragmentShader::Name n) { return s_fragmentShaders[n]; }

    inline cocos2d::GLProgram* getProgram(EPrograms::Name n) { return s_programs[n]; }
    inline GLint getUniformLocation(EUniforms::Name n) { return s_uniformLocations[n]; }
    inline const char* getUniformName(EUniforms::Name n) { return s_uniformNames[n]; }

    void Initialize(bool force = false);

private:
    const char* s_fragmentShaders[EFragmentShader::SIZE];
    cocos2d::GLProgram* s_programs[EPrograms::SIZE];
    const char* s_uniformNames[EUniforms::SIZE];
    GLint s_uniformLocations[EUniforms::SIZE];

    bool s_initialized;
}; // GAFShaderManager

NS_GAF_END
