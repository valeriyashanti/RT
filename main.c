#include "rt.h"

int main(int argc, char *args[])
{
	t_env *env = (t_env *)malloc(sizeof(t_env));
	env->width = WIDTH;
	env->height = HEIGHT;
	//env->gpu_calculations = malloc(WIDTH * HEIGHT * sizeof(float) * 3);

	//env->cl = (t_cl *)malloc(sizeof(t_cl));

	env->seeds.size = (uint)(env->width * env->height * 2);
	env->seeds.seeds = (uint*)malloc(sizeof(uint) * env->seeds.size);

	env->state = 1;
	env->render_step = 1;
	env->render = 1;
	env->buff = (t_vec3 *)malloc(sizeof(t_vec3) * WIDTH * HEIGHT);


	// env->list[0] = sphere(vec3(0, 0, -1), 0.5, lambertian(vec3(0.1, 0.2, 0.5)));
	// env->world = hitable_list(env->list, 1);

	env->list[0] = sphere(vec3(0, 0, -1), 0.5, lambertian(vec3(0.1, 0.2, 0.5)));
	// env->list[1] = sphere(vec3(0, -100.5, -1), 100, lambertian(vec3(0.8, 0.8, 0.0)));
	env->list[1] = sphere(vec3(0, -100.5, -1), 100, texture(1, env));

	env->list[2] = sphere(vec3(1, 0, -1), 0.5, metal(vec3(0.8, 0.6, 0.2), 0.0));
	env->list[3] = sphere(vec3(1, 1, -1), 0.5, texture(1, env));
	// env->list[4] = sphere(vec3(-1, 0, -1), -0.45, dielectric(1.5));
	env->list[4] = sphere(vec3(3, 1, 1), 1.0, texture(1, env));
	env->world = hitable_list(env->list, 5);

	//env->world = random_scene(env);

	t_vec3 lookfrom = vec3(13, 2, 3);
	t_vec3 lookat = vec3(0, 0, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;

	env->cam = camera(lookfrom, lookat, vec3(0, 1, 0), 20, (float)WIDTH / (float)HEIGHT, aperture, dist_to_focus);

	for (int i = 0; i < WIDTH*HEIGHT; i++)
	{
		env->buff[i] = vec3(0, 0, 0);
	}

	//init_gpu(env);


	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return (1);
	}

	env->win = SDL_CreateWindow("PATH TRACING", 600, 300, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
	env->sur = SDL_GetWindowSurface(env->win);


	SDL_Thread *render_thread;

	render_thread = SDL_CreateThread(render, "render_thread", (void *)env);

	while (env->state)
		while (SDL_PollEvent(&env->event))
		{
			if (env->event.type == SDL_QUIT)
			{
				env->state = 0;
				break;
			}
			else if (env->event.type == SDL_KEYDOWN)
			{
				if (env->event.key.keysym.sym == 27)
					env->state = 0;
				break;
			}
			else if (env->event.type == SDL_MOUSEMOTION)
			{
				//printf("mouse: %d %d %d %d\n", env->event.motion.x, env->event.motion.y, env->event.motion.xrel, env->event.motion.yrel);
			}
		}
	SDL_WaitThread(render_thread, &env->render_threadReturnValue);
	return (0);
}
