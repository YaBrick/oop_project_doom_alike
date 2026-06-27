#include <QGraphicsItem>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include "constants.hpp"

#pragma once


// DTO — a snapshot of the character parameters exchanged between Game_logic,
// Render and the scenes (ERD: returned by get_char_params / taken by set_char_params).
struct character_t {
    float posx = 0;
    float posy = 0;
    float rotation_angle = 0;   // degrees, 0 = right, grows clockwise
    float fov_angle = 60;
    float speed_px_sec = 2;
};

// The character as a Map-scene item: keeps its state and paints itself.
// Position is applied via setPos, so paint() draws in local coordinates
// around (0,0).
class Character : public QGraphicsItem {
private:
    float posx = 100;
    float posy = 100;
    float rotation_angle = 0;
    float fov_angle = 60;
    float speed_px_sec = 2;

public:
    character_t get_char_params() const;
    void set_char_params(const character_t& p);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;   // body used for collisions (collidesWithItem)
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};
