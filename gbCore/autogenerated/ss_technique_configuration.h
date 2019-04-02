// autogenerated: do not add any changes
#ifndef ss_technique_configuration_h
#define ss_technique_configuration_h
#include "configuration.h"
#include "material_configuration.h"
#include "attachment_configuration.h"
namespace gb
{
class ss_technique_configuration : public configuration
{
public:
ss_technique_configuration(void) = default;
~ss_technique_configuration(void) = default;
std::string get_guid(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_guid(std::string guid);
#endif
ui32 get_frame_width(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_frame_width(ui32 frame_width);
#endif
ui32 get_frame_height(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_frame_height(ui32 frame_height);
#endif
std::shared_ptr<material_configuration> get_material_configuration(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_material_configuration(const std::shared_ptr<gb::material_configuration>& material);
#endif
std::vector<std::shared_ptr<configuration>> get_attachments_configurations(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void add_attachments_configurations(const std::shared_ptr<gb::attachment_configuration>& attachment);
#endif
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_attachments_configurations(const std::shared_ptr<gb::attachment_configuration>& attachment, i32 index);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif
