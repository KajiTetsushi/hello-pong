//
//  main.c
//  hello-pong
//
//  Created by Kaji Tetsushi on 1/31/22.
//  Copyright © 2022 Kaji Tetsushi. All rights reserved.
//

#include <SDL2/SDL.h>
#include <stdbool.h>

#pragma mark Game
const int WIDTH = 600;
const int HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool served = false;

bool Initialize(void);
void Update(float);
void Shutdown(void);

#pragma mark Ball
const int BALL_SIZE = 10;

typedef struct Ball {
    float x;
    float y;
    float xSpeed;
    float ySpeed;
    int size;
} Ball;

Ball ball;

Ball MakeBall(int size);
void UpdateBall(Ball *ball, float elapsed);
void RenderBall(const Ball*);

#pragma mark Player
const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 75;
const int PLAYER_MARGIN = 10;
const int PLAYER_MOVE_SPEED = 150.0f;

typedef struct Player {
    int score;
    float yPosition;
} Player;

Player player1;
Player player2;

Player MakePlayer(void);
void UpdatePlayers(float elapsed);
void RenderPlayers(void);

void UpdateScore(int player, int points);

#pragma mark Game Implementation
int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));
    atexit(Shutdown);
    
    if (!Initialize()) {
        exit(1);
    }
    
    bool quit = false;
    SDL_Event event;
    
    Uint32 lastTick = SDL_GetTicks();
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        Uint32 curTick = SDL_GetTicks();
        Uint32 diff = curTick - lastTick;
        float elapsed = diff / 1000.0f;
        Update(elapsed);
        lastTick = curTick;
    }
    
    SDL_Quit();
}

bool Initialize(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    
    window = SDL_CreateWindow("Pong",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WIDTH, HEIGHT,
                              SDL_WINDOW_SHOWN);
    
    if (!window) {
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!window) {
        return false;
    }
    
    ball = MakeBall(BALL_SIZE);
    player1 = MakePlayer();
    player2 = MakePlayer();
    
    return true;
}

void Update(float elapsed) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    UpdateBall(&ball, elapsed);
    RenderBall(&ball);
    
    UpdatePlayers(elapsed);
    RenderPlayers();
    
    SDL_RenderPresent(renderer);
}

void Shutdown(void) {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    
    if (window) {
        SDL_DestroyWindow(window);
    }
    
    SDL_Quit();
}

bool CoinFlip(void) {
    return rand() % 2 == 1 ? true : false;
}

Ball MakeBall(int size) {
    const float SPEED = 120;
    Ball ball = {
        .x = WIDTH / 2 - size / 2,
        .y = HEIGHT / 2 - size / 2,
        .xSpeed = SPEED * (CoinFlip() ? 1 : -1),
        .ySpeed = SPEED * (CoinFlip() ? 1 : -1),
        .size = size,
    };
    return ball;
}

void UpdateBall(Ball *ball, float elapsed) {
    // Set the ball to the center when it isn't served.
    if (!served) {
        ball->x = WIDTH / 2;
        ball->y = HEIGHT / 2;
        return;
    }
    
    ball->x += ball->xSpeed * elapsed;
    ball->y += ball->ySpeed * elapsed;
    
    int ballHalfSize = ball->size / 2;
    
    if (ball->x < ballHalfSize) {
//        ball->xSpeed = fabs(ball->xSpeed);
        UpdateScore(2, 100);
    }
    if (ball->x > WIDTH - ballHalfSize) {
//        ball->xSpeed = -fabs(ball->xSpeed);
        UpdateScore(1, 100);
    }
    
    if (ball->y < ballHalfSize) {
        ball->ySpeed = fabs(ball->ySpeed);
    }
    if (ball->y > HEIGHT - ballHalfSize) {
        ball->ySpeed = -fabs(ball->ySpeed);
    }
}

