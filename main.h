#ifndef MAIN_H
# define MAIN_H

# define MAP_WIDTH 10
# define MAP_HEIGHT 10
# define WIN_WIDTH 640
# define WIN_HEIGHT 480
# define CELL_SIZE 32
#define KEY_W 119
#define KEY_S 115
#define KEY_A 97
#define KEY_D 100
#define KEY_LEFT 65361
#define KEY_RIGHT 65363
#define KEY_ESC 65307

# include <stdio.h>
# include <math.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stdint.h>
# include "./minilibx/mlx.h"

typedef struct s_dda
{
	int		map_check_x;
	int		map_check_y;
	// (vRayLength1D nel tutorial)
	double	side_dist_x;
	double	side_dist_y;
	// (vRayUnitStepSize nel tutorial)
	double	delta_dist_x;
	double	delta_dist_y;
	// (vStep nel tutorial)
	int		step_x;
	int		step_y;
}	t_dda;

typedef struct s_player
{
    double x;
    double y;
    double dir_x;
    double dir_y;
    double plane_x;
    double plane_y;
} t_player;

typedef struct s_game
{
    void        *mlx;
    void        *win;
    void        *img;
    char        *addr;
    int         bpp;
    int         line_len;
    int         endian;
    t_player    player;
} t_game;

#endif
