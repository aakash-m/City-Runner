#include "raylib.h"

struct AnimData
{
  Rectangle rect;
  Vector2 pos;
  int frame;
  float runningTime;
  float updateTime;
};

//Function to check if the player sprite is on ground or not
bool isOnGround(AnimData data, int windowHeight)
{
  return data.pos.y >= windowHeight - data.rect.height;
}

//
AnimData updateAnimData(AnimData data, float deltaTime, int maxFrameVal)
{
  //update running time
  data.runningTime += deltaTime;
  if (data.runningTime >= data.updateTime)
  {
    data.runningTime = 0.0;
    //update animation frame
    data.rect.x = data.frame * data.rect.width;
    data.frame++;

    if (data.frame > maxFrameVal)
    {
      data.frame = 0;
    }
  }

  return data;
}

int main()
{

  //Window dimensions
  int windowDimensions[2];
  windowDimensions[0] = 800;
  windowDimensions[1] = 450;

  //Initialize the window
  InitWindow(windowDimensions[0], windowDimensions[1], "City Runner");

  //acceleration due to gravity (pixels/sec)/sec
  const int gravity = 1000;

  // Is the rectangle in the air
  bool isInAir;

  //Texture 2D player - Scarfy
  Texture2D scarfy = LoadTexture("textures/scarfy.png");

  AnimData scarfyData;
  scarfyData.rect.width = scarfy.width / 6;
  scarfyData.rect.height = scarfy.height;
  scarfyData.rect.x = scarfyData.rect.y = 0;
  scarfyData.pos.x = windowDimensions[0] / 2 - scarfyData.rect.width / 2;
  scarfyData.pos.y = windowDimensions[1] - scarfyData.rect.height;
  scarfyData.updateTime = 1.0 / 12.0;
  scarfyData.runningTime = 0.0;

  //Jump velocity(pixels/sec)
  const int jumpVel = -600;

  //Texture 2D player - Nebula
  Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

  const int sizeOfNebulae{3};
  AnimData nebulae[sizeOfNebulae]{};

  for (int i = 0; i < sizeOfNebulae; i++)
  {
    nebulae[i].rect.x = 0;
    nebulae[i].rect.y = 0;
    nebulae[i].rect.width = nebula.width / 8;
    nebulae[i].rect.height = nebula.height / 8;
    nebulae[i].pos.x = windowDimensions[0] + i * 300;
    nebulae[i].pos.y = windowDimensions[1] - nebula.height / 8;
    nebulae[i].frame = 0;
    nebulae[i].runningTime = 0.0;
    nebulae[i].updateTime = 0.0;
  }

  float finishLine{nebulae[sizeOfNebulae - 1].pos.x};
  //nebula x velocity (pixels/sec)
  int nebulaVel{-200};

  int velocity = 0;

  SetTargetFPS(60);

  //background
  Texture2D background = LoadTexture("textures/far-buildings.png");
  float bgX{};
  //midground
  Texture2D midground = LoadTexture("textures/back-buildings.png");
  float mgX{};
  //foreground
  Texture2D foreground = LoadTexture("textures/foreground.png");
  float fgX{};

  bool collision{};

  while (!WindowShouldClose())
  {
    //delta time(time since last frame)
    const float dT = GetFrameTime();

    //Start drawing
    BeginDrawing();

    ClearBackground(WHITE);

    //scroll background,midground,foreground
    bgX -= 50 * dT;
    mgX -= 100 * dT;
    fgX -= 150 * dT;

    if (bgX <= -background.width * 3)
    {
      bgX = 0.0;
    }

    if (mgX <= -midground.width * 3)
    {
      mgX = 0.0;
    }

    if (fgX <= -foreground.width * 3)
    {
      fgX = 0.0;
    }

    //Draw texture for the background,midground and foreground
    Vector2 bgPos1{bgX, 0.0};
    DrawTextureEx(background, bgPos1, 0.0, 3, WHITE);
    Vector2 bgPos2{bgX + background.width * 3, 0.0};
    DrawTextureEx(background, bgPos2, 0.0, 3, WHITE);
    Vector2 mgPos1{mgX, 0.0};
    DrawTextureEx(midground, mgPos1, 0.0, 3, WHITE);
    Vector2 mgPos2{mgX + midground.width * 3, 0.0};
    DrawTextureEx(midground, mgPos2, 0.0, 3, WHITE);
    Vector2 fgPos1{fgX, 0.0};
    DrawTextureEx(foreground, fgPos1, 0.0, 3, WHITE);
    Vector2 fgPos2{fgX + foreground.width * 3, 0.0};
    DrawTextureEx(foreground, fgPos2, 0.0, 3, WHITE);

    //Perform ground check
    if (isOnGround(scarfyData, windowDimensions[1]))
    {
      //rect is on ground
      velocity = 0;
      isInAir = false;
    }
    else
    {
      //rect is in air
      //apply gravity
      velocity += gravity * dT;
      isInAir = true;
    }

    if (IsKeyDown(KEY_SPACE) && !isInAir)
    {
      velocity += jumpVel;
    }

    //update scarfy position
    scarfyData.pos.y += velocity * dT;

    //update each nebula position
    for (int i = 0; i < sizeOfNebulae; i++)
    {
      nebulae[i].pos.x += nebulaVel * dT;
    }

    //Update finish line
    finishLine += nebulaVel * dT;

    //Update Scarfy's animation
    if (!isInAir)
    {
      scarfyData = updateAnimData(scarfyData, dT, 5);
    }

    //Update Nebula's animation

    for (int i = 0; i < sizeOfNebulae; i++)
    {

      nebulae[i] = updateAnimData(nebulae[i], dT, 7);
    }

    for (AnimData nebula : nebulae)
    {
      float padding{50};
      Rectangle nebulaRect{
          nebula.pos.x + padding,
          nebula.pos.y + padding,
          nebula.rect.width - 2 * padding,
          nebula.rect.height - 2 * padding};
      Rectangle scarfyRect{
          scarfyData.pos.x,
          scarfyData.pos.y,
          scarfyData.rect.width,
          scarfyData.rect.height};

      if (CheckCollisionRecs(nebulaRect, scarfyRect))
      {
        collision = true;
      }
    }

    if (collision)
    {
      //lose the game
      DrawText("Game Over..!!", windowDimensions[0] / 4, windowDimensions[1] / 2, 40, RED);
    }
    else if (scarfyData.pos.x >= finishLine)
    {
      //Win the game
      DrawText("You Win..!!", windowDimensions[0] / 4, windowDimensions[1] / 2, 40, RED);
    }
    else
    {
      //Draw Nebula
      for (int i = 0; i < sizeOfNebulae; i++)
      {
        DrawTextureRec(nebula, nebulae[i].rect, nebulae[i].pos, WHITE);
      }

      //Draw Scarfy
      DrawTextureRec(scarfy, scarfyData.rect, scarfyData.pos, WHITE);
    }

    //End drawing
    EndDrawing();
  }

  //Unload Texture
  UnloadTexture(scarfy);
  UnloadTexture(nebula);
  UnloadTexture(background);
  UnloadTexture(midground);
  UnloadTexture(foreground);
  CloseWindow();
}
