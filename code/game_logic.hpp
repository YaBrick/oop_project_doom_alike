#pragma once

#include <QSet>

#include "character.hpp"
#include "walls.hpp"

// Movement intent for a single frame — result of reading the pressed keys.
struct input_t {
    bool fwd = false, back = false, left = false, right = false;
    bool rot_l = false, rot_r = false;
};


class Game_logic {
private:
    Character* character;   // non-owning pointer — the Map scene owns the item
    Walls& walls;

    bool if_collides_w_character(const character_t& c) const;
public:
    Game_logic(Character* ch, Walls& w) : character(ch), walls(w) {}

    Walls& get_walls() const { return walls; }

    // Translate the pressed keys into a movement intent.
    input_t get_char_input(const QSet<int>& keys) const;
    
    // Current state + input -> new state (collision-aware).
    // Does not mutate the item itself — Map::refresh does, keeping the data flow explicit.
    character_t get_char_calculated(const input_t& in);

};