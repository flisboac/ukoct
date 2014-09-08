ukoct_STRFY(

		
__kernel
void octdiff_forget__global(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__const  int    nvars,
	__const  int    octVar
) {
	// Enqueuing a 2x2 kernel
	const int g_i = 2 * octVar - (1 - get_global_id(0)) - 1;
	const int g_j = 2 * octVar - (1 - get_global_id(1)) - 1;
	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	
	float v_ij = (g_i == g_j) ? 0 : INFINITY;
	from[g_ij] = v_ij;
}


)
