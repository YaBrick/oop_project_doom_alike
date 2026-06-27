#pragma once

#include "game_logic.hpp"
#include "character.hpp"

class Map;       // forward-declared: only used by reference in the declarations below
class Scene3D;   // forward-declared

// ─────────────────────────── Render (coordinator) ────────────────────────────

class Render {
private:
    int render_fps = 60;
    Game_logic logic;

public:
    Render(Character* ch, Walls& w) : logic(ch, w) {}

    Game_logic& game() { return logic; }

    void draw_map(Map& scene, const character_t& c);
    void draw_3d(Scene3D& scene, const character_t& c);
};
