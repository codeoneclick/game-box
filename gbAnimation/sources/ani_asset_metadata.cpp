#include "ani_asset_metadata.h"
#include "ani_timeline.h"
#include "ani_texture_atlas.h"

namespace gb
{
    namespace anim
    {
        ani_asset_metadata::ani_asset_metadata() :
          m_root_timeline(nullptr)
        , m_filename("")
        {
        }
        
        ani_asset_metadata::~ani_asset_metadata()
        {
        }
        
        
        std::shared_ptr<ani_asset_metadata> ani_asset_metadata::create(const std::string& filepath)
        {
            std::shared_ptr<ani_asset_metadata> asset_metadata = std::make_shared<ani_asset_metadata>();
            if (asset_metadata->init(filepath))
            {
                return asset_metadata;
            }
            return nullptr;
        }
        
        bool ani_asset_metadata::init(const std::string& filepath)
        {
            m_filename = filepath;
            return true;
        }
        
        void ani_asset_metadata::set_root_timeline(const std::shared_ptr<ani_timeline>& timeline)
        {
            m_root_timeline = timeline;
        }
        
        bool ani_asset_metadata::set_root_timeline(const std::string& name)
        {
            for (ani_timelines_t::iterator iterator = m_timelines.begin(), end = m_timelines.end(); iterator != end; ++iterator)
            {
                const std::string& timeline_name = iterator->second->get_linkage_name();
                if (timeline_name.compare(name) == 0)
                {
                    set_root_timeline(iterator->second);
                    return true;
                }
            }
            return false;
        }
        
        bool ani_asset_metadata::set_root_timeline(ui32 id)
        {
            ani_timelines_t::iterator timeline = m_timelines.find(id);
            if (timeline != m_timelines.end())
            {
                set_root_timeline(timeline->second);
                return true;
            }
            return false;
        }
        
        std::shared_ptr<ani_timeline> ani_asset_metadata::get_root_timeline() const
        {
            return m_root_timeline;
        }
        
        std::shared_ptr<ani_timeline> ani_asset_metadata::get_timeline_by_name(const std::string& name) const
        {
            for (ani_timelines_t::value_type iterator : m_timelines)
            {
                if (iterator.second->get_linkage_name() == name)
                {
                    return iterator.second;
                }
            }
            return nullptr;
        }
        
        void ani_asset_metadata::push_timeline(ui32 timeline_id_reference, const std::shared_ptr<ani_timeline>& timeline)
        {
            m_timelines[timeline_id_reference] = timeline;
        }
        
        void ani_asset_metadata::set_header(const std::shared_ptr<ani_header>& header)
        {
            m_header = header;
        }
        
        ani_timelines_t& ani_asset_metadata::get_timelines()
        {
            return m_timelines;
        }
        
        const ani_timelines_t& ani_asset_metadata::get_timelines() const
        {
            return m_timelines;
        }
        
        const std::shared_ptr<ani_header> ani_asset_metadata::get_header() const
        {
            return m_header;
        }
    }
}
