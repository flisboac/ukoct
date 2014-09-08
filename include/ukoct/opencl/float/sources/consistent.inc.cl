ukoct_STRFY(


/**
 * A very naive and rudimentary consistency check.
 * This operator does not consider any kind of optimization, but it is the
 * probably less troublesome approach.
 */
__kernel
void octdiff_consistent__global_reduce(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__global float* result
) {
	const int nelems = nvars;
	
	const int g_id = get_global_id(0);
	const int g_kk = idx(rowmajor, nvars, g_id, g_id);
	
	float v = from[g_kk];
	if (v > 0) from[g_kk] = v = 0;
	result[g_id] = v;
	barrier(CLK_GLOBAL_MEM_FENCE);
	float_reduceMin__global(result, g_id, nelems);
	if (g_id == 0) result[0] = (result[0] < 0 ? 0 : 1)
}


/**
 * ... Now using local memory and hybrid global/local reduction. More suited
 * for ridiculously big DBMs.
 * Some assumptions are made regarding the state of the DBM, though. Please
 * read the comments!
 */
__kernel
void octdiff_consistent__local(
		__global float* from,     //!< The original (and destination) matrix of size `nvars * nvars`.
		__global float* to,       //!< Final reduction in global memory, must have size equal to the number of groups
		__const  int    rowmajor, //!< Flag indicating row or column-major matrix ordering.
		__local  float* vec,      //!< Temporary buffer used for new diagonal values, of size `nvars`.
		__local  float* buf       //!< Temporary buffer used for reduction, of size `nvars`.
) {
	// This algorithm takes into consideration that, because of coherency,
	// `m(ii) = m(II)`. As `i = I` and `I = i`, we can infer that each element
	// of the diagonal depends on its even-odd counterpart (e.g. (0,0) depends
	// on (1, 1), (2, 2) depends on (3, 3), etc.), we can reduce the number of
	// operations by half and eliminate the chances for race conditions.
	// when updating these values.
	// IMPORTANT NOTE: This kernel assumes the DBM was coherent prior to the
	// call. 
	const int nvars = get_global_size(0);
	
	// The global / local IDs
	const int g_id = get_global_id(0);
	const int w_id = get_group_id(0);
	const int l_id = get_local_id(0);
	
	// The indices on the matrix, for updating.
	const int g_kk = idx(rowmajor, nvars, g_id, g_id);
	
	// Load values to local memory
	vec[l_id] = from[g_kk]; // This is only... (coherent?)... if the DBM is coherent!
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	// Do the reduction:
	// For each item in the vector, copy it to the temporary buffer, and then
	// set its value in the output local vector accordingly (if value is > 0,
	// set 0).
	for (unsigned int i = get_local_size(0); i > 0; i >>= 1) {
		if(l_id < i) {
			if (i < get_local_size(0)) {
				// To avoid branch misses the most we can, we check for the
				// second-and-onwards condition first.
				
				// The second step would be to reduce the values, up to the  
				buf[l_id] = min(buf[l_id], buf[l_id + i]);
				
			} else {
				// The first step is to set zero to all values on the diagonal
				// that are greater than zero.
				float v = vec[g_kk];
				if (v > 0)
					vec[g_kk] = v = 0;
				buf[l_id] = v;
			}
		}
		
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	// Reduce in global memory
	// Done like this because there's no sharing of local memory between
	// workgroups, so the first item on the group does the final reduction
	// in global memory.
	for (unsigned int i = get_num_groups(0); i > 0; i >>= 1) {
		if (l_id == 0) {
			if (i < get_num_groups(0)) {
				to[w_id] = min(to[w_id], to[w_id + i]);
				
			} else {
				to[w_id] = buf[0];
			}
		}

		barrier(CLK_GLOBAL_MEM_FENCE);
	}
	
	// Copy the new diagonal values
	from[g_kk] = vec[l_id];
	//barrier(CLK_GLOBAL_MEM_FENCE);
}




)
