# Trace-marker FD preservation and native log bounds

During app specialization, a Zygisk loader can snapshot the inherited descriptors, load
modules and run their pre callbacks, then close newly opened descriptors that were not
exempted. Loading a module can initialize bionic/linker's cached `trace_marker` FD in
that interval. Closing the FD without updating the cache leaves a stale descriptor
number, which may later be reused by an unrelated pipe.

Vector now scans its open descriptors when `preAppSpecialize` returns, including early
returns. Only writable descriptors whose procfs targets exactly match
`/sys/kernel/tracing/trace_marker` or `/sys/kernel/debug/tracing/trace_marker` are
passed to Zygisk's `exemptFd` API. It preserves descriptors opened by the loader before
Vector's callback as well as those opened during the callback. No descriptor number
is hardcoded, and unrelated files and pipes are not exempted or closed. Rejected
exemptions and discovery failures produce a warning. This is one startup scan, not
a background monitor.

This is a Vector-side defense for loaders honoring the exemption API. It does not
repair a cache that already points to a pipe, handle independent FD closes after this
callback, or establish which component caused a historical incident. The system-server
callback is not changed: the public exemption API is intended for pre-app specialization.
Activation requires newly specialized app processes after installing the new module;
existing bad application processes are not repaired by replacing a ZIP on disk.

The common native logger also now writes the terminating NUL through `format_to_n`'s
output iterator. Its `size` field reports the full formatted length, including truncated
characters, and must not be used as a buffer index. Messages remain truncated to 1023
bytes. The old implementation has a reproducible stack-buffer overflow at 1024 bytes;
that defect has not been established as the cause of the trace-FD incident.

## Validation

Run `bash tests/run-native-regressions.sh` on Linux/WSL with clang++ and the existing fmt
submodule. The script builds isolated ASan/UBSan host tests in a temporary directory:

- Empty, 1023-byte, 1024-byte, 8192-byte and multi-argument log messages, checking actual
  emitted text and its bounds using the production logging header.
- Both accepted trace paths, invalid numeric names, closed/read-only descriptors,
  unrelated pipes and lookalike paths.
- Guard execution on normal and early returns, errno preservation, rejected exemptions
  and missing proc directories.
- A procfs-link fixture with real open FDs, followed by loader-style cleanup and a new
  pipe, checking that the preserved marker FD cannot be reused by that pipe.
- An actual `/proc/self/fd` scan that rejects ordinary host files and pipes.

The fixture simulates kernel tracing path names; it does not reproduce Android's
complete app specialization or a real hprof storm. Android native builds and the Core
workflow supply compilation/integration coverage. A fresh-process device capture is
still required to verify the original close/reuse sequence and sustained incident
prevention on a particular ROM/loader.
