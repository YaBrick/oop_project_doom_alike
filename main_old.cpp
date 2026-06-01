#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QSet>
#include <cmath>
#include <vector>
#include "main.hpp"

static const double DEG2RAD = M_PI / 180.0;


class WallsRender{
private:
    struct Wall{
        int x1 = 0, y1 = 0;
        int x2 = 0, y2 = 0;
        int width = 4;
    };

    std::vector<Wall> Walls = {};

public:

    void wallgen(int _x1, int _y1, int _x2, int _y2){
        Wall buff;
        buff.x1 = _x1; buff.y1 = _y1; buff.x2 = _x2; buff.y2 = _y2;
        Walls.push_back(buff);
    }

    void draw(QPainter& painter) const{
        for (const Wall& w : Walls) {
            painter.setPen(QPen(Qt::black, w.width));
            painter.drawLine(QPoint(w.x1, w.y1), QPoint(w.x2, w.y2));
        }
    }
};

class Character {
private:
    int xpos;
    int ypos;
    int vis_angle; // градусы, 0 = вправо, растёт по часовой
    int speed;
    int fov;

public:
    Character(int x, int y, int angle, int spd, int fv)
        : xpos(x), ypos(y), vis_angle(angle), speed(spd), fov(fv) {}

    void moveUp()    { xpos += static_cast<int>(speed * std::sin((vis_angle + 90) * DEG2RAD));
                       ypos += static_cast<int>(speed * std::cos((vis_angle - 90) * DEG2RAD)); 
                       }
    void moveDown()  { xpos += static_cast<int>(speed * std::sin((vis_angle - 90) * DEG2RAD));
                        ypos += static_cast<int>(speed * std::cos((vis_angle + 90) * DEG2RAD)); }
    void moveLeft()  { xpos += static_cast<int>(speed *std::sin(vis_angle * DEG2RAD));
                        ypos -= static_cast<int>(speed * std::cos(vis_angle * DEG2RAD)); }
    void moveRight()  { xpos -= static_cast<int>(speed *std::sin(vis_angle * DEG2RAD));
                        ypos += static_cast<int>(speed * std::cos(vis_angle * DEG2RAD)); }

    void rotateLeft()  { vis_angle = (vis_angle - 3 + 360) % 360; }
    void rotateRight() { vis_angle = (vis_angle + 3) % 360; }

    void clampToRect(int w, int h) {
        if (xpos < 0) xpos = 0;
        if (ypos < 0) ypos = 0;
        if (xpos > w) xpos = w;
        if (ypos > h) ypos = h;
    }

    void draw(QPainter& painter) const {
        const int radius = 8;
        const int lineLen = 20;
        const int fovline = 60;


        // Тело — закрашенный круг
        painter.setBrush(Qt::blue);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPoint(xpos, ypos), radius, radius);

        // Линия направления (vis_angle)
        painter.setPen(QPen(Qt::red, 2));
        int dx = static_cast<int>(lineLen * std::cos(vis_angle * DEG2RAD));
        int dy = static_cast<int>(lineLen * std::sin(vis_angle * DEG2RAD));
        painter.drawLine(QPoint(xpos, ypos), QPoint(xpos + dx, ypos + dy));

        painter.setPen(QPen(Qt::green, 2, Qt::DotLine));
        int dx_fov1 = static_cast<int>(fovline * std::cos((vis_angle - fov/2) * DEG2RAD));
        int dy_fov1 = static_cast<int>(fovline * std::sin((vis_angle - fov/2) * DEG2RAD));
        int dx_fov2 = static_cast<int>(fovline * std::cos((vis_angle + fov/2) * DEG2RAD));
        int dy_fov2 = static_cast<int>(fovline * std::sin((vis_angle + fov/2) * DEG2RAD));
        painter.drawLine(QPoint(xpos, ypos), QPoint(xpos + dx_fov1, ypos + dy_fov1));
        painter.drawLine(QPoint(xpos, ypos), QPoint(xpos + dx_fov2, ypos + dy_fov2));

    }
};

class TheMap : public QWidget {
    Character character;
    WallsRender wallsrender;
    QSet<int> pressedKeys;
    QTimer timer;

public:
    TheMap() : character(100, 100, 0, 2, 60), wallsrender() {
        setFocusPolicy(Qt::StrongFocus);
        connect(&timer, &QTimer::timeout, this, &TheMap::tick);
        timer.start(16); // ~60 FPS
        wallsrender.wallgen(10, 10, 10, 200);
        wallsrender.wallgen(140, 10, 140, 100);
        wallsrender.wallgen(100, 100, 150, 200);

    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), Qt::white);
        character.draw(painter);
        wallsrender.draw(painter);
    }

    void keyPressEvent(QKeyEvent* event) override {
        if (!event->isAutoRepeat())
            pressedKeys.insert(event->key());
    }

    void keyReleaseEvent(QKeyEvent* event) override {
        if (!event->isAutoRepeat())
            pressedKeys.remove(event->key());
    }

    void collisionCheck(){
        
    }

private slots:
    void tick() {
        if (pressedKeys.contains(Qt::Key_W)) character.moveUp();
        if (pressedKeys.contains(Qt::Key_S)) character.moveDown();
        if (pressedKeys.contains(Qt::Key_A)) character.moveLeft();
        if (pressedKeys.contains(Qt::Key_D)) character.moveRight();
        if (pressedKeys.contains(Qt::Key_Q)) character.rotateLeft();
        if (pressedKeys.contains(Qt::Key_E)) character.rotateRight();
        if (!pressedKeys.isEmpty()) {
            character.clampToRect(width(), height());
            update();
        }
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    TheMap window;
    window.setFixedSize(300, 300);
    window.show();
    window.move(10, 10);  

    return app.exec();
}
