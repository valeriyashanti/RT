#include "structs.h"

t_hitable_list  hitable_list(t_sphere *l, int n)
{
    t_hitable_list hl;
    hl.list = l;
    hl.list_size = n;
    return hl;
}

int hitable_list_hit(t_hitable_list hl, t_ray r, float t_min, float t_max, t_hit_record *rec)
{
	t_hit_record temp_rec;
	int hit_anything = 0;
	double closest_so_far = t_max;
	for (int i = 0; i < hl.list_size; i++) {
		if (sphere_hit(hl.list[i], r, t_min, closest_so_far, &temp_rec)) {
			hit_anything = 1;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
		}
	}
	return hit_anything;
}
