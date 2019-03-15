#include "structs_gpu.h"

#define FLT_MAX	0x1.fffffep127f


static float get_random(uint2 *seeds)
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

static t_vec3 vec3(float x, float y, float z)
{
    t_vec3 v;
    v.e[0] = x;
    v.e[1] = y;
    v.e[2] = z;
    return v;
}

static float vec3_length(t_vec3 v)
{
    return sqrt(v.e[0] * v.e[0] + v.e[1] * v.e[1] + v.e[2] * v.e[2]);
}

static float vec3_squared_length(t_vec3 v)
{
    return v.e[0] * v.e[0] + v.e[1] * v.e[1] + v.e[2] * v.e[2];
}

static void vec3_make_unit_vector(t_vec3 *v)
{
    float k = 1.0 / sqrt(v->e[0] * v->e[0] + v->e[1] * v->e[1] + v->e[2] * v->e[2]);
    v->e[0] *= k;
    v->e[1] *= k;
    v->e[2] *= k;
}

static t_vec3 vec3_cross(t_vec3 v1, t_vec3 v2) 
{
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		        (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
		        (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

static float vec3_dot(t_vec3 v1, t_vec3 v2)
{
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

static t_vec3 vec3_add(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

static t_vec3 vec3_sub(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

static t_vec3 vec3_mul(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

static t_vec3 vec3_mul_num(float n, t_vec3 v)
{
    return vec3(n * v.e[0], n * v.e[1], n * v.e[2]);
}

static t_vec3 vec3_div(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

static t_vec3 vec3_div_num(t_vec3 v, float n)
{
    return vec3(v.e[0] / n, v.e[1] / n, v.e[2] / n);
}

static t_vec3 vec3_unit_vector(t_vec3 v) 
{
	return vec3_div_num(v, vec3_length(v));
}

static t_ray   ray(t_vec3 a, t_vec3 b)
{
    t_ray r;
    r.A = a;
    r.B = b;
    return r;
}

static t_vec3 ray_origin(t_ray r)
{ 
    return r.A; 
}

static t_vec3 ray_direction(t_ray r)
{ 
    return r.B;
}

static t_vec3 ray_point_at_parameter(t_ray r, float t)
{ 
    return vec3_add(r.A, vec3_mul_num(t, r.B)); 
}

static t_vec3 random_in_unit_disk(uint2 *seeds)
{
    t_vec3 p;
    do
    {
        p = vec3_mul_num(2.0, vec3_sub(vec3(get_random(seeds), get_random(seeds), 0),vec3(1, 1, 0)));
    } while (vec3_dot(p, p) >= 1.0);
    return p;
}

static t_camera	camera(t_vec3 lookfrom, t_vec3 lookat, t_vec3 vup, float vfov, float aspect, float aperture, float focus_dist) 
{
    t_camera cam;
    cam.lens_radius = aperture / 2;
    float theta = vfov * M_PI / 180;
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;
    cam.origin = lookfrom;
    cam.w = vec3_unit_vector(vec3_sub(lookfrom,lookat));
    cam.u = vec3_unit_vector(vec3_cross(vup, cam.w));
    cam.v = vec3_cross(cam.w, cam.u);
    cam.lower_left_corner = vec3_sub(vec3_sub(vec3_sub(cam.origin, vec3_mul_num(half_width * focus_dist, cam.u)), vec3_mul_num(half_height * focus_dist, cam.v)),vec3_mul_num(focus_dist, cam.w));
    cam.horizontal = vec3_mul_num(2 * half_width * focus_dist, cam.u);
    cam.vertical = vec3_mul_num(2 * half_height * focus_dist, cam.v);
    return cam;
}

static t_ray camera_get_ray(t_camera cam, float s, float t, uint2 *seeds) 
{
    t_vec3 rd = vec3_mul_num(cam.lens_radius, random_in_unit_disk(seeds));
    t_vec3 offset = vec3_add(vec3_mul_num(rd.e[0],cam.u), vec3_mul_num(rd.e[1],cam.v));
    return ray(vec3_add(cam.origin, offset), 
            vec3_sub(vec3_sub(vec3_add(vec3_add(cam.lower_left_corner, vec3_mul_num(s, cam.horizontal)), vec3_mul_num(t, cam.vertical)), cam.origin), offset));
}

static int sphere_hit(t_sphere s, t_ray r, float t_min, float t_max, t_hit_record *rec/*, __global t_debug *logfile, int *l*/)
{
	// int id = get_global_id(0);
	// if (id == 33460)
	// {
	// 	logfile[*l].step = '?';
	// 	logfile[*l].x = s.center.e[0];
	// 	logfile[*l].y = s.center.e[1];
	// 	logfile[*l].z = s.center.e[2];
	// 	*l = *l + 1;
	// }
	t_vec3 oc = vec3_sub(ray_origin(r), s.center);
	float a = vec3_dot(ray_direction(r), ray_direction(r));
	float b = vec3_dot(oc, ray_direction(r));
	float c = vec3_dot(oc, oc) - s.radius * s.radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec->t = temp;
			rec->p = ray_point_at_parameter(r, rec->t);
			rec->normal = vec3_div_num(vec3_sub(rec->p, s.center), s.radius);
			rec->mat_ptr = s.mat_ptr;
			return 1;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec->t = temp;
			rec->p = ray_point_at_parameter(r, rec->t);
			rec->normal = vec3_div_num(vec3_sub(rec->p, s.center), s.radius);
			rec->mat_ptr = s.mat_ptr;
			return 1;
		}
	}
	return 0;
}

static t_hitable_list  hitable_list(__global t_sphere *l, int n)
{
    t_hitable_list hl;
    hl.list = l;
    hl.list_size = n;
    return hl;
}

static t_sphere extraxt_sphere(__global t_sphere *l, int i)
{
	t_sphere s;
	s.center.e[0] = l[i].center.e[0];
	s.center.e[1] = l[i].center.e[1];
	s.center.e[2] = l[i].center.e[2];
	s.radius = l[i].radius;
	s.mat_ptr.type = l[i].mat_ptr.type;
	s.mat_ptr.fuzz = l[i].mat_ptr.fuzz;
	s.mat_ptr.ref_idx = l[i].mat_ptr.ref_idx;
	s.mat_ptr.albedo.e[0] = l[i].mat_ptr.albedo.e[0];
	s.mat_ptr.albedo.e[1] = l[i].mat_ptr.albedo.e[1];
	s.mat_ptr.albedo.e[2] = l[i].mat_ptr.albedo.e[2];
	return s;
}

static int hitable_list_hit(t_hitable_list hl, t_ray r, float t_min, float t_max, t_hit_record *rec/*, __global t_debug *logfile, int *l*/)
{
	t_hit_record temp_rec;
	int hit_anything = 0;
	double closest_so_far = t_max;
	for (int i = 0; i < hl.list_size; i++) {
		if (sphere_hit(extraxt_sphere(hl.list, i), r, t_min, closest_so_far, &temp_rec/*, logfile, l*/)) {
			hit_anything = 1;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
		}
	}
	return hit_anything;
}

static float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

static int refract(t_vec3 v, t_vec3 n, float ni_over_nt, t_vec3 *refracted) {
	t_vec3 uv = vec3_unit_vector(v);
	float dt = vec3_dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);
	if (discriminant > 0) {
		*refracted = vec3_sub(vec3_mul_num(ni_over_nt, vec3_sub(uv, vec3_mul_num(dt,n))), vec3_mul_num(sqrt(discriminant), n));
		return 1;
	}
	else
		return 0;
}

static t_vec3 reflect(t_vec3 v, t_vec3 n) {
	return vec3_sub(v, vec3_mul_num(2 * vec3_dot(v, n), n));
}

static t_vec3 random_in_unit_sphere(uint2 *seeds) {
	t_vec3 p;
	do {
		p = vec3_mul_num(2.0, vec3_sub(vec3(get_random(seeds), get_random(seeds), get_random(seeds)), vec3(1, 1, 1)));
	} while (vec3_squared_length(p) >= 1.0);
	return p;
}

static t_material lambertian(t_vec3 a) 
{
    t_material m;
    m.type = 0;
    m.albedo = a;
    return m;
}

static int lambertian_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, uint2 *seeds)
{
    t_vec3 target = vec3_add(vec3_add(rec.p, rec.normal), random_in_unit_sphere(seeds));
    *scattered = ray(rec.p, vec3_sub(target, rec.p));
    *attenuation = m.albedo;
    return 1;
}

static t_material metal(t_vec3 a, float f) 
{
    t_material m;
    m.type = 1;
    m.albedo = a;
    if (f < 1) 
        m.fuzz = f; 
    else 
        m.fuzz = 1; 
    return m;
}

static int metal_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, uint2 *seeds)
{
    t_vec3 reflected = reflect(vec3_unit_vector(ray_direction(r_in)), rec.normal);
	*scattered = ray(rec.p, vec3_add(reflected, vec3_mul_num(m.fuzz, random_in_unit_sphere(seeds))));
	*attenuation = m.albedo;
	return (vec3_dot(ray_direction(*scattered), rec.normal) > 0);
}

static t_material dielectric(float ri) 
{
    t_material m;
    m.type = 2;
    m.ref_idx = ri;

    return m;
}

static int dielectric_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, uint2 *seeds)
{
    t_vec3 outward_normal;
    t_vec3 reflected = reflect(ray_direction(r_in), rec.normal);
    float ni_over_nt;
    *attenuation = vec3(1.0, 1.0, 1.0);
    t_vec3 refracted;
    float reflect_prob;
    float cosine;
    if (vec3_dot(ray_direction(r_in), rec.normal) > 0) 
    {
        outward_normal = vec3_mul_num(-1, rec.normal);
        ni_over_nt = m.ref_idx;
        cosine = vec3_dot(ray_direction(r_in), rec.normal) / vec3_length(ray_direction(r_in));
        cosine = sqrt(1 - m.ref_idx * m.ref_idx*(1 - cosine * cosine));
    }
    else {
        outward_normal = rec.normal;
        ni_over_nt = 1.0 / m.ref_idx;
        cosine = -vec3_dot(ray_direction(r_in), rec.normal) / vec3_length(ray_direction(r_in));
    }
    if (refract(ray_direction(r_in), outward_normal, ni_over_nt, &refracted))
        reflect_prob = schlick(cosine, m.ref_idx);
    else
        reflect_prob = 1.0;
    if (get_random(seeds) < reflect_prob)
        *scattered = ray(rec.p, reflected);
    else
        *scattered = ray(rec.p, refracted);
    return 1;
}

static t_vec3 color_gpu(t_ray r, t_hitable_list world, uint2 *seeds/*, int id, __global t_debug *logfile, int *l*/)
{
	t_vec3 stack[51];
	int depth = 0;
	int state = 1;
	while (state)
	{		
		t_hit_record rec;
		if (hitable_list_hit(world, r, 0.001, FLT_MAX, &rec/*, logfile, l*/)) {
			t_ray scattered;
			t_vec3 attenuation;
			int tcase = 0;
			if (rec.mat_ptr.type==0)
			{
				tcase =	lambertian_scatter(rec.mat_ptr, r, rec, &attenuation, &scattered, seeds);
			}
			else if(rec.mat_ptr.type==1)
			{
				tcase =	metal_scatter(rec.mat_ptr, r, rec, &attenuation, &scattered, seeds);
			}
			else if(rec.mat_ptr.type==2)
			{
				tcase =	dielectric_scatter(rec.mat_ptr, r, rec, &attenuation, &scattered, seeds);
			}
			if (depth < 50 && tcase) {

				// if (id == 33460)
				// {
				// 	logfile[*l].step = 'A';
				// 	logfile[*l].depth = depth;
				// 	logfile[*l].x = attenuation.e[0];
				// 	logfile[*l].y = attenuation.e[1];
				// 	logfile[*l].z = attenuation.e[2];
				// 	*l = *l + 1;
				// }

				stack[depth] = attenuation;
                r = scattered;
				depth++;
			}
			else {
				// if (id == 33460)
				// {
				// 	logfile[*l].step = 'B';
				// 	logfile[*l].depth = depth;
				// 	logfile[*l].x = 0.0;
				// 	logfile[*l].y = 0.0;
				// 	logfile[*l].z = 0.0;
				// 	*l = *l + 1;
				// }

				stack[depth] = vec3(0, 0, 0);
				state = 0;
			}
		}
		else {
			t_vec3 unit_direction = vec3_unit_vector(ray_direction(r));
			float t = 0.5*(unit_direction.e[1] + 1.0);
			stack[depth] = vec3_add(vec3_mul_num((1.0 - t),vec3(1.0, 1.0, 1.0)), vec3_mul_num(t, vec3(0.5, 0.7, 1.0)));
			// if (id == 33460)
			// {
			// 	logfile[*l].step = 'C';
			// 	logfile[*l].depth = depth;
			// 	logfile[*l].x = stack[depth].e[0];
			// 	logfile[*l].y = stack[depth].e[1];
			// 	logfile[*l].z = stack[depth].e[2];
			// 	*l = *l + 1;
			// }
			state = 0;
		}
	}
	t_vec3 res;
	res = stack[depth];
	depth--;
	while(depth >= 0)
	{
		res = vec3_mul(stack[depth], res);
		depth--;
	}
	// if (id == 33460)
	// {
	// 	logfile[*l].step = 'R';
	// 	logfile[*l].depth = depth;
	// 	logfile[*l].x = res.e[0];
	// 	logfile[*l].y = res.e[1];
	// 	logfile[*l].z = res.e[2];
	// 	*l = *l + 1;
	// }
	return res;
}

__kernel void processed (
	__global float *output,
	__global unsigned int *seed,
	t_camera cam,
    t_sphere sphere,
	int width,
	int height,
	// __global t_debug *logfile,
	__global t_sphere *objects,
	int size
	)
{
	int l = 0;

    t_sphere list[1];
    list[0] = sphere;

    t_hitable_list world = hitable_list(objects, size);

	int id;
    int x;
    int y;
	id = get_global_id(0);
	if (id < 0 || id >= width * height)
		return ;

	y = id / width;
	x = id % width;

	uint2	seeds;
	seeds.x = seed[id];
	seeds.y = seed[id + width * height];


	int ind = width * (height - 1 - y) + x;
	float u = (float)(x + get_random(&seeds)) / (float)width;
	float v = (float)(y + get_random(&seeds)) / (float)height;
	t_ray r = camera_get_ray(cam, u, v, &seeds);
	t_vec3 p = ray_point_at_parameter(r, 2.0);

	// if (id == 33460)
	// {
	// 	for (int i = 0; i < 5; i++)
	// 	{
	// 		logfile[l].step = 'X';
	// 		logfile[l].x = objects[i].center.e[0];
	// 		logfile[l].y = objects[i].center.e[1];
	// 		logfile[l].z = objects[i].center.e[2];
	// 		l++;
	// 	}
	// }

	// if (id == 33460)
	// {
	// 	for (int i = 0; i < 5; i++)
	// 	{
	// 		logfile[l].step = 'O';
	// 		logfile[l].x = world.list[i].center.e[0];
	// 		logfile[l].y = world.list[i].center.e[1];
	// 		logfile[l].z = world.list[i].center.e[2];
	// 		l++;
	// 	}
	// }
	
	t_vec3 col = color_gpu(r, world, &seeds/*, id, logfile, &l*/);

	// ((__global unsigned int*)output[id * 3] = col.e[0];
	// ((__global unsigned int*)output)[id * 3 + 1] = col.e[1];
	// ((__global unsigned int*)output)[id * 3 + 2] = col.e[2];

	// if (id == 33460)
	// {
	// 	logfile[l].step = 'Z';
	// 	logfile[l].x = col.e[0];
	// 	logfile[l].y = col.e[1];
	// 	logfile[l].z = col.e[2];
	// 	l++;
	// }

	// if (id == 33460)
	// {
	// output[33460] = col.e[0];
	// output[33460 + 1] = col.e[1];
	// output[33460 + 2] = col.e[2];
	// }
	// else{

	output[ind * 3] = col.e[0];
	output[ind * 3 + 1] = col.e[1];
	output[ind * 3 + 2] = col.e[2];
	//}
}
