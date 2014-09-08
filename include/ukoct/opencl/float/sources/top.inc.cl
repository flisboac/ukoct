ukoct_STRFY(

		
__kernel
void octdiff_top__global(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor
) {
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);

	from[g_ij] = (g_i != g_j) ? INFINITY : 0;
	// coherency here is ensured as a consequence of the operation itself
	//barrier(CLK_GLOBAL_MEM_FENCE);
}


)
