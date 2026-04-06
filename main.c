#include <stdio.h>
#include "group29_manager.h"

/*
 * main.c
 * ------
 * Entry point for the required Lab 6 virtual memory manager.
 *
 * The assignment expects the program to read logical addresses from standard
 * input. We optionally allow the backing-store path to be passed on the command
 * line; otherwise, the default file name BACKING_STORE.bin is used.
 */
int main(int argc, char *argv[]) {
    /* Default backing store file expected by the lab. */
    const char *backing_store_path = "BACKING_STORE.bin";

    /*
     * If the user supplies a file path, use it instead. This makes the program
     * easier to test without changing the core logic.
     */
    if (argc >= 2) {
        backing_store_path = argv[1];
    }

    /* Hand off all translation work to the lab implementation. */
    return run_lab6(backing_store_path);
}
