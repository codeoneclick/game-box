// autogenerated: do not add any changes
#ifndef quest_task_configuration_h
#define quest_task_configuration_h
#include "configuration.h"
namespace gb
{
class quest_task_configuration : public configuration
{
public:
quest_task_configuration(void) = default;
~quest_task_configuration(void) = default;
i32 get_id(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_id(i32 id);
#endif
i32 get_type(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_type(i32 type);
#endif
i32 get_goal_id(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_goal_id(i32 goal_id);
#endif
i32 get_goal_count(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_goal_count(i32 goal_count);
#endif
i32 get_goal_time(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_goal_time(i32 goal_time);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif