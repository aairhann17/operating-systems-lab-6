#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "group29_manager.h"

/*
 * group29_manager.c
 * -----------------
 * Main solution for the virtual memory manager lab.
 *
 * Design summary:
 * 1. Read each logical address from standard input.
 * 2. Keep only the low 16 bits, because the simulated address space is 16-bit.
 * 3. Split the address into an 8-bit page number and 8-bit offset.
 * 4. Search the TLB first for a fast translation.
 * 5. If the TLB misses, consult the page table.
 * 6. If the page is not in memory, service a page fault by loading the page
 *    from BACKING_STORE.bin into the next free physical frame.
 * 7. Print the translated physical address and signed byte value.
 * 8. At the end, print the page-fault rate and TLB-hit rate.
 */

/* System parameters given by the assignment. */
#define PAGE_SIZE 256
#define PAGE_COUNT 256
#define FRAME_COUNT 256
#define PHYSICAL_MEMORY_SIZE (FRAME_COUNT * PAGE_SIZE)
#define TLB_SIZE 16

/*
 * main
 * ----
 * Program entry point. The lab input is read from standard input, and the
 * backing-store file defaults to BACKING_STORE.bin unless a custom path is
 * supplied as the first command-line argument.
 */
int main(int argc, char *argv[]) {
    const char *backing_store_path = "BACKING_STORE.bin";

    if (argc >= 2) {
        backing_store_path = argv[1];
    }

    return run_lab6(backing_store_path);
}

int run_lab6(const char *backing_store_path) {
    /*
     * page_table[page] = frame number for that page, or -1 if the page has not
     * been loaded into physical memory yet.
     */
    int page_table[PAGE_COUNT];

    /* Simulated physical memory: 256 frames * 256 bytes each. */
    signed char physical_memory[PHYSICAL_MEMORY_SIZE];

    /*
     * TLB is stored as parallel arrays. Each entry maps a page number to a
     * frame number. A value of -1 means the entry is unused.
     */
    int tlb_pages[TLB_SIZE];
    int tlb_frames[TLB_SIZE];

    /* FIFO index for the next TLB entry to overwrite. */
    int tlb_next_index = 0;

    /* Next free frame in physical memory. */
    int next_free_frame = 0;

    /* Counters used for the required statistics. */
    int translated_count = 0;
    int page_fault_count = 0;
    int tlb_hit_count = 0;

    /* Open the backing store binary in read-only mode. */
    FILE *backing_store_file = fopen(backing_store_path, "rb");
    if (backing_store_file == NULL) {
        fprintf(stderr, "Error: could not open backing store file: %s\n", backing_store_path);
        return 1;
    }

    /* Initially, no logical page is loaded in memory. */
    for (int i = 0; i < PAGE_COUNT; i++) {
        page_table[i] = -1;
    }

    /* Initially, the TLB is empty. */
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb_pages[i] = -1;
        tlb_frames[i] = -1;
    }

    /* Clear simulated memory to keep the initial state predictable. */
    memset(physical_memory, 0, sizeof(physical_memory));

    /* Read one logical address per line from standard input. */
    char line[128];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        char *endptr = NULL;
        long logical_address_raw = strtol(line, &endptr, 10);

        /* Skip lines that do not begin with an integer. */
        if (endptr == line) {
            continue;
        }

        /*
         * The lab specifies that only the lower 16 bits of the input address are
         * part of the logical address space.
         */
        int virtual_address = (int)(((uint32_t)logical_address_raw) & 0xFFFFu);

        /* High 8 bits = page number, low 8 bits = offset within the page. */
        int page_number = (virtual_address >> 8) & 0xFF;
        int offset = virtual_address & 0xFF;

        /* We will determine the correct frame number for this page. */
        int frame_number = -1;

        /* Step 1: Search the TLB for a fast translation. */
        for (int i = 0; i < TLB_SIZE; i++) {
            if (tlb_pages[i] == page_number) {
                frame_number = tlb_frames[i];
                tlb_hit_count++;
                break;
            }
        }

        /* Step 2: If the TLB missed, consult the page table. */
        if (frame_number == -1) {
            frame_number = page_table[page_number];
        }

        /*
         * Step 3: If the page is still not mapped, then we have a page fault and
         * must load the page from the backing store into the next free frame.
         */
        if (frame_number == -1) {
            page_fault_count++;

            /* The required version assumes enough free frames for all pages. */
            if (next_free_frame >= FRAME_COUNT) {
                fprintf(stderr, "Error: out of free frames; page replacement not implemented.\n");
                fclose(backing_store_file);
                return 1;
            }

            /* Each page occupies PAGE_SIZE bytes in the backing store file. */
            long seek_position = (long)page_number * PAGE_SIZE;
            if (fseek(backing_store_file, seek_position, SEEK_SET) != 0) {
                fprintf(stderr, "Error: failed to seek backing store for page %d\n", page_number);
                fclose(backing_store_file);
                return 1;
            }

            /* Load exactly one page from disk into the next free frame. */
            size_t read_count = fread(&physical_memory[next_free_frame * PAGE_SIZE],
                                      1,
                                      PAGE_SIZE,
                                      backing_store_file);
            if (read_count != PAGE_SIZE) {
                fprintf(stderr, "Error: failed to read full page %d from backing store\n", page_number);
                fclose(backing_store_file);
                return 1;
            }

            /* Update the page table to remember where the page now lives. */
            page_table[page_number] = next_free_frame;
            frame_number = next_free_frame;
            next_free_frame++;

            /* Also insert the new translation into the TLB using FIFO replacement. */
            tlb_pages[tlb_next_index] = page_number;
            tlb_frames[tlb_next_index] = frame_number;
            tlb_next_index = (tlb_next_index + 1) % TLB_SIZE;
        } else {
            /*
             * If the page table had the page but the TLB did not, insert the
             * translation into the TLB so future accesses may hit there.
             */
            bool tlb_contains_page = false;
            for (int i = 0; i < TLB_SIZE; i++) {
                if (tlb_pages[i] == page_number) {
                    tlb_contains_page = true;
                    break;
                }
            }

            if (!tlb_contains_page) {
                tlb_pages[tlb_next_index] = page_number;
                tlb_frames[tlb_next_index] = frame_number;
                tlb_next_index = (tlb_next_index + 1) % TLB_SIZE;
            }
        }

        /* Build the final physical address from the frame number and offset. */
        int physical_address = (frame_number * PAGE_SIZE) + offset;

        /* Read the signed byte stored at that physical memory location. */
        signed char value = physical_memory[physical_address];

        /* Print output in the exact style expected by the lab. */
        printf("Virtual address: %d Physical address: %d Value: %d\n",
               virtual_address,
               physical_address,
               (int)value);
        translated_count++;
    }

    /* Print the required summary statistics after all addresses are processed. */
    if (translated_count > 0) {
        double page_fault_rate = (double)page_fault_count / (double)translated_count;
        double tlb_hit_rate = (double)tlb_hit_count / (double)translated_count;
        printf("Page Fault Rate = %.3f\n", page_fault_rate);
        printf("TLB Hit Rate = %.3f\n", tlb_hit_rate);
    }

    fclose(backing_store_file);
    return 0;
}
