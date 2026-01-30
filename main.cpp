#include "raylib.h"
#include "operator_overloads.h"
#include "map_utils.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>

constexpr int SCREEN_W = 750;
constexpr int SCREEN_H = 500; 


//////////////////////
///// RAYCASTER /////
//////////////////////
class Raycaster {
private:
    float fov = 90.0 * (M_PI / 180.0);
    float zoom = 1.0;
    const int width = SCREEN_W, height = SCREEN_H;

public:
    void raycaster(Vector2 dir, Vector2 plane, Vector2 pos, float CELL_SIZE, std::vector<std::vector<Color>> &map) {
        Color tileCellColor = mapColors[0];

        Vector2 rayPos = pos/CELL_SIZE;

        for (int i = 0; i < width; i++) {
            int mapX = int(rayPos.x), mapY = int(rayPos.y);
            float cameraX = (2*i/(float)width)-1;
            // convert dir and plane to unit vectors relative to map before calculating ray direction
            // Vector2 rayDir = (dir-pos)/CELL_SIZE + (plane-pos)/CELL_SIZE * cameraX;
            Vector2 rayDir = normalize(dir) + normalize(plane) * cameraX;

            // length of ray from 1 side to next side
            // (determines distance for both the vertical and horizontal sides of cells)
            Vector2 dDist = {
                rayDir.x == 0 ? 1e7 : std::fabs(1.0f / rayDir.x),
                rayDir.y == 0 ? 1e7 : std::fabs(1.0f / rayDir.y)
            };

            // length of ray from curr pos to next cell-side
            Vector2 dSide = {
                rayDir.x < 0 ? (rayPos.x - mapX) * dDist.x : (mapX + 1.0 - rayPos.x) * dDist.x,
                rayDir.y < 0 ? (rayPos.y - mapY) * dDist.y : (mapY + 1.0 - rayPos.y) * dDist.y
            };

            // direction to step in x and y directions (+1 or -1) - used for travelling through map
            int stepX = (rayDir.x < 0 ? -1 : 1), stepY = (rayDir.y < 0 ? -1 : 1);
            bool hit = false;
            int side;

            while (!hit) {
                if (dSide.x < dSide.y) {
                    dSide.x += dDist.x;
                    mapX += stepX;
                    side = 0;
                } else {
                    dSide.y += dDist.y;
                    mapY += stepY;
                    side = 1;
                }

                if (mapX < 0 || mapX >= map[0].size() || mapY < 0 || mapY >= map.size()) {
                    break;
                }
                // not good design, but eh... whatever
                if (map[mapY][mapX] != tileCellColor) {
                    hit = true;
                }
            }

            if (mapX < 0 || mapX >= map[0].size() || mapY < 0 || mapY >= map.size()) {
                continue;
            }
            // want to check out how the fisheye effect works
            // but not now
            // Vector2 posWall = { (float)mapX*(float)CELL_SIZE, (float)mapY*(float)CELL_SIZE };
            // float dx = (posWall.x-pos.x), dy = (posWall.y-pos.y);
            // float distWall = sqrt(dx*dx + dy*dy);

            float distCamPlaneWall = dSide.x - dDist.x;
            if (side == 1) {
                distCamPlaneWall = dSide.y - dDist.y;
            }

            int lHeight = (int)(height / distCamPlaneWall);
            int drawStart = -lHeight/2 + height/2;
            if (drawStart < 0) {
                drawStart = 0;
            }

            int drawEnd = lHeight/2 + height/2;
            if (drawEnd >= height) {
                drawEnd = height - 1;
            }

            Color wallColor = map[mapY][mapX];
            // wallColor /= ()
            if (side == 1) {
                wallColor /= 2;
            }

            // Vector2 distWall = {
            //     fabs(pos.x-((float)mapX*CELL_SIZE)+(distCamPlaneWall*(side==0))),
            //     fabs(pos.y-((float)mapY*CELL_SIZE)+(distCamPlaneWall*side))
            // };

            // float euclidDistWall = vec_mag(normalize(distWall));
            // // if (euclidDistWall < 0.95)
            //     std::cout << "distance to wall: " << euclidDistWall << std::endl;

            // // std::cout << "wallColor(before)=" << wallColor << ", ";
            // euclidDistWall = std::min(1.0f, std::max(euclidDistWall, 0.2f));

            // wallColor *= euclidDistWall;
            // // std::cout << "wallColor(after)=" << wallColor << std::endl;


            DrawLine(i, drawStart, i, drawEnd, wallColor);
        }
    }
};


