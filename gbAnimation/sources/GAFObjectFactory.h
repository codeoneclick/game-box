#pragma once
#include "GAFMacros.h"
#include "GAFCollections.h"

NS_GAF_BEGIN

class GAFTimeline;
class GAFTextData;
class GAFTextureAtlasElement;
class GAFAsset;
class GAFObject;

class GafObjectFactory
{
private:
    typedef std::map<std::string, std::function<gaf::GAFObject*(GAFAsset*, GAFTimeline*)>> FactorySignatures_t;
public:
    static GAFObject* create(GAFAsset* asset, GAFTextureAtlasElement* txElemet, bool isMask = false, uint32_t id = IDNONE);
    static GAFObject* create(GAFAsset* asset, GAFTextData* txtData, bool isMask = false, uint32_t id = IDNONE);
    static GAFObject* create(GAFAsset* asset, GAFTimeline* timeline, bool isMask = false, uint32_t id = IDNONE);
};

NS_GAF_END