ukoct_STRFY(

		
__kernel
void octdiff_addDiffConstraint__global(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__const  int    a,
	__const  int    b,
	__const  float  d,
) {
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	const int g_JI = idx(rowmajor, nvars, sw(g_j), sw(g_i));
	
	const int g_ic = idx(rowmajor, nvars,    g_i,       a);
	const int g_cj = idx(rowmajor, nvars,      b,       j);
	const int g_Jc = idx(rowmajor, nvars, sw(g_j),      a);
	const int g_cI = idx(rowmajor, nvars,      b,  sw(g_i));
	
	float v_ij = min(from[v_ij], from[v_ic] + d + from[v_cj]);
	float v_JI = min(from[v_JI], from[v_Jc] + d + from[v_cI]);
	barrier(CLK_GLOBAL_MEM_FENCE);
	from[g_ij] = min(v_ij, v_JI);
}



)
