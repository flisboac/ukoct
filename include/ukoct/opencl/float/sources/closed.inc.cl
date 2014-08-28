ukoct_STRFY(

/**
 * result[0] will be 0 if from is coherent, and 1 otherwise.
 * This method will test consistency and closedness, leading to probably high
 * idleness on most worker during the consistency check. 
 */
__kernel
void octdiff_completeCoherent__global_reduce(
	__global float* from,
	__global float* result,
	__const  int    rowmajor
) {
	const int nvars = get_global_size(0); // equals `dbm.size()`
	const int nelems = nvars * nvars;
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);
	const int g_k = get_global_id(2);

	const int g_id = ridx(nvars, g_i, g_j);
	
	// First, check consistency. 
	for (unsigned int i = nvars; i > 0; i >>= 1) {
		if (g_id < nvars) {
			int g_ii = idx(rowmajor, nvars,    g_id,     g_id);
			
			if (i < nvars) {
				result[g_id] = min(result[g_id], result[g_id + i]);
				
			} else {
				float v = from[g_ii];
				if (v > 0)
					from[g_ii] = v = 0;
				result[g_id] = v;
			}
		}
		
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
	
	// If matrix is consistent, do the reduction
	if (result[0] >= 0) {
		for (unsigned int k = 0; k < nvars; ++k) {
			int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
			int g_ik = idx(rowmajor, nvars,    g_i,     g_k);
			int g_kj = idx(rowmajor, nvars,    g_k,     g_j);
			
			for (unsigned int i = nelems; i > 0; i >>= 1) {
				if (i < nelems) {
					if(g_id < i) {  
						result[g_id] = max(result[g_id], result[g_id + i]);
					}
					
				} else {
					result[g_ij] = (from[g_ij] > from[g_ik] + from[g_kj]);
				}
				
				barrier(CLK_GLOBAL_MEM_FENCE);
			}
		}
	}
}


/**
 * result[0] will be 0 if from is coherent, and 1 otherwise.
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
	const int g_k = get_global_id(2);

	const int g_id = ridx(nvars, g_i, g_j);
	
	for (unsigned int k = 0; k < nvars; ++k) {
		int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
		int g_ik = idx(rowmajor, nvars,    g_i,     g_k);
		int g_kj = idx(rowmajor, nvars,    g_k,     g_j);
		
		for (unsigned int i = nelems; i > 0; i >>= 1) {
			
			if (i < nelems) {
				if(g_id < i) {  
					result[g_id] = max(result[g_id], result[g_id + i]);
				}
				
			} else {
				result[g_ij] = (from[g_ij] > from[g_ik] + from[g_kj]);
			}
			
			barrier(CLK_GLOBAL_MEM_FENCE);
		}
	}
}

)
