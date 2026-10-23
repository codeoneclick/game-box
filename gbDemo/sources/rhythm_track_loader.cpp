#include "rhythm_track_loader.h"
#include <exception>
namespace game
{
RhythmTrackLoader::RhythmTrackLoader() : worker_([this] { run(); })
{
}
RhythmTrackLoader::~RhythmTrackLoader()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        pending_ = false;
    }
    wake_.notify_one();
    worker_.join();
}
std::uint64_t RhythmTrackLoader::request(const std::string &path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    path_ = path;
    pending_ = true;
    ready_ = false;
    ++generation_;
    wake_.notify_one();
    return generation_;
}
bool RhythmTrackLoader::poll(RhythmLoadResult &out)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!ready_)
        return false;
    out = std::move(result_);
    ready_ = false;
    return true;
}
void RhythmTrackLoader::run()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        wake_.wait(lock, [this] { return stop_ || pending_; });
        if (stop_)
            return;
        auto path = path_;
        auto generation = generation_;
        pending_ = false;
        lock.unlock();
        RhythmLoadResult result;
        result.generation = generation;
        try
        {
            auto pcm = std::make_shared<gb::al::RhythmPCM>();
            if (gb::al::decode_rhythm_mp3(path, *pcm, result.error))
                result.pcm = pcm;
        }
        catch (const std::exception &e)
        {
            result.error = e.what();
        }
        lock.lock();
        if (stop_)
            return;
        if (generation == generation_)
        {
            result_ = std::move(result);
            ready_ = true;
        }
    }
}
} // namespace game
