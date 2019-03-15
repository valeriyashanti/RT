#include "structs.h"

t_vec3 vec3(float x, float y, float z)
{
    t_vec3 v;
    v.e[0] = x;
    v.e[1] = y;
    v.e[2] = z;
    return v;
}

float vec3_length(t_vec3 v)
{
    return sqrt(v.e[0] * v.e[0] + v.e[1] * v.e[1] + v.e[2] * v.e[2]);
}

float vec3_squared_length(t_vec3 v)
{
    return v.e[0] * v.e[0] + v.e[1] * v.e[1] + v.e[2] * v.e[2];
}

void vec3_make_unit_vector(t_vec3 *v)
{
    float k = 1.0 / sqrt(v->e[0] * v->e[0] + v->e[1] * v->e[1] + v->e[2] * v->e[2]);
    v->e[0] *= k;
    v->e[1] *= k;
    v->e[2] *= k;
}

t_vec3 vec3_unit_vector(t_vec3 v) 
{
	return vec3_div_num(v, vec3_length(v));
}

t_vec3 vec3_cross(t_vec3 v1, t_vec3 v2) 
{
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		        (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
		        (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

float vec3_dot(t_vec3 v1, t_vec3 v2)
{
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

t_vec3 vec3_add(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

t_vec3 vec3_sub(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

t_vec3 vec3_mul(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

t_vec3 vec3_mul_num(float n, t_vec3 v)
{
    return vec3(n * v.e[0], n * v.e[1], n * v.e[2]);
}

t_vec3 vec3_div(t_vec3 v1, t_vec3 v2)
{
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

t_vec3 vec3_div_num(t_vec3 v, float n)
{
    return vec3(v.e[0] / n, v.e[1] / n, v.e[2] / n);
}