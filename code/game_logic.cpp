#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QKeyEvent>

#include <cmath>

#include "game_logic.hpp"
#include "constants.hpp"
// ─────────────────────────── Game_logic ──────────────────────────────────────

// Whether the character body collides with any wall at position c.
// Use Qt's built-in test: temporarily move the item to the candidate
// position, query the scene items via collidesWithItem(), then restore
// the item (no observable state change to the outside).
bool Game_logic::if_collides_w_character(const character_t& c) const {
        QGraphicsScene* sc = character->scene();
        if (!sc) return false;

        const character_t saved = character->get_char_params();
        character->set_char_params(c);

        bool collided = false;
        for (QGraphicsItem* item : sc->items()) {
            if (item == character) continue;            // don't collide with ourselves
            if (character->collidesWithItem(item)) {    // QGraphicsItem::collidesWithItem
                collided = true;
                break;
            }
        }

        character->set_char_params(saved);
        return collided;
    }


input_t Game_logic::get_char_input(const QSet<int>& keys) const {
    input_t in;
    in.fwd   = keys.contains(Qt::Key_W);
    in.back  = keys.contains(Qt::Key_S);
    in.left  = keys.contains(Qt::Key_A);
    in.right = keys.contains(Qt::Key_D);
    in.rot_l = keys.contains(Qt::Key_Q);
    in.rot_r = keys.contains(Qt::Key_E);
    return in;
}

character_t Game_logic::get_char_calculated(const input_t& in) {
    character_t cur = character->get_char_params();
    character_t next = cur;

    const float ang = cur.rotation_angle * DEG2RAD;
    const float spd = cur.speed_px_sec;

    if (in.fwd)  { next.posx += spd * std::cos(ang);                    next.posy += spd * std::sin(ang); }
    if (in.back) { next.posx -= spd * std::cos(ang);                    next.posy -= spd * std::sin(ang); }
    if (in.left) { next.posx += spd * std::cos(ang - 90 * DEG2RAD);     next.posy += spd * std::sin(ang - 90 * DEG2RAD); }
    if (in.right){ next.posx += spd * std::cos(ang + 90 * DEG2RAD);     next.posy += spd * std::sin(ang + 90 * DEG2RAD); }
    if (in.rot_l) next.rotation_angle -= 2;
    if (in.rot_r) next.rotation_angle += 2;

    // hit a wall — revert the move, keep the rotation
    if (if_collides_w_character(next)) {
        next.posx = cur.posx;
        next.posy = cur.posy;
    }
    return next;
}
