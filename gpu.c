#include "rt.h"

int		read_cl(char **code)
{
	int		fd;
	char	buf[20000];
	int		rd;

	bzero(buf, 20000);
	fd = open("gpu.cl", O_RDONLY);
	if (fd < 0)
		return (-1);
	rd = read(fd, buf, 20000);
	if (rd < 0)
		return (-1);
	close(fd);
	*code = strdup(buf);
	if (!code)
		return (-1);
	return (0);
}

void	error_finish(char *str)
{
	printf("%s\n",str);
	exit(1);
}

void	init_gpu(t_env *env)
{
	char	*kernel_source;
	t_cl	*cl;

	cl = env->cl;
	if ((cl->err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &cl->device_id, NULL)) != CL_SUCCESS)
		error_finish("gpu error: clGetDeviceIDs error");
	if (!(cl->context = clCreateContext(0, 1, &cl->device_id, NULL, NULL, &cl->err)))
		error_finish("gpu error: clCreateContext error");
	if (!(cl->commands = clCreateCommandQueue(cl->context, cl->device_id, 0, &cl->err)))
		error_finish("gpu error: clCreateCommandQueue error");
	if (read_cl(&kernel_source) != 0)
		error_finish("gpu error: read_cl error");
	if (!(cl->program = clCreateProgramWithSource(cl->context, 1, (const char **)&kernel_source, NULL, &cl->err)))
		error_finish("gpu error: clCreateProgramWithSource error");
	if ((cl->err = clBuildProgram(cl->program, 0, NULL, "-I.", NULL, NULL)) != CL_SUCCESS)
	{
		char		*buffer = malloc(100000);
		size_t		len;
		cl->err = clGetProgramBuildInfo(cl->program, cl->device_id,	CL_PROGRAM_BUILD_LOG, 100000, buffer, &len);
		if (cl->err == CL_SUCCESS)
		{
			printf("Compiler error message:\n%s\n", buffer);
		}
		else
		{
			printf("Error while retrieving compiler log\n");
			printf("Try increasing buffer size to 100 000\n");
		}
		error_finish("gpu error: clBuildProgram error");
		free(buffer);
	}
	cl->kernel = clCreateKernel(cl->program, "processed", &cl->err);
	if (!cl->kernel || cl->err != CL_SUCCESS)
		error_finish("gpu error: clCreateKernel error\n");
	if (!(cl->output = clCreateBuffer(cl->context, CL_MEM_WRITE_ONLY, WIDTH * HEIGHT * sizeof(float) * 3, NULL, NULL)))
		error_finish("gpu error: clCreateBuffer error\n");

	if (!(cl->seed_gpu = clCreateBuffer(cl->context, CL_MEM_READ_WRITE, sizeof(int) * env->seeds.size, NULL, NULL)))
		error_finish("gpu error: clCreateBuffer error\n");

	if (!(cl->logfile = clCreateBuffer(cl->context, CL_MEM_READ_WRITE, sizeof(t_debug) * 60, NULL, NULL)))
		error_finish("gpu error: clCreateBuffer error\n");

	if (!(cl->objects = clCreateBuffer(cl->context, CL_MEM_READ_WRITE, sizeof(t_sphere) * env->world.list_size, NULL, NULL)))
		error_finish("gpu error: clCreateBuffer error\n");
	if ((cl->err =clEnqueueWriteBuffer(cl->commands, cl->objects, CL_FALSE, 0, sizeof(t_sphere) * env->world.list_size, env->world.list, 0, 0, 0)) != CL_SUCCESS)
		error_finish("gpu error: clEnqueueWriteBuffer error cl->objects");
}

int		set_gpu_args(t_env *env, t_cl *cl)
{
	int err;

	err = clSetKernelArg(cl->kernel, 0, sizeof(cl_mem), &cl->output);
	err |= clSetKernelArg(cl->kernel, 1, sizeof(cl_mem), &cl->seed_gpu);
	err |= clSetKernelArg(cl->kernel, 2, sizeof(t_camera), &env->cam);
	err |= clSetKernelArg(cl->kernel, 3, sizeof(t_sphere), &env->list[0]);
	err |= clSetKernelArg(cl->kernel, 4, sizeof(int), &env->width);
	err |= clSetKernelArg(cl->kernel, 5, sizeof(int), &env->height);
	err |= clSetKernelArg(cl->kernel, 6, sizeof(cl_mem), &cl->objects);
	err |= clSetKernelArg(cl->kernel, 7, sizeof(int), &env->world.list_size);

	return (err);
}

