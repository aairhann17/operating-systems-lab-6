#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 256
#define PAGE_COUNT 256
#define TLB_SIZE 16
#define FRAME_COUNT_BONUS 128
#define PHYSICAL_MEMORY_SIZE_BONUS (FRAME_COUNT_BONUS * PAGE_SIZE)

static int tlb_lookup(const int tlb_pages[TLB_SIZE],
                      const int tlb_frames[TLB_SIZE],
                      int page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb_pages[i] == page_number) {
            return tlb_frames[i];
        }
    }
    return -1;
}

static void tlb_invalidate_page(int tlb_pages[TLB_SIZE],
                                int tlb_frames[TLB_SIZE],
                                int page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb_pages[i] == page_number) {
            tlb_pages[i] = -1;
            tlb_frames[i] = -1;
        }
    }
}

static void tlb_insert(int tlb_pages[TLB_SIZE],
                       int tlb_frames[TLB_SIZE],
                       int *tlb_next_index,
                       int page_number,
                       int frame_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb_pages[i] == page_number) {
            tlb_frames[i] = frame_number;
            return;
        }
    }

    tlb_pages[*tlb_next_index] = page_number;
    tlb_frames[*tlb_next_index] = frame_number;
    *tlb_next_index = (*tlb_next_index + 1) % TLB_SIZE;
}

int main(int argc, char *argv[]) {
    const char *backing_store_path = "BACKING_STORE.bin";
    if (argc >= 2) {
        backing_store_path = argv[1];
    }

    FILE *backing_store_file = fopen(backing_store_path, "rb");
    if (backing_store_file == NULL) {
        fprintf(stderr, "Error: could not open backing store file: %s\n", backing_store_path);
        return 1;
    }

    int page_table[PAGE_COUNT];
    int frame_page[FRAME_COUNT_BONUS];
    signed char physical_memory[PHYSICAL_MEMORY_SIZE_BONUS];

    int tlb_pages[TLB_SIZE];
    int tlb_frames[TLB_SIZE];
    int tlb_next_index = 0;

    int next_free_frame = 0;
    int next_victim_frame = 0;

    int translated_count = 0;
    int page_fault_count = 0;
    int tlb_hit_count = 0;
    int replacement_count = 0;

    for (int i = 0; i < PAGE_COUNT; i++) {
        page_table[i] = -1;
    }
    for (int i = 0; i < FRAME_COUNT_BONUS; i++) {
        frame_page[i] = -1;
    }
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb_pages[i] = -1;
        tlb_frames[i] = -1;
    }

    char line[128];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        char *endptr = NULL;
        long logical_address_raw = strtol(line, &endptr, 10);
        if (endptr == line) {
            continue;
        }

        int virtual_address = (int)(((uint32_t)logical_address_raw) & 0xFFFFu);
        int page_number = (virtual_address >> 8) & 0xFF;
        int offset = virtual_address & 0xFF;

        int frame_number = tlb_lookup(tlb_pages, tlb_frames, page_number);
        if (frame_number != -1) {
            tlb_hit_count++;
        } else {
            frame_number = page_table[page_number];
        }

        if (frame_number == -1) {
            page_fault_count++;

            if (next_free_frame < FRAME_COUNT_BONUS) {
                frame_number = next_free_frame;
                next_free_frame++;
            } else {
                frame_number = next_victim_frame;
                next_victim_frame = (next_victim_frame + 1) % FRAME_COUNT_BONUS;

                int victim_page = frame_page[frame_number];
                if (victim_page != -1) {
                    page_table[victim_page] = -1;
                    tlb_invalidate_page(tlb_pages, tlb_frames, victim_page);
                }
                replacement_count++;
            }

            long seek_position = (long)page_number * PAGE_SIZE;
            if (fseek(backing_store_file, seek_position, SEEK_SET) != 0) {
                fprintf(stderr, "Error: failed to seek backing store for page %d\n", page_number);
                fclose(backing_store_file);
                return 1;
            }

            size_t read_count = fread(&physical_memory[frame_number * PAGE_SIZE],
                                      1,
                                      PAGE_SIZE,
                                      backing_store_file);
            if (read_count != PAGE_SIZE) {
                fprintf(stderr, "Error: failed to read full page %d from backing store\n", page_number);
                fclose(backing_store_file);
                return 1;
            }

            page_table[page_number] = frame_number;
            frame_page[frame_number] = page_number;
            tlb_insert(tlb_pages, tlb_frames, &tlb_next_index, page_number, frame_number);
        } else {
            tlb_insert(tlb_pages, tlb_frames, &tlb_next_index, page_number, frame_number);
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
        printf("Page Replacements = %d\n", replacement_count);
    }

    fclose(backing_store_file);
    return 0;
}
