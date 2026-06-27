#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QTimer>
#include <QSet>

#include "character.hpp"
#include "walls.hpp"
#include "map.hpp"
#include "scene3d.hpp"
#include "render.hpp"

class QKeyEvent;     // used only by pointer in the event overrides
class QResizeEvent;

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
    Window();

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    void tick();
};
