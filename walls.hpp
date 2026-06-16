#pragma once

#include <vector>

// Plain data for a single wall: segment (x1,y1)-(x2,y2) plus thickness.
// The QGraphicsLineItem geometry is built by the Map scene, so the data is
// kept separate from the presentation (ERD: struct wall).
struct wall {
    float posx1 = 0, posy1 = 0;
    float posx2 = 0, posy2 = 0;
    float width = 4;
};

// Set of map walls (ERD: Walls + list: list<wall>).
class Walls {
public:
    std::vector<wall> list;

    Walls() {
        list = {
            // closed 400x400 box so raycasting rays always hit something
            {   0,   0, 400,   0 },   // top
            { 400,   0, 400, 400 },   // right
            { 400, 400,   0, 400 },   // bottom
            {   0, 400,   0,   0 },   // left
            // interior walls
            { 200,  50, 200, 200 },
            { 200,  200, 100, 200 },
            { 200,  50, 100, 200 },
            { 200,  500, 200, 200 },


            { 250, 300, 380, 300 },
        };
    }
};
