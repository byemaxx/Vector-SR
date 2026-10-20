#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>

#include "trace_marker_fds.h"

using namespace vector::native::module;
#define CHECK(condition) do { if (!(condition)) { \
    std::cerr << "Failed at line " << __LINE__ << ": " #condition << '\n'; \
    std::exit(EXIT_FAILURE); } } while (false)

int main() {
    CHECK(IsTraceMarkerPath("/sys/kernel/tracing/trace_marker"));
    CHECK(IsTraceMarkerPath("/sys/kernel/debug/tracing/trace_marker"));
    CHECK(!IsTraceMarkerPath("/tmp/trace_marker"));
    CHECK(!IsTraceMarkerPath("/sys/kernel/tracing/trace_marker_backup"));
    CHECK(!IsTraceMarkerPath("/sys/kernel/tracing/trace_marker (deleted)"));

    char directory_template[] = "/tmp/vector-trace-fds-XXXXXX";
    char *directory = mkdtemp(directory_template);
    CHECK(directory != nullptr);
    const std::filesystem::path root(directory);
    const auto proc = root / "fd";
    std::filesystem::create_directory(proc);
    const auto backing = root / "marker";
    const int marker = open(backing.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    const int debug_marker = open(backing.c_str(), O_WRONLY | O_CLOEXEC);
    const int readonly_marker = open(backing.c_str(), O_RDONLY | O_CLOEXEC);
    int pipe_fds[2];
    CHECK(marker >= 0 && debug_marker >= 0 && readonly_marker >= 0 && pipe(pipe_fds) == 0);
    // Procfs link fixture: actual open FDs, simulated kernel tracing path names.
    auto link = [&](std::string name, const char *target) {
        CHECK(symlink(target, (proc / name).c_str()) == 0);
    };
    link(std::to_string(marker), "/sys/kernel/tracing/trace_marker");
    link(std::to_string(debug_marker), "/sys/kernel/debug/tracing/trace_marker");
    link(std::to_string(readonly_marker), "/sys/kernel/tracing/trace_marker");
    link(std::to_string(pipe_fds[1]), "pipe:[123]");
    link("100junk", "/sys/kernel/tracing/trace_marker");
    link("-1", "/sys/kernel/tracing/trace_marker");
    link("999999999999999999999", "/sys/kernel/tracing/trace_marker");
    link("2147483646", "/sys/kernel/tracing/trace_marker"); // closed FD: skip

    std::set<int> exemptions;
    int failures = 0;
    auto pre_callback = [&](bool skip_injection) {
        const TraceMarkerFdGuard guard(
            [&](int fd) { exemptions.insert(fd); return true; }, [&] { ++failures; }, proc.c_str());
        errno = EDOM;
        if (skip_injection) return;
        // Model unrelated initialization; the guard must run on the normal path too.
    };
    pre_callback(true);
    CHECK(errno == EDOM && failures == 0);
    CHECK(exemptions == std::set<int>({marker, debug_marker}));
    exemptions.clear();
    pre_callback(false);
    CHECK(errno == EDOM && failures == 0);
    CHECK(exemptions == std::set<int>({marker, debug_marker}));
    CHECK(fcntl(pipe_fds[1], F_GETFD) >= 0 && fcntl(readonly_marker, F_GETFD) >= 0);

    // Model loader cleanup using the exemptions actually discovered by production code.
    // A subsequent pipe cannot reuse a preserved writable marker descriptor.
    for (int fd : {marker, debug_marker, readonly_marker}) {
        if (!exemptions.contains(fd)) CHECK(close(fd) == 0);
    }
    CHECK(fcntl(marker, F_GETFD) >= 0 && fcntl(debug_marker, F_GETFD) >= 0);
    int later_pipe[2];
    CHECK(pipe(later_pipe) == 0);
    CHECK(later_pipe[0] != marker && later_pipe[1] != marker);
    CHECK(later_pipe[0] != debug_marker && later_pipe[1] != debug_marker);

    CHECK(!PreserveTraceMarkerFds([](int) { return false; }, proc.c_str()));
    {
        const TraceMarkerFdGuard rejected([](int) { return false; }, [&] { ++failures; }, proc.c_str());
    }
    CHECK(failures == 1);
    CHECK(!PreserveTraceMarkerFds([](int) { return true; }, "/nonexistent/vector-fds"));
    // Also exercise real procfs; ordinary files and pipes must not be exempted.
    std::set<int> real_exemptions;
    CHECK(PreserveTraceMarkerFds([&](int fd) { real_exemptions.insert(fd); return true; }));
    CHECK(!real_exemptions.contains(marker) && !real_exemptions.contains(pipe_fds[1]));

    for (int fd : {marker, debug_marker, pipe_fds[0], pipe_fds[1], later_pipe[0], later_pipe[1]}) close(fd);
    std::filesystem::remove_all(root);
    std::cout << "trace FD: selective preservation, early return, normal return, API failure, "
                 "closed/read-only FDs, loader cleanup and real procfs passed\n";
}
