
#ifndef STRUCTS_GPU_H
#define STRUCTS_GPU_H 

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
	uint			*seeds;
	size_t			size;
}						t_seeds;

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
}				t_hit_record;

typedef struct	s_hitable_list
{
 	__global t_sphere *list;
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

#endif