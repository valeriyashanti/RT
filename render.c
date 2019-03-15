/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkessler <gkessler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/15 11:18:54 by gkessler          #+#    #+#             */
/*   Updated: 2019/03/15 11:23:17 by gkessler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"


t_hitable_list random_scene(t_env *env) 
{
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

			t_uint2 seeds;
			seeds.x = env->seeds.seeds[ind];
			seeds.y = env->seeds.seeds[ind + env->width * env->height];

			float u = (float)(x + get_random(&seeds)) / (float)env->sur->w;
			float v = (float)(y + get_random(&seeds)) / (float)env->sur->h;
			t_ray r = camera_get_ray(env->cam, u, v, &seeds);
			t_vec3 p = ray_point_at_parameter(r, 2.0);

			
			//env->buff[ind] = vec3_add(env->buff[ind], color_gpu(r, env->world));
			t_vec3 tmp;
			tmp =  color_gpu(r, env->world, &seeds);

			env->buff[ind] = vec3_add(env->buff[ind], tmp);

			t_vec3 col = vec3(0, 0, 0);
			col = vec3_div_num(env->buff[ind], env->render_step);
			col = vec3(sqrt(col.e[0]), sqrt(col.e[1]),sqrt(col.e[2]));

			c.rgba.r = (int)(255.99*col.e[0]);
			c.rgba.g = (int)(255.99*col.e[1]);
			c.rgba.b = (int)(255.99*col.e[2]);

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
			// env->render = 0;
			render_cpu(env);
			
			//printf("%zu, %zu\n", sizeof(float), sizeof(double));

			// render_gpu(env);
			// for (int y = 0; y < env->sur->h; y++)
			// {
			// 	for (int x = 0; x < env->sur->w; x++)
			// 	{
			// 		int ind = env->sur->w * (env->sur->h - 1 - y) + x;
			// 		t_vec3 col = vec3(0, 0, 0);

			// 		// col.e[0] = env->gpu_calculations[ind];
			// 		// col.e[1] = env->gpu_calculations[ind + 1];
			// 		// col.e[2] = env->gpu_calculations[ind + 2];

			// 		col.e[0] = ((float *)env->gpu_calculations)[ind * 3];
			// 		col.e[1] = ((float *)env->gpu_calculations)[ind * 3 + 1];
			// 		col.e[2] = ((float *)env->gpu_calculations)[ind * 3 + 2];

			// 		// if (ind == 33460)
			// 		// {
			// 		// 	printf("color_gpu[0][0]: %.2f, %.2f, %.2f\n", col.e[0], col.e[1], col.e[2]);
			// 		// }

			// 		env->buff[ind] = vec3_add(env->buff[ind], col);
					
			// 		col = vec3_div_num(env->buff[ind], env->render_step);
			// 		col = vec3(sqrt(col.e[0]), sqrt(col.e[1]),sqrt(col.e[2]));

			// 		c.rgba.r = (int)(255.99*col.e[0]);
			// 		c.rgba.g = (int)(255.99*col.e[1]);
			// 		c.rgba.b = (int)(255.99*col.e[2]);


			// 		((unsigned int *)env->sur->pixels)[ind] = c.value;
			// 	}
			// }

			// printf("-------------------\ninfo\n-------------------\n");
			// for(size_t d = 0; d < 60; d++)
			// {
			// 	if(env->logfile[d].step != 0)
			// 		printf("%c %2d [%.2f][%.2f][%.2f]\n",
			// 		env->logfile[d].step, env->logfile[d].depth, env->logfile[d].x, env->logfile[d].y,  env->logfile[d].z);
			// }


			// printf("RENDER STEP: %d\n", env->render_step);
			SDL_UpdateWindowSurface(env->win);
		}
		env->render_step++;
	}
	return (0);
}

