
#include "conduit/internal/core/sparseSet.h"
#include "conduit/logging.h"
using namespace cndt;

int main(int argc, char **argv) {

    internal::SparseSet<int, int> test;

    test[20] = 5;
    test[25] = 7;
    test[26] = 9;
    test[27] = 6;

    for (auto k : test) {
        log::core::info("Key: {}, Value: {} - {}", k.first, k.second, test[k.first]);
    }

    test.erase(25);

    for (auto k : test) {
        log::core::info("Key: {}, Value: {} - {}", k.first, k.second, test[k.first]);
    }

    test[25] = 1;

    for (auto k : test) {
        log::core::info("Key: {}, Value: {} - {}", k.first, k.second, test[k.first]);
    }
}
