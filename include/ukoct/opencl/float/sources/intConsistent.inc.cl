ukoct_STRFY(

		
/**
 * A very naive and rudimentary consistency check.
 * This operator does not consider any kind of optimization, but it is the
 * probably less troublesome approach.
 */
__kernel
void octdiff_intConsistent__global_reduce(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__global float* result
) {
	const int nelems = nvars;
	
	const int g_id = get_global_id(0);
	const int g_iI = idx(rowmajor, nvars,    g_id, sw(g_id));
	const int g_Ii = idx(rowmajor, nvars, sw(g_id),   g_id);
	
	result[g_id] = (from[g_iI] + from[g_Ii] < 0) ? 0 : 1;
	barrier(CLK_GLOBAL_MEM_FENCE);
	float_reduceMin__global(result, g_id, nelems);
	if (g_id == 0) result[0] = (result[0] < 0 ? 0 : 1)
}


)
