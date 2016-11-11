#pragma once

#include "TagDefines.h"
#include "GAFCollections.h"

NS_GAF_BEGIN

class GAFAsset;
class GAFTimeline;

class GAFStream;
class DefinitionTagBase;
class GAFHeader;
class GAFFile;

class GAFLoader
{
public:
    typedef std::map<uint32_t, CustomProperties_t> CustomPropertiesMap_t; // Custom properties set by timeline id

private:
    GAFStream*              m_stream;
    CustomPropertiesMap_t   m_customProperties;

    void                    _readHeaderEnd(GAFHeader&);
    void                    _readHeaderEndV4(GAFHeader&);

    void                    _registerTagLoadersV3();
    void                    _registerTagLoadersCommon();
    void                    _registerTagLoadersV4();

protected:
    typedef std::map</*Tags::Enum*/ uint32_t, DefinitionTagBase*> TagLoaders_t;
    TagLoaders_t            m_tagLoaders;

    virtual void            _processLoad(GAFFile* file, GAFAsset*);

public:
    GAFLoader();
    ~GAFLoader();

    bool                    loadFile(const std::string& fname, GAFAsset* context);
    bool                    loadData(const unsigned char* data, size_t len, GAFAsset* context);
    bool                    isFileLoaded() const;

    GAFStream*              getStream() const;

    const GAFHeader&        getHeader() const;

    void                    registerTagLoader(unsigned int idx, DefinitionTagBase*);

    void                    loadTags(GAFStream* in, GAFAsset* asset, GAFTimeline* timeline);
    void                    readCustomProperties(GAFStream* in, CustomProperties_t* customProperties) const;

    const                   CustomProperties_t& getCustomProperties(uint32_t timeline) const;
    void                    setCustomProperties(uint32_t timeline, CustomProperties_t cp);
};

NS_GAF_END