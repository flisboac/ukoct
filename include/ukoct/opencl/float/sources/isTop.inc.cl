ukoct_STRFY(


__kernel
void octdiff_isTop__global_reduce(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__global float* result,
) {
	const int nelems = nvars * nvars;
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_id = ridx(nvars, g_i, g_j);
	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);

	result[g_ij] = (g_i != g_j ? (isinf(from[g_ij]) ? 1 : 0) : 1); //(from[g_ij] < 0 ? 0 : 1));  
	barrier(CLK_GLOBAL_MEM_FENCE);
	float_reduceMin__global(result, g_id, nelems);
}


)
