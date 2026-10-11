#pragma once
#include <chrono>
#include <cstdint>

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
            double previous_end = -1., elapsed = 0., fps = 0.;
            uint64_t frames = 0;
        };
        static state& data() { static state value; return value; }
        static double now() {
            return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
        }
    public:
        static void begin_frame() { data().current = {}; }
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
        }
        static render_frame_statistics last_frame() { return data().completed; }
    };
}
