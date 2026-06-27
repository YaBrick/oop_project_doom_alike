#pragma once

#include <QGraphicsScene>
#include <optional>

#include "character.hpp"
#include "walls.hpp"

// ─────────────────────────── Scene3D (first-person raycasting view) ───────────

class Scene3D : public QGraphicsScene {
private:
    static constexpr int   NUM_RAYS   = 320;     // number of vertical columns
    static constexpr float VIEW_W     = 640.f;   // 3d view width, px
    static constexpr float VIEW_H     = 400.f;   // height, px
    static constexpr float WALL_SCALE = 16000.f; // wall height scale (∝ 1/distance)

    // Intersection of ray (ox,oy)+t*(dx,dy), t>0, with a wall segment. Returns t or nullopt.
    static std::optional<float> ray_vs_segment(float ox, float oy, float dx, float dy, const wall& w);

    // Distance to the nearest wall along the ray (angle in degrees).
    float cast_ray(float ray_angle_deg, const character_t& cam, const Walls& walls) const;

    void draw_background();
    void draw_column(int col, float corrected_dist);

public:
    explicit Scene3D(QObject* parent = nullptr) : QGraphicsScene(parent) {
        setSceneRect(0, 0, VIEW_W, VIEW_H);
    }

    // Full frame redraw: background + one column per ray across the FOV sector.
    void render(const character_t& cam, const Walls& walls);
};
