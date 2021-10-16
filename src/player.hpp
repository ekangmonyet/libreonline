#ifndef __LO_INC_PLAYER_HPP
#define __LO_INC_PLAYER_HPP

#include "raylib.h"

typedef unsigned int PlayerID;

class Player
{
    Texture2D texture;
public:
    PlayerID Id;
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
#endif
};

#endif
