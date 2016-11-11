#pragma once

#include "GAFQuadCommand.h"
#include "GAFCollections.h"

#include <array>

NS_GAF_BEGIN

typedef struct _gafBlendFuncSeparate
{
    // source blend function
    GLenum src;
    // destination blend function
    GLenum dst;
    // source alpha channel blend function
    GLenum srcAlpha;
    // destination alpha channel blend function
    GLenum dstAlpha;

} gafBlendFuncSeparate;

/// @class GAFSprite 
///	This is utility class used by GAF playback. Base class for all sprites
/// used by GAF.

class GAFSprite : public cocos2d::Sprite
{
public:
    GAFSprite();
    virtual ~GAFSprite();

    virtual bool initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame, GAFRotation rotation);
    virtual bool initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame, GAFRotation rotation, const cocos2d::Rect& capInsets);
    virtual bool initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame) override;
    virtual bool initWithTexture(cocos2d::Texture2D *pTexture, const cocos2d::Rect& rect, bool rotated) override;
    virtual bool initWithTexture(cocos2d::Texture2D *pTexture, const cocos2d::Rect& rect, bool rotated, const cocos2d::Rect& capInsets);
    virtual void setTexture(cocos2d::Texture2D *texture) override;

    virtual void setVertexRect(const cocos2d::Rect& rect) override;
    virtual void setTextureRect(const cocos2d::Rect& rect, bool rotated, const cocos2d::Size& untrimmedSize) override;
    virtual void setTextureCoords(cocos2d::Rect rect) override;

    virtual void setExternalTransform(const cocos2d::AffineTransform& transform);
    virtual const cocos2d::AffineTransform& getExternalTransform() const;

    virtual const cocos2d::Mat4& getNodeToParentTransform() const override;
    virtual cocos2d::AffineTransform getNodeToParentAffineTransform() const override;
#if COCOS2D_VERSION < 0x00030200
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated) override;
#else
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
#endif
    
    void setAtlasScale(float scale);

    virtual void setLocator(bool locator)
    {
        m_isLocator = locator;
    };

    inline float getAtlasScale() const { return m_atlasScale; }
    
protected:

    /**
    * Sets Uniforms for shader.
    *
    * Will be called before task with current program state is pushed to
    * the render queue. Must return hash of the current shader program 
    * state including uniforms, textures, program using. If 0 is returned
    * renderer will not attempt to batch this call.
    *
    * @return  hash value of the shader program state.
    */
    virtual uint32_t setUniforms();


    /**
    * Manually draws sprite.
    *
    * This call will set up a GL state and manually draw sprite.
    * Used as callback for m_customCommand
    *
    * @property model-view-projection matrix for current sprite
    */
    virtual void customDraw(cocos2d::Mat4& transform);

    void updateSlicedQuads(const cocos2d::Mat4 &transform);

    void updateColor() override;
private:
    typedef std::array<cocos2d::V3F_C4B_T2F_Quad, 9> Scale9Slices_t;

    void updateScale9GridQuads();
    /* Members */
public:
    uint32_t                  objectIdRef;
protected:
    cocos2d::AffineTransform  m_externalTransform;
    cocos2d::CustomCommand    m_customCommand;
    GAFQuadCommand            m_quadCommand;
    bool                      m_isLocator;

private:
    /**
    * Quad is equal to _quad but transformed to view space
    */
    cocos2d::V3F_C4B_T2F_Quad m_quad;

    // Scale9Grid -------------------------------------------------------------------------------------
    Scale9Slices_t            m_scale9Slices; // holds the 9 quads (tl, tc, tr, cl, c, cr, bl, bc, br);

    bool                      m_scale9Enabled;
    cocos2d::Rect             m_capInsets;

    cocos2d::Size             m_topLeftSize;
    cocos2d::Size             m_centerSize;
    cocos2d::Size             m_bottomRightSize;

    //-------------------------------------------------------------------------------------------------

    float                     m_atlasScale;

    /* Never used */
    gafBlendFuncSeparate      m_blendFuncSeparate;
    bool                      m_useSeparateBlendFunc;
    GLint                     m_blendEquation;

    GAFRotation               m_rotation;
};

NS_GAF_END
