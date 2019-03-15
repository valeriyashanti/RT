#include "structs.h"

float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

int refract(t_vec3 v, t_vec3 n, float ni_over_nt, t_vec3 *refracted) {
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

t_vec3 reflect(t_vec3 v, t_vec3 n) {
	return vec3_sub(v, vec3_mul_num(2 * vec3_dot(v, n), n));
}

t_vec3 random_in_unit_sphere(t_uint2 *seeds) {
	t_vec3 p;
	do {
		p = vec3_mul_num(2.0, vec3_sub(vec3(get_random(seeds), get_random(seeds), get_random(seeds)), vec3(1, 1, 1)));
	} while (vec3_squared_length(p) >= 1.0);
	return p;
}

t_material lambertian(t_vec3 a) 
{
    t_material m;
    m.type = 0;
    m.albedo = a;
    return m;
}

int lambertian_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds)
{
    t_vec3 target = vec3_add(vec3_add(rec.p, rec.normal), random_in_unit_sphere(seeds));
    *scattered = ray(rec.p, vec3_sub(target, rec.p));
    *attenuation = m.albedo;
    return 1;
}

t_material metal(t_vec3 a, float f) 
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

int metal_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds)
{
    t_vec3 reflected = reflect(vec3_unit_vector(ray_direction(r_in)), rec.normal);
	*scattered = ray(rec.p, vec3_add(reflected, vec3_mul_num(m.fuzz, random_in_unit_sphere(seeds))));
	*attenuation = m.albedo;
	return (vec3_dot(ray_direction(*scattered), rec.normal) > 0);
}

t_material dielectric(float ri) 
{
    t_material m;
    m.type = 2;
    m.ref_idx = ri;

    return m;
}

int dielectric_scatter(t_material m, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered, t_uint2 *seeds)
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