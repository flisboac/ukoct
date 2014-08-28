ukoct_STRFY(

/**
 */
__kernel
void octdiff_coherent__global_reduce(
	__global float* from,
	__global float* result,
	__const  int    rowmajor
) {
	const int nvars = get_global_size(0); // equals `dbm.size()`
	const int nelems = nvars * nvars;
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_id = ridx(nvars, g_i, g_j);
	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	const int g_JI = idx(rowmajor, nvars, sw(g_j), sw(g_i));
	
	// Do the reduction
	for (unsigned int i = nelems; i > 0; i >>= 1) {
		if (i < nelems) {
			// To avoid branch misses the most we can, we check for the
			// second-and-onwards condition first.
			
			if(g_id < i) {
				// The second step would be to reduce the values, up to the  
				result[g_id] = min(result[g_id], result[g_id + i]);
			}
			
		} else {
			result[g_ij] = (from[g_ij] == from[g_JI] ? 1 : 0);
		}
		
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
}


)
