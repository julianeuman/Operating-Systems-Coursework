#include <stdlib.h>
#include <stdio.h>
#include "tlb.h"
#include "pagetable.h"
#include "global.h" /* for tlb_size */
#include "statistics.h"

/*******************************************************************************
 * Looks up an address in the TLB. If no entry is found, attempts to access the
 * current page table via cpu_pagetable_lookup().
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t tlb_lookup(vpn_t vpn, int write) {
   pfn_t pfn;

   /* 
    * FIX ME : Step 6
    */
	int index;
	for (index = 0; index < tlb_size; index++) {
		if (tlb[index].vpn == vpn && tlb[index].valid) {
			pfn = tlb[index].pfn;
			if (write) {
				tlb[index].dirty = write;
           			current_pagetable[vpn].dirty = write;			
			}
			tlb[index].valid = 1;
			tlb[index].used = 1;
			count_tlbhits++;
			// update current table
			current_pagetable[vpn].valid = 1;
			current_pagetable[vpn].used = 1;
			current_pagetable[vpn].pfn = pfn;
			return pfn;
		}			
	}

   /* 
    * Search the TLB for the given VPN. Make sure to increment count_tlbhits if
    * it was a hit!
    */
    
   /* If it does not exist (it was not a hit), call the page table reader */
   pfn = pagetable_lookup(vpn, write);

   /* 
    * Replace an entry in the TLB if we missed. Pick invalid entries first,
    * then do a clock-sweep to find a victim.
    */
	int found = 0;
	tlbe_t* replaced_entry = tlb;
	int i;
   	for (i = 0; i < tlb_size; i++) {
		if (!tlb[i].valid) {
			replaced_entry = &tlb[i];
			found = 1;
			break; // break out of loop
       		}
   	}


  	if(!found) {
	       for (i = 0; i < tlb_size; i++) {
		   if (tlb[i].used) {
		       tlb[i].used = 0;
		   } else {
		       replaced_entry = &tlb[i];
		       break;
		   }
		   if (i + 1 == tlb_size) {
		       i = -1;
		   }
       		}
   	}
   /*
    * Perform TLB house keeping. This means marking the found TLB entry as
    * accessed and if we had a write, dirty. We also need to update the page
    * table in memory with the same data.
    *
    * We'll assume that this write is scheduled and the CPU doesn't actually
    * have to wait for it to finish (there wouldn't be much point to a TLB if
    * we didn't!).
    */
	// update entry
	replaced_entry->vpn=vpn;
	replaced_entry->pfn=pfn;
	replaced_entry->valid=1;
	replaced_entry->used=1;
	// update page table
	current_pagetable[vpn].valid = 1;
	current_pagetable[vpn].used = 1;
	current_pagetable[vpn].pfn = pfn;
	// if writen
	if (write) {
		replaced_entry->dirty=write;
		current_pagetable[vpn].dirty = write;	
	}

   	return pfn;
}

