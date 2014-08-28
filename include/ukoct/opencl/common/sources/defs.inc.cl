ukoct_STRFY(

inline int ridx(
	int nvars,
	int i,
	int j
) {
	return i * nvars + j;
}


inline int cidx(
	int nvars,
	int i,
	int j
) {
	return i + j *  nvars;
}


inline int idx(
	bool rowmajor,
	int nvars,
	int i,
	int j
) {
	return rowmajor ? ridx(nvars, i, j) : cidx(nvars, i, j);
}


inline int sw(int i) {
	return (i & 1) ? (i + 1) : (i - 1);
}


)
