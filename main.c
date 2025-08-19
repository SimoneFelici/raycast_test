#include "main.h"

char map[MAP_WIDTH][MAP_HEIGHT] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void	put_pixel(t_game *game, int x, int y, int color)
{
	char *dst;
	if (x >= WIN_WIDTH || y >= WIN_HEIGHT || x < 0 || y < 0)
		return ;
	dst = game->addr + (y * game->line_len + x * (game->bpp / 8));
	*(unsigned int*)dst = color;
}

void draw_square(t_game *game, int cx, int cy, int size, int color)
{
    int x, y;

    for (y = -size; y <= size; y++)
    {
        for (x = -size; x <= size; x++)
        {
            put_pixel(game, cx + x, cy + y, color);
        }
    }
}

void draw_circle(t_game *game, int cx, int cy, int radius, int color)
{
    int x, y;
    int r2 = radius * radius;

    for (y = -radius; y <= radius; y++)
    {
        for (x = -radius; x <= radius; x++)
        {
            if (x*x + y*y <= r2)
                put_pixel(game, cx + x, cy + y, color);
        }
    }
}

void	draw_map_2d(t_game *game)
{
	int	map_x; 
	int	map_y;
	int	screen_x;
	int	screen_y;

	map_y = 0;
	while (map_y < MAP_HEIGHT)
	{
		map_x = 0;
		while (map_x < MAP_WIDTH)
		{
			screen_x = 0;
			while (screen_x < CELL_SIZE)
			{
				screen_y = 0;
				while (screen_y < CELL_SIZE)
				{
					int pixel_x = map_x * CELL_SIZE + screen_x;
					int pixel_y = map_y * CELL_SIZE + screen_y;
					
					if (map[map_y][map_x] == 1)
						put_pixel(game, pixel_x, pixel_y, 0x808080);
					else
						put_pixel(game, pixel_x, pixel_y, 0xFFFFFF);
					screen_y++;
				}
				screen_x++;
			}
			map_x++;
		}
		map_y++;
	}
}

void	draw_player_2d(t_game *game)
{
	int player_screen_x;
	int player_screen_y;
	int	dx;
	int dy;
	int size;
	
	size = 2;
	dx = -1;
	dy = -1;
	player_screen_x = (int)(game->player.x * CELL_SIZE);
	player_screen_y = (int)(game->player.y * CELL_SIZE);
	draw_square(game, player_screen_x, player_screen_y, 2, 0x0000FF);
//	draw_circle(game, player_screen_x, player_screen_y, 2, 0x0000FF);
}

// main.c

void	draw_ray_2d(t_game *game, int screen_x)
{
	double	camera_x;
	double	ray_dir_x;
	double	ray_dir_y;
	t_dda	dda;
	int i;

	camera_x = 2 * screen_x / (double)WIN_WIDTH - 1;
	ray_dir_x = game->player.dir_x + game->player.plane_x * camera_x;
	ray_dir_y = game->player.dir_y + game->player.plane_y * camera_x;
	
	dda.map_check_x = (int)game->player.x;
	dda.map_check_y = (int)game->player.y;

	if (ray_dir_x == 0)
		dda.delta_dist_x = 1e30;
	else
		dda.delta_dist_x = fabs(1.0 / ray_dir_x);
	
	if (ray_dir_y == 0)
		dda.delta_dist_y = 1e30;
	else
		dda.delta_dist_y = fabs(1.0 / ray_dir_y);
	if (ray_dir_x < 0)
	{
		dda.step_x = -1;
		dda.side_dist_x = (game->player.x - dda.map_check_x) * dda.delta_dist_x;
	}
	else
	{
		dda.step_x = 1;
		dda.side_dist_x = (dda.map_check_x + 1.0 - game->player.x) * dda.delta_dist_x;
	}
	if (ray_dir_y < 0)
	{
		dda.step_y = -1;
		dda.side_dist_y = (game->player.y - dda.map_check_y) * dda.delta_dist_y;
	}
	else
	{
		dda.step_y = 1;
		dda.side_dist_y = (dda.map_check_y + 1.0 - game->player.y) * dda.delta_dist_y;
	}

	double distance = 0;
	double max_distance = 100;
	bool tile_found = false;

	while (!tile_found && distance < max_distance)
	{
		if (dda.side_dist_x < dda.side_dist_y)
		{
			dda.map_check_x += dda.step_x;
			distance = dda.side_dist_x;
			dda.side_dist_x += dda.delta_dist_x;
		}
		else
		{
			dda.map_check_y += dda.step_y;
			distance = dda.side_dist_y;
			dda.side_dist_y += dda.delta_dist_y;
		}
		if (dda.map_check_x >= 0 && dda.map_check_x < MAP_WIDTH &&	dda.map_check_y >= 0 && dda.map_check_y < MAP_HEIGHT)
		{
			if (map[dda.map_check_y][dda.map_check_x] == 1)
				tile_found = true;
		}
	}
	if (tile_found)
	{
		double intersection_x = game->player.x + ray_dir_x * distance;
		double intersection_y = game->player.y + ray_dir_y * distance;
		draw_circle(game, intersection_x * CELL_SIZE, intersection_y * CELL_SIZE, 2, 0xFF0000);
	}
}

