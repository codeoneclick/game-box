#include "rhythm_ship_transition.h"
#include "rhythm_ship_scene.h"
#include "scene_fabricator.h"
namespace game
{
void rhythm_ship_transition::create_scene()
{
    m_scene = gb::ces_entity::construct<rhythm_ship_scene>(shared_from_this());
    set_fabricator(std::make_shared<gb::scene_fabricator>());
}
void rhythm_ship_transition::destroy_scene() { m_scene = nullptr; }
} // namespace game
