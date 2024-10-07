#include <SDL.h>
#include <iostream>

constexpr auto SCREEN_HEIGHT = 1000;
constexpr auto SCREEN_WIDTH = 1000;
constexpr auto PADDLE_MOVEMENT_SPEED = 20;
constexpr auto BALL_BASE_X_VELOCITY = 10;
constexpr auto BALL_BASE_Y_VELOCITY = 10;
constexpr auto PI = 3.14;

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

void movePaddle(Paddle* paddle, enum Direction dir) {
	if (dir == LEFT) {
		(*(*paddle).paddle_rectangle).x -= PADDLE_MOVEMENT_SPEED;
	}
	else {
		(*(*paddle).paddle_rectangle).x += PADDLE_MOVEMENT_SPEED;
	}
}

void updateScreen(SDL_Renderer* renderer, Paddle* paddle, Ball* ball) {
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

	Ball game_ball = { NULL, ((3 / 2) * PI) * (1 / PI), 10, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6 };
	game_ball.hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));

	if (game_ball.hitbox != NULL) {
		*(game_ball.hitbox) = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6, 20, 20};
	}
	else {
		std::cout << "malloc failed";
		return 1;
	}
	
	updateScreen(gRenderer, &game_paddle, &game_ball);

	while (running) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else {
				SDL_Keycode pressed_key = event.key.keysym.sym;
				if (pressed_key == SDLK_a) {
					movePaddle(&game_paddle, LEFT);
				}
				else if (pressed_key == SDLK_d) {
					movePaddle(&game_paddle, RIGHT);
				}
			}
			updateScreen(gRenderer, &game_paddle, &game_ball);
		}
	}

	free(game_paddle.paddle_rectangle);

	return 0;
}