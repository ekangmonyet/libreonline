#ifndef __LO_INC_PLAYER_HPP
#define __LO_INC_PLAYER_HPP

#include "raylib.h"

class Player
{
    Texture2D texture;
public:
    Model M;
    Vector3 Position;
    int Rotation;
    void Draw();
    Player();
};

#ifdef IMPLEMENTATION
Player::Player() : Position{}, Rotation{0}
{
    M = LoadModel("assets/Mecha01.obj");
    texture = LoadTexture("assets/Mecha01.png");
    M.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
}

void Player::Draw()
{
    DrawModelEx(M, Position, {0.0f, 1.0f, 0.0f}, Rotation,
                {1.0f, 1.0f, 1.0f}, BLUE);
}

#endif


#endif
