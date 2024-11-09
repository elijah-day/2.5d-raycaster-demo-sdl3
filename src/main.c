#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define KEY_COUNT 322
#define PI 3.14159
#define DEGREES_TO_RADIANS PI / 180

#define MAP_W 16
#define MAP_H 16

//static int map[MAP_W][MAP_H] =
//{
//	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
//};

typedef struct input_t
{
	bool key_presses[KEY_COUNT];
	bool running;
	float mouse_x_pos;
	float mouse_y_pos;
	int mouse_state;
}
input_t;

typedef struct player_t
{
	double x;
	double y;
	double theta;
	double movement_speed;
	double turn_speed;
}
player_t;

typedef struct timing_t
{
	uint64_t t_current;
	uint64_t t_start;
	uint64_t delta_t_target;
}
timing_t;

typedef struct video_t
{
	SDL_Renderer *renderer;
	SDL_Window *window;

	int window_width;
	int window_height;

	double field_of_view;
	double view_distance;
	int ray_count;
}
video_t;

bool check_time_elapsed(timing_t *timing)
{
	timing->t_current = SDL_GetTicks();

	if(timing->t_current - timing->t_start > timing->delta_t_target)
	{
		timing->t_start = timing->t_current;
		return true;
	}

	return false;
}

void handle_controls(input_t *input, player_t *player)
{
	if(input->key_presses[SDL_SCANCODE_W])
	{
		player->x += player->movement_speed * cos(player->theta * DEGREES_TO_RADIANS);
		player->y += player->movement_speed * sin(player->theta * DEGREES_TO_RADIANS);
	}

	if(input->key_presses[SDL_SCANCODE_A])
	{
		player->x -= player->movement_speed * sin(player->theta * DEGREES_TO_RADIANS);
		player->y += player->movement_speed * cos(player->theta * DEGREES_TO_RADIANS);
	}

	if(input->key_presses[SDL_SCANCODE_S])
	{
		player->x -= player->movement_speed * cos(player->theta * DEGREES_TO_RADIANS);
		player->y -= player->movement_speed * sin(player->theta * DEGREES_TO_RADIANS);
	}

	if(input->key_presses[SDL_SCANCODE_D])
	{
		player->x += player->movement_speed * sin(player->theta * DEGREES_TO_RADIANS);
		player->y -= player->movement_speed * cos(player->theta * DEGREES_TO_RADIANS);
	}

	if(input->key_presses[SDL_SCANCODE_LEFT])
	{
		player->theta += player->turn_speed;
	}

	if(input->key_presses[SDL_SCANCODE_RIGHT])
	{
		player->theta -= player->turn_speed;
	}
}

void handle_input(input_t *input, video_t *video)
{
	static SDL_Event event;

	while(SDL_PollEvent(&event)) switch(event.type)
	{
		case SDL_EVENT_KEY_DOWN:
			input->key_presses[event.key.scancode] = true;
			break;

		case SDL_EVENT_KEY_UP:
			input->key_presses[event.key.scancode] = false;
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			input->mouse_state = SDL_GetMouseState(NULL, NULL);
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			input->mouse_state = SDL_GetMouseState(NULL, NULL);
			break;

		case SDL_EVENT_MOUSE_MOTION:
			SDL_GetMouseState(&input->mouse_x_pos, &input->mouse_y_pos);
			break;

		case SDL_EVENT_QUIT:
			input->running = false;
			break;

		case SDL_EVENT_WINDOW_RESIZED:
			/*
			 * Really we shouldn't be handling video events in here directly...
			 * It'd probably be better to have some sort of state variable for
			 * anything outside of keyboard and mouse input.
			 */
			SDL_GetWindowSize(video->window, &video->window_width, &video->window_height);
			break;
	}
}

void init_input(input_t *input)
{
	memset(input->key_presses, false, KEY_COUNT);
}

void init_player(player_t *player)
{
	player->x = 0;
	player->y = 0;
	player->theta = 0;
	player->movement_speed = 0.05;
	player->turn_speed = 1;
}

void init_timing(timing_t *timing)
{
	timing->t_current = 0;
	timing->t_start = 0;
	timing->delta_t_target = 0;
}

