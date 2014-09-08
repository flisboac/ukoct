ukoct_STRFY(

inline int ridx(
	int nvars,
	int i,
	int j
) {
	return i * nvars + j;
}


inline int ridx3(
	int nvars,
	int i,
	int j,
	int k
) {
	return k + nvars * (j + nvars * i);
}


inline int cidx(
	int nvars,
	int i,
	int j
) {
	return i + j *  nvars;
}


inline int cidx3(
	int nvars,
	int i,
	int j,
	int k
) {
	return i + nvars * (j + nvars * k);
}


inline int idx(
	bool rowmajor,
	int nvars,
	int i,
	int j
) {
	return rowmajor ? ridx(nvars, i, j) : cidx(nvars, i, j);
}


inline int idx3(
	bool rowmajor,
	int nvars,
	int i,
	int j,
	int k
) {
	return rowmajor ? ridx3(nvars, i, j) : cidx3(nvars, i, j);
}


inline int sw(int i) {
	return (i & 1) ? (i + 1) : (i - 1);
}


)
