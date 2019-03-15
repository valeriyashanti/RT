#include "structs.h"

t_ray   ray(t_vec3 a, t_vec3 b)
{
    t_ray r;
    r.A = a;
    r.B = b;
    return r;
}

t_vec3 ray_origin(t_ray r)
{ 
    return r.A; 
}

t_vec3 ray_direction(t_ray r)
{ 
    return r.B;
}

t_vec3 ray_point_at_parameter(t_ray r, float t)
{ 
    return vec3_add(r.A, vec3_mul_num(t, r.B)); 
}
