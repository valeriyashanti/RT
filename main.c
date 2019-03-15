#include "rt.h"

int				render(void *env);

// t_vec3 color(t_ray r, t_hitable_list world, int depth) {
// 	t_hit_record rec;
// 	if (hitable_list_hit(world, r, 0.001, FLT_MAX, &rec)) {
// 		t_ray scattered;
// 		t_vec3 attenuation;
// 		int tcase = 0;
// 		if (rec.mat_ptr.type==0)
// 		{
// 			tcase =	lambertian_scatter(rec.mat_ptr, r, rec, &attenuation, &scattered);
// 		}
// 		else if(rec.mat_ptr.type==1)
// 		{
// 			tcase =	metal_scatter(rec.mat_ptr, r, rec, &attenuation, &scattered);
// 		}
// 		else if(rec.mat_ptr.type==2)
// 		{
// 			tcase =	dielectric_scatter(rec.mat_ptr, r, rec, &attenuation, &scattered);
// 		}
// 		if (depth < 50 && tcase) {
// 			// t_vec3 info = vec3_mul(attenuation, color(scattered, world, depth + 1));
// 			// if (LOG)
// 			// {	
// 			// 	printf("A %2d [%.2f][%.2f][%.2f]\n",depth, info.e[0], info.e[1], info.e[2]);
// 			// }
// 			// return info;
// 			return vec3_mul(attenuation, color(scattered, world, depth + 1));
// 		}
// 		else {
// 			// t_vec3 info = vec3(0, 0, 0);
// 			// if (LOG)
// 			// {
// 			// 	printf("B %2d [%.2f][%.2f][%.2f]\n",depth, info.e[0], info.e[1], info.e[2]);
// 			// }
// 			// return info;
// 			return vec3(0, 0, 0);
// 		}
// 	}
// 	else {
// 		t_vec3 unit_direction = vec3_unit_vector(ray_direction(r));
// 		float t = 0.5*(unit_direction.e[1] + 1.0);
// 		// t_vec3 info = vec3_add(vec3_mul_num((1.0 - t),vec3(1.0, 1.0, 1.0)), vec3_mul_num(t, vec3(0.5, 0.7, 1.0)));
// 		// if (LOG)
// 		// {
// 		// 	printf("C %2d [%.2f][%.2f][%.2f]\n",depth, info.e[0], info.e[1], info.e[2]);
// 		// }
// 		// return info;
// 		return vec3_add(vec3_mul_num((1.0 - t),vec3(1.0, 1.0, 1.0)), vec3_mul_num(t, vec3(0.5, 0.7, 1.0)));
// 	}
// }

t_hitable_list random_scene(t_env *env) {
	// int n = 500;
	// t_sphere list[n + 1];
	env->list_random[0] = sphere(vec3(0, -1000, 0), 1000, lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = drand48();
			t_vec3 center = vec3(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
			if (vec3_length(vec3_sub(center, vec3(4, 0.2, 0))) > 0.9) 
			{
				if (choose_mat < 0.8) {  // diffuse
					env->list_random[i++] = sphere(center, 0.2, lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
				}
				else if (choose_mat < 0.95) { // metal
					env->list_random[i++] = sphere(center, 0.2,
						metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())), 0.5*drand48()));
				}
				else {  // glass
					env->list_random[i++] = sphere(center, 0.2, dielectric(1.5));
				}
			}
		}
	}

	env->list_random[i++] = sphere(vec3(0, 1, 0), 1.0, dielectric(1.5));
	env->list_random[i++] = sphere(vec3(-4, 1, 0), 1.0, lambertian(vec3(0.4, 0.2, 0.1)));
	env->list_random[i++] = sphere(vec3(4, 1, 0), 1.0, metal(vec3(0.7, 0.6, 0.5), 0.0));

	return hitable_list(env->list_random, i);
}

float get_random(t_uint2 *seeds)
{
	seeds->x = 36969 * ((seeds->x) & 65535) + ((seeds->x) >> 16);
	seeds->y = 18000 * ((seeds->y) & 65535) + ((seeds->y) >> 16);

	unsigned int ires = ((seeds->x) << 16) + (seeds->y);

	union {
		float f;
		unsigned int ui;
	} res;

	res.ui = (ires & 0x007fffff) | 0x40000000;
	return (res.f - 2.0f) / 2.0f;
}

void	init_seeds(t_env *env)
{
	size_t		i;

	srand((uint)clock());
	i = 0;
	while (i < env->seeds.size)
		env->seeds.seeds[i++] = (uint)rand();
}

