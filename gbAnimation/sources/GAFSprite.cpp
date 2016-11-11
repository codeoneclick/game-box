#include "GAFPrecompiled.h"
#include "GAFSprite.h"
#include "GAFCollections.h"

#include "math/TransformUtils.h"
#include "../external/xxhash/xxhash.h"

USING_NS_CC;

#if CC_SPRITEBATCHNODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__A__) ( (int)(__A__))
#endif

NS_GAF_BEGIN

static void setQuadUVs(V3F_C4B_T2F_Quad &quad, const Rect &rect,
    const GAFRotation rotation = GAFRotation::NONE,
    bool flippedX = false, bool flippedY = false)
{
    float left = rect.origin.x;
    float right = rect.origin.x + rect.size.width;
    float top = rect.origin.y;
    float bottom = rect.origin.y + rect.size.height;

    switch (rotation)
    {
    case gaf::GAFRotation::CCW_90:
    {
        if (flippedX)
            std::swap(top, bottom);

        if (flippedY)
            std::swap(left, right);

        quad.bl.texCoords.u = right;
        quad.bl.texCoords.v = bottom;
        quad.br.texCoords.u = right;
        quad.br.texCoords.v = top;
        quad.tl.texCoords.u = left;
        quad.tl.texCoords.v = bottom;
        quad.tr.texCoords.u = left;
        quad.tr.texCoords.v = top;
    }
    break;

    case gaf::GAFRotation::CW_90:
    {
        if (flippedX)
            std::swap(top, bottom);

        if (flippedY)
            std::swap(left, right);

        quad.bl.texCoords.u = left;
        quad.bl.texCoords.v = top;
        quad.br.texCoords.u = left;
        quad.br.texCoords.v = bottom;
        quad.tl.texCoords.u = right;
        quad.tl.texCoords.v = top;
        quad.tr.texCoords.u = right;
        quad.tr.texCoords.v = bottom;
    }
    break;

    case gaf::GAFRotation::NONE:
    default:
    {
        if (flippedX)
            std::swap(left, right);

        if (flippedY)
            std::swap(top, bottom);

        quad.bl.texCoords.u = left;
        quad.bl.texCoords.v = bottom;
        quad.br.texCoords.u = right;
        quad.br.texCoords.v = bottom;
        quad.tl.texCoords.u = left;
        quad.tl.texCoords.v = top;
        quad.tr.texCoords.u = right;
        quad.tr.texCoords.v = top;
    }
    break;
    }
}

GAFSprite::GAFSprite()
    : objectIdRef(IDNONE)
    , m_externalTransform(AffineTransform::IDENTITY)
    , m_scale9Enabled(false)
    , m_capInsets(Rect::ZERO)
    , m_isLocator(false)
    , m_useSeparateBlendFunc(false)
    , m_blendEquation(-1)
    , m_atlasScale(1.0f)
    , m_rotation(GAFRotation::NONE)
{
#if COCOS2D_VERSION < 0x00030300
    _batchNode = nullptr; // this will fix a bug in cocos2dx 3.2 tag
#endif
    setFlippedX(false); // Fix non-inited vars in cocos
    setFlippedY(false);
    _rectRotated = false;
}

GAFSprite::~GAFSprite()
{
}

void GAFSprite::updateScale9GridQuads()
{
    auto width = _rect.size.width;
    auto height = _rect.size.height;

    // If there is no specified center region
    if (m_capInsets.equals(Rect::ZERO))
    {
        // log("... cap insets not specified : using default cap insets ...");
        m_capInsets = Rect(width / 3, height / 3, width / 3, height / 3);
    }

    m_topLeftSize.width = m_capInsets.origin.x;
    m_topLeftSize.height = m_capInsets.origin.y;

    m_centerSize.width = m_capInsets.size.width;
    m_centerSize.height = m_capInsets.size.height;

    m_bottomRightSize.width = _rect.size.width - (m_topLeftSize.width + m_centerSize.width);
    m_bottomRightSize.height = _rect.size.height - (m_topLeftSize.height + m_centerSize.height);
}

bool GAFSprite::initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame, GAFRotation rotation)
{
    m_rotation = rotation;
    return initWithSpriteFrame(spriteFrame);
}

