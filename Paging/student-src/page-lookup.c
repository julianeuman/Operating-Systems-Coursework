#include "swapfile.h"
#include "statistics.h"
#include "pagetable.h"

/*******************************************************************************
 * Looks up an address in the current page table. If the entry for the given
 * page is not valid, increments count_pagefaults and traps to the OS.
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t pagetable_lookup(vpn_t vpn, int write) {
	pfn_t pfn = 0; /* CHANGE ME */   
	pte_t pte_entry = current_pagetable[vpn];
	if (pte_entry.valid) {
		pfn = pte_entry.pfn;
		return pfn;
	}
	count_pagefaults++;
	return pagefault_handler(vpn, write);
	
}
