#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <unordered_map>


constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;


////////////////////////////////////
/////   OPERATOR OVERLOADING   /////
////////////////////////////////////

std::ostream& operator<< (std::ostream &out, const Vector2 &temp) {
    out << temp.x << "," << temp.y;
    return out;
}

Vector2 operator* (const Vector2 &a, const float scale) {
    return (Vector2){a.x*scale, a.y*scale};
}

Vector2& operator+= (Vector2 &a, const Vector2 &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

Vector2 operator+ (Vector2 a, const Vector2 &b) {
    a += b;
    return a;
}

/////////////////////////////
/////// PLAYER CLASS ///////
////////////////////////////
class Player {
private:
    /*
        player would be an isoceles triangle with a height longer than the base (considering the player faces along the Y-axis - straight ahead)
        to represent the direction the player is facing along, we'll draw a circle (point at the top vertex - head)
        
        the pos (centroid of the triangle) represents the axis along which the player rotates
    */

    std::vector<Vector2> playerVs = {{0,0},{0,0},{0,0}};
    Vector2 dims = {0, 0}; // {base, height}
    Vector2 pos = {SCREEN_W/2, SCREEN_H/2}; // centroid of the triangle
    Color color = WHITE;

public:
    Player(Vector2 pos, Vector2 dims, Color color) {
        this->pos = pos;
        this->dims = dims;
        this->color = color;
    }

    std::vector<Vector2> updatePlayer(float angle, Vector2 offset) {
        std::vector<Vector2> playerVsLocal = {
            { -dims.x/2,   dims.y/3 },
            {  dims.x/2,   dims.y/3 },
            {         0,-2*dims.y/3 }
        };
        
        // translate the centroid
        translate(offset);

        // std::vector<Vector2> playerOut;

        for (int i = 0; i < 3; i++) {
            Vector2 temp = rotate(playerVsLocal[i], angle);
            playerVs[i] = { temp.x+pos.x, temp.y+pos.y };
        }
        // std::cout << "player vertices:- ";
        // for (auto &v : playerVs) {
        //     std::cout << "(" << v.x << "," << v.y << ") , ";
        // }
        // std::cout << "\n";
        return playerVs;
    }

    void drawPlayer(float angle, Vector2 offset) {
        updatePlayer(angle, offset);
        DrawTriangle(playerVs[1], playerVs[2], playerVs[0], color);
        DrawCircleV(playerVs[2], 3, YELLOW);
    }

    void translate(Vector2 offset) {
        pos += offset;
        pos.x = std::min(std::max(pos.x, 0.0f), (float)SCREEN_W);
        pos.y = std::min(std::max(pos.y, 0.0f), (float)SCREEN_H);
    }

    Vector2 rotate(Vector2 v, float angle) {
        float sine = sin(angle), cosine = cos(angle);
        return (Vector2){
            v.x*cosine - v.y*sine,
            v.x*sine   + v.y*cosine
        };
    }

    void SetPlayerPos(Vector2 newPos) {
        pos = newPos;
    }

    Vector2 GetPlayerPos() const {
        return pos;
    }
};


int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "raycasting go brrr....");

    const Vector2 playerPosInit = {400,300};
    const Vector2 playerDims = {20,40};
    Color playerColor = {0xfa, 0xfa, 0xfa, 0xff};
    Player player(playerPosInit, playerDims, playerColor);

    constexpr float FPS = 60;
    SetTargetFPS(FPS);
    float dt = 1/FPS;
    Vector2 moveOffset;
    float angle = 0.0;

    // std::unordered_map<int, Vector2> offset = { {KEY_W,{0.0,-10.0}}, {KEY_A,{-10.0,0.0}}, {KEY_S,{0.0,10.0}}, {KEY_D,{10.0,0.0}} };
    float offsetScale = 100.0;
    while (!WindowShouldClose()) {
        // Update
        // FPS = GetFPS();
        dt = 1/FPS;
        moveOffset = {0.0,0.0};
        if (IsKeyDown(KEY_W)) {
            moveOffset.y -= offsetScale*dt;
        }
        if (IsKeyDown(KEY_S)) {
            moveOffset.y += offsetScale*dt;
        }
        if (IsKeyDown(KEY_A)) {
            moveOffset.x -= offsetScale*dt;
        }
        if (IsKeyDown(KEY_D)) {
            moveOffset.x += offsetScale*dt;
        }

        Vector2 mousePos = GetMousePosition();
        Vector2 playerPos = player.GetPlayerPos();

        angle = atan2((mousePos.y-playerPos.y), (mousePos.x-playerPos.x));
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            player.drawPlayer(angle, moveOffset);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