bool GAFSprite::initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame, GAFRotation rotation, const cocos2d::Rect& capInsets)
{
    m_capInsets = capInsets;

    m_scale9Enabled = !m_capInsets.equals(Rect::ZERO);

    bool bRet = initWithSpriteFrame(spriteFrame, rotation);

    if (m_scale9Enabled)
        updateScale9GridQuads();

    return bRet;
}

bool GAFSprite::initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame)
{
    CCASSERT(spriteFrame != nullptr, "");

    bool bRet = cocos2d::Sprite::initWithTexture(spriteFrame->getTexture(), spriteFrame->getRect());
    setSpriteFrame(spriteFrame);

    return bRet;
}

bool GAFSprite::initWithTexture(cocos2d::Texture2D *pTexture, const cocos2d::Rect& rect, bool rotated)
{
    if (cocos2d::Sprite::initWithTexture(pTexture, rect, rotated))
    {
        setGLProgram(cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
        return true;
    }
    else
    {
        return false;
    }
}

bool GAFSprite::initWithTexture(cocos2d::Texture2D *pTexture, const cocos2d::Rect& rect, bool rotated, const cocos2d::Rect& capInsets)
{
    m_capInsets = capInsets;

    m_scale9Enabled = !m_capInsets.equals(Rect::ZERO);

    bool bRet = initWithTexture(pTexture, rect, rotated);

    if (m_scale9Enabled)
        updateScale9GridQuads();

    return bRet;
}

void GAFSprite::setTexture(cocos2d::Texture2D *texture)
{
    // If batch node, then texture id should be the same
    CC_ASSERT((!_batchNode || texture->getName() == _batchNode->getTexture()->getName()) && "cocos2d::Sprite: Batched sprites should use the same texture as the batchnode");
    // accept texture==nil as argument
    CC_ASSERT((!texture || dynamic_cast<cocos2d::Texture2D*>(texture)) && "setTexture expects a cocos2d::Texture2D. Invalid argument");

    if (!_batchNode && _texture != texture)
    {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE(_texture);
        _texture = texture;
        updateBlendFunc();
    }
}

void GAFSprite::setVertexRect(const cocos2d::Rect& rect)
{
    _rect = rect;
    if (m_rotation != GAFRotation::NONE)
    {
        std::swap(_rect.size.width, _rect.size.height);
    }
}

void GAFSprite::setTextureRect(const cocos2d::Rect& rect, bool rotated, const cocos2d::Size& untrimmedSize)
{
    cocos2d::Size rotatedSize = untrimmedSize;
    if (m_rotation != GAFRotation::NONE)
    {
        rotatedSize = cocos2d::Size(rotatedSize.height, rotatedSize.width);
    }
    cocos2d::Sprite::setTextureRect(rect, rotated, rotatedSize);
}

void GAFSprite::setTextureCoords(cocos2d::Rect rect)
{
    rect = CC_RECT_POINTS_TO_PIXELS(rect);

    cocos2d::Texture2D *tex = _batchNode ? _textureAtlas->getTexture() : _texture;

    if (!tex) 
        return;

    float atlasWidth = static_cast<float>(tex->getPixelsWide());
    float atlasHeight = static_cast<float>(tex->getPixelsHigh());

    rect.origin.x /= atlasWidth;
    rect.origin.y /= atlasHeight;
    rect.size.width /= atlasWidth;
    rect.size.height /= atlasHeight;

    setQuadUVs(_quad, rect, m_rotation, _flippedX, _flippedY);

    if (m_scale9Enabled)
    {
        Tex2F tl = _quad.tl.texCoords;
        Tex2F tr = _quad.tr.texCoords;
        Tex2F bl = _quad.bl.texCoords;
        Tex2F br = _quad.br.texCoords;

        V3F_C4B_T2F_Quad &tlq = m_scale9Slices[0];
        V3F_C4B_T2F_Quad &tcq = m_scale9Slices[1];
        V3F_C4B_T2F_Quad &trq = m_scale9Slices[2];
        V3F_C4B_T2F_Quad &clq = m_scale9Slices[3];
        V3F_C4B_T2F_Quad &ccq = m_scale9Slices[4];
        V3F_C4B_T2F_Quad &crq = m_scale9Slices[5];
        V3F_C4B_T2F_Quad &blq = m_scale9Slices[6];
        V3F_C4B_T2F_Quad &bcq = m_scale9Slices[7];
        V3F_C4B_T2F_Quad &brq = m_scale9Slices[8];

        Rect originalRect = _rect;

        float leftWidthRatio = m_capInsets.origin.x / originalRect.size.width;
        float centerWidthRatio = m_capInsets.size.width / originalRect.size.width;
        float rightWidthRatio = 1.f - (leftWidthRatio + centerWidthRatio);

        float topHeightRatio = m_capInsets.origin.y / originalRect.size.height;
        float centerHeightRatio = m_capInsets.size.height / originalRect.size.height;
        float bottomHeightRatio = 1.f - (topHeightRatio + centerHeightRatio);

        Vec2 quadBaseUVVec = { tl.u, tl.v };

        Vec2 quadWidthUVVec = { tr.u - tl.u, tr.v - tl.v };
        Vec2 quadHeightUVVec = { bl.u - tl.u, bl.v - tl.v };

        Vec2 leftUVVec = quadWidthUVVec; leftUVVec.scale(leftWidthRatio);
        Vec2 leftCenterUVVec = quadWidthUVVec; leftCenterUVVec.scale(centerWidthRatio);
        Vec2 rightUVVec = quadWidthUVVec; rightUVVec.scale(rightWidthRatio);

        Vec2 topUVVec = quadHeightUVVec; topUVVec.scale(topHeightRatio);
        Vec2 topCenterUVVec = quadHeightUVVec; topCenterUVVec.scale(centerHeightRatio);
        Vec2 bottomUVVec = quadHeightUVVec; bottomUVVec.scale(bottomHeightRatio);

        Vec2 temp = quadBaseUVVec;

        // 1 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        tlq.tl.texCoords = tl;

        // 0 1 0 0
        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        temp.add(leftUVVec);
        tlq.tr.texCoords = tcq.tl.texCoords = { temp.x, temp.y };

        // 0 0 1 0
        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        temp.add(leftCenterUVVec);
        tcq.tr.texCoords = trq.tl.texCoords = { temp.x, temp.y };

        // 0 0 0 1
        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        trq.tr.texCoords = tr;

        // 0 0 0 0
        // 1 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        temp = quadBaseUVVec;
        temp.add(topUVVec);
        tlq.bl.texCoords = clq.tl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 1 0 0
        // 0 0 0 0
        // 0 0 0 0
        temp.add(leftUVVec);
        tlq.br.texCoords = tcq.bl.texCoords = clq.tr.texCoords = ccq.tl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 1 0
        // 0 0 0 0
        // 0 0 0 0
        temp.add(leftCenterUVVec);
        tcq.br.texCoords = trq.bl.texCoords = ccq.tr.texCoords = crq.tl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 1
        // 0 0 0 0
        // 0 0 0 0
        temp.add(rightUVVec);
        trq.br.texCoords = crq.tr.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 0
        // 1 0 0 0
        // 0 0 0 0
        temp = quadBaseUVVec;
        temp.add(topUVVec);
        temp.add(topCenterUVVec);
        clq.bl.texCoords = blq.tl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 0
        // 0 1 0 0
        // 0 0 0 0
        temp.add(leftUVVec);
        clq.br.texCoords = ccq.bl.texCoords = blq.tr.texCoords = bcq.tl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 0
        // 0 0 1 0
        // 0 0 0 0
        temp.add(leftCenterUVVec);
        ccq.br.texCoords = crq.bl.texCoords = bcq.tr.texCoords = brq.tl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 1
        // 0 0 0 0
        temp.add(rightUVVec);
        crq.br.texCoords = brq.tr.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        // 1 0 0 0
        temp = quadBaseUVVec;
        temp.add(quadHeightUVVec);
        blq.bl.texCoords = bl;

        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        // 0 1 0 0
        temp.add(leftUVVec);
        blq.br.texCoords = bcq.bl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        // 0 0 1 0
        temp.add(leftCenterUVVec);
        bcq.br.texCoords = brq.bl.texCoords = { temp.x, temp.y };

        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 0
        // 0 0 0 1
        brq.br.texCoords = br;

        tlq.tl.colors = tlq.tr.colors = tlq.bl.colors = tlq.br.colors = Color4B::WHITE;
        tcq.tl.colors = tcq.tr.colors = tcq.bl.colors = tcq.br.colors = Color4B::WHITE;
        trq.tl.colors = trq.tr.colors = trq.bl.colors = trq.br.colors = Color4B::WHITE;
        clq.tl.colors = clq.tr.colors = clq.bl.colors = clq.br.colors = Color4B::WHITE;
        ccq.tl.colors = ccq.tr.colors = ccq.bl.colors = ccq.br.colors = Color4B::WHITE;
        crq.tl.colors = crq.tr.colors = crq.bl.colors = crq.br.colors = Color4B::WHITE;
        blq.tl.colors = blq.tr.colors = blq.bl.colors = blq.br.colors = Color4B::WHITE;
        bcq.tl.colors = bcq.tr.colors = bcq.bl.colors = bcq.br.colors = Color4B::WHITE;
        brq.tl.colors = brq.tr.colors = brq.bl.colors = brq.br.colors = Color4B::WHITE;
    }
}

void GAFSprite::updateSlicedQuads(const cocos2d::Mat4 &transform)
{
    //auto currTransform = this->getNodeToParentAffineTransform();
    float a = transform.m[0];
    float b = transform.m[1];
    float c = transform.m[4];
    float d = transform.m[5];

    Size unscaledSize = this->_contentSize;

    Size topLeftSize = m_topLeftSize;
    Size bottomRightSize = m_bottomRightSize;

    float minWidth = topLeftSize.width + bottomRightSize.width;
    float minHeight = topLeftSize.height + bottomRightSize.height;

    float scaleX = std::sqrt(a * a + b * b);
    float scaleY = std::sqrt(c * c + d * d);

    float scaledWidth = unscaledSize.width * scaleX;
    float scaledHeight = unscaledSize.height * scaleY;

    // downscale corners if actional size is smaller than min size
    if (scaledWidth < minWidth)
    {
        float cornersScaleX = scaledWidth / minWidth;
        topLeftSize.width *= cornersScaleX;
        bottomRightSize.width *= cornersScaleX;
    }

    if (scaledHeight < minHeight)
    {
        float cornersScaleY = scaledHeight / minHeight;
        topLeftSize.height *= cornersScaleY;
        bottomRightSize.height *= cornersScaleY;
    }

    Vec2 rlVerticesDelta = { m_quad.tr.vertices.x - m_quad.tl.vertices.x, m_quad.tr.vertices.y - m_quad.tl.vertices.y };
    Vec2 btVerticesDelta = { m_quad.bl.vertices.x - m_quad.tl.vertices.x, m_quad.bl.vertices.y - m_quad.tl.vertices.y };

    float tlWidthRatio = topLeftSize.width / scaledWidth;
    float tlHeightRatio = topLeftSize.height / scaledHeight;
    float brWidthRatio = bottomRightSize.width / scaledWidth;
    float brHeightRatio = bottomRightSize.height / scaledHeight;

    V3F_C4B_T2F_Quad &tlq = m_scale9Slices[0];
    V3F_C4B_T2F_Quad &tcq = m_scale9Slices[1];
    V3F_C4B_T2F_Quad &trq = m_scale9Slices[2];
    V3F_C4B_T2F_Quad &clq = m_scale9Slices[3];
    V3F_C4B_T2F_Quad &ccq = m_scale9Slices[4];
    V3F_C4B_T2F_Quad &crq = m_scale9Slices[5];
    V3F_C4B_T2F_Quad &blq = m_scale9Slices[6];
    V3F_C4B_T2F_Quad &bcq = m_scale9Slices[7];
    V3F_C4B_T2F_Quad &brq = m_scale9Slices[8];

    //---------------------------------------------

    // top
    tlq.tl = m_quad.tl;

    Vec2 tempVec = rlVerticesDelta;
    tempVec.scale(tlWidthRatio);
    tlq.tr.vertices.x = tcq.tl.vertices.x = tlq.tl.vertices.x + tempVec.x;
    tlq.tr.vertices.y = tcq.tl.vertices.y = tlq.tl.vertices.y + tempVec.y;

    tempVec = rlVerticesDelta;
    tempVec.scale(brWidthRatio);
    tcq.tr.vertices.x = trq.tl.vertices.x = m_quad.tr.vertices.x - tempVec.x;
    tcq.tr.vertices.y = trq.tl.vertices.y = m_quad.tr.vertices.y - tempVec.y;

    trq.tr = m_quad.tr;

    // center-top
    tempVec = btVerticesDelta;
    tempVec.scale(tlHeightRatio);
    tlq.bl.vertices.x = clq.tl.vertices.x = tlq.tl.vertices.x + tempVec.x;
    tlq.bl.vertices.y = clq.tl.vertices.y = tlq.tl.vertices.y + tempVec.y;

    tempVec = rlVerticesDelta;
    tempVec.scale(tlWidthRatio);
    tlq.br.vertices.x = tcq.bl.vertices.x = clq.tr.vertices.x = ccq.tl.vertices.x = tlq.bl.vertices.x + tempVec.x;
    tlq.br.vertices.y = tcq.bl.vertices.y = clq.tr.vertices.y = ccq.tl.vertices.y = tlq.bl.vertices.y + tempVec.y;

    tempVec = rlVerticesDelta;
    tempVec.scale(brWidthRatio);
    tcq.br.vertices.x = trq.bl.vertices.x = ccq.tr.vertices.x = crq.tl.vertices.x = tlq.bl.vertices.x + rlVerticesDelta.x - tempVec.x;
    tcq.br.vertices.y = trq.bl.vertices.y = ccq.tr.vertices.y = crq.tl.vertices.y = tlq.bl.vertices.y + rlVerticesDelta.y - tempVec.y;

    trq.br.vertices.x = crq.tr.vertices.x = tlq.bl.vertices.x + rlVerticesDelta.x;
    trq.br.vertices.y = crq.tr.vertices.y = tlq.bl.vertices.y + rlVerticesDelta.y;

    // center-bottom
    tempVec = btVerticesDelta;
    tempVec.scale(brHeightRatio);
    clq.bl.vertices.x = blq.tl.vertices.x = tlq.tl.vertices.x + btVerticesDelta.x - tempVec.x;
    clq.bl.vertices.y = blq.tl.vertices.y = tlq.tl.vertices.y + btVerticesDelta.y - tempVec.y;

    tempVec = rlVerticesDelta;
    tempVec.scale(tlWidthRatio);
    clq.br.vertices.x = ccq.bl.vertices.x = blq.tr.vertices.x = bcq.tl.vertices.x = clq.bl.vertices.x + tempVec.x;
    clq.br.vertices.y = ccq.bl.vertices.y = blq.tr.vertices.y = bcq.tl.vertices.y = clq.bl.vertices.y + tempVec.y;

    tempVec = rlVerticesDelta;
    tempVec.scale(brWidthRatio);
    ccq.br.vertices.x = crq.bl.vertices.x = bcq.tr.vertices.x = brq.tl.vertices.x = clq.bl.vertices.x + rlVerticesDelta.x - tempVec.x;
    ccq.br.vertices.y = crq.bl.vertices.y = bcq.tr.vertices.y = brq.tl.vertices.y = clq.bl.vertices.y + rlVerticesDelta.y - tempVec.y;

    crq.br.vertices.x = brq.tr.vertices.x = clq.bl.vertices.x + rlVerticesDelta.x;
    crq.br.vertices.y = brq.tr.vertices.y = clq.bl.vertices.y + rlVerticesDelta.y;

    // bottom
    blq.bl = m_quad.bl;

    tempVec = rlVerticesDelta;
    tempVec.scale(tlWidthRatio);
    blq.br.vertices.x = bcq.bl.vertices.x = blq.bl.vertices.x + tempVec.x;
    blq.br.vertices.y = bcq.bl.vertices.y = blq.bl.vertices.y + tempVec.y;

    tempVec = rlVerticesDelta;
    tempVec.scale(brWidthRatio);
    bcq.br.vertices.x = brq.bl.vertices.x = m_quad.br.vertices.x - tempVec.x;
    bcq.br.vertices.y = brq.bl.vertices.y = m_quad.br.vertices.y - tempVec.y;

    brq.br = m_quad.br;
}

void GAFSprite::updateColor()
{
    Sprite::updateColor();

    if (m_scale9Enabled)
    {
        Color4B color4(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);

        // special opacity for premultiplied textures
        if (_opacityModifyRGB)
        {
            color4.r *= _displayedOpacity / 255.0f;
            color4.g *= _displayedOpacity / 255.0f;
            color4.b *= _displayedOpacity / 255.0f;
        }

        V3F_C4B_T2F_Quad &tlq = m_scale9Slices[0];
        V3F_C4B_T2F_Quad &tcq = m_scale9Slices[1];
        V3F_C4B_T2F_Quad &trq = m_scale9Slices[2];
        V3F_C4B_T2F_Quad &clq = m_scale9Slices[3];
        V3F_C4B_T2F_Quad &ccq = m_scale9Slices[4];
        V3F_C4B_T2F_Quad &crq = m_scale9Slices[5];
        V3F_C4B_T2F_Quad &blq = m_scale9Slices[6];
        V3F_C4B_T2F_Quad &bcq = m_scale9Slices[7];
        V3F_C4B_T2F_Quad &brq = m_scale9Slices[8];

        tlq.tl.colors = tlq.tr.colors = tlq.bl.colors = tlq.br.colors = color4;
        tcq.tl.colors = tcq.tr.colors = tcq.bl.colors = tcq.br.colors = color4;
        trq.tl.colors = trq.tr.colors = trq.bl.colors = trq.br.colors = color4;
        clq.tl.colors = clq.tr.colors = clq.bl.colors = clq.br.colors = color4;
        ccq.tl.colors = ccq.tr.colors = ccq.bl.colors = ccq.br.colors = color4;
        crq.tl.colors = crq.tr.colors = crq.bl.colors = crq.br.colors = color4;
        blq.tl.colors = blq.tr.colors = blq.bl.colors = blq.br.colors = color4;
        bcq.tl.colors = bcq.tr.colors = bcq.bl.colors = bcq.br.colors = color4;
        brq.tl.colors = brq.tr.colors = brq.bl.colors = brq.br.colors = color4;
    }
}

void GAFSprite::setExternalTransform(const cocos2d::AffineTransform& transform)
{
    if (!cocos2d::AffineTransformEqualToTransform(getExternalTransform(), transform))
    {
        m_externalTransform = transform;
        _transformUpdated = true;
        _transformDirty = true;
        _inverseDirty = true;
    }
}

const cocos2d::AffineTransform& GAFSprite::getExternalTransform() const
{
    return m_externalTransform;
}

const cocos2d::Mat4& GAFSprite::getNodeToParentTransform() const
{
    if (_transformDirty)
    {
        if (m_atlasScale != 1.f)
        {
            cocos2d::AffineTransform transform = cocos2d::AffineTransformScale(getExternalTransform(), m_atlasScale, m_atlasScale);
            cocos2d::CGAffineToGL(cocos2d::AffineTransformTranslate(transform, -_anchorPointInPoints.x, -_anchorPointInPoints.y), _transform.m);
        }
        else
        {
            cocos2d::CGAffineToGL(cocos2d::AffineTransformTranslate(getExternalTransform(), -_anchorPointInPoints.x, -_anchorPointInPoints.y), _transform.m);
        }
        _transformDirty = false;
    }

    return _transform;
}

cocos2d::AffineTransform GAFSprite::getNodeToParentAffineTransform() const
{
    cocos2d::AffineTransform transform;
    if (_transformDirty)
    {
        transform = getExternalTransform();
        if (m_atlasScale != 1.0f)
        {
            transform = cocos2d::AffineTransformScale(transform, m_atlasScale, m_atlasScale);
        }

        cocos2d::CGAffineToGL(cocos2d::AffineTransformTranslate(transform, -_anchorPointInPoints.x, -_anchorPointInPoints.y), _transform.m);
        _transformDirty = false;
    }
    cocos2d::GLToCGAffine(_transform.m, &transform);

    return transform;
}

#if COCOS2D_VERSION < 0x00030200
void GAFSprite::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated)
{
    (void)transformUpdated;
#else
void GAFSprite::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    (void)flags;
#endif
    
    bool isInsideCameraSpace = renderer->checkVisibility(transform, _contentSize);
    if (m_isLocator || !isInsideCameraSpace)
    {
        return;
    }

    uint32_t id = setUniforms();

    if (m_useSeparateBlendFunc || (m_blendEquation != -1))
    {
        m_customCommand.init(_globalZOrder);
        m_customCommand.func = CC_CALLBACK_0(GAFSprite::customDraw, this, transform);
        renderer->addCommand(&m_customCommand);
    }
    else
    {
        m_quad = _quad;

        transform.transformPoint(&m_quad.tl.vertices);
        transform.transformPoint(&m_quad.tr.vertices);
        transform.transformPoint(&m_quad.bl.vertices);
        transform.transformPoint(&m_quad.br.vertices);

        if (!m_scale9Enabled)
        {
            m_quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, &m_quad, 1, Mat4::IDENTITY, id);
        }
        else
        {
            updateSlicedQuads(transform);
            m_quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, &m_scale9Slices[0], 9, Mat4::IDENTITY, id);
        }

        renderer->addCommand(&m_quadCommand);
    }
}

