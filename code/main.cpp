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
#include "character.hpp"
#include "game_logic.hpp"
#include "render.hpp"
#include "window.hpp"

#include "constants.hpp"

// ─────────────────────────── main ────────────────────────────────────────────

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Window window;
    window.setWindowTitle("Doom-alike");
    window.showMaximized();   // maximized, but not fullscreen (keeps frame and title bar)

    return app.exec();
}
