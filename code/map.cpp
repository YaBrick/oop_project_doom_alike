#include "map.hpp"

#include <QPen>
#include <QColor>

void Map::build(const Walls& walls, Character* character) {
    clear();
    setBackgroundBrush(QColor(100, 100, 130));
    QPen pen(Qt::black);
    for (const wall& w : walls.list) {
        pen.setWidthF(w.width);
        addLine(w.posx1, w.posy1, w.posx2, w.posy2, pen);
    }
    character_ = character;
    addItem(character_);
}

void Map::refresh(const character_t& c) {
    if (character_) character_->set_char_params(c);
}
