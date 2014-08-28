ukoct_STRFY(

/**
 * A very naive and rudimentary consistency check.
 * This operator does not consider any kind of optimization, but it is the
 * probably less troublesome approach.
 */
__kernel
void octdiff_intConsistent__global_reduce(
	__global float* from,
	__global float* result,
	__const  int    rowmajor
) {
	const int nvars = get_global_size(0); // equals `dbm.size()`
	
	const int g_id = get_global_id(0);
	const int g_iI = idx(rowmajor, nvars,    g_id, sw(g_id));
	const int g_Ii = idx(rowmajor, nvars, sw(g_id),   g_id);
	
	// Do the reduction
	for (unsigned int i = nvars; i > 0; i >>= 1) {
		if (i < nvars) {
			// To avoid branch misses the most we can, we check for the
			// second-and-onwards condition first.
			
			if(g_id < i) {
				// The second step would be to reduce the values, up to the  
				result[g_id] = min(result[g_id], result[g_id + i]);
			}
			
		} else {
			result[g_id] = from[g_iI] + from[g_Ii];
		}
		
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
}


)
