#include <QApplication>
#include <iostream>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QSet>
#include <cmath>
#include <vector>
#include "main.hpp"
#include "walls.hpp"

static const double DEG2RAD = M_PI / 180.0;

class Character {
private:
character_t values;

public: 

character_t get_char_params() {
 return this->values;
}

void set_char_params(character_t buff){
    values = buff;
}
};


class Game_logic {

    private:
    bool if_collides_w_character(character_t character_buff){
        for(int i = 0; i < Walls.size(); i++){
            if(character_buff.xpos <= std::max(Walls[i].x1, Walls[i].x2) &&
               character_buff.xpos >= std::min(Walls[i].x1, Walls[i].x2) &&
               character_buff.ypos <= std::max(Walls[i].y1, Walls[i].y2) &&
               character_buff.ypos >= std::min(Walls[i].y1, Walls[i].y2) &&
                < width){
                return true;
            }
        }
        return false;
    }
    
};

class Render{
    private:
    int render_fps = 60;

    public:

    void draw_walls_map(QPainter& painter) const{
        for (const Wall& w : Walls) {
            painter.setPen(QPen(Qt::black, w.width));
            painter.drawLine(QPoint(w.x1, w.y1), QPoint(w.x2, w.y2));
        }
    }
    void draw_character_map(QPainter& painter) const{
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

    void draw_map(){
        draw_walls_map();
        draw_character_map();

    }
}