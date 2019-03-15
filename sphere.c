#include "structs.h"

t_sphere sphere(t_vec3 cen, float r, t_material m)
{
    t_sphere s;
    s.center = cen;
    s.radius = r;
    s.mat_ptr = m;
    return s;
}

int sphere_hit(t_sphere s, t_ray r, float t_min, float t_max, t_hit_record *rec)
{
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