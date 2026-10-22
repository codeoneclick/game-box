#pragma once
#include "ces_base_system.h"
#include "rhythm_audio_transport.h"
#include "rhythm_session.h"
namespace game
{
class ces_dance_crowd_system : public gb::ces_base_system
{
    std::bitset<std::numeric_limits<ui8>::max()> mask_;
    std::shared_ptr<RhythmSession> session_;
    std::shared_ptr<gb::al::RhythmAudioTransport> audio_;
    int count_ = 40;

  protected:
    void on_feed_start(f32) override {}
    void on_feed(const gb::ces_entity_shared_ptr &, f32) override;
    void on_feed_end(f32) override {}

  public:
    STTI_CLASS_GUID(ces_dance_crowd_system, gb::ces_base_system::g_guids_container)
    ces_dance_crowd_system(std::shared_ptr<RhythmSession>,
                           std::shared_ptr<gb::al::RhythmAudioTransport>);
    void set_active_count(int n) { count_ = glm::clamp(n, 30, 50); }
};
void populate_rhythm_ship_crowd(const gb::scene_fabricator_shared_ptr &,
                                const gb::ces_entity_shared_ptr &);
} // namespace game
