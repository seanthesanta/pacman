#include "Game.h"

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow("PACMAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!( IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia(Tile* tiles[], Yummy* yummy[])
{
	//Loading success flag
	bool success = true;

	//Load blank texture
	if (!gBlankTexture.loadFromFile(gRenderer, "test/blank.bmp"))
    {
        printf("Failed to load blank texture!\n");
        success = false;
    }

	//Load dot texture
	if (!gPacmanTexture.loadFromFile(gRenderer, "test/pacman.bmp"))
	{
		printf("Failed to load pacman texture!\n");
		success = false;
	}

	//Load wall tile texture
	if (!gWallTexture.loadFromFile(gRenderer, "test/wall.bmp"))
	{
		printf("Failed to load wall tile texture!\n");
		success = false;
	}

    //Load space tile texture
    if (!gSpaceTexture.loadFromFile(gRenderer, "test/space.bmp"))
    {
        printf("Failed to load space tile texture!\n");
        success = false;
    }

	//Load small yummy texture
	if (!gSmallYummyTexture.loadFromFile(gRenderer, "test/smallyummy.bmp"))
    {
        printf("Failed to load small yummy texture!\n");
        success = false;
    }

    //Load big yummy texture
    if (!gBigYummyTexture.loadFromFile(gRenderer, "test/bigyummy.bmp"))
    {
        printf("Failed to load big yummy texture!\n");
        success = false;
    }

	//Load tiles: space, wall, yummy
	if (!setTiles(tiles, yummy))
	{
		printf("Failed to load tile set!\n");
		success = false;
	}

	return success;
}

void close(Tile* tiles[], Yummy* yummy[])
{
	//Deallocate space and wall tiles
	for (int i = 0; i < TOTAL_TILES; i++)
	{
		 if (tiles[i] != NULL)
		 {
			delete tiles[i];
			tiles[i] = NULL;
		 }
	}

    //Deallocate yummy
    for (int i = 0; i < TOTAL_TILES; i++)
    {
        if (yummy[i] != NULL)
        {
            delete yummy[i];
            yummy[i] = NULL;
        }
    }

	//Free loaded images
	gPacmanTexture.free();
	gWallTexture.free();
	gSpaceTexture.free();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool setTiles(Tile* tiles[], Yummy* yummy[])
{
	//Success flag
	bool success = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream Map("test/Map.txt");

    //If the map couldn't be loaded
    if(!Map.is_open())
    {
		printf("Unable to load map file!\n");
		success = false;
    }
	else
	{
		//Initialize the tiles
		for (int i = 0; i < TOTAL_TILES; i++)
		{
			//Determines what kind of tile will be made
			int tileType;

			//Read tile from map file
			Map >> tileType;

			//If the was a problem in reading the map
			if (Map.fail())
			{
				//Stop loading map
				printf("Error loading map: Unexpected end of file!\n");
				success = false;
				break;
			}

			//If the number is a valid tile number
			if (((tileType >= 0) && (tileType < TOTAL_TILE_TYPES)) || tileType == BLANK_TILE)
			{
				tiles[i] = new Tile(x, y, tileType);

				//Set big/small yummy
				switch (tileType)
                {
                case BLANK_TILE:
                    yummy[i] = new Yummy(UNDEFINED_X, UNDEFINED_Y, NO_YUMMY);
                    break;
                case WALL_TILE:
                    yummy[i] = new Yummy(UNDEFINED_X, UNDEFINED_Y, NO_YUMMY);
                    break;
                case SPACE_TILE:
                    if (i == 146 || i == 161 || i == 566 || i == 581)
                    {
                        int _x = tiles[i]->getBox().x + (TILE_WIDTH - BIG_YUMMY_WIDTH) / 2;
                        int _y = tiles[i]->getBox().y + (TILE_HEIGHT - BIG_YUMMY_HEIGHT) / 2;
                        yummy[i] = new Yummy(_x, _y, BIG_YUMMY);
                    }
                    else
                    {
                        int _x = tiles[i]->getBox().x + (TILE_WIDTH - SMALL_YUMMY_WIDTH) / 2;
                        int _y = tiles[i]->getBox().y + (TILE_HEIGHT - SMALL_YUMMY_HEIGHT) / 2;
                        yummy[i] = new Yummy(_x, _y, SMALL_YUMMY);
                    }
                    break;
                }
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf( "Error loading map: Invalid tile type at %d!\n", i );
				success = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if (x >= LEVEL_WIDTH)
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
	}

    //Close the file
    Map.close();

    //If the map was loaded fine
    return success;
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if(!init())
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//The space and wall tiles
		Tile* tileSet[TOTAL_TILES];

		//The yummy
        Yummy* yummySet[TOTAL_TILES];

		//Load media
		if (!loadMedia(tileSet, yummySet))
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The pacman
			Pacman pacman(TILE_WIDTH, TILE_HEIGHT);

			//While application is running
			while (!quit)
			{
			    //Check if the game has finished
			    if (pacman.getYummy() <= 0)
                {
                    quit = true;
                }

				//Handle events on queue
				if (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//Handle input for the pacman and update velocity
                    pacman.handleEvent(e, tileSet);
				}
				else
                {
                    pacman.handlePending(tileSet);
                }

                //Move pacman
				pacman.move(tileSet, SCREEN_WIDTH, SCREEN_HEIGHT);

				//Did pacman eat some yummy?
                std::vector<int> eatenYummy = pacman.eatYummy(yummySet);
                for (std::vector<int>::iterator itr = eatenYummy.begin(); itr != eatenYummy.end(); itr++)
                {
                    yummySet[*itr]->deleteYummy();
                }

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render space tiles and wall tiles
				for (int i = 0; i < TOTAL_TILES; i++)
				{
					switch (tileSet[i]->getType())
					{
                    //Render space tiles along with big yummy and small yummy
                    case SPACE_TILE:
                        tileSet[i]->render(gSpaceTexture, gRenderer);
                        break;
                    case WALL_TILE:
                        tileSet[i]->render(gWallTexture, gRenderer);
                        break;
                    case BLANK_TILE:
                        tileSet[i]->render(gBlankTexture, gRenderer);
                        break;
                    default:
                        break;
					}
				}

				//Render big yummy and small yummy
				for (int i = 0; i < TOTAL_TILES; i++)
                {
                    switch (yummySet[i]->getType())
                    {
                    case SMALL_YUMMY:
                        yummySet[i]->render(gSmallYummyTexture, gRenderer);
                        break;
                    case BIG_YUMMY:
                        yummySet[i]->render(gBigYummyTexture, gRenderer);
                        break;
                    case NO_YUMMY:
                        break;
                    default:
                        break;
                    }
                }

				//Render pacman
				pacman.render(gPacmanTexture, gRenderer);

				//Update screen
				SDL_RenderPresent(gRenderer);

				//Debug
				printf("yummy left: %d\n", pacman.getYummy());
			}
		}

		//Free resources and close SDL
		close(tileSet, yummySet);
	}
	return 0;
}
