#pragma once
#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <cassert>

namespace gb {
    // Single game-loop thread; counts submitted work, including all passes.
    struct render_frame_statistics {
        uint64_t triangles = 0;
        uint64_t draw_calls = 0;
        double fps = 0.;
    };
    class render_statistics {
        struct state {
            render_frame_statistics current, completed;
            std::map<std::string,render_frame_statistics> passes, completed_passes;
            render_frame_statistics pass_start;
            render_frame_statistics* active_pass = nullptr;
            double previous_end = -1., elapsed = 0., fps = 0.;
            uint64_t frames = 0;
        };
        static state& data() { static state value; return value; }
        static double now() {
            return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
        }
    public:
        static void begin_frame() {
            auto& s=data();s.current={};s.active_pass=nullptr;
            for(auto& pass:s.passes)pass.second={};
        }
        // Bracket actual submissions, including batching, instances and bypass draws.
        // Reuse entries so profiling does not allocate for each draw or frame.
        static void begin_pass(const std::string& name) {
            auto& s=data();assert(!s.active_pass);
            s.active_pass=&s.passes[name];s.pass_start=s.current;
        }
        static void end_pass() {
            auto& s=data();assert(s.active_pass);
            s.active_pass->triangles+=s.current.triangles-s.pass_start.triangles;
            s.active_pass->draw_calls+=s.current.draw_calls-s.pass_start.draw_calls;
            s.active_pass=nullptr;
        }
        static void record_indexed(uint32_t indices, uint32_t instances = 1, bool triangle_list = true) {
            auto& s=data();
            ++s.current.draw_calls;
            if(triangle_list) s.current.triangles += uint64_t(indices/3)*instances;
        }
        static void end_frame(double seconds = now()) {
            auto& s=data();
            if(s.previous_end >= 0. && seconds > s.previous_end) {
                s.elapsed += seconds-s.previous_end;
                ++s.frames;
                if(s.elapsed >= .5) {
                    s.fps = s.frames/s.elapsed;
                    s.frames=0; s.elapsed=0.;
                }
            }
            s.previous_end=seconds;
            s.current.fps=s.fps;
            s.completed=s.current;
            s.completed_passes=s.passes;
        }
        static render_frame_statistics last_frame() { return data().completed; }
        static render_frame_statistics last_pass(const std::string& name) {
            const auto& passes=data().completed_passes;
            const auto found=passes.find(name);
            return found==passes.end()?render_frame_statistics{}:found->second;
        }
    };
}
