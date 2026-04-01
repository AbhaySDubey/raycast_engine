#include "raylib.h"
#include "operator_overloads.h"
#include "map_utils.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <exception>

constexpr int SCREEN_W = 1750;
constexpr int SCREEN_H = 1000; 


//////////////////////
///// RAYCASTER /////
//////////////////////
class Raycaster {
private:
    float fov = 90.0 * (M_PI / 180.0);
    float zoom = 1.0;
    const int width = SCREEN_W, height = SCREEN_H;

public:
    void raycaster(Vector2 &dir, Vector2 &plane, Vector2 &pos, Vector2 &CELL_SIZE, std::vector<std::vector<Color>> &map) {
        Color tileCellColor = mapColors[0]; // {0,0,0,0}

        Vector2 rayPos = pos/CELL_SIZE;

        // bool collision = false;

        for (int i = 0; i < width; i++) {
            int mapX = int(rayPos.x), mapY = int(rayPos.y);
            float cameraX = (2*i/(float)width)-1;
            // convert dir and plane to unit vectors relative to map before calculating ray direction
            // Vector2 rayDir = (dir-pos)/CELL_SIZE + (plane-pos)/CELL_SIZE * cameraX;
            Vector2 rayDir = normalize(dir)+normalize(plane)*cameraX;

            // fish-eye effect
            // Vector2 rayDir = normalize(dir+plane*cameraX);

            // length of ray from 1 side to next side
            // (determines distance for both the vertical and horizontal sides of cells)
            Vector2 dDist = {
                rayDir.x == 0 ? (float)1e7 : std::fabs(1.0f/rayDir.x),
                rayDir.y == 0 ? (float)1e7 : std::fabs(1.0f/rayDir.y)
            };

            // length of ray from curr pos to next cell-side
            Vector2 dSide = {
                rayDir.x < 0 ? (rayPos.x-mapX) * dDist.x : (mapX+1.0f-rayPos.x) * dDist.x,
                rayDir.y < 0 ? (rayPos.y-mapY) * dDist.y : (mapY+1.0f-rayPos.y) * dDist.y
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
                }
                else {
                    dSide.y += dDist.y;
                    mapY += stepY;
                    side = 1;
                }

                if (mapX < 0 || mapX >= (int)map[0].size() || mapY < 0 || mapY >= (int)map.size()) {
                    break;
                }

                // not good design, but eh... whatever
                if (map[mapY][mapX] != tileCellColor) {
                    hit = true;
                }
            }

            if (mapX < 0 || mapX >= (int)map[0].size() || mapY < 0 || mapY >= (int)map.size()) {
                continue;
            }

            float distCamPlaneWall = (mapX-rayPos.x+(1-stepX)/2) / rayDir.x;
            if (side == 1) {
                distCamPlaneWall = (mapY-rayPos.y+(1-stepY)/2) / rayDir.y;
            }

            int lHeight = (int)(height/distCamPlaneWall);
            int drawStart = -lHeight/2 + height/2;
            if (drawStart < 0) {
                drawStart = 0;
            }
            int drawEnd = lHeight/2 + height/2;
            if (drawEnd >= height) {
                drawEnd = height-1;
            }

            Color wallColor = map[mapY][mapX];
            if (side == 1) {
                wallColor /= 2;
            }
            float shade = 1.0f/(1.0f+distCamPlaneWall*0.50f);
            // std::cout << shade << ",";
            shade = std::clamp(shade, 0.2f, 1.0f);
            wallColor = {
                (unsigned char)(wallColor.r * shade),
                (unsigned char)(wallColor.g * shade),
                (unsigned char)(wallColor.b * shade),
                255
            };
            
            DrawLine(i, drawStart, i, drawEnd, wallColor);
        }
        // std::cout << std::endl;
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
        planeLen = dirLen*tan(FOV/2.0f);
        posPlane = { dir.x+planeLen, dir.y };
        negPlane = { dir.x-planeLen, dir.y };
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

    // draw
    // void drawPlayer(RenderTexture2D &minimap, float scale) {
    //     float mm_radius = radius*scale;
    //     Vector2 mm_pos = pos*scale, mm_dir = dir*scale, mm_posPlane = posPlane*scale, mm_negPlane = negPlane*scale;

    //     BeginTextureMode(minimap);
    //         DrawCircleV(pos, radius, color);
    //         DrawLineV(pos, dir, YELLOW);
    //         DrawLineV(dir, posPlane, GREEN);
    //         DrawLineV(dir, negPlane, GREEN);
    //         DrawLineV(pos, posPlane, WHITE);
    //         DrawLineV(pos, negPlane, WHITE);
    //     EndTextureMode();
    // }

    void drawPlayer(RenderTexture2D &minimap, Vector2 CELL_SIZE)
    {
        Vector2 mm_cell_size = {
            minimap.texture.width  / (float)NTILES().second,
            minimap.texture.height / (float)NTILES().first
        };

        Vector2 mapPos = {
            pos.x / CELL_SIZE.x,
            pos.y / CELL_SIZE.y
        };

        Vector2 mmPos = {
            mapPos.x * mm_cell_size.x,
            mapPos.y * mm_cell_size.y
        };

        float mmRadius = radius/CELL_SIZE.x*mm_cell_size.x;

        BeginTextureMode(minimap);
            DrawCircleV(mmPos, mmRadius, color);
            Vector2 dirEnd = {
                (dir.x/CELL_SIZE.x)*mm_cell_size.x,
                (dir.y/CELL_SIZE.y)*mm_cell_size.y
            };
            DrawLineV(mmPos, dirEnd, YELLOW);
        EndTextureMode();
    }


