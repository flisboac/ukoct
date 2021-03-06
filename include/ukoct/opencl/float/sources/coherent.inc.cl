ukoct_STRFY(


__kernel
void octdiff_coherent__global_reduce(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__global float* result
) {
	const int nelems = nvars * nvars;
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_id = ridx(nvars, g_i, g_j);
	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	const int g_JI = idx(rowmajor, nvars, sw(g_j), sw(g_i));

	result[g_ij] = (from[g_ij] == from[g_JI] ? 1 : 0);
	barrier(CLK_GLOBAL_MEM_FENCE);
	float_reduceMin__global(result, g_id, nelems);
}


)
