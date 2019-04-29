#ifndef TILE_H
#define TILE_H

#include <SDL2/SDL.h>
#include "Texture.h"

//Tile constants
const int TILE_WIDTH = 20;
const int TILE_HEIGHT = 20;
const int TOTAL_TILES = 868; // 868 = 28*31
const int TOTAL_TILE_TYPES = 2;

//Types of tiles
const int SPACE_TILE = 0;
const int WALL_TILE = 1;
const int BLANK_TILE = 9;

//The dimensions of the level
const int LEVEL_WIDTH = 560;
const int LEVEL_HEIGHT = 620;

//The tile
class Tile
{
public:
    //Initializes position and type
    Tile(int x, int y, int tileType);

    //Shows the tile
    void render(Texture& tileTexture, SDL_Renderer* renderer);

    //Get the tile type
    int getType();

    //Get the collision box
    SDL_Rect getBox();

private:
    //The attributes of the tile
    SDL_Rect mBox;

    //The tile type
    int mType;
};

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

//Checks collision box against set of tiles
bool touchesWall(SDL_Rect box, Tile* tiles[]);

//Can the object move in the provided direction?
bool canMove(SDL_Rect box, int vel, int direction, Tile* tiles[]);

#endif // TILE_H
