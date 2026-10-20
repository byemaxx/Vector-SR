#include <cstdlib>
#include <iostream>
#include <string>

#include "common/logging.h"

static std::string last_log;
int __android_log_write(int, const char *, const char *message) {
    last_log = message;
    return 0;
}

int main() {
    for (size_t length : {size_t(0), size_t(1), size_t(1023), size_t(1024), size_t(8192)}) {
        const std::string message(length, 'x');
        LOGE("{}", message);
        if (last_log != message.substr(0, 1023)) {
            std::cerr << "Incorrect log truncation for length " << length << '\n';
            return EXIT_FAILURE;
        }
    }
    LOGW("{} {}", std::string(1023, 'a'), std::string(4096, 'b'));
    if (last_log != std::string(1023, 'a')) return EXIT_FAILURE;
    std::cout << "logging: empty, boundary, oversized and multi-argument messages passed\n";
}
