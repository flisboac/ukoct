ukoct_STRFY(

/* Naming: <domain>_<operation>(_[_<qualifiers>]*)?
 * Memory Qualifiers, mutually exclusive:
 * - global: Operates on global memory
 * - local: Operates on local memory and copies results
 *   to global memory afterwards
 * - dualmem: Uses two matrices to operate on values from
 *   different iterations
 * - queued: The function is queued as many times as dictated
 *   by a third index (e.g. in (\/k, i, j), k is given explicitely
 *   as a function argument, while i and j are obtained from
 *   global/local indices.
 * - fullgroup: The workgroup (local) sizes must be the same as the
 *   global sizes, e.g., if G = {i:512, j:512}, L = {i:512, j:512}.
 *   
 */


inline void float_reduceMin__global(
	__global     float* vec,
	const          int  vec_id,
	const unsigned int  nelems
) {
	for (unsigned int i = nelems >> 1; i > 0; i >>= 1) {
		if(vec_id < i)
			vec[vec_id] = min(vec[vec_id], vec[vec_id + i]);
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
}


inline void float_reduceMax__global(
	__global     float* vec,
	const          int  vec_id,
	const unsigned int  nelems
) {
	for (unsigned int i = nelems >> 1; i > 0; i >>= 1) {
		if(vec_id < i)
			vec[vec_id] = max(vec[vec_id], vec[vec_id + i]);
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
}


inline void float_reduceMin__local(
	__local      float* vec,
	const          int  vec_id,
	const unsigned int  nelems
) {
	for (unsigned int i = nelems >> 1; i > 0; i >>= 1) {
		if(vec_id < i)
			vec[vec_id] = min(vec[vec_id], vec[vec_id + i]);
		barrier(CLK_LOCAL_MEM_FENCE);
	}
}


inline void float_reduceMax__local(
	__local      float* vec,
	const          int  vec_id,
	const unsigned int  nelems
) {
	for (unsigned int i = nelems >> 1; i > 0; i >>= 1) {
		if(vec_id < i)
			vec[vec_id] = max(vec[vec_id], vec[vec_id + i]);
		barrier(CLK_LOCAL_MEM_FENCE);
	}
}


)
