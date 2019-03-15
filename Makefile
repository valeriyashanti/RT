all:
	 gcc main.c ray.c camera.c vec3.c material.c hitable.c texture.c sphere.c render.c random.c gpu.c -I ~/Library/Frameworks/SDL2.framework/Versions/A/Headers -F ~/Library/Frameworks -framework SDL2 -framework OpenCL

test:
	gcc -c ray.c camera.c vec3.c material.c hitable.c sphere.c texture.c