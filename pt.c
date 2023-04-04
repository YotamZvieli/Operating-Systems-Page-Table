#include "os.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

uint64_t pross_level(uint64_t* current_pos, uint64_t address){
    current_pos = (uint64_t *)((uint64_t)(current_pos) & 0xfffffffffffff000); 
    if(current_pos[address] == 0x0){
        current_pos[address] = ((alloc_page_frame() << 12) | 0x1);
        return current_pos[address];
    }
    else{
        return current_pos[address];
    }
}

uint64_t* page_walk (uint64_t pt, uint64_t vpn_trans){
    vpn_trans = vpn_trans & 0x1fffffffffff;
    uint64_t vr1 = vpn_trans & 0xfffffffff;
    uint64_t vr2 = vpn_trans & 0x7ffffff;
    uint64_t vr3 = vpn_trans & 0x3ffff;
    uint64_t curr = pt;
    for(int i = 1; i < 5; i++){
        if( i == 1){
            curr = pross_level((uint64_t *)phys_to_virt(curr),vpn_trans >> 36);
        }
        if (i == 2)
        {
            curr = pross_level((uint64_t *)phys_to_virt(curr),vr1 >> 27);
        }
        if (i == 3)
        {
            curr = pross_level((uint64_t *)phys_to_virt(curr),vr2 >> 18);
        }
        if(i == 4){
            curr = pross_level((uint64_t *)phys_to_virt(curr),vr3 >> 9);
        } 
    }
    return phys_to_virt(curr);

       
                
}
uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    uint64_t full_pa = pt << 12;
    uint64_t* level5;
    level5 = page_walk(full_pa,vpn);
    level5 = (uint64_t *)((uint64_t)(level5) & 0xfffffffffffff000);
    int PTE = (vpn & 0x1ff);
    if((level5[PTE] & 0x1) == 0){
        return NO_MAPPING;
    }
    else
    {
        return (level5[PTE] >> 12);
    }
    
    
}
void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    uint64_t full_pa = pt << 12;
    uint64_t* level5;
    level5 = page_walk(full_pa,vpn);
    level5 = (uint64_t *)((uint64_t)(level5) & 0xfffffffffffff000);
    uint64_t PTE = (vpn & 0x1ff);
    if(ppn == NO_MAPPING){
        level5[PTE] = 0x0;
    }
    else
    {
         level5[PTE] = ((ppn << 12) | 0x1);
    }
}