///////////////////
///// PLAYER /////
///////////////////
class Player {
private:
    // for now player is a circle... an orb... of life (too dramatic)
    Vector2 pos = { 0, 0 };
    float radius = 0.0;
    Vector2 dir = { 0, 0 };
    float planeLen = 0.0;
    Vector2 posPlane = { 0, 0 };
    Vector2 negPlane = { 0, 0 };
    float FOV = 0.0;
    Color color = WHITE;

public:
    /*
    player must be initialized with the length of the perpendicular direction vector (float),
    and the FOV (again, a float)
    FOV = |dir| / |plane|
    hence, we can find the length of the plane vector as:
    |plane| = |dir| / FOV
    the plane vector is supposed to be perpendicular to the direction vector; therefore, if
    */
    Player(Vector2 pos, float radius, float dirLen, float FOV, Color color) {
        this->FOV = FOV;
        this->pos = pos;
        this->color = color;
        this->radius = radius;
        dir = { pos.x, pos.y-dirLen };
        // since plane is supposed to be perpendicular to dir
        planeLen = dirLen/FOV;
        posPlane = { dir.x+planeLen, dir.y };
        negPlane = { dir.x-planeLen, dir.y };
        // std::cout << "planeLen=" << planeLen << ", posPlaneV=" << posPlane << ", negPlaneV=" << negPlane << std::endl;
    }

    // getters
    Vector2 getPlayerPos() {
        return pos;
    }

    Vector2 getPlayerDir() {
        return dir-pos;
    }

    Vector2 getPlayerPlane() {
        return posPlane-pos;
    }

    void drawPlayer() {
        DrawCircleV(pos, radius, color);
        DrawLineV(pos, dir, YELLOW);
        DrawLineV(dir, posPlane, GREEN);
        DrawLineV(dir, negPlane, GREEN);
        DrawLineV(pos, posPlane, WHITE);
        DrawLineV(pos, negPlane, WHITE);
    }

    void updatePlayer(float dAngle, Vector2 offset, float CELL_SIZE) {
        float cosine = cos(dAngle), sine = sin(dAngle);

        Vector2 dir_ = dir;
        dir.x = (dir_.x-pos.x)*cosine - (dir_.y-pos.y)*sine   + pos.x;
        dir.y = (dir_.x-pos.x)*sine   + (dir_.y-pos.y)*cosine + pos.y;

        Vector2 posPlane_ = posPlane;
        posPlane.x = (posPlane_.x-pos.x)*cosine - (posPlane_.y-pos.y)*sine   + pos.x;
        posPlane.y = (posPlane_.x-pos.x)*sine   + (posPlane_.y-pos.y)*cosine + pos.y;

        Vector2 negPlane_ = negPlane;
        negPlane.x = (negPlane_.x-pos.x)*cosine - (negPlane_.y-pos.y)*sine   + pos.x;
        negPlane.y = (negPlane_.x-pos.x)*sine   + (negPlane_.y-pos.y)*cosine + pos.y;

        pos += offset;
        dir += offset;
        posPlane += offset;
        negPlane += offset;
    }
};


int main() {
    InitWindow(SCREEN_W, SCREEN_H, "raycasting go brr...");

    // player params
    Vector2 pInitPos = { SCREEN_W / 2, SCREEN_H / 2 };
    Color pColor = BLUE;
    float pCircleRadius = 10.0;
    float pFocusDirLen = 50.0;
    float pFOV = 90.0 * (M_PI / 180);  // in radians

    Raycaster pRaycaster;

    // update params
    int FPS = 60;
    float dt = 1.0f / (float)FPS;

    // map params
    std::string mapfn = "map1.txt";
    float CELL_SIZE = SCREEN_H / NTILES.first;
    std::vector<std::vector<Color>> map = createMap(mapfn);

    Player player(pInitPos, pCircleRadius, pFocusDirLen, pFOV, pColor);
    float prev_angle = M_PI_2;

    while (!WindowShouldClose()) {
        // Update
        
        // render-related variables
        // int curr_fps = GetFPS();

        // player-related variables
        Vector2 mousePos = GetMousePosition();
        Vector2 playerPos = player.getPlayerPos();
        Vector2 playerDir = player.getPlayerDir();

        Vector2 forward = normalize(playerDir);
        Vector2 right = { forward.y, -forward.x };

        Vector2 offset = { 0.0,0.0 };


        if (IsKeyDown(KEY_W)) {
            offset += forward*dt;
        }
        if (IsKeyDown(KEY_S)) {
            offset -= forward*dt;
        }
        if (IsKeyDown(KEY_A)) {
            offset += right*dt;
        }
        if (IsKeyDown(KEY_D)) {
            offset -= right*dt;
        }

        // std::cout << "offset=" << offset << std::endl;

        float dx = playerPos.x-mousePos.x, dy = playerPos.y-mousePos.y;
        float angle = atan2(dy, dx);
        // std::cout << "angle=" << angle * 180 / M_PI << std::endl;

        player.updatePlayer(angle-prev_angle, offset, CELL_SIZE);
        prev_angle = angle;

        auto pDir = player.getPlayerDir();
        auto pPlane = player.getPlayerPlane();
        auto pPos = player.getPlayerPos();

        // Draw
        BeginDrawing();

        ClearBackground(BLACK);

        DrawFPS(25,15);

        DrawRectangle(0,SCREEN_H/2, SCREEN_W,SCREEN_H, (Color){ 10,100,0,150 });
        // drawMap(map, CELL_SIZE);
        // player.drawPlayer();
        pRaycaster.raycaster(pDir, pPlane, pPos, CELL_SIZE, map);
        EndDrawing();
    }

    return 0;
}
