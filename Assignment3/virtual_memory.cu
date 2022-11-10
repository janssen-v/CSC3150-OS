#include "virtual_memory.h"
#include <cuda.h>
#include <cuda_runtime.h>

__device__ void init_invert_page_table(VirtualMemory *vm)
{
  for (int i = 0; i < vm->PAGE_ENTRIES; i++)
  {
    vm->invert_page_table[i] = 0x80000000; // invalid := MSB is 1
    vm->invert_page_table[i + vm->PAGE_ENTRIES] = i;
    // Page table limit is 1024 entries, since physical memory (data) only 32kb
    // 32kb/32 = 1024 pages (max) -> 10 bits
    // Remaining bits can be used to store LRU information with offset
  }
}

__device__ void vm_init(VirtualMemory *vm, uchar *buffer, uchar *storage,
                        u32 *invert_page_table, int *pagefault_num_ptr,
                        int PAGESIZE, int INVERT_PAGE_TABLE_SIZE,
                        int PHYSICAL_MEM_SIZE, int STORAGE_SIZE,
                        int PAGE_ENTRIES)
{
  // init variables
  vm->buffer = buffer;
  vm->storage = storage;
  vm->invert_page_table = invert_page_table;
  vm->pagefault_num_ptr = pagefault_num_ptr;

  // init constants
  vm->PAGESIZE = PAGESIZE;
  vm->INVERT_PAGE_TABLE_SIZE = INVERT_PAGE_TABLE_SIZE;
  vm->PHYSICAL_MEM_SIZE = PHYSICAL_MEM_SIZE;
  vm->STORAGE_SIZE = STORAGE_SIZE;
  vm->PAGE_ENTRIES = PAGE_ENTRIES;

  // before first vm_write or vm_read
  init_invert_page_table(vm);
}

// Additional function for updating page table
__device__ void lru_update(VirtualMemory *vm, int page_address)
{
  for (int i = 0; i < vm->PAGE_ENTRIES; i++)
  {
    int page_rank = vm->invert_page_table[i + vm->PAGE_ENTRIES];
    if (vm->invert_page_table[page_address + vm->PAGE_ENTRIES] < page_rank)
    {
      vm->invert_page_table[i + vm->PAGE_ENTRIES]--;
    }
  }
  vm->invert_page_table[page_address + vm->PAGE_ENTRIES] = vm->PAGE_ENTRIES - 1;
}

__device__ uchar vm_read(VirtualMemory *vm, u32 addr)
{
  /* Complete vm_read function to read single element from data buffer */
  u32 page_num = addr / vm->PAGESIZE;
  u32 offset = addr % vm->PAGESIZE;
  // Initialize address as invalid
  u32 memory_address = 0xFFFFFFFF;

  // Get physical address if it exists
  for (int i = 0; i < vm->PAGE_ENTRIES; i++)
  {
    if ((vm->invert_page_table[i]) == (page_num))
    {
      memory_address = (u32)i;
      break;
    }
  }

  // If in shared memory, update page page_rank (LRU)
  if (memory_address != 0xFFFFFFFF)
  {
    lru_update(vm, memory_address);
  }

  // Swap if not in shared memory
  else
  {
    // Get the least recently used page address
    u32 current_least = vm->invert_page_table[vm->PAGE_ENTRIES];
    u32 least_address = 0;
    for (int i = 0; i < vm->PAGE_ENTRIES; i++)
    {
      if (vm->invert_page_table[i + vm->PAGE_ENTRIES] < current_least)
      {
        current_least = vm->invert_page_table[i + vm->PAGE_ENTRIES];
        least_address = (u32)i;
      }
    }

    // Swap out least recently used page
    u32 LRU = vm->invert_page_table[least_address];
    for (int i = 0; i < vm->PAGESIZE; i++)
    {
      vm->storage[LRU * vm->PAGESIZE + i] = vm->buffer[least_address * vm->PAGESIZE + i];
      vm->buffer[least_address * vm->PAGESIZE + i] = vm->storage[page_num * vm->PAGESIZE + i];
    }

    // Increment pagefault number after swap
    atomicAdd(vm->pagefault_num_ptr, 1);
    lru_update(vm, least_address);
    vm->invert_page_table[least_address] = page_num;
    memory_address = least_address;
  }
  return vm->buffer[memory_address * vm->PAGESIZE + offset];
}

__device__ void vm_write(VirtualMemory *vm, u32 addr, uchar value)
{
  /* Complete vm_write function to write value into data buffer */
  u32 page_num = addr / vm->PAGESIZE;
  u32 offset = addr % vm->PAGESIZE;
  u32 physical_address = 0xFFFFFFFF;

  // Get physical address if it exists
  for (int i = 0; i < vm->PAGE_ENTRIES; i++)
  {
    if ((vm->invert_page_table[i]) == (page_num))
    {
      physical_address = (u32)i;
      break;
    }
  }

  // If in shared memory, write to address and update page rank (LRU)
  if (physical_address != 0xFFFFFFFF)
  {
    u32 address = physical_address * vm->PAGESIZE + offset;
    vm->buffer[address] = value;
    lru_update(vm, physical_address);
  }
  // If not in shared memory, swap or initialize before write
  else
  {   
    // Get the least recently used page address
    u32 current_least = vm->invert_page_table[vm->PAGE_ENTRIES];
    u32 least_address = 0;
    for (int i = 0; i < vm->PAGE_ENTRIES; i++)
    {
      if (vm->invert_page_table[i + vm->PAGE_ENTRIES] < current_least)
      {
        current_least = vm->invert_page_table[i + vm->PAGE_ENTRIES];
        least_address = (u32)i;
      }
    }

    // If LRU page is empty, write to it
    if (vm->invert_page_table[least_address] == 0x80000000)
    {
      vm->buffer[least_address * vm->PAGESIZE] = value;
      vm->invert_page_table[least_address] = page_num;
      atomicAdd(vm->pagefault_num_ptr, 1);
      lru_update(vm, least_address);
    }
    // If LRU page has existing content, evict the victim to storage
    else
    {
      // Swap out least recently used page
      u32 LRU = vm->invert_page_table[least_address];
      for (int i = 0; i < vm->PAGESIZE; i++)
      {
        vm->storage[LRU * vm->PAGESIZE + i] = vm->buffer[least_address * vm->PAGESIZE + i];
        vm->buffer[least_address * vm->PAGESIZE + i] = vm->storage[page_num * vm->PAGESIZE + i];
      }

      // Increment pagefault number after swap
      atomicAdd(vm->pagefault_num_ptr, 1);

      // Overwrite the evicted victim page
      vm->buffer[least_address * vm->PAGESIZE] = value;
      vm->invert_page_table[least_address] = page_num;

      // Update the pagetable & LRU
      lru_update(vm, least_address);
    }
  }
}

__device__ void vm_snapshot(VirtualMemory *vm, uchar *results, int offset,
                            int input_size)
{
  /* Complete snapshot function togther with vm_read to load elements from data
   * to result buffer */
  for (int i = 0; i < input_size; i++)
  {
    results[offset + i] = vm_read(vm, i);
  }
}
