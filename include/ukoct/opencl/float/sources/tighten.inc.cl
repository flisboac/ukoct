ukoct_STRFY(

		
__kernel
void octdiff_tighten__global(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor
) {
	const int g_i = get_global_id(0);
	const int g_iI = idx(rowmajor, nvars,    g_i,  sw(g_i));

	from[g_iI] = 2 * floor(from[g_iI] / 2);
	// here, g_ij == g_iI, and therefore, because g_ij == g_JI
}


)
