ukoct_STRFY(

		
__kernel
void octdiff_equals__global_reduce(
	__const  int    nvars,
	__global float* self,
	__const  int    self_rowmajor,
	__global float* other,
	__const  int    other_rowmajor,
	__global float* result
) {
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int gs_ij = idx(self_rowmajor,  nvars,    g_i,     g_j);
	const int go_ij = idx(other_rowmajor, nvars,    g_i,     g_j);

	result[g_id] = (self[gs_ij] == other[go_is]) ? 1 : 0;
	barrier(CLK_GLOBAL_MEM_FENCE);
	float_reduceMin__global(result, g_id, nelems);
}


)
