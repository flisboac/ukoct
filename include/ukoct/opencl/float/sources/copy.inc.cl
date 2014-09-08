ukoct_STRFY(

		
__kernel
void octdiff_copy__global(
	__const  int    nvars,
	__global float* self,
	__const  int    self_rowmajor,
	__global float* other,
	__const  int    other_rowmajor
) {
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int gs_ij = idx(self_rowmajor,  nvars,    g_i,     g_j);
	const int go_ij = idx(other_rowmajor, nvars,    g_i,     g_j);
	
	self[gs_ij] = other[go_ij];
}


)
