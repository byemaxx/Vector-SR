#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#include <cerrno>
#include <charconv>
#include <string_view>
#include <utility>

namespace vector::native::module {

inline bool IsTraceMarkerPath(std::string_view path) {
    return path == "/sys/kernel/tracing/trace_marker" ||
           path == "/sys/kernel/debug/tracing/trace_marker";
}

// Module loading itself can initialize the linker's cached trace FD before preAppSpecialize.
// Preserve only actual writable trace_marker descriptors, never a fixed descriptor number.
// fd_directory is injectable so host tests can model procfs links using real open descriptors.
template <typename ExemptFd>
bool PreserveTraceMarkerFds(ExemptFd exempt_fd, const char *fd_directory = "/proc/self/fd") {
    DIR *directory = opendir(fd_directory);
    if (!directory) return false;
    const int directory_fd = dirfd(directory);
    bool success = true;
    for (;;) {
        errno = 0;
        dirent *entry = readdir(directory);
        if (!entry) {
            if (errno != 0) success = false;
            break;
        }
        const std::string_view name(entry->d_name);
        int fd = -1;
        const auto [end, error] = std::from_chars(name.data(), name.data() + name.size(), fd);
        if (error != std::errc{} || end != name.data() + name.size() || fd < 0 ||
            fd == directory_fd) continue;

        char target[PATH_MAX];
        const ssize_t length = readlinkat(directory_fd, entry->d_name, target, sizeof(target));
        if (length < 0) {
            if (errno != ENOENT) success = false;
            continue;
        }
        if (!IsTraceMarkerPath(std::string_view(target, static_cast<size_t>(length)))) continue;
        const int flags = fcntl(fd, F_GETFL);
        if (flags < 0) {
            if (errno != EBADF) success = false;
            continue;
        }
        if ((flags & O_ACCMODE) == O_RDONLY) continue;
        if (!exempt_fd(fd)) success = false;
    }
    if (closedir(directory) != 0) success = false;
    return success;
}

// Run at every exit from the pre callback, including skipped and failed injection paths.
template <typename ExemptFd, typename OnFailure>
class TraceMarkerFdGuard {
public:
    TraceMarkerFdGuard(ExemptFd exempt_fd, OnFailure on_failure,
                       const char *fd_directory = "/proc/self/fd")
        : exempt_fd_(std::move(exempt_fd)), on_failure_(std::move(on_failure)),
          fd_directory_(fd_directory) {}
    TraceMarkerFdGuard(const TraceMarkerFdGuard &) = delete;
    TraceMarkerFdGuard &operator=(const TraceMarkerFdGuard &) = delete;
    ~TraceMarkerFdGuard() {
        const int saved_errno = errno;
        if (!PreserveTraceMarkerFds(exempt_fd_, fd_directory_)) on_failure_();
        errno = saved_errno;
    }

private:
    ExemptFd exempt_fd_;
    OnFailure on_failure_;
    const char *fd_directory_;
};

}  // namespace vector::native::module