int main(int argc, char *args[])
{
	t_env *env = (t_env *)malloc(sizeof(t_env));
	env->width = WIDTH;
	env->height = HEIGHT;
	env->gpu_calculations = malloc(WIDTH * HEIGHT * sizeof(float) * 3);

	env->cl = (t_cl *)malloc(sizeof(t_cl));

	env->seeds.size = (uint)(env->width * env->height * 2);
	env->seeds.seeds = (uint*)malloc(sizeof(uint) * env->seeds.size);
	//init_seeds(env);

	// uint2	seeds;
	// seeds.x = env->seeds.seeds[env->width * env->height / 2];
	// seeds.y = env->seeds.seeds[env->width * env->height / 2 + env->width * env->height];
	// printf("%f\n", get_random(&seeds));
	// return 0;





	

	env->state = 1;
	env->render_step = 1;
	env->render = 1;
	env->buff = (t_vec3 *)malloc(sizeof(t_vec3) * WIDTH * HEIGHT);


	// env->list[0] = sphere(vec3(0, 0, -1), 0.5, lambertian(vec3(0.1, 0.2, 0.5)));
	// env->world = hitable_list(env->list, 1);

	env->list[0] = sphere(vec3(0, 0, -1), 0.5, lambertian(vec3(0.1, 0.2, 0.5)));
	env->list[1] = sphere(vec3(0, -100.5, -1), 100, lambertian(vec3(0.8, 0.8, 0.0)));
	env->list[2] = sphere(vec3(1, 0, -1), 0.5, metal(vec3(0.8, 0.6, 0.2), 0.0));
	env->list[3] = sphere(vec3(-1, 0, -1), 0.5, dielectric(1.5));
	env->list[4] = sphere(vec3(-1, 0, -1), -0.45, dielectric(1.5));
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

	init_gpu(env);


	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return (1);
	}

	env->win = SDL_CreateWindow("lbradama", 600, 300, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
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
				//printf("key down: %d %c\n", env->event.key.keysym.sym, env->event.key.keysym.sym);
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
	printf("DONE!\n");

	return (0);
}


void    render_cpu(t_env *env)
{
	init_seeds(env);
	t_color c;
	c.rgba.a = 0xFF;

	for (int y = 0; y < env->sur->h; y++)
	{
		for (int x = 0; x < env->sur->w; x++)
		{
			int ind = env->sur->w * (env->sur->h - 1 - y) + x;



			LOG = 0;
			if(x == 180 && y == 125)
			{
				printf("index: %d\n", ind);
				LOG = 1;
			}

			

			t_uint2 seeds;
			seeds.x = env->seeds.seeds[ind];
			seeds.y = env->seeds.seeds[ind + env->width * env->height];

			float u = (float)(x + get_random(&seeds)) / (float)env->sur->w;
			float v = (float)(y + get_random(&seeds)) / (float)env->sur->h;
			t_ray r = camera_get_ray(env->cam, u, v, &seeds);
			t_vec3 p = ray_point_at_parameter(r, 2.0);

			////env->buff[ind] = vec3_add(env->buff[ind], color(r, env->world, 0));
			
			//env->buff[ind] = vec3_add(env->buff[ind], color_gpu(r, env->world));


			t_vec3 tmp;
			tmp =  color_gpu(r, env->world, &seeds);

			if(x == 180 && y == 125)
			{
				printf("color_gpu[0][0]: %.2f, %.2f, %.2f\n", tmp.e[0], tmp.e[1], tmp.e[2]);
			}

			env->buff[ind] = vec3_add(env->buff[ind], tmp);

			t_vec3 col = vec3(0, 0, 0);
			col = vec3_div_num(env->buff[ind], env->render_step);
			col = vec3(sqrt(col.e[0]), sqrt(col.e[1]),sqrt(col.e[2]));

			c.rgba.r = (int)(255.99*col.e[0]);
			c.rgba.g = (int)(255.99*col.e[1]);
			c.rgba.b = (int)(255.99*col.e[2]);

			

			if(((x == 181 ||x == 179) && y == 125) || ((y == 124 ||y == 126) && x == 180))
			{
				c.rgba.r = 255;
				c.rgba.g = 0;
				c.rgba.b = 0;
			}

			((unsigned int *)env->sur->pixels)[ind] = c.value;
		}
	}
}

int    render(void *arg)
{
	t_env *env = (t_env *)arg;

	t_color c;
	c.rgba.a = 0xFF;
	
	while (env->state)
	{
		if (env->render)
		{
			env->render = 0;
			//render_cpu(env);
			
			//printf("%zu, %zu\n", sizeof(float), sizeof(double));

			render_gpu(env);
			for (int y = 0; y < env->sur->h; y++)
			{
				for (int x = 0; x < env->sur->w; x++)
				{
					int ind = env->sur->w * (env->sur->h - 1 - y) + x;
					t_vec3 col = vec3(0, 0, 0);

					// col.e[0] = env->gpu_calculations[ind];
					// col.e[1] = env->gpu_calculations[ind + 1];
					// col.e[2] = env->gpu_calculations[ind + 2];

					col.e[0] = ((float *)env->gpu_calculations)[ind*3];
					col.e[1] = ((float *)env->gpu_calculations)[ind*3 + 1];
					col.e[2] = ((float *)env->gpu_calculations)[ind*3 + 2];

					// if (ind == 33460)
					// {
					// 	printf("color_gpu[0][0]: %.2f, %.2f, %.2f\n", col.e[0], col.e[1], col.e[2]);
					// }

					env->buff[ind] = vec3_add(env->buff[ind], col);
					
					col = vec3_div_num(env->buff[ind], env->render_step);
					col = vec3(sqrt(col.e[0]), sqrt(col.e[1]),sqrt(col.e[2]));

					c.rgba.r = (int)(255.99*col.e[0]);
					c.rgba.g = (int)(255.99*col.e[1]);
					c.rgba.b = (int)(255.99*col.e[2]);


					((unsigned int *)env->sur->pixels)[ind] = c.value;
				}
			}

			// printf("-------------------\ninfo\n-------------------\n");
			// for(size_t d = 0; d < 60; d++)
			// {
			// 	if(env->logfile[d].step != 0)
			// 		printf("%c %2d [%.2f][%.2f][%.2f]\n",
			// 		env->logfile[d].step, env->logfile[d].depth, env->logfile[d].x, env->logfile[d].y,  env->logfile[d].z);
			// }


			//printf("RENDER STEP\n");
			SDL_UpdateWindowSurface(env->win);
		}
		env->render_step++;
	}
	return (0);
}

