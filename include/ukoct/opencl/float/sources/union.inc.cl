ukoct_STRFY(

		
__kernel
void octdiff_union__global(
	__const  int    nvars,
	__global float* self,
	__const  int    self_rowmajor,
	__global float* other,
	__const  int    other_rowmajor
) {
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int gs_ij = idx(self_rowmajor,  nvars,    g_i,     g_j);
	const int gs_JI = idx(self_rowmajor,  nvars, sw(g_j), sw(g_i));
	const int go_ij = idx(other_rowmajor, nvars,    g_i,     g_j);
	const int go_JI = idx(other_rowmajor, nvars, sw(g_j), sw(g_i));
	
	float v_ij = max(self[gs_ij], other[go_ij]);
	float v_JI = max(self[gs_JI], other[go_JI]);
	barrier(CLK_GLOBAL_MEM_FENCE);
	self[gs_ij] = max(v_ij, v_JI);
	//barrier(CLK_GLOBAL_MEM_FENCE);
}


)
