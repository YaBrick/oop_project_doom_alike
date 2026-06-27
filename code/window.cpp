#include "window.hpp"

#include <QKeyEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QFrame>

Window::Window()
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

void Window::keyPressEvent(QKeyEvent* e) {
    if (!e->isAutoRepeat()) pressedKeys.insert(e->key());
}

void Window::keyReleaseEvent(QKeyEvent* e) {
    if (!e->isAutoRepeat()) pressedKeys.remove(e->key());
}

// Fit the views to the current window size (including on maximize).
// Done here, not in the constructor: only now does the window have a real size.
void Window::resizeEvent(QResizeEvent*) {
    const int mapSize = 280, margin = 12;
    view_3d->setGeometry(0, 0, width(), height());
    view_3d->fitInView(scene_3d.sceneRect(), Qt::KeepAspectRatioByExpanding);
    map_view->setGeometry(width() - mapSize - margin, margin, mapSize, mapSize);
    map_view->fitInView(map_scene.sceneRect(), Qt::KeepAspectRatio);
}

void Window::tick() {
    input_t in = render.game().get_char_input(pressedKeys);
    character_t c = render.game().get_char_calculated(in);
    render.draw_map(map_scene, c);   // update the top-down view
    render.draw_3d(scene_3d, c);     // redraw the raycasting view
}
