#include <QApplication>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QSet>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPointF>
#include <QPainterPath>

#include <cmath>
#include <vector>
#include <optional>
#include <algorithm>
#include <limits>

#include "main.hpp"
#include "walls.hpp"

namespace {
const double DEG2RAD = 3.14159265358979323846 / 180.0;
const float CHAR_RADIUS = 9.f;   // character body radius (used for shape/collisions)
}  // namespace

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

// ─────────────────────────── Game_logic ──────────────────────────────────────

class Game_logic {
private:
    Character* character;   // non-owning pointer — the Map scene owns the item
    Walls& walls;

    // Whether the character body collides with any wall at position c.
    // Use Qt's built-in test: temporarily move the item to the candidate
    // position, query the scene items via collidesWithItem(), then restore
    // the item (no observable state change to the outside).
    bool if_collides_w_character(const character_t& c) const {
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

public:
    Game_logic(Character* ch, Walls& w) : character(ch), walls(w) {}

    Walls& get_walls() const { return walls; }

    // Translate the pressed keys into a movement intent.
    input_t get_char_input(const QSet<int>& keys) const {
        input_t in;
        in.fwd   = keys.contains(Qt::Key_W);
        in.back  = keys.contains(Qt::Key_S);
        in.left  = keys.contains(Qt::Key_A);
        in.right = keys.contains(Qt::Key_D);
        in.rot_l = keys.contains(Qt::Key_Q);
        in.rot_r = keys.contains(Qt::Key_E);
        return in;
    }

    // Current state + input -> new state (collision-aware).
    // Does not mutate the item itself — Map::refresh does, keeping the data flow explicit.
    character_t get_char_calculated(const input_t& in) {
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
};

// ─────────────────────────── Map (top-down 2D view) ──────────────────────────

class Map : public QGraphicsScene {
private:
    Character* character_ = nullptr;   // owned by the scene (addItem)

public:
    explicit Map(QObject* parent = nullptr) : QGraphicsScene(parent) {}

    // One-time setup: walls as lines + the character as an item.
    void build(const Walls& walls, Character* character) {
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

    // Each frame: apply the new state to the character (it repaints itself).
    void refresh(const character_t& c) {
        if (character_) character_->set_char_params(c);
    }
};

// ─────────────────────────── Scene3D (first-person raycasting view) ───────────

class Scene3D : public QGraphicsScene {
private:
    static constexpr int   NUM_RAYS   = 320;     // number of vertical columns
    static constexpr float VIEW_W     = 640.f;   // 3d view width, px
    static constexpr float VIEW_H     = 400.f;   // height, px
    static constexpr float WALL_SCALE = 16000.f; // wall height scale (∝ 1/distance)

    // Intersection of ray (ox,oy)+t*(dx,dy), t>0, with a wall segment. Returns t or nullopt.
    static std::optional<float> ray_vs_segment(float ox, float oy, float dx, float dy, const wall& w) {
        float x1 = w.posx1, y1 = w.posy1, x2 = w.posx2, y2 = w.posy2;
        float denom = dx * (y2 - y1) - dy * (x2 - x1);
        if (std::fabs(denom) < 1e-6f) return std::nullopt;   // parallel
        float sx = x1 - ox, sy = y1 - oy;
        float t = (sx * (y2 - y1) - sy * (x2 - x1)) / denom;  // along the ray
        float u = (sx * dy - sy * dx) / denom;                // along the segment [0..1]
        if (t > 0.f && u >= 0.f && u <= 1.f) return t;
        return std::nullopt;
    }

    // Distance to the nearest wall along the ray (angle in degrees).
    float cast_ray(float ray_angle_deg, const character_t& cam, const Walls& walls) const {
        float a = ray_angle_deg * DEG2RAD;
        float dx = std::cos(a), dy = std::sin(a);
        float best = std::numeric_limits<float>::max();
        for (const wall& w : walls.list) {
            auto t = ray_vs_segment(cam.posx, cam.posy, dx, dy, w);
            if (t && *t < best) best = *t;
        }
        return best;
    }

    void draw_background() {
        addRect(0, 0, VIEW_W, VIEW_H / 2, QPen(Qt::NoPen), QBrush(QColor(60, 60, 90)));        // ceiling
        addRect(0, VIEW_H / 2, VIEW_W, VIEW_H / 2, QPen(Qt::NoPen), QBrush(QColor(40, 40, 40))); // floor
    }

    // Vertical wall slice: height is inversely proportional to distance.
    void draw_column(int col, float corrected_dist) {
        if (corrected_dist <= 0.f) return;
        const float colW = VIEW_W / NUM_RAYS;
        float h = WALL_SCALE / corrected_dist;
        if (h > VIEW_H) h = VIEW_H;
        float top = (VIEW_H - h) / 2.f;
        int shade = std::clamp(int(255.f - corrected_dist * 0.6f), 30, 255);  // farther = darker
        QColor c(shade, shade, shade);
        addRect(col * colW, top, colW + 1.f, h, QPen(Qt::NoPen), QBrush(c));
    }

public:
    explicit Scene3D(QObject* parent = nullptr) : QGraphicsScene(parent) {
        setSceneRect(0, 0, VIEW_W, VIEW_H);
    }

    // Full frame redraw: background + one column per ray across the FOV sector.
    void render(const character_t& cam, const Walls& walls) {
        clear();
        draw_background();
        const float half = cam.fov_angle / 2.f;
        for (int i = 0; i < NUM_RAYS; ++i) {
            float ray_angle = cam.rotation_angle - half + cam.fov_angle * i / (NUM_RAYS - 1);
            float dist = cast_ray(ray_angle, cam, walls);
            if (dist == std::numeric_limits<float>::max()) continue;
            // fisheye correction
            float corrected = dist * std::cos((ray_angle - cam.rotation_angle) * DEG2RAD);
            draw_column(i, corrected);
        }
    }
};

// ─────────────────────────── Render (coordinator) ────────────────────────────

class Render {
private:
    int render_fps = 60;
    Game_logic logic;

public:
    Render(Character* ch, Walls& w) : logic(ch, w) {}

    Game_logic& game() { return logic; }

    void draw_map(Map& scene, const character_t& c) { scene.refresh(c); }
    void draw_3d(Scene3D& scene, const character_t& c) { scene.render(c, logic.get_walls()); }
};

// ─────────────────────────── Window (QWidget) ────────────────────────────────

class Window : public QWidget {
private:
    int sizex = 1600;
    int sizey = 700;
    QTimer timer;
    QSet<int> pressedKeys;

    Walls walls;
    Character* character;   // owned by map_scene
    Map map_scene;
    Scene3D scene_3d;
    Render render;          // owns Game_logic

    QGraphicsView* map_view;
    QGraphicsView* view_3d;

public:
    Window()
        : character(new Character()),
          render(character, walls)
    {
        // initial character state
        character->set_char_params({ 100, 100, 0, 60, 2 });

        map_scene.build(walls, character);
        map_scene.setSceneRect(0, 0, 400, 400);

        // 3d — main view, fills the whole window
        view_3d = new QGraphicsView(&scene_3d, this);
        view_3d->setFrameShape(QFrame::NoFrame);
        view_3d->setRenderHint(QPainter::Antialiasing);
        view_3d->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view_3d->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // map — minimap on top of 3d, in the top-right corner (picture-in-picture)
        map_view = new QGraphicsView(&map_scene, this);
        map_view->setStyleSheet("QGraphicsView { border: 2px solid white; }");
        map_view->setRenderHint(QPainter::Antialiasing);
        map_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        map_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        map_view->raise();   // above the main view

        setFocusPolicy(Qt::StrongFocus);
        resize(sizex, sizey);   // initial size; main() maximizes the window

        QObject::connect(&timer, &QTimer::timeout, this, &Window::tick);
        timer.start(16);   // ~60 FPS
    }

protected:
    void keyPressEvent(QKeyEvent* e) override {
        if (!e->isAutoRepeat()) pressedKeys.insert(e->key());
    }
    void keyReleaseEvent(QKeyEvent* e) override {
        if (!e->isAutoRepeat()) pressedKeys.remove(e->key());
    }

    // Fit the views to the current window size (including on maximize).
    // Done here, not in the constructor: only now does the window have a real size.
    void resizeEvent(QResizeEvent*) override {
        const int mapSize = 280, margin = 12;
        view_3d->setGeometry(0, 0, width(), height());
        view_3d->fitInView(scene_3d.sceneRect(), Qt::KeepAspectRatioByExpanding);
        map_view->setGeometry(width() - mapSize - margin, margin, mapSize, mapSize);
        map_view->fitInView(map_scene.sceneRect(), Qt::KeepAspectRatio);
    }

private:
    void tick() {
        input_t in = render.game().get_char_input(pressedKeys);
        character_t c = render.game().get_char_calculated(in);
        render.draw_map(map_scene, c);   // update the top-down view
        render.draw_3d(scene_3d, c);     // redraw the raycasting view
    }
};

// ─────────────────────────── main ────────────────────────────────────────────

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Window window;
    window.setWindowTitle("Doom-alike");
    window.showMaximized();   // maximized, but not fullscreen (keeps frame and title bar)

    return app.exec();
}
