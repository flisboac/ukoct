ukoct_STRFY(

		
__kernel
void octdiff_shortestPath__global_loop(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__global float* buf,
	__const  int    niters
) {
	const int nelems = nvars * nvars;
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	const int g_JI = idx(rowmajor, nvars, sw(g_J), sw(g_i));

	for (unsigned int k = 0; k < niters; ++k, ++k) {
		const int g_kK = idx(rowmajor, nvars,    g_k,  sw(g_k));
		const int g_Kk = idx(rowmajor, nvars, sw(g_k),    g_k);
		// ij
		const int g_ik = idx(rowmajor, nvars,    g_i,     g_k);
		const int g_iK = idx(rowmajor, nvars,    g_i,  sw(g_k));
		const int g_kj = idx(rowmajor, nvars,    g_k,     g_j);
		const int g_Kj = idx(rowmajor, nvars, sw(g_k),    g_j);
		// JI
		const int g_Jk = idx(rowmajor, nvars, sw(g_j),    g_k);
		const int g_JK = idx(rowmajor, nvars, sw(g_j), sw(g_k));
		const int g_kI = idx(rowmajor, nvars,    g_k,  sw(g_i));
		const int g_KI = idx(rowmajor, nvars, sw(g_k), sw(g_i));
		
		// Oh my gosh look at all these global memory accesses! :O
		float v_ij = min(
			from[g_ij], min(
				from[g_ik] + from[g_kj], min(
					from[g_iK] + from[g_Kj], min (
						from[g_ik] + from[g_kK] + from[Kj],
						from[g_iK] + from[g_Kk] + from[kj]
					)
				)
			)
		);
		float v_JI = min(
			from[g_JI], min(
				from[g_Jk] + from[g_kI], min(
					from[g_JK] + from[g_KI], min (
						from[g_Jk] + from[g_kK] + from[KI],
						from[g_JK] + from[g_Kk] + from[kI]
					)
				)
			)
		);

		// Must use barrier throughout because of the loop, so that execution
		// stops until everything before the barrier is executed.
		// Example: What would happen if, during the calculation of
		// v_JI, another work-item (namely, this item's counterpart) writes
		// its computed value to the matrix? Would the computed value of this
		// work-item's v_JI be valid (e.g. made from memory in a consistent
		// state)? 
		barrier(CLK_GLOBAL_MEM_FENCE);
		from[g_ij] = min(v_ij, v_JI);
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
}


)
