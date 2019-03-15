#include "structs.h"

t_vec3 random_in_unit_disk(t_uint2 *seeds)
{
    t_vec3 p;
    do
    {
        p = vec3_mul_num(2.0, vec3_sub(vec3(get_random(seeds), get_random(seeds), 0),vec3(1, 1, 0)));
    } while (vec3_dot(p, p) >= 1.0);
    return p;
}

t_camera	camera(t_vec3 lookfrom, t_vec3 lookat, t_vec3 vup, float vfov, float aspect, float aperture, float focus_dist) 
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

t_ray camera_get_ray(t_camera cam, float s, float t, t_uint2 *seeds) 
{
    t_vec3 rd = vec3_mul_num(cam.lens_radius, random_in_unit_disk(seeds));
    t_vec3 offset = vec3_add(vec3_mul_num(rd.e[0],cam.u), vec3_mul_num(rd.e[1],cam.v));
    return ray(vec3_add(cam.origin, offset), 
            vec3_sub(vec3_sub(vec3_add(vec3_add(cam.lower_left_corner, vec3_mul_num(s, cam.horizontal)), vec3_mul_num(t, cam.vertical)), cam.origin), offset));
}
