#ifndef MAP_UTILS_H
#define MAP_UTILS_H

#include "raylib.h"
#include <string>
#include <vector>
#include <utility>

// currently only a 10x15 grid is being used
constexpr std::pair<int, int> NTILES = { 10,15 };
extern const std::vector<Color> mapColors;

// functions
std::vector<int> getSpaceSepVals(std::string &line);
std::vector<std::vector<Color>> createMap(std::string &mapFn);
void drawMap(std::vector<std::vector<Color>> &map, float CELL_SIZE);
Vector2 normalize(Vector2 a);
float vec_mag(Vector2 a);

#endif

