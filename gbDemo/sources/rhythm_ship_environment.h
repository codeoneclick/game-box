#pragma once
#include "rhythm_stage_state.h"
#include "scene_graph.h"
#include "terrain_data.h"
namespace game
{
class ces_ocean_system;
struct RhythmShipEnvironment
{
    std::shared_ptr<ces_ocean_system> ocean;
    std::vector<gb::shape_3d_shared_ptr> speakers, panels, strips;
    gb::shape_3d_shared_ptr ring, crown;
    std::array<gb::shape_3d_shared_ptr, 3> floor_pulses;
    RhythmStagePulses pulses;
};
gb::mesh_3d_shared_ptr rhythm_upload_mesh(const terrain_mesh_data &, const std::string &);
gb::shape_3d_shared_ptr rhythm_shape(const gb::scene_fabricator_shared_ptr &,
                                     const gb::ces_entity_shared_ptr &, const terrain_mesh_data &,
                                     const std::string &, const std::string &);
RhythmShipEnvironment create_rhythm_ship_environment(const gb::scene_fabricator_shared_ptr &,
                                                     const gb::ces_entity_shared_ptr &,
                                                     const gb::game_transition_shared_ptr &);
void update_rhythm_ship_effects(RhythmShipEnvironment &, double, double, double, float, bool,
                                const RhythmStageFrame &);
} // namespace game
