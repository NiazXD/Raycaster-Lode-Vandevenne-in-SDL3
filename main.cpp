
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

constexpr int TEX_WIDTH = 64;
constexpr int TEX_HEIGHT = 64;

constexpr int MAP_WIDTH = 24;
constexpr int MAP_HEIGHT = 24;

int worldMap[MAP_HEIGHT][MAP_WIDTH] =
{
    {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
  {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
  {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
  {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
  {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
  {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
  {2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};


bool loadImage(std::vector<uint32_t>& texture, const char* path)
{
    SDL_Surface* surface = IMG_Load(path);

    if (!surface)
    {
        std::cerr << "Image loading failed: "
                  << path << " : " << SDL_GetError() << '\n';
        return false;
    }

    SDL_Surface* converted = SDL_ConvertSurface(
        surface,
        SDL_PIXELFORMAT_ARGB8888
    );

    SDL_DestroySurface(surface);

    if (!converted)
    {
        std::cerr << "Surface conversion failed: "
                  << SDL_GetError() << '\n';
        return false;
    }

    if (converted->w != TEX_WIDTH || converted->h != TEX_HEIGHT)
    {
        std::cerr << "Texture must be 64x64: " << path << '\n';
        SDL_DestroySurface(converted);
        return false;
    }

    texture.resize(TEX_WIDTH * TEX_HEIGHT);

    const uint8_t* pixels =
        static_cast<const uint8_t*>(converted->pixels);

    for (int y = 0; y < TEX_HEIGHT; y++)
    {
        const uint32_t* row =
            reinterpret_cast<const uint32_t*>(
                pixels + y * converted->pitch
            );

        for (int x = 0; x < TEX_WIDTH; x++)
        {
            texture[y * TEX_WIDTH + x] = row[x];
        }
    }

    SDL_DestroySurface(converted);
    return true;
}

uint32_t darken(uint32_t color)
{
    return (color & 0xFF000000u) |
           ((color & 0x00FEFEFEu) >> 1);
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL initialization failed: "
                  << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "SDL3 Raycaster",
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    if (!window)
    {
        std::cerr << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer)
    {
        std::cerr << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    
    std::vector<uint32_t> texture[8];

    const char* texturePaths[8] =
    {
        "pics/eagle.png",
        "pics/redbrick.png",
        "pics/purplestone.png",
        "pics/greystone.png",
        "pics/bluestone.png",
        "pics/mossy.png",
        "pics/wood.png",
        "pics/colorstone.png"
    };

    for (int i = 0; i < 8; i++)
    {
        if (!loadImage(texture[i], texturePaths[i]))
        {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    
    SDL_Texture* screenTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (!screenTexture)
    {
        std::cerr << SDL_GetError() << '\n';
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    std::vector<uint32_t> buffer(
        SCREEN_WIDTH * SCREEN_HEIGHT, 0
    );

    
    double posX = 22.0;
    double posY = 1.5;

    
    double dirX = -1.0;
    double dirY = 0.0;

    double planeX = 0.0;
    double planeY = 0.66;

    Uint64 oldTime = SDL_GetTicks();

    bool quit = false;

    while (!quit)
    {
        SDL_Event e;

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                quit = true;
        }

        Uint64 currentTime = SDL_GetTicks();

        double frameTime =
            (currentTime - oldTime) / 1000.0;

        oldTime = currentTime;

        frameTime = std::min(frameTime, 0.1);

        
        std::fill(buffer.begin(), buffer.end(), 0);

        for(int y = 0; y < SCREEN_HEIGHT; y++){
            float rayDirX0 = dirX - planeX;
            float rayDirY0 = dirY - planeY;
            float rayDirX1 = dirX + planeX;
            float rayDirY1 = dirY + planeY;

            int p = y - SCREEN_HEIGHT/2;

            float posZ = 0.5 * SCREEN_HEIGHT;

            float rowDistance = posZ / p;

            float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
            float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;

            float floorX = posX + rowDistance * rayDirX0;
            float floorY = posY + rowDistance * rayDirY0;

            for(int x = 0; x < SCREEN_WIDTH; ++x){
                int cellX = (int)(floorX);
                int cellY = (int)(floorY);

                int tx = (int)(TEX_WIDTH * (floorX - cellX)) & (TEX_WIDTH -1);
                int ty = (int)(TEX_HEIGHT * (floorY - cellY)) & (TEX_HEIGHT - 1);

                floorX += floorStepX;
                floorY += floorStepY;

                int floorTexture = 3;
                int ceilingTexture = 6;
                Uint32 color;

                color = texture[floorTexture][TEX_WIDTH * ty + tx];
                color = darken(color);
                buffer[y * SCREEN_WIDTH + x] = color;

                int ceilingTy = TEX_HEIGHT - ty - 1;
                color = texture[ceilingTexture][TEX_WIDTH * ceilingTy + tx];
                buffer[(SCREEN_HEIGHT - y - 1) * SCREEN_WIDTH + x] = color;
            }
        }

        
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            double cameraX =
                2.0 * x / double(SCREEN_WIDTH) - 1.0;

            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = static_cast<int>(posX);
            int mapY = static_cast<int>(posY);

            double sideDistX;
            double sideDistY;

            double deltaDistX =
                (rayDirX == 0.0) ? 1e30 : std::abs(1.0 / rayDirX);

            double deltaDistY =
                (rayDirY == 0.0) ? 1e30 : std::abs(1.0 / rayDirY);

            double perpWallDist = 0.0;

            int stepX;
            int stepY;

            int hit = 0;
            int side = 0;

            
            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }

            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }

            
            while (hit == 0)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                
                if (mapX < 0 || mapX >= MAP_WIDTH ||
                    mapY < 0 || mapY >= MAP_HEIGHT)
                {
                    break;
                }

                if (worldMap[mapY][mapX] > 0)
                {
                    hit = 1;
                }
            }

            
            if (!hit)
                continue;

            
            if (side == 0)
            {
                perpWallDist = sideDistX - deltaDistX;
            }
            else
            {
                perpWallDist = sideDistY - deltaDistY;
            }

            perpWallDist = std::max(perpWallDist, 0.001);

            
            int lineHeight =
                static_cast<int>(SCREEN_HEIGHT / perpWallDist);

            int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
            int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;

            drawStart = std::max(drawStart, 0);
            drawEnd = std::min(drawEnd, SCREEN_HEIGHT);

            
            int texNum = worldMap[mapY][mapX] - 1;

            if (texNum < 0 || texNum >= 8)
                continue;

            
            double wallX;

            if (side == 0)
            {
                wallX = posY + perpWallDist * rayDirY;
            }
            else
            {
                wallX = posX + perpWallDist * rayDirX;
            }

            wallX -= std::floor(wallX);

           
            int texX = static_cast<int>(
                wallX * TEX_WIDTH
            );

            texX = std::clamp(texX, 0, TEX_WIDTH - 1);

            
            if (side == 0 && rayDirX > 0)
                texX = TEX_WIDTH - texX - 1;

            if (side == 1 && rayDirY < 0)
                texX = TEX_WIDTH - texX - 1;

           
            double step =
                1.0 * TEX_HEIGHT / lineHeight;

            double texPos =
                (drawStart - SCREEN_HEIGHT / 2.0 +
                 lineHeight / 2.0) * step;

            
            for (int y = drawStart; y < drawEnd; y++)
            {
                int texY =
                    static_cast<int>(texPos) & (TEX_HEIGHT - 1);

                texPos += step;

                int textureIndex =
                    texY * TEX_WIDTH + texX;

                uint32_t color =
                    texture[texNum][textureIndex];

                // Darken one wall orientation.
                if (side == 1)
                {
                    color = darken(color);
                }

                buffer[y * SCREEN_WIDTH + x] = color;
            }
        }

    
        if (!SDL_UpdateTexture(
            screenTexture,
            nullptr,
            buffer.data(),
            SCREEN_WIDTH * sizeof(uint32_t)))
        {
            std::cerr << SDL_GetError() << '\n';
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderTexture(
            renderer,
            screenTexture,
            nullptr,
            nullptr
        );

        SDL_RenderPresent(renderer);

        
        const bool* keys = SDL_GetKeyboardState(nullptr);

        double moveSpeed = frameTime * 5.0;
        double rotSpeed = frameTime * 3.0;

        
        if (keys[SDL_SCANCODE_UP])
        {
            double nextX = posX + dirX * moveSpeed;
            double nextY = posY + dirY * moveSpeed;

            int checkX = static_cast<int>(nextX);
            int checkY = static_cast<int>(nextY);

            if (checkX >= 0 && checkX < MAP_WIDTH &&
                static_cast<int>(posY) >= 0 &&
                static_cast<int>(posY) < MAP_HEIGHT &&
                worldMap[static_cast<int>(posY)][checkX] == 0)
            {
                posX = nextX;
            }

            if (static_cast<int>(posX) >= 0 &&
                static_cast<int>(posX) < MAP_WIDTH &&
                checkY >= 0 && checkY < MAP_HEIGHT &&
                worldMap[checkY][static_cast<int>(posX)] == 0)
            {
                posY = nextY;
            }
        }

        
        if (keys[SDL_SCANCODE_DOWN])
        {
            double nextX = posX - dirX * moveSpeed;
            double nextY = posY - dirY * moveSpeed;

            int checkX = static_cast<int>(nextX);
            int checkY = static_cast<int>(nextY);

            if (checkX >= 0 && checkX < MAP_WIDTH &&
                static_cast<int>(posY) >= 0 &&
                static_cast<int>(posY) < MAP_HEIGHT &&
                worldMap[static_cast<int>(posY)][checkX] == 0)
            {
                posX = nextX;
            }

            if (static_cast<int>(posX) >= 0 &&
                static_cast<int>(posX) < MAP_WIDTH &&
                checkY >= 0 && checkY < MAP_HEIGHT &&
                worldMap[checkY][static_cast<int>(posX)] == 0)
            {
                posY = nextY;
            }
        }

        
        if (keys[SDL_SCANCODE_RIGHT])
        {
            double oldDirX = dirX;

            dirX = dirX * cos(-rotSpeed) -
                   dirY * sin(-rotSpeed);

            dirY = oldDirX * sin(-rotSpeed) +
                   dirY * cos(-rotSpeed);

            double oldPlaneX = planeX;

            planeX = planeX * cos(-rotSpeed) -
                     planeY * sin(-rotSpeed);

            planeY = oldPlaneX * sin(-rotSpeed) +
                     planeY * cos(-rotSpeed);
        }

        
        if (keys[SDL_SCANCODE_LEFT])
        {
            double oldDirX = dirX;

            dirX = dirX * cos(rotSpeed) -
                   dirY * sin(rotSpeed);

            dirY = oldDirX * sin(rotSpeed) +
                   dirY * cos(rotSpeed);

            double oldPlaneX = planeX;

            planeX = planeX * cos(rotSpeed) -
                     planeY * sin(rotSpeed);

            planeY = oldPlaneX * sin(rotSpeed) +
                     planeY * cos(rotSpeed);
        }
    }

    
    SDL_DestroyTexture(screenTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