    // update
    void updatePlayer(float dAngle, Vector2 offset, Vector2 CELL_SIZE) {
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
    // faster io buffer
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(NULL);
    std::cout.tie(NULL);

    InitWindow(SCREEN_W, SCREEN_H, "raycasting go brr...");
    
    // map params
    std::string mapfn = "map1.txt";
    std::vector<std::vector<Color>> map = createMap(mapfn);
    Vector2 CELL_SIZE = { (float)SCREEN_W/NTILES().second, (float)SCREEN_H/NTILES().first };

    // player params
    Vector2 pInitPos = { SCREEN_W/2, SCREEN_H/2 };
    Color pColor = BLUE;
    float pCircleRadius = 25.0;
    float pFocusDirLen = 50.0;
    float pFOV = 90.0 * (M_PI / 180);  // in radians
    float pAccel = 1.0f; // run ding-ding-ding
    
    Player player(pInitPos, pCircleRadius, pFocusDirLen, pFOV, pColor);
    Raycaster pRaycaster;


    // mini-map
    float minimap_scale = 0.25;
    // Vector2 minimap_dims = { SCREEN_W, SCREEN_H };
    Vector2 minimap_dims = { SCREEN_W*minimap_scale, SCREEN_H*minimap_scale };
    RenderTexture2D minimap = LoadRenderTexture((int)minimap_dims.x, (int)minimap_dims.y);
    

    // update params
    int FPS = 60;
    float dt = 1.0f/(float)FPS;
    float full_angle = 2*M_PI;
    // float angle_step = full_angle/(dt*10);
    float prev_mouse_cx = SCREEN_W/2;

    // background
    Texture2D ceiling = LoadTexture("./res/starry_night_ceiling.png");
    Texture2D floor = LoadTexture("./res/grassy_floor.png");

    RenderTexture2D background = LoadRenderTexture(SCREEN_W, SCREEN_H);

    BeginTextureMode(background);
    
        ClearBackground(BLANK);
        DrawTextureRec(
            floor,
            (Rectangle){ 0,0, SCREEN_W,SCREEN_H },
            (Vector2){ 0,0 },
            (Color){50,75,150,200}
        );
        DrawTextureRec(
            ceiling,
            (Rectangle){ 0,0, SCREEN_W,SCREEN_H/2 },
            (Vector2){ 0,SCREEN_H/2 },
            (Color){50,100,150,200}
        );

    EndTextureMode();


    while (!WindowShouldClose()) {
        // Update

        // player-related variables
        Vector2 mousePos = GetMousePosition();
        Vector2 playerPos = player.getPlayerPos();
        Vector2 playerDir = player.getPlayerDir();

        Vector2 forward = normalize(playerDir);
        Vector2 right = { forward.y, -forward.x };

        Vector2 offset = { 0.0,0.0 };

        pAccel = (IsKeyDown(KEY_LEFT_SHIFT) ? 25.00f : 10.00f);

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

        offset *= pAccel;

        // check collision with walls
        Vector2 playerHitBox_offset = {
            offset.x < 0 ? -pCircleRadius : pCircleRadius,
            offset.y < 0 ? -pCircleRadius : pCircleRadius,
        };
        int mapX = int((playerPos.x+playerHitBox_offset.x)/CELL_SIZE.x), mapY = int((playerPos.y+playerHitBox_offset.y)/CELL_SIZE.y);
        int map_offX = int((playerPos.x+playerHitBox_offset.x+offset.x)/CELL_SIZE.x), map_offY = int((playerPos.y+playerHitBox_offset.y+offset.y)/CELL_SIZE.y);

        if (map_offY >= 0 && map_offY < (int)map.size() && map_offX >= 0 && map_offX < (int)map[0].size()) {
            if (map[map_offY][mapX] != mapColors[0]) {
                offset.y = 0.0f;
            }
            if (map[mapY][map_offX] != mapColors[0]) {
                offset.x = 0.0f;
            }
        }

        // float dx = playerPos.x-mousePos.x, dy = playerPos.y-mousePos.y;
        // float angle = atan2(dy, dx);
        // float dAngle = angle-prev_angle;
        float dAngle = ((mousePos.x-prev_mouse_cx)/(SCREEN_W/2))*full_angle;

        player.updatePlayer(dAngle, offset, CELL_SIZE);
        prev_mouse_cx = mousePos.x;

        auto pDir = player.getPlayerDir();
        auto pPlane = player.getPlayerPlane();
        auto pPos = player.getPlayerPos();

        drawMap(map, minimap);
        player.drawPlayer(minimap, CELL_SIZE);

        // Draw
        BeginDrawing();
            ClearBackground(BLACK);

            DrawTextureV(background.texture, {0.0f,0.0f}, WHITE);
            // drawMap(map, CELL_SIZE);
            pRaycaster.raycaster(pDir, pPlane, pPos, CELL_SIZE, map);
            DrawTextureV(
                minimap.texture,
                {(float)SCREEN_W-minimap.texture.width-50.0f, 25.0f},
                WHITE
            );
            // DrawRectangleLines
            
            DrawFPS(25,15);
        EndDrawing();
    }

    UnloadRenderTexture(background);
    UnloadRenderTexture(minimap);
    CloseWindow();
    return 0;
}
