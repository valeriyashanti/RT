/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkessler <gkessler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/14 19:30:09 by gkessler          #+#    #+#             */
/*   Updated: 2019/03/15 16:36:52 by gkessler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

t_material texture(int n, t_env *env) 
{
    t_material tex;
    tex.type = 3;
    if (n == 1)
    {
      /*   SDL_Window *win1;
        win1 = SDL_CreateWindow("title", 0 , 0, 400, 400, SDL_WINDOW_RESIZABLE); */
        SDL_Surface *sur = SDL_LoadBMP("kesh.bmp");

       /*  SDL_Surface *sur_win = SDL_GetWindowSurface(win1); 
        SDL_BlitSurface(sur, NULL, sur_win, NULL); */
        tex.texture = sur;
      /*   SDL_UpdateWindowSurface(win1);
        SDL_Delay(100000); */
    }
    return tex;
}

/* int apply_texture(t_material tex, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds)
{
    t_vec3 target = vec3_add(vec3_add(rec.p, rec.normal), random_in_unit_sphere(seeds));
    *scattered = ray(rec.p, vec3_sub(target, rec.p));
    t_vec3 start;
    start.e[0] = rec.center.e[0] - rec.radius;
    start.e[1] = rec.center.e[1] - rec.radius;
    float u = rec.p.e[0] - start.e[0];
    float v = rec.p.e[1] - start.e[1];
    float zoom_x = (float)tex.texture->w / (rec.radius * 2);
    float zoom_y = (float)tex.texture->h / (rec.radius * 2);
    int x_zoom = u * zoom_x;
    int y_zoom = v * zoom_y;
    int bpp = tex.texture->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)tex.texture->pixels + y_zoom * tex.texture->pitch + x_zoom * bpp;
    t_vec3 color;
    color.e[2] = (float)p[0] / 255.0;
    color.e[1] = (float)p[1] / 255.0;
    color.e[0] = (float)p[2] / 255.0;
    *attenuation = color;

    return 1;
} */

int apply_texture(t_material tex, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds)
{
    t_vec3 target = vec3_add(vec3_add(rec.p, rec.normal), random_in_unit_sphere(seeds));
    *scattered = ray(rec.p, vec3_sub(target, rec.p));
    t_vec3 start;
    start.e[0] = rec.center.e[0] - rec.radius;
    start.e[1] = rec.center.e[1] - rec.radius;
    float u = rec.p.e[0] - start.e[0];
    float v = rec.p.e[1] - start.e[1];
    float zoom_x = (float)tex.texture->w / (rec.radius * 2);
    float zoom_y = (float)tex.texture->h / (rec.radius * 2);
    // int x_zoom = u * zoom_x;
    // int y_zoom = v * zoom_y;

    u = atan2(rec.p.e[0], rec.p.e[2]);
    v = acos(rec.p.e[1] / (sqrt(rec.p.e[0] * rec.p.e[0] + rec.p.e[1] * rec.p.e[1])));

    int x_zoom = u * zoom_x * 1.2;
    int y_zoom = v * zoom_y * 1.2;

    int bpp = tex.texture->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)tex.texture->pixels + y_zoom * tex.texture->pitch + x_zoom * bpp;
    t_vec3 color;
    color.e[2] = (float)p[0] / 255.0;
    color.e[1] = (float)p[1] / 255.0;
    color.e[0] = (float)p[2] / 255.0;
    *attenuation = color;

    return 1;
}