void	render_gpu(t_env *env)
{
	t_cl *cl;

	cl = env->cl;
	
	bzero(env->logfile, 60 * sizeof(t_debug));
	bzero(env->gpu_calculations, env->width * env->height * sizeof(float) * 3);


	if ((cl->err =clEnqueueWriteBuffer(cl->commands, cl->logfile, CL_FALSE, 0, sizeof(t_debug) * 60, env->logfile, 0, 0, 0)) != CL_SUCCESS)
	error_finish("gpu error: clEnqueueWriteBuffer error");

	init_seeds(env);
	if ((cl->err =clEnqueueWriteBuffer(cl->commands, cl->seed_gpu, CL_FALSE, 0, sizeof(uint) * env->seeds.size, env->seeds.seeds, 0, 0, 0)) != CL_SUCCESS)
		error_finish("gpu error: clEnqueueWriteBuffer error");

	if (set_gpu_args(env, cl) != CL_SUCCESS)
		error_finish("gpu error: clSetKernelArg error\n");
	if (clGetKernelWorkGroupInfo(cl->kernel, cl->device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(cl->local), &cl->local, NULL) != CL_SUCCESS)
		error_finish("gpu error: clGetKernelWorkGroupInfo error\n");
	if ((cl->global = env->width * env->height) % cl->local)
	{
		printf("global size: %zu, local size: %zu\n", cl->global, cl->local);
		error_finish("gpu error: global size must be multiple");
	}
	if (clEnqueueNDRangeKernel(cl->commands, cl->kernel, 1, NULL, &cl->global, &cl->local, 0, NULL, NULL) != CL_SUCCESS)
		error_finish("gpu error: clEnqueueNDRangeKernel error");
	clFinish(cl->commands);
	

	if (clEnqueueReadBuffer(cl->commands, cl->output, CL_TRUE, 0, env->width * env->height * sizeof(float) * 3,	env->gpu_calculations, 0, NULL, NULL) != CL_SUCCESS)
		error_finish("gpu error: clEnqueueReadBuffer error");


	if (clEnqueueReadBuffer(cl->commands, cl->logfile, CL_TRUE, 0, sizeof(t_debug) * 60, env->logfile, 0, NULL, NULL) != CL_SUCCESS)
		error_finish("gpu error: clEnqueueReadBuffer error");
}

t_vec3 color_gpu(t_ray r, t_hitable_list world, t_uint2 *seeds)
{
	t_vec3 stack[51];
	int depth = 0;
	int state = 1;
	while (state)
	{		
		t_hit_record rec;


		if (hitable_list_hit(world, r, 0.001, FLT_MAX, &rec)) {
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
			else if (rec.mat_ptr.type == 3)
			{
				tcase = apply_texture(rec.mat_ptr, r, rec, &attenuation, &scattered, seeds);
			}
			if (depth < 50 && tcase) 
			{
				stack[depth] = attenuation;
                r = scattered;
				depth++;
			}
			else 
			{
				stack[depth] = vec3(0, 0, 0);
				state = 0;
			}
		}
		else 
		{
			t_vec3 unit_direction = vec3_unit_vector(ray_direction(r));
			float t = 0.5*(unit_direction.e[1] + 1.0);
			stack[depth] = vec3_add(vec3_mul_num((1.0 - t), vec3(1.0, 1.0, 1.0)), vec3_mul_num(t, vec3(0.5, 0.7, 1.0)));
			state = 0;
		}
	}
	t_vec3 res;
	res = stack[depth];
	depth--;
	while (depth >= 0)
	{
		res = vec3_mul(stack[depth], res);
		depth--;
	}
	return res;
}
