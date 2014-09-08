ukoct_STRFY(

__kernel
void octdiff_weaklyClosed__global_reduce(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__global float* result
) {
	const int nelems = nvars * nvars * nvars;
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);
	const int g_k = get_global_id(2);

	const int g_id = ridx3(nvars, g_i, g_j, g_k);
	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	const int g_ik = idx(rowmajor, nvars,    g_i,     g_k);
	const int g_kj = idx(rowmajor, nvars,    g_k,     g_j);
	const int g_iI = idx(rowmajor, nvars,    g_i,  sw(g_i));
	const int g_Jj = idx(rowmajor, nvars, sw(g_j),    g_j);

	// Consistent
	if (g_id >= nvars)
		result[g_id] = 0;
	else {
		const int g_kk = idx(rowmajor, nvars, g_id, g_id);
		float v = from[g_kk];
		if (v > 0)
			from[g_kk] = v = 0;
		result[g_id] = v;
	}
	barrier(CLK_GLOBAL_MEM_FENCE);
	float_reduceMin__global(result, g_id, nelems);
	
	// Tightness and closedness
	if (result[0] >= 0) {
		_mat(i, k) + _mat(k, j) < std::min(_mat(i, j), (_mat(i, I) + _mat(J, j)) / 2)
		bool check = ( from[g_ik] + from[g_kj] >= min(from[g_ij], (from[g_iI] + from[g_Jj]) / 2) )
		result[g_id] = check ? 1 : 0;
		barrier(CLK_GLOBAL_MEM_FENCE);
		float_reduceMin__global(result, g_id, nelems);
		
	} else {
		if (g_id == 0) result[0] = (result[0] < 0) ? 0 : 1;
		//barrier(CLK_GLOBAL_MEM_FENCE);
	}
}


)
