ukoct_STRFY(

/* Naming: <domain>_<operation>(_[_<qualifiers>]*)?
 * Memory Qualifiers, mutually exclusive:
 * - global: Operates on global memory
 * - local: Operates on local memory and copies results
 *   to global memory afterwards
 * - dualmem: Uses two matrices to operate on values from
 *   different iterations
 * - queued: The function is queued as many times as dictated
 *   by a third index (e.g. in (\/k, i, j), k is given explicitely
 *   as a function argument, while i and j are obtained from
 *   global/local indices.
 * - fullgroup: The workgroup (local) sizes must be the same as the
 *   global sizes, e.g., if G = {i:512, j:512}, L = {i:512, j:512}.
 *   
 */


)
