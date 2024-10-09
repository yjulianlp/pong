#include <SDL.h>
#include <iostream>
#include <cmath>
#include <cstdlib>

constexpr auto SCREEN_HEIGHT = 1000;
constexpr auto SCREEN_WIDTH = 1000;
constexpr auto PADDLE_MOVEMENT_SPEED = 20;
constexpr auto BALL_BASE_X_VELOCITY = -5;
constexpr auto BALL_BASE_Y_VELOCITY = 5;
constexpr auto PI = 3.14;
constexpr auto INITIAL_BALL_ANGLE = (3.14 / 4.0);
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
	int x_velocity;
	int y_velocity;
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

void updateBall(Ball* ball, Paddle* paddle, Paddle* paddle2) {
	float x_diff = ((*ball).x_velocity) * cos((*ball).angle);
	float y_diff = ((*ball).y_velocity) * sin((*ball).angle);
	(*(*ball).hitbox).x += x_diff;
	(*(*ball).hitbox).y += y_diff;
	(*ball).center_x += x_diff;
	(*ball).center_y += y_diff;

	
	if (SDL_HasIntersection((*ball).hitbox, (*paddle).paddle_rectangle)) {
		std::cout << "hit\n";
		std::cout << "\nold angle: " << (*ball).angle;
		(*ball).angle = ((rand() % 70) + 20) * (PI / 180.0);
		if (rand() % 2 == 0) {
			(*ball).x_velocity *= -1;
		}
		(*ball).y_velocity = -BALL_BASE_Y_VELOCITY;
		(*(*ball).hitbox).x += x_diff;
		(*(*ball).hitbox).y += y_diff;
		(*ball).center_x += x_diff;
		(*ball).center_y += y_diff;
	}
	else if (SDL_HasIntersection((*ball).hitbox, (*paddle2).paddle_rectangle)) {
		std::cout << "hit\n";
		std::cout << "\nold angle: " << (*ball).angle;
		(*ball).angle = ((rand() % 70)+20) * (PI / 180.0);
		if (rand() % 2 == 0) {
			(*ball).x_velocity *= -1;
		}
		(*ball).y_velocity = BALL_BASE_Y_VELOCITY;
		(*(*ball).hitbox).x += x_diff;
		(*(*ball).hitbox).y += y_diff;
		(*ball).center_x += x_diff;
		(*ball).center_y += y_diff;
	}
	else if ((*(*ball).hitbox).x <= (*ball).radius || (*(*ball).hitbox).x >= SCREEN_WIDTH - (*ball).radius) {
		std::cout << "\nhit side\n";
		(*ball).x_velocity *= -1;
		x_diff = ((*ball).x_velocity * 10) * cos((*ball).angle);
		y_diff = ((*ball).y_velocity * 10) * sin((*ball).angle);
		std::cout << "\n\ncurrent velocity:\nx: " << (*ball).x_velocity << "\ny: " << (*ball).y_velocity << "\n";
		std::cout << "current values:\ncos: " << cos((*ball).angle) << "\n sin: " << sin((*ball).angle) << "\n";
		if (y_diff < (*ball).y_velocity / 20) {

			std::cout << "\nsped up bounce";
			(*ball).angle += (PI / 12);
		}
		(*(*ball).hitbox).x += x_diff;
		(*(*ball).hitbox).y += y_diff;
		(*ball).center_x += x_diff;
		(*ball).center_y += y_diff;
	}
	else if ((*(*ball).hitbox).y <= (*ball).radius || (*(*ball).hitbox).y >= SCREEN_HEIGHT - (*ball).radius) {
		std::cout << "hit top or bottom\n";
		(*ball).y_velocity *= -1;
		x_diff = ((*ball).x_velocity * 10) * cos((*ball).angle);
		y_diff = ((*ball).y_velocity * 10) * sin((*ball).angle);
		(*(*ball).hitbox).x += x_diff;
		(*(*ball).hitbox).y += y_diff;
		(*ball).center_x += x_diff;
		(*ball).center_y += y_diff;
	}

}

void updatePaddle(Paddle* paddle, Paddle* paddle2, SDL_Event* event) {
	const Uint8* pressed_keys = SDL_GetKeyboardState(NULL);
	if (pressed_keys[SDL_SCANCODE_A]) {
		if ((*(*paddle).paddle_rectangle).x > 0) {
			(*(*paddle).paddle_rectangle).x -= PADDLE_MOVEMENT_SPEED;
		}	
	}
	else if(pressed_keys[SDL_SCANCODE_D]) {
		if ((*(*paddle).paddle_rectangle).x < SCREEN_WIDTH) {
			(*(*paddle).paddle_rectangle).x += PADDLE_MOVEMENT_SPEED;
		}
	}
	else if (pressed_keys[SDL_SCANCODE_J]) {
		if ((*(*paddle2).paddle_rectangle).x > 0) {
			(*(*paddle2).paddle_rectangle).x -= PADDLE_MOVEMENT_SPEED;
		}
	}
	else if (pressed_keys[SDL_SCANCODE_L]) {
		if ((*(*paddle2).paddle_rectangle).x < SCREEN_WIDTH) {
			(*(*paddle2).paddle_rectangle).x += PADDLE_MOVEMENT_SPEED;
		}
	}

}

void renderScreen(SDL_Renderer* renderer, Paddle* paddle, Paddle* paddle2, Ball* ball) {
	SDL_SetRenderDrawColor(renderer, 0 ,0 ,0 , 1);
	SDL_RenderClear(renderer);
	SDL_RenderFillRect(renderer, (*ball).hitbox);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	SDL_RenderFillRect(renderer, (*paddle).paddle_rectangle);
	SDL_RenderFillRect(renderer, (*paddle2).paddle_rectangle);
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

	Paddle game_paddle, game_paddle2;
	game_paddle.paddle_rectangle = (SDL_Rect*) malloc(sizeof(SDL_Rect));
	game_paddle2.paddle_rectangle = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	if (game_paddle.paddle_rectangle != NULL) {
		*(game_paddle.paddle_rectangle) = { SCREEN_WIDTH / 2, SCREEN_HEIGHT - (SCREEN_HEIGHT / 8), 100, 20 };
	}
	else {
		std::cout << "malloc failed";
		return 1;
	}
	if (game_paddle2.paddle_rectangle != NULL) {
		*(game_paddle2.paddle_rectangle) = { SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 8), 100, 20 };
	}
	else {
		std::cout << "malloc failed";
		return 1;
	}

	Ball game_ball = { NULL, INITIAL_BALL_ANGLE, 10, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6, BALL_BASE_X_VELOCITY, BALL_BASE_Y_VELOCITY};
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
			updatePaddle(&game_paddle, &game_paddle2, &event);
		}
		updateBall(&game_ball, &game_paddle, &game_paddle2);
		renderScreen(gRenderer, &game_paddle, &game_paddle2, &game_ball);

		SDL_Delay(FRAME_DELAY);
	}
	free(game_paddle.paddle_rectangle);

	return 0;
}