#ifndef RT_H
#define RT_H 

#include <SDL.h>
#include "structs.h"
#include <string.h>
#include <stdio.h>
#include <OpenCL/opencl.h>
#include <time.h>

int STATE;
int LOG;

#define WIDTH 256
#define HEIGHT 256

typedef struct s_debug
{
	char	step;
	int		depth;
	float	x;
	float	y;
	float	z;
	char	bytes[4];
	char	floats[3];
}				t_debug;

typedef struct			s_seed
{
	unsigned int		*seeds;
	size_t				size;
}						t_seeds;

typedef struct	s_cl
{
	int					err;
	cl_device_id		device_id;
	cl_context			context;
	cl_command_queue	commands;
	cl_program			program;
	cl_kernel			kernel;
	cl_mem				output;
	cl_mem				objects;
	cl_mem				seed_gpu;
	cl_mem				logfile;
	size_t				local;
	size_t				global;
}				t_cl;

typedef struct	s_rgba
{
	unsigned char	b;
	unsigned char	g;
	unsigned char	r;
	unsigned char	a;
}				t_rgba;

typedef union	u_color
{
	t_rgba			rgba;
	unsigned int	value;
}				t_color;

typedef struct	s_env
{
	t_cl *cl;

	t_seeds seeds;

	int state;
	int render;
	int render_step;

	t_debug logfile[60];

	int width;
	int height;

	t_sphere list[5];
	t_sphere list_random[501];
	t_hitable_list world;
	t_camera cam;

	t_vec3	*buff;
	char *gpu_calculations;
	
	SDL_Window *win;
	SDL_Surface *sur;
	SDL_Thread *render_thread;
	SDL_Event   event;
	int render_threadReturnValue;
}				t_env;

void	init_seeds(t_env *env);
t_vec3  color_gpu(t_ray r, t_hitable_list world, t_uint2 *seeds);
void	render_gpu(t_env *env);
int 	set_gpu_args(t_env *env, t_cl *cl);
void	init_gpu(t_env *env);
void	error_finish(char *str);
int		read_cl(char **code);

#endif