#pragma once
#include "rhythm_pcm.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
namespace game
{
struct RhythmLoadResult
{
    std::uint64_t generation = 0;
    std::shared_ptr<const gb::al::RhythmPCM> pcm;
    std::string error;
};
class RhythmTrackLoader
{
    std::mutex mutex_;
    std::condition_variable wake_;
    bool stop_ = false, pending_ = false, ready_ = false;
    std::string path_;
    std::uint64_t generation_ = 0;
    RhythmLoadResult result_;
    std::thread worker_; // Start only after all state observed by run() is initialized.
    void run();

  public:
    RhythmTrackLoader();
    ~RhythmTrackLoader();
    std::uint64_t request(const std::string &);
    bool poll(RhythmLoadResult &);
    RhythmTrackLoader(const RhythmTrackLoader &) = delete;
    RhythmTrackLoader &operator=(const RhythmTrackLoader &) = delete;
};
} // namespace game
