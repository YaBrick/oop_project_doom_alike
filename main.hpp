#pragma once

#include <vector>
#include <QPainter>
#include <QPen>


struct character_t{
    ///* @brief Type with

    
    int xpos = 10;
    int ypos = 10;
    int rotation_angle = 0;
    int speed_px_sec = 5;
    int fov_angle = 45;
};