"""Serialize identification work and bound its address space on Linux."""
from contextlib import contextmanager
import fcntl
from functools import wraps
import resource

if __package__:
    from . import lib
else:
    import lib

DEFAULT_MEMORY_MB = 1024


@contextmanager
def identification_lock():
    """Reserve the shared heavy-job slot without limiting child address space."""
    directory = lib.ROOT / "build/analysis"
    directory.mkdir(parents=True, exist_ok=True)
    with (directory / ".identification-heavy.lock").open("a+") as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as error:
            raise ValueError("Another identification job is running; full graph jobs must run sequentially") from error
        yield


@contextmanager
def identification_budget(memory_mb=DEFAULT_MEMORY_MB):
    if isinstance(memory_mb, bool) or not isinstance(memory_mb, int) or memory_mb < 128:
        raise ValueError("Identification memory limit must be at least 128 MiB")
    with identification_lock():
        previous = resource.getrlimit(resource.RLIMIT_AS)
        limit = memory_mb * 1024 * 1024
        if previous[0] != resource.RLIM_INFINITY:
            limit = min(limit, previous[0])
        if previous[1] != resource.RLIM_INFINITY:
            limit = min(limit, previous[1])
        try:
            resource.setrlimit(resource.RLIMIT_AS, (limit, previous[1]))
            yield
        except MemoryError as error:
            raise ValueError(f"Identification exceeded its {memory_mb} MiB address-space limit; no completed run was published") from error
        finally:
            resource.setrlimit(resource.RLIMIT_AS, previous)


def bounded_identification(function):
    @wraps(function)
    def bounded(*args, memory_limit_mb=DEFAULT_MEMORY_MB, **kwargs):
        with identification_budget(memory_limit_mb):
            return function(*args, **kwargs)
    return bounded
