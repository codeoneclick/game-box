#include "render_statistics.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main() {
    gb::render_statistics::begin_frame();
    gb::render_statistics::record_indexed(300, 1, true);
    gb::render_statistics::record_indexed(6, 4, true);
    gb::render_statistics::record_indexed(8, 1, false);
    gb::render_statistics::end_frame(0.0);
    auto s=gb::render_statistics::last_frame();
    assert(s.draw_calls==3 && s.triangles==108);
    gb::render_statistics::begin_frame();
    assert(gb::render_statistics::last_frame().triangles==108);
    gb::render_statistics::record_indexed(0, 1, true);
    gb::render_statistics::end_frame(.25);
    s=gb::render_statistics::last_frame();
    assert(s.draw_calls==1 && s.triangles==0);
    gb::render_statistics::begin_frame();
    gb::render_statistics::end_frame(.5);
    s=gb::render_statistics::last_frame();
    assert(s.draw_calls==0 && s.triangles==0 && std::abs(s.fps-4.)<.001);
    std::cout << "Render statistics: frame reset, submitted triangles, instances, empty/non-triangle draws and FPS passed\n";
}
