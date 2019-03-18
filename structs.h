#ifndef STRUCTS_H
#define STRUCTS_H 

#include "float.h"
#include "math.h"
#include <stdlib.h>
#include <SDL.h>

typedef struct	s_uint2
{
	unsigned int x;
	unsigned int y;
}				t_uint2;

typedef struct	s_vec3
{
	float e[3];
}				t_vec3;

typedef struct	s_ray
{
	t_vec3 A;
	t_vec3 B;
}				t_ray;

typedef struct	s_material
{
	int type;
	t_vec3 albedo;
	float fuzz;
	float ref_idx;
	SDL_Surface *texture_bump;
	SDL_Surface *texture_origin;
}				t_material;

typedef struct	s_sphere
{
	t_vec3 center;
	float radius;
	t_material mat_ptr;
}				t_sphere;



typedef struct	s_hit_record
{
	float t;
	t_vec3 p;
	t_vec3 normal;
	t_material mat_ptr;
	float	radius;
	t_vec3 center;

}				t_hit_record;

typedef struct	s_hitable_list
{
	t_sphere *list;
 	int list_size;
}				t_hitable_list;

typedef struct	s_camera
{
	t_vec3 origin;
	t_vec3 lower_left_corner;
	t_vec3 horizontal;
	t_vec3 vertical;
	t_vec3 u;
	t_vec3 v;
	t_vec3 w;
	float lens_radius;
}				t_camera;







float get_random(t_uint2 *seeds);

t_hitable_list  hitable_list(t_sphere *l, int n);
int hitable_list_hit(t_hitable_list hl, t_ray r, float t_min, float t_max, t_hit_record *rec);

t_vec3 vec3(float x, float y, float z);
float vec3_length(t_vec3 v);
float vec3_squared_length(t_vec3 v);
void vec3_make_unit_vector(t_vec3 *v);
t_vec3 vec3_unit_vector(t_vec3 v);
t_vec3 vec3_cross(t_vec3 v1, t_vec3 v2);
float vec3_dot(t_vec3 v1, t_vec3 v2);
t_vec3 vec3_add(t_vec3 v1, t_vec3 v2);
t_vec3 vec3_sub(t_vec3 v1, t_vec3 v2);
t_vec3 vec3_mul(t_vec3 v1, t_vec3 v2);
t_vec3 vec3_mul_num(float n, t_vec3 v);
t_vec3 vec3_div(t_vec3 v1, t_vec3 v2);
t_vec3 vec3_div_num(t_vec3 v, float n);

t_camera	camera(t_vec3 lookfrom, t_vec3 lookat, t_vec3 vup, float vfov, float aspect, float aperture, float focus_dist);
t_vec3		random_in_unit_disk(t_uint2 *seeds);
t_ray		camera_get_ray(t_camera cam, float s, float t, t_uint2 *seeds);

t_sphere sphere(t_vec3 cen, float r, t_material m);
int sphere_hit(t_sphere s, t_ray r, float t_min, float t_max, t_hit_record *rec);

t_ray   ray(t_vec3 a, t_vec3 b);
t_vec3	ray_origin(t_ray r);
t_vec3	ray_direction(t_ray r);
t_vec3	ray_point_at_parameter(t_ray r, float t);

t_material lambertian(t_vec3 a);
int lambertian_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds);
t_material metal(t_vec3 a, float f);
int metal_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds);
t_material dielectric(float ri);
int dielectric_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds);

#endif