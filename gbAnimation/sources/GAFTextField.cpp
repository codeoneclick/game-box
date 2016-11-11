#include "GAFPrecompiled.h"
#include "GAFTextField.h"

NS_GAF_BEGIN

GAFTextField::GAFTextField()
{
    m_charType = GAFCharacterType::TextField;
}

GAFTextField::~GAFTextField()
{
    CC_SAFE_RELEASE(m_label);
}

// TODO: Factory
void GAFTextField::initWithTextData(GAFTextData const* data)
{
    bool prevPopupNotify = cocos2d::FileUtils::getInstance()->isPopupNotify(); // disabling annoing missing font notifiers
    cocos2d::FileUtils::getInstance()->setPopupNotify(false);
    
    std::string generatedFileName = data->m_textFormat.m_font;
    std::replace(generatedFileName.begin(), generatedFileName.end(), ' ', '_');
    generatedFileName += ".ttf";
    //

    // TEMPORARY STUB. Since isFileExist consumes a lot of CPU time, we temporary check for name of our font
#if 1
    if (generatedFileName != "Exo_Bold.ttf" && generatedFileName != "Exo_Regular.ttf")
        generatedFileName = "Exo_Bold.ttf";

    {
        m_label = cocos2d::Label::createWithTTF(data->m_text, generatedFileName, static_cast<float>(data->m_textFormat.m_size));
    }
#else
    if (cocos2d::FileUtils::getInstance()->isFileExist(generatedFileName))
    {
        m_label = cocos2d::Label::createWithTTF(data->m_text, generatedFileName, static_cast<float>(data->m_textFormat.m_size));
    }
    else // Using old name from config
    {
        m_label = cocos2d::Label::createWithSystemFont(data->m_text, data->m_textFormat.m_font, static_cast<float>(data->m_textFormat.m_size));
    }
#endif
    
    cocos2d::FileUtils::getInstance()->setPopupNotify(prevPopupNotify);
    
    CC_SAFE_RETAIN(m_label);
    
    m_label->setDimensions(data->m_width, data->m_height);
    m_label->setTextColor(cocos2d::Color4B(data->m_textFormat.m_color));
    m_label->setHorizontalAlignment(data->m_textFormat.getTextAlignForCocos());
    
    cocos2d::Point anchor = cocos2d::Point(data->m_pivot.x / data->m_width, 1 - data->m_pivot.y / data->m_height);
    m_label->setAnchorPoint(anchor);
    
    addChild(m_label);
}

void GAFTextField::setText(const std::string& text)
{
    m_label->setString(text);
}

cocos2d::Rect GAFTextField::getInternalBoundingBox() const
{
    return m_label->getBoundingBox();
}

NS_GAF_END
