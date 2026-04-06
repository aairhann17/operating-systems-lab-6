#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "group29_manager.h"

#define PAGE_SIZE 256
#define PAGE_COUNT 256
#define FRAME_COUNT 256
#define PHYSICAL_MEMORY_SIZE (FRAME_COUNT * PAGE_SIZE)
#define TLB_SIZE 16

int run_lab6(const char *backing_store_path) {
    int page_table[PAGE_COUNT];
    signed char physical_memory[PHYSICAL_MEMORY_SIZE];
    int tlb_pages[TLB_SIZE];
    int tlb_frames[TLB_SIZE];
    int tlb_next_index = 0;
    int next_free_frame = 0;
    int translated_count = 0;
    int page_fault_count = 0;
    int tlb_hit_count = 0;

    FILE *backing_store_file = fopen(backing_store_path, "rb");
    if (backing_store_file == NULL) {
        fprintf(stderr, "Error: could not open backing store file: %s\n", backing_store_path);
        return 1;
    }

    for (int i = 0; i < PAGE_COUNT; i++) {
        page_table[i] = -1;
    }

    for (int i = 0; i < TLB_SIZE; i++) {
        tlb_pages[i] = -1;
        tlb_frames[i] = -1;
    }

    memset(physical_memory, 0, sizeof(physical_memory));

    char line[128];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        char *endptr = NULL;
        long logical_address_raw = strtol(line, &endptr, 10);
        if (endptr == line) {
            continue;
        }

        /* Keep only the low 16 bits of each logical address as required. */
        int virtual_address = (int)(((uint32_t)logical_address_raw) & 0xFFFFu);
        int page_number = (virtual_address >> 8) & 0xFF;
        int offset = virtual_address & 0xFF;
        int frame_number = -1;

        for (int i = 0; i < TLB_SIZE; i++) {
            if (tlb_pages[i] == page_number) {
                frame_number = tlb_frames[i];
                tlb_hit_count++;
                break;
            }
        }

        if (frame_number == -1) {
            frame_number = page_table[page_number];
        }

        if (frame_number == -1) {
            page_fault_count++;
            if (next_free_frame >= FRAME_COUNT) {
                fprintf(stderr, "Error: out of free frames; page replacement not implemented.\n");
                fclose(backing_store_file);
                return 1;
            }

            long seek_position = (long)page_number * PAGE_SIZE;
            if (fseek(backing_store_file, seek_position, SEEK_SET) != 0) {
                fprintf(stderr, "Error: failed to seek backing store for page %d\n", page_number);
                fclose(backing_store_file);
                return 1;
            }

            size_t read_count = fread(&physical_memory[next_free_frame * PAGE_SIZE],
                                      1,
                                      PAGE_SIZE,
                                      backing_store_file);
            if (read_count != PAGE_SIZE) {
                fprintf(stderr, "Error: failed to read full page %d from backing store\n", page_number);
                fclose(backing_store_file);
                return 1;
            }

            page_table[page_number] = next_free_frame;
            frame_number = next_free_frame;
            next_free_frame++;

            tlb_pages[tlb_next_index] = page_number;
            tlb_frames[tlb_next_index] = frame_number;
            tlb_next_index = (tlb_next_index + 1) % TLB_SIZE;
        } else {
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

        int physical_address = (frame_number * PAGE_SIZE) + offset;
        signed char value = physical_memory[physical_address];

        printf("Virtual address: %d Physical address: %d Value: %d\n",
               virtual_address,
               physical_address,
               (int)value);
        translated_count++;
    }

    if (translated_count > 0) {
        double page_fault_rate = (double)page_fault_count / (double)translated_count;
        double tlb_hit_rate = (double)tlb_hit_count / (double)translated_count;
        printf("Page Fault Rate = %.3f\n", page_fault_rate);
        printf("TLB Hit Rate = %.3f\n", tlb_hit_rate);
    }

    fclose(backing_store_file);
    return 0;
}
