#pragma once

#include <QGraphicsScene>

#include "character.hpp"
#include "walls.hpp"

// ─────────────────────────── Map (top-down 2D view) ──────────────────────────

class Map : public QGraphicsScene {
private:
    Character* character_ = nullptr;   // owned by the scene (addItem)

public:
    explicit Map(QObject* parent = nullptr) : QGraphicsScene(parent) {}

    // One-time setup: walls as lines + the character as an item.
    void build(const Walls& walls, Character* character);

    // Each frame: apply the new state to the character (it repaints itself).
    void refresh(const character_t& c);
};
