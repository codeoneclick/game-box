#include "GAFPrecompiled.h"
#include "GAFObjectFactory.h"
#include "GAFAsset.h"
#include "GAFTextureAtlasElement.h"
#include "GAFAssetTextureManager.h"
#include "GAFMovieClip.h"
#include "GAFMask.h"
#include "GAFTextField.h"
#include "UI/GAFCanvasView.h"
#include "UI/GAFBoxLayoutView.h"
#include "UI/GAFScrollView.h"

NS_GAF_BEGIN

GAFObject* GafObjectFactory::create(GAFAsset* asset, GAFTextureAtlasElement* txElemet, bool isMask, uint32_t id)
{
    GAFObject* result = nullptr;
    GAFAssetTextureManager* txMgr = asset->getTextureManager();
    cocos2d::Texture2D * texture = txMgr->getTextureById(txElemet->atlasIdx + 1);
    if (texture)
    {
        cocos2d::SpriteFrame * spriteFrame = cocos2d::SpriteFrame::createWithTexture(texture, txElemet->bounds);

        if (!isMask)
            result = new GAFMovieClip();
        else
            result = new GAFMask();

        result->initWithSpriteFrame(spriteFrame, txElemet->rotation, txElemet->scale9GridRect);
        result->objectIdRef = id;
        cocos2d::Vec2 pt = cocos2d::Vec2(txElemet->pivotPoint.x / result->getContentSize().width,
            1 - txElemet->pivotPoint.y / result->getContentSize().height);
        result->setAnchorPoint(pt);

        if (txElemet->getScale() != 1.0f)
            result->setAtlasScale(1.0f / txElemet->getScale());

        result->setBlendFunc(cocos2d::BlendFunc::ALPHA_PREMULTIPLIED);
        result->autorelease();
    }
    else
    {
        CCLOGERROR("Cannot add sub object with Id: %d, atlas with idx: %d not found.", id, txElemet->atlasIdx);
    }

    return result;
}

GAFObject* GafObjectFactory::create(GAFAsset* asset, GAFTextData* txtData, bool isMask, uint32_t id)
{
    (void)asset;
    (void)isMask;

    GAFTextField* result = new GAFTextField();
    result->initWithTextData(txtData);
    result->objectIdRef = id;

    result->autorelease();

    return result;
}

GAFObject* GafObjectFactory::create(GAFAsset* asset, GAFTimeline* timeline, bool isMask, uint32_t id)
{
    (void)isMask;

    static FactorySignatures_t signatures;

    static std::once_flag once;
    std::call_once(once, []()
    {
        signatures[GAFObjectClass::toString(GAFObjectClass::UI_BOX_LAYOUT)] = std::bind(&GAFBoxLayoutView::create, std::placeholders::_1, std::placeholders::_2);
        signatures[GAFObjectClass::toString(GAFObjectClass::UI_CANVAS)] = std::bind(&GAFCanvasView::create, std::placeholders::_1, std::placeholders::_2);
        signatures[GAFObjectClass::toString(GAFObjectClass::UI_BUTTON)] = std::bind(&GAFObject::create, std::placeholders::_1, std::placeholders::_2);
        signatures[GAFObjectClass::toString(GAFObjectClass::UI_SCROLL_VIEW)] = std::bind(&GAFScrollView::create, std::placeholders::_1, std::placeholders::_2);
        signatures[GAFObjectClass::toString(GAFObjectClass::UI_TEXT_AREA)] = std::bind(&GAFScrollView::create, std::placeholders::_1, std::placeholders::_2);
    });

    GAFObject* result;
    std::string linkageName = timeline->getLinkageName();
    std::string baseClass = timeline->getBaseClass();

    FactorySignatures_t::iterator it = signatures.find(baseClass);

    if (it != signatures.end())
    {
        result = it->second(asset, timeline);
    }
    else
    {
        result = GAFObject::create(asset, timeline);
    }

    result->objectIdRef = id;

    return result;
}

NS_GAF_END