void init_video(video_t *video)
{
	/* Initialize SDL-related */

	SDL_Init(SDL_INIT_VIDEO);

	video->renderer = NULL;
	video->window = NULL;

	video->window = SDL_CreateWindow
	(
		"TODO",
		640,
		320,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	video->renderer = SDL_CreateRenderer
	(
		video->window,
		NULL
	);

	/* Initialize default video settings */

	SDL_GetWindowSize(video->window, &video->window_width, &video->window_height);

	/* TODO: Global consts...? */
	video->ray_count = 64;
	video->field_of_view = 75;
	video->view_distance = 64;
}

SDL_Texture *load_sprite_texture(video_t *video, const char *file_path)
{
	if(IMG_Load(file_path))
	{
		SDL_Surface *surface = IMG_Load(file_path);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(video->renderer, surface);

		SDL_DestroySurface(surface);

		if(texture)
		{
			return texture;
		}
		else
		{
			printf("Texture load error\n");
		}
	}
	else
	{
		printf("Image load error\n");
	}

	return NULL;
}

void draw_floor(video_t *video)
{
	SDL_FRect rect;

	rect.x = 0;
	rect.y = video->window_height / 2;
	rect.w = video->window_width;
	rect.h = video->window_height / 2;

	SDL_SetRenderDrawColor(video->renderer, 15, 63, 15, 255);
	SDL_RenderFillRect(video->renderer, &rect);
}

/*
 * TODO: It may be worth separating raycast calculations from draw calls...
 *
 * 2 separate functions:
 *
 * One calculates the distances and places them in an array or something...
 * The other takes those values from the array and draws...
 */
void draw_walls(video_t *video, player_t *player)
{
	/* Stuff to draw walls */

	SDL_FRect rect;
	int ray_count = video->ray_count;
	int window_width = video->window_width;
	int window_height = video->window_height;

	rect.w = window_width / ray_count;

	/* Stuff for calculating rays */

	bool ray_hit = false;
	double ray_x = player->x;
	double ray_y = player->y;
	double ray_theta = player->theta + video->field_of_view / 2;
	double ray_length = 0;
	double render_scale = 2 * video->window_height;

	const double view_distance = video->view_distance;
	const double ray_length_inc = 0.01; /*TODO: GLOBAL CONST?? */
	const double ray_theta_inc = video->field_of_view / ray_count;

	/* Draw loop */

	for(int i = 0; i < ray_count; i++)
	{
		/* Reset values for this loop iteration */

		ray_length = 0;
		ray_theta -= ray_theta_inc; /* WE NEED TO DECREMENT THE ANGLE SO THAT RENDERING IS NOT MIRRORED */
		ray_hit = false;

		/* Calculate distance */
		ray_x = player->x;
		ray_y = player->y;

		while(!ray_hit && ray_length < view_distance)
		{
			ray_x += ray_length_inc * cos(ray_theta * DEGREES_TO_RADIANS);
			ray_y += ray_length_inc * sin(ray_theta * DEGREES_TO_RADIANS);

			if
			(
				(ray_x > 5 && ray_x < 8 && ray_y > -5 && ray_y < 5) ||
				(ray_x > -5 && ray_x < 0 && ray_y > 3 && ray_y < 5) ||
				(ray_x > -2 && ray_x < 3 && ray_y > -2 && ray_y < 5) ||
				(ray_x > -10 && ray_x < 2 && ray_y > 6 && ray_y < 8) ||
				(ray_x > 0 && ray_x < 8 && ray_y > 7 && ray_y < 10)
			)
			{
				ray_hit = true;
				break;
			}

			ray_length += ray_length_inc;
		}

		/* Draw */

		if(ray_hit)
		{
			rect.h = render_scale / ray_length;
			rect.x = i * rect.w;
			rect.y = window_height / 2 - rect.h / 2;

			if(ray_length < 1)
			{
				ray_length = 1;
			}

			SDL_SetRenderDrawColor(video->renderer, 71 / ray_length, 71 / ray_length, 71 / ray_length, 255);
			SDL_RenderFillRect(video->renderer, &rect);
		}
	}
}

//void draw_sprites(video_t *video, player_t *player, SDL_Texture *texture)
//{
//	SDL_FRect rect;
//
//	rect.x = player->theta;
//	rect.y = video->window_height / 2 - 40; /* 40 px is the sprite height and width... */
//
//	rect.w = 10 * sqrt(pow(5 - player->x, 2) + pow(5 - player->y, 2));
//	rect.h = 10 * sqrt(pow(5 - player->x, 2) + pow(5 - player->y, 2));
//
//	SDL_RenderTexture(video->renderer, texture, NULL, &rect);
//}

void clear_video(video_t *video)
{
	SDL_SetRenderDrawColor(video->renderer, 16, 0, 31, 0);
	SDL_RenderClear(video->renderer);
}

void render_video(video_t *video)
{
	SDL_RenderPresent(video->renderer);
}

void set_timing(timing_t *timing, uint64_t delta_t_target)
{
	timing->delta_t_target = delta_t_target;
}

void terminate_video(video_t *video)
{
	SDL_DestroyRenderer(video->renderer);
	SDL_DestroyWindow(video->window);
	SDL_Quit();
}

int main(void)
{
	input_t input;
	player_t player;
	timing_t video_timing;
	timing_t game_timing;
	video_t video;

	init_input(&input);
	init_player(&player);
	init_timing(&game_timing);
	init_video(&video);

	set_timing(&video_timing, 1000 / 32);
	set_timing(&game_timing, 1000 / 64);

	input.running = true;

	//SDL_Texture *texture = load_sprite_texture(&video, "data/sprite.png");

	while(input.running)
	{
		handle_input(&input, &video);

		if(check_time_elapsed(&game_timing))
		{
			handle_controls(&input, &player);
			printf("x=%f\ny=%f\ntheta=%f\n\n", player.x, player.y, player.theta);
		}

		if(check_time_elapsed(&video_timing))
		{
			clear_video(&video);
			draw_floor(&video);
			draw_walls(&video, &player);
//			draw_sprites(&video, &player, texture);
			render_video(&video);
		}
	}

	SDL_DestroyTexture(texture);
	terminate_video(&video);
}