void GAFSprite::setAtlasScale(float scale)
{
    if (m_atlasScale != scale)
    {
        m_atlasScale = scale;
        _transformDirty = true;
        _inverseDirty = true;
    }
}

uint32_t GAFSprite::setUniforms()
{
#if COCOS2D_VERSION < 0x00030300
    uint32_t materialID = QuadCommand::MATERIAL_ID_DO_NOT_BATCH;
#else
    uint32_t materialID = Renderer::MATERIAL_ID_DO_NOT_BATCH;
#endif
    if (_glProgramState->getUniformCount() == 0)
    {
        int glProgram = (int)getGLProgram()->getProgram();
        int intArray[4] = { glProgram, (int)getTexture()->getName(), (int)getBlendFunc().src, (int)getBlendFunc().dst };

        materialID = XXH32((const void*)intArray, sizeof(intArray), 0);
    }
    return materialID;
}

void GAFSprite::customDraw(cocos2d::Mat4& transform)
{
    CC_ASSERT(!_batchNode && "If cocos2d::Sprite is being rendered by CCSpriteBatchNode, cocos2d::Sprite#draw SHOULD NOT be called");

    getGLProgramState()->apply(transform);

    if (m_useSeparateBlendFunc)
    {
        glBlendFuncSeparate(m_blendFuncSeparate.src, m_blendFuncSeparate.dst,
            m_blendFuncSeparate.srcAlpha, m_blendFuncSeparate.dstAlpha);
    }
    else
    {
        cocos2d::GL::blendFunc(_blendFunc.src, _blendFunc.dst);
    }

    if (m_blendEquation != -1)
    {
        glBlendEquation(m_blendEquation);
    }

    if (_texture != NULL)
    {
        cocos2d::GL::bindTexture2D(_texture->getName());
    }
    else
    {
        cocos2d::GL::bindTexture2D(0);
    }

    //
    // Attributes
    //

    cocos2d::GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
    CHECK_GL_ERROR_DEBUG();

#define kQuadSize sizeof(_quad.bl)
    long offset = (long)&_quad;

    // vertex
    int diff = offsetof(cocos2d::V3F_C4B_T2F, vertices);
    glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

    // texCoods
    diff = offsetof(cocos2d::V3F_C4B_T2F, texCoords);
    glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

    // color
    diff = offsetof(cocos2d::V3F_C4B_T2F, colors);
    glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    CHECK_GL_ERROR_DEBUG();

    //USING_NS_CC;
    //CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);
    //CC_INCREMENT_GL_DRAWS(1);
}

NS_GAF_END
