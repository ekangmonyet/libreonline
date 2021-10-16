#ifndef __LO_INC_PLAYER_HPP
#define __LO_INC_PLAYER_HPP

#include "raylib.h"

#include <string>

typedef unsigned int PlayerID;

class Player
{
    Texture2D texture;
public:
    PlayerID Id;
    std::string Name;
    Vector3 Position;
    int Rotation;
#ifdef BASIC_IMPL
    Player() : Position{Vector3{0.0f,0.0f,0.0f}}, Rotation{0} {}
#endif

#ifdef GRAPHIC_IMPL
    Model M;
    Color Tint;

    Player() : Position{Vector3{0.0f,0.0f,0.0f}}, Rotation{0}, Tint{BLUE}
    {
        M = LoadModel("assets/Mecha01.obj");
        texture = LoadTexture("assets/Mecha01.png");
        M.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    }

    void Draw()
    {
        DrawModelEx(M, Position, {0.0f, 1.0f, 0.0f}, Rotation,
                    {1.0f, 1.0f, 1.0f}, Tint);
    }

    void Draw2D(Camera& cam)
    {
        Vector2 textPos = GetWorldToScreen(
                Vector3{Position.x, Position.y + 6.0f, Position.z},
                cam);
        int w = MeasureText(Name.c_str(), 24);

        DrawText(Name.c_str(), textPos.x - w / 2, textPos.y, 24, BLACK);
        DrawText(Name.c_str(), textPos.x - w / 2 - 3, textPos.y - 3, 24, WHITE);
    }
#endif
};

#endif
