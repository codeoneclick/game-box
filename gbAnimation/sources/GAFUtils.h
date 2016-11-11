#pragma once
#include <sstream>
#include <iomanip>

inline bool to_bool(const std::string& str)
{
    /*std::string temp = str;
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    std::istringstream is(temp);*/
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

inline cocos2d::AffineTransform& affineTransformSetFrom(cocos2d::AffineTransform& to, const cocos2d::AffineTransform& from)
{
    to.a = from.a;
    to.b = from.b;
    to.c = from.c;
    to.d = from.d;
    to.tx = from.tx;
    to.ty = from.ty;

    return to;
}

inline cocos2d::Rect scaleRect(const cocos2d::Rect& rect, const cocos2d::Vec2& scale)
{
    cocos2d::Rect result = cocos2d::Rect(
        rect.origin.x * scale.x, rect.origin.y * scale.y,
        rect.size.width * scale.x, rect.size.height * scale.y);

    return result;
}

inline static cocos2d::Vec2 affineTransformGetScale(const cocos2d::AffineTransform& mtx)
{
    cocos2d::Vec2 scale;

    scale.x = cocos2d::Vec2(mtx.a, mtx.b).length();
    scale.y = cocos2d::Vec2(mtx.c, mtx.d).length();

    return scale;
}