void	draw_2d(t_game *game)
{
	int	x;
	int ray_count = 60;

	draw_map_2d(game);
	draw_player_2d(game);
	
	for (x = 0; x < ray_count; x++)
	{
		draw_ray_2d(game, x * (WIN_WIDTH / ray_count));
	}
}

int key_press_handler(int keycode, t_game *game)
{
	double move_speed;
	double rot_speed;
	
	move_speed = 0.1;
	rot_speed = 0.1;
	if (keycode == KEY_ESC)
	{
		exit(0);
	}
	if (keycode == KEY_W)
	{
		double new_x;
		double new_y; 

		new_x = game->player.x + game->player.dir_x * move_speed;
		new_y = game->player.y + game->player.dir_y * move_speed;
		if (map[(int)new_y][(int)game->player.x] == 0)
			game->player.y = new_y;
		if (map[(int)game->player.y][(int)new_x] == 0)
			game->player.x = new_x;
	}
	if (keycode == KEY_S)
	{
		double new_x;
		double new_y;
		
		new_x = game->player.x - game->player.dir_x * move_speed;
		new_y = game->player.y - game->player.dir_y * move_speed;
		if (map[(int)new_y][(int)game->player.x] == 0)
			game->player.y = new_y;
		if (map[(int)game->player.y][(int)new_x] == 0)
			game->player.x = new_x;
	}
	if (keycode == KEY_D)
	{
		double new_x = game->player.x - game->player.plane_x * move_speed;
		double new_y = game->player.y - game->player.plane_y * move_speed;

		if (map[(int)new_y][(int)game->player.x] == 0)
			game->player.y = new_y;
		if (map[(int)game->player.y][(int)new_x] == 0)
			game->player.x = new_x;

	}
	if (keycode == KEY_A)
	{
		double new_x = game->player.x + game->player.plane_x * move_speed;
		double new_y = game->player.y + game->player.plane_y * move_speed;
		
		if (map[(int)new_y][(int)game->player.x] == 0)
			game->player.y = new_y;
		if (map[(int)game->player.y][(int)new_x] == 0)
			game->player.x = new_x;
	}
	if (keycode == KEY_RIGHT)
	{
		double old_dir_x = game->player.dir_x;
		game->player.dir_x = game->player.dir_x * cos(rot_speed) - game->player.dir_y * sin(rot_speed);
		game->player.dir_y = old_dir_x * sin(rot_speed) + game->player.dir_y * cos(rot_speed);

		double old_plane_x = game->player.plane_x;
		game->player.plane_x = game->player.plane_x * cos(rot_speed) - game->player.plane_y * sin(rot_speed);
		game->player.plane_y = old_plane_x * sin(rot_speed) + game->player.plane_y * cos(rot_speed);
	}
	if (keycode == KEY_LEFT)
	{
		double old_dir_x = game->player.dir_x;
		game->player.dir_x = game->player.dir_x * cos(-rot_speed) - game->player.dir_y * sin(-rot_speed);
		game->player.dir_y = old_dir_x * sin(-rot_speed) + game->player.dir_y * cos(-rot_speed);
		
		double old_plane_x = game->player.plane_x;
		game->player.plane_x = game->player.plane_x * cos(-rot_speed) - game->player.plane_y * sin(-rot_speed);
		game->player.plane_y = old_plane_x * sin(-rot_speed) + game->player.plane_y * cos(-rot_speed);

	}
	return (0);
}

void	init_player(t_player *player)
{
	player->x = 5.0;
	player->y = 5.0;
	player->dir_x = -1.0; // left
	player->dir_y = 0.0; // center
	player->plane_x = 0.0;
	player->plane_y = 0.66; // FOV
}

void	init_game(t_game *game)
{
	game->mlx = mlx_init();
	game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "RAY");
	game->img = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
	game->addr = mlx_get_data_addr(game->img, &game->bpp, &game->line_len, &game->endian);
	init_player(&game->player);
}

int game_loop(t_game *game)
{
	draw_2d(game);
	mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
	return (0);
}

int main()
{
	t_game game;
	init_game(&game);
	mlx_hook(game.win, 2, 1L<<0, key_press_handler, &game);
	mlx_loop_hook(game.mlx, game_loop, &game);
	mlx_loop(game.mlx);
}
