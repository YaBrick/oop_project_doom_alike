#include "character.hpp"
// ─────────────────────────── Character (QGraphicsItem) ───────────────────────

character_t Character::get_char_params() const {
    return { posx, posy, rotation_angle, fov_angle, speed_px_sec };
}

void Character::set_char_params(const character_t& p) {
    posx = p.posx;
    posy = p.posy;
    rotation_angle = p.rotation_angle;
    fov_angle = p.fov_angle;
    speed_px_sec = p.speed_px_sec;
    setPos(posx, posy);   // item position; paint draws relative to (0,0)
    update();
}

QRectF Character::boundingRect() const {
    // covers the body triangle at any rotation plus a pen margin
    return QRectF(-14, -14, 28, 28);
}

QPainterPath Character::shape() const {
    // Circle of radius CHAR_RADIUS — rotation-invariant, so collisions via
    // collidesWithItem() stay correct for any facing direction.
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), CHAR_RADIUS, CHAR_RADIUS);
    return path;
}

void Character::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    const int radius = 8;
    const int lineLen = 20;
    const int fovline = 60;

    static const QPoint character_polygon[4] = {
       QPoint(-7, -8),
       QPoint(7, -8),
       QPoint(0, 10),
       QPoint(-7, -8)
    };

    // body — polygon
    painter->setBrush(Qt::red);
    painter->setPen(Qt::NoPen);
    painter->rotate(rotation_angle - 90);
    painter->drawConvexPolygon(character_polygon, 3);

    QPen pen(Qt::white, 3);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPolyline(character_polygon, 4);
    }