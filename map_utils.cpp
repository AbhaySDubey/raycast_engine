#include "map_utils.h"
#include "raylib.h"
#include "operator_overloads.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

const std::vector<Color> mapColors = {
    BLANK, DARKGRAY, MAROON, ORANGE, DARKGREEN, DARKBLUE, DARKPURPLE,
    DARKBROWN, GRAY, RED, GOLD, LIME, BLUE, VIOLET, BROWN,
    LIGHTGRAY, PINK, YELLOW, GREEN, SKYBLUE, PURPLE, BEIGE, BLACK,
};

std::vector<int> getSpaceSepVals(std::string &line) {
    std::string buff;
    std::vector<int> vals;
    bool errCheck = false;
    for (int i = 0; i <= line.size(); i++) {
        auto c = (i < line.size() ? line[i] : ' ');
        if (c == ' ' && !buff.empty()) {
            try {
                auto val = std::stoi(buff);
                vals.push_back(val);
            }
            catch (std::invalid_argument const &ex) {
                std::cerr << "Error: Invalid argument. No valid conversion found.\n" << ex.what() << "\nExiting..." << std::endl; 
                errCheck = true;
                break;
            }
            catch (std::out_of_range const& ex) {
                std::cerr << "Error: value of range.\n" << ex.what() << "Exiting..." << std::endl;
                errCheck = true;
                break;
            }
            // std::cout << "buffer: " << buff << ", type(buffer): " << typeid(buff).name() << " ; ";
            buff.clear();
        }
        else {
            buff += c;
        }
    }
    // std::cout << std::endl;
    return (errCheck ? std::vector<int>() : vals);
}

std::vector<std::vector<Color>> createMap(std::string &mapFn) {
    std::ifstream map1(mapFn);
    std::string line;

    std::vector<std::vector<int>> wallDefs;

    while (std::getline(map1, line)) {
        std::vector<int> vals = getSpaceSepVals(line);
        if (!vals.empty()) {
            wallDefs.push_back(vals);
        }
    }

    std::vector<std::vector<Color>> map(NTILES.first, std::vector<Color>(NTILES.second, mapColors[0]));
    
    // map boundaries
    for (int i = 0; i < NTILES.first; i++) {
        map[i][0] = mapColors[1];
        map[i][NTILES.second-1] = mapColors[1];
    }
    for (int j = 0; j < NTILES.second; j++) {
        map[0][j] = mapColors[1];
        map[NTILES.first-1][j] = mapColors[1];
    }
    // actual walls
    for (auto &it : wallDefs) {
        int i = it[0], j = it[1], colI = it[2];
        map[i][j] = mapColors[colI];
    }

    return map;
}

void drawMap(std::vector<std::vector<Color>> &map, float CELL_SIZE) {
    // Vector2 tilePos = { 0, 0 };
    int x = 0, y = 0;
    for (int i = 0; i < NTILES.first; i++) {
        for (int j = 0; j < NTILES.second; j++) {
            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, map[i][j]);
            // purely for visualization
            DrawRectangleLines(x, y, CELL_SIZE,CELL_SIZE, WHITE);
            x += CELL_SIZE;
        }
        y += CELL_SIZE;
        x = 0;
    }
}


Vector2 normalize(Vector2 a) {
    float mag = (float)(sqrt(a.x*a.x + a.y*a.y));

    return a/mag;
}

float vec_mag(Vector2 a) {
    float mag = (float)(std::sqrt(a.x*a.x + a.y*a.y));
    return mag;
}