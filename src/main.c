#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct PongPaddle {
    float x;
    float y;
} PongPaddle;

typedef struct PongBall {
    float x;
    float y;
    float velocity_x;
    float velocity_y;
} PongBall;

enum CollisionType {
    COLLIDE_PADDLE_AI,
    COLLIDE_PADDLE_P1,
    COLLIDE_WALL,
    COLLIDE_NONE
};

void pong_paddle_render(
    SDL_Renderer *renderer,
    PongPaddle paddle
) {
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_Rect rect = { paddle.x,paddle.y,12,100 };
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
}

void pong_ball_render(
    SDL_Renderer *renderer,
    PongBall ball
) {
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_Rect rect = { ball.x,ball.y,6,6 };
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
}

// epic ai
int pong_paddle_ai_step(
    PongPaddle *paddle,
    PongBall ball
) {
    if (ball.y > paddle->y + 47) {
        return 1;
    }
    else if (ball.y < paddle->y + 47){
        return -1;
    }
    return 0;
}

void pong_ball_step(PongBall *ball) {
    ball->y += ball->velocity_y;
    ball->x += ball->velocity_x;
}

void pong_ball_reset(PongBall *ball) {
    ball->x = 397;
    ball->y = 297;
    ball->velocity_x = 1;
    ball->velocity_y = 0;
}

enum CollisionType pong_test_collision(
    PongPaddle p1,
    PongPaddle ai,
    PongBall ball
) {
    if (ball.x > p1.x - 6 && (ball.y > p1.y && ball.y < p1.y + 100)) {
        return COLLIDE_PADDLE_P1;
    }
    else if (ball.x < ai.x + 12 && (ball.y > ai.y && ball.y < ai.y + 100)) {
        return COLLIDE_PADDLE_AI;
    } else if (ball.y < 0 || ball.y > 594) {
        return COLLIDE_WALL;
    }
    else return COLLIDE_NONE;
}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("pong",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_OPENGL);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    bool running = true;
    SDL_Event event;

    // initialize pong paddles
    PongPaddle paddle_p1 = { 688, 250 };
    PongPaddle paddle_ai = { 100, 250 };
    PongBall ball = { 397, 297, 1, 0 };

    const uint8_t* keystate;
    float p1_y_move_speed;
    float ai_y_move_speed;

    while(running) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        keystate = SDL_GetKeyboardState(NULL);
        if(keystate[SDL_SCANCODE_W]) {
            p1_y_move_speed = -1;
        }
        else if(keystate[SDL_SCANCODE_S]) {
            p1_y_move_speed = 1;
        } else {
            p1_y_move_speed = 0;
        }
        paddle_p1.y += p1_y_move_speed;
        paddle_ai.y += ai_y_move_speed;

        if(paddle_p1.y < 0)
            paddle_p1.y = 0;
        if(paddle_p1.y > 500)
            paddle_p1.y = 500;
        if(paddle_ai.y < 0)
            paddle_ai.y = 0;
        if(paddle_ai.y > 500)
            paddle_ai.y = 500;


        switch(pong_test_collision(paddle_p1, paddle_ai, ball)) {
            case COLLIDE_PADDLE_AI: ball.velocity_x = -ball.velocity_x + 0.5;
                                    ball.velocity_y += ai_y_move_speed;
                                    break;
            case COLLIDE_PADDLE_P1: ball.velocity_x = -(ball.velocity_x + 0.5);
                                    ball.velocity_y += p1_y_move_speed;
                                    break;
            case COLLIDE_WALL: ball.velocity_y = -ball.velocity_y;
                               break;
            case COLLIDE_NONE: break;
        }

        ai_y_move_speed = pong_paddle_ai_step(&paddle_ai, ball);
        pong_ball_step(&ball);

        if(ball.x > paddle_p1.x + 12) {
            pong_ball_reset(&ball);
            printf("AI Won\n");
        }
        else if (ball.x < paddle_ai.x) {
            pong_ball_reset(&ball);
            printf("You Won\n");
        }

        SDL_RenderClear(renderer);
        pong_paddle_render(renderer, paddle_p1);
        pong_paddle_render(renderer, paddle_ai);
        pong_ball_render(renderer, ball);

        SDL_RenderPresent(renderer);
    }

    // Release resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
