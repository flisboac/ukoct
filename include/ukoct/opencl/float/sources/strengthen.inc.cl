ukoct_STRFY(

		
__kernel
void octdiff_strengthen__global(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__const  int    intBased
) {
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	const int g_JI = idx(rowmajor, nvars, sw(g_J), sw(g_i));
	const int g_iI = idx(rowmajor, nvars,    g_i,  sw(g_i));
	const int g_Jj = idx(rowmajor, nvars, sw(g_j),    g_j);

	float v_ij = (intBased == 0)
		? min(from[g_ij], (from[g_iI] + from[g_Jj]) / 2)
		: g_i != g_j
			? (2 * floor(from[g_iI] / 2)
			: min( from[g_ij], floor(from[g_iI] + from[g_Jj]) / 2) )
		;
	float v_JI = (intBased == 0)
			? min(from[g_JI], (from[g_Jj] + from[g_iI]) / 2)
			: g_i != g_j
				? (2 * floor(from[g_iI] / 2)
				: min( from[g_JI], floor(from[g_Jj] + from[g_iI]) / 2) )
			;
	barrier(CLK_GLOBAL_MEM_FENCE);
	from[g_ij] = min(v_ij, v_JI);
	barrier(CLK_GLOBAL_MEM_FENCE);
}


)
