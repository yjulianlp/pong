#include <SDL.h>
#include <iostream>
#include <cmath>

constexpr auto SCREEN_HEIGHT = 1000;
constexpr auto SCREEN_WIDTH = 1000;
constexpr auto PADDLE_MOVEMENT_SPEED = 20;
constexpr auto BALL_BASE_X_VELOCITY = 5;
constexpr auto BALL_BASE_Y_VELOCITY = -5;
constexpr auto PI = 3.14;
constexpr auto INITIAL_BALL_ANGLE = ((3 / 2) * PI) * (180 / PI);
constexpr auto FRAME_DELAY = 1000 / 60;

enum Direction {
	LEFT,
	RIGHT
};

struct Paddle {
	SDL_Rect* paddle_rectangle;
};

struct Ball {
	SDL_Rect* hitbox;
	float angle;
	int radius;
	int center_x;
	int center_y;
};

void drawBallPoints(SDL_Renderer* renderer, int center_x, int center_y, int x_diff, int y_diff) {
	SDL_RenderDrawPoint(renderer, center_x + x_diff, center_y + y_diff);
	SDL_RenderDrawPoint(renderer, center_x + x_diff, center_y - y_diff);
	SDL_RenderDrawPoint(renderer, center_x - x_diff, center_y + y_diff);
	SDL_RenderDrawPoint(renderer, center_x - x_diff, center_y - y_diff);

	SDL_RenderDrawPoint(renderer, center_x + y_diff, center_y + x_diff);
	SDL_RenderDrawPoint(renderer, center_x + y_diff, center_y - x_diff);
	SDL_RenderDrawPoint(renderer, center_x - y_diff, center_y + x_diff);
	SDL_RenderDrawPoint(renderer, center_x - y_diff, center_y - x_diff);
}

void drawBall(SDL_Renderer* renderer, Ball* ball) {
	int x = 0, y = (*ball).radius;
	float decision_param = 1 - y;

	drawBallPoints(renderer, (*ball).center_x, (*ball).center_y, x, y);

	while (y > x) {
		x += 1;

		if (decision_param >= 0) {
			y -= 1;
			decision_param += (x * 2) - (y * 2) + 1;
		}
		else {
			decision_param += (x * 2) + 1;
		}
		drawBallPoints(renderer, (*ball).center_x, (*ball).center_y, x, y);
	}

}

void updateBall(Ball* ball) {
	float x_diff = BALL_BASE_X_VELOCITY * cos((*ball).angle);
	float y_diff = BALL_BASE_Y_VELOCITY * sin((*ball).angle);
	(*(*ball).hitbox).x += x_diff;
	(*(*ball).hitbox).y += y_diff;
	(*ball).center_x += x_diff;
	(*ball).center_y += y_diff;
}

void updatePaddle(Paddle* paddle, SDL_Event* event) {
	const Uint8* pressed_keys = SDL_GetKeyboardState(NULL);
	if (pressed_keys[SDL_SCANCODE_A]) {
		if ((*(*paddle).paddle_rectangle).x > 0) {
			(*(*paddle).paddle_rectangle).x -= PADDLE_MOVEMENT_SPEED;
		}	
	}else if(pressed_keys[SDL_SCANCODE_D]) {
		if ((*(*paddle).paddle_rectangle).x < SCREEN_WIDTH) {
			(*(*paddle).paddle_rectangle).x += PADDLE_MOVEMENT_SPEED;
		}
	}
}

void renderScreen(SDL_Renderer* renderer, Paddle* paddle, Ball* ball) {
	SDL_SetRenderDrawColor(renderer, 0 ,0 ,0 , 1);
	SDL_RenderClear(renderer);
	SDL_RenderFillRect(renderer, (*ball).hitbox);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	SDL_RenderFillRect(renderer, (*paddle).paddle_rectangle);
	drawBall(renderer, ball);
	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	bool running = true;
	SDL_Event event;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "failed to initialize";
	}

	SDL_Window* gWindow = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	Paddle game_paddle;
	game_paddle.paddle_rectangle = (SDL_Rect*) malloc(sizeof(SDL_Rect));
	if (game_paddle.paddle_rectangle != NULL) {
		*(game_paddle.paddle_rectangle) = { SCREEN_WIDTH / 2, SCREEN_HEIGHT - (SCREEN_HEIGHT / 6), 100, 20 };
	}
	else {
		std::cout << "malloc failed";
		return 1;
	}

	Ball game_ball = { NULL, INITIAL_BALL_ANGLE, 10, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6 };
	game_ball.hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));

	if (game_ball.hitbox != NULL) {
		*(game_ball.hitbox) = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6, 20, 20};
	}
	else {
		std::cout << "malloc failed";
		return 1;
	}

	while (running) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			updatePaddle(&game_paddle, &event);
		}
		updateBall(&game_ball);
		renderScreen(gRenderer, &game_paddle, &game_ball);

		SDL_Delay(FRAME_DELAY);
	}
	free(game_paddle.paddle_rectangle);

	return 0;
}