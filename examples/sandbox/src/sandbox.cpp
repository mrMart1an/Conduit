#include "conduit/logging.h"

using namespace cndt;

int main(int argc, char **argv) {
    log::app::info("Test sandbox: {}", "Hello world");
}
