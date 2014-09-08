ukoct_STRFY(

		
__kernel
void octdiff_addConstraint__global(
	__const  int    nvars,
	__global float* from,
	__const  int    rowmajor,
	__const  int    aa,
	__const  int    ab,
	__const  float  ad,
	__const  int    ba,
	__const  int    bb,
	__const  float  bd,
) {
	const int g_i = get_global_id(0);
	const int g_j = get_global_id(1);

	const int g_ij = idx(rowmajor, nvars,    g_i,     g_j);
	const int g_JI = idx(rowmajor, nvars, sw(g_j), sw(g_i));
	
	const int g_ia = idx(rowmajor, nvars,    g_i,      aa);
	const int g_aj = idx(rowmajor, nvars,     ab,       j);
	const int g_ib = idx(rowmajor, nvars,    g_i,      ba);
	const int g_bj = idx(rowmajor, nvars,     bb,       j);
	
	const int g_Ja = idx(rowmajor, nvars, sw(g_j),     aa);
	const int g_aI = idx(rowmajor, nvars,     ab,  sw(g_i));
	const int g_Jb = idx(rowmajor, nvars, sw(g_j),     ba);
	const int g_bI = idx(rowmajor, nvars,     bb,  sw(g_i));
	
	float v_ij = min(
		from[v_ij], min(
			from[v_ia] + ad + from[v_aj],
			from[v_ib] + bd + from[v_bj]
		)
	);
	float v_JI = min(
		from[v_JI], min(
			from[v_Ja] + ad + from[v_aI],
			from[v_Jb] + bd + from[v_bI]
		)
	);
	barrier(CLK_GLOBAL_MEM_FENCE);
	from[g_ij] = min(v_ij, v_JI);
}


)