void RenderBall(const Ball *ball) {
    int ballSize = ball->size;
    int ballHalfSize = ballSize / 2;
    SDL_Rect rect = {
        .x = ball->x - ballHalfSize,
        .y = ball->y - ballHalfSize,
        .w = ballSize,
        .h = ballSize,
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

Player MakePlayer(void) {
    Player player = {
        .score = 0,
        .yPosition = HEIGHT / 2,
    };
    return player;
}

void UpdatePlayer(Player *player) {
    
}

void UpdatePlayers(float elapsed) {
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    
    if (keyboardState[SDL_SCANCODE_SPACE]) {
        served = true;
    }
    
    if (keyboardState[SDL_SCANCODE_W]) {
        player1.yPosition -= PLAYER_MOVE_SPEED * elapsed;
    }
    
    if (keyboardState[SDL_SCANCODE_S]) {
        player1.yPosition += PLAYER_MOVE_SPEED * elapsed;
    }
    
    if (keyboardState[SDL_SCANCODE_UP]) {
        player2.yPosition -= PLAYER_MOVE_SPEED * elapsed;
    }
    
    if (keyboardState[SDL_SCANCODE_DOWN]) {
        player2.yPosition += PLAYER_MOVE_SPEED * elapsed;
    }
    
    // Check if player paddle goes out of bounds.
    if (player1.yPosition < PLAYER_HEIGHT / 2) {
        player1.yPosition = PLAYER_HEIGHT / 2;
    }
    
    if (player1.yPosition >= HEIGHT - PLAYER_HEIGHT / 2) {
        player1.yPosition = HEIGHT - PLAYER_HEIGHT / 2;
    }
    
    if (player2.yPosition < PLAYER_HEIGHT / 2) {
        player2.yPosition = PLAYER_HEIGHT / 2;
    }
    
    if (player2.yPosition >= HEIGHT - PLAYER_HEIGHT / 2) {
        player2.yPosition = HEIGHT - PLAYER_HEIGHT / 2;
    }
    
    int ballHalfSize = ball.size / 2;
    
    // Check if ball rect overlaps with either player rect
    SDL_Rect ballRect = {
        .x = ball.x - ballHalfSize,
        .y = ball.y - ballHalfSize,
        .w = ball.size,
        .h = ball.size,
    };
    
    SDL_Rect player1Rect = {
        .x = PLAYER_MARGIN,
        .y = (int)(player1.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT,
    };
    
    if (SDL_HasIntersection(&ballRect, &player1Rect)) {
        // Ball goes to the right, but doesn't change vertical direction.
        ball.xSpeed = fabs(ball.xSpeed);
    }
    
    SDL_Rect player2Rect = {
        .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
        .y = (int)(player2.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT,
    };
    
    if (SDL_HasIntersection(&ballRect, &player2Rect)) {
        // Ball goes to the left, but doesn't change vertical direction.
        ball.xSpeed = -fabs(ball.xSpeed);
    }
}

void RenderPlayer(Player *player) {
    
}

void RenderPlayers(void) {
    // Render Player 1 (left, red);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect player1Rect = {
        .x = PLAYER_MARGIN,
        .y = (int)(player1.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT,
    };
    SDL_RenderFillRect(renderer, &player1Rect);
    
    // Render Player 2 (right, blue);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect player2Rect = {
        .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
        .y = (int)(player2.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT,
    };
    SDL_RenderFillRect(renderer, &player2Rect);
}

void UpdateScore(int player, int points) {
    // Start over.
    served = false;
    
    if (player == 1) {
        player1.score += points;
    }
    if (player == 2) {
        player2.score += points;
    }
    
    char *fmt = "Player 1: %d | Player 2: %d";
    int len = snprintf(NULL, 0, fmt, player1.score, player2.score);
    char buf[len + 1];
    snprintf(buf, len + 1, fmt, player1.score, player2.score);
    SDL_SetWindowTitle(window, buf);
}

//SDL_Rect GetPlayerRect(Player *player);
