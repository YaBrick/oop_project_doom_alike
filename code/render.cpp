#include "render.hpp"

#include "map.hpp"
#include "scene3d.hpp"

void Render::draw_map(Map& scene, const character_t& c) {
    scene.refresh(c);
}

void Render::draw_3d(Scene3D& scene, const character_t& c) {
    scene.render(c, logic.get_walls());
}
