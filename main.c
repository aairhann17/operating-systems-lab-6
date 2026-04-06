#include <stdio.h>
#include "group29_manager.h"

int main(int argc, char *argv[]) {
    const char *backing_store_path = "BACKING_STORE.bin";

    if (argc >= 2) {
        backing_store_path = argv[1];
    }

    return run_lab6(backing_store_path);
}
