#include <vector>

struct Wall{
    int x1 = 0, y1 = 0;
    int x2 = 0, y2 = 0;
    int width = 4;
};

inline std::vector<Wall> Walls = { {0, 0, 0, 400},
                            {0, 0, 400, 400},
                            {0, 400, 0, 0},
                            {0, 400, 0, 400},
                            {200, 0, 200, 200},
                            {400, 200, 300, 200}};