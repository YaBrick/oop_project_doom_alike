#include "scene3d.hpp"

#include <QPen>
#include <QBrush>
#include <QColor>

#include <cmath>
#include <algorithm>
#include <limits>

#include "constants.hpp"

// Intersection of ray (ox,oy)+t*(dx,dy), t>0, with a wall segment. Returns t or nullopt.
std::optional<float> Scene3D::ray_vs_segment(float ox, float oy, float dx, float dy, const wall& w) {
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
float Scene3D::cast_ray(float ray_angle_deg, const character_t& cam, const Walls& walls) const {
    float a = ray_angle_deg * DEG2RAD;
    float dx = std::cos(a), dy = std::sin(a);
    float best = std::numeric_limits<float>::max();
    for (const wall& w : walls.list) {
        auto t = ray_vs_segment(cam.posx, cam.posy, dx, dy, w);
        if (t && *t < best) best = *t;
    }
    return best;
}

void Scene3D::draw_background() {
    addRect(0, 0, VIEW_W, VIEW_H / 2, QPen(Qt::NoPen), QBrush(QColor(60, 60, 90)));        // ceiling
    addRect(0, VIEW_H / 2, VIEW_W, VIEW_H / 2, QPen(Qt::NoPen), QBrush(QColor(40, 40, 40))); // floor
}

// Vertical wall slice: height is inversely proportional to distance.
void Scene3D::draw_column(int col, float corrected_dist) {
    if (corrected_dist <= 0.f) return;
    const float colW = VIEW_W / NUM_RAYS;
    float h = WALL_SCALE / corrected_dist;
    if (h > VIEW_H) h = VIEW_H;
    float top = (VIEW_H - h) / 2.f;
    int shade = std::clamp(int(255.f - corrected_dist * 0.6f), 30, 255);  // farther = darker
    QColor c(shade, shade, shade);
    addRect(col * colW, top, colW + 1.f, h, QPen(Qt::NoPen), QBrush(c));
}

// Full frame redraw: background + one column per ray across the FOV sector.
void Scene3D::render(const character_t& cam, const Walls& walls) {
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
