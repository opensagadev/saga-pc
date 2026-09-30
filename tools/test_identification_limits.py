"""Tiny subprocess checks for bounded, serialized identification work."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

from tools import identification_limits as limits


class IdentificationLimitsTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def child(self, body):
        prefix = """import json, resource, sys
from pathlib import Path
from tools import identification_limits as limits
limits.lib.ROOT = Path(sys.argv[1])
"""
        return subprocess.run([sys.executable, "-c", prefix + body, str(self.root)],
                              capture_output=True, text=True, timeout=10, check=True)

    def test_allocation_above_128_mib_is_rejected_and_previous_caps_restored(self):
        result = self.child("""
previous = resource.getrlimit(resource.RLIMIT_AS)
try:
    with limits.identification_budget(128):
        oversized = bytearray(160 * 1024 * 1024)
except ValueError as error:
    assert '128 MiB' in str(error), str(error)
else:
    raise AssertionError('oversized allocation unexpectedly succeeded')
assert resource.getrlimit(resource.RLIMIT_AS) == previous
print(json.dumps({'rejected': True, 'restored': True}))
""")
        self.assertEqual(json.loads(result.stdout), {"rejected": True, "restored": True})

    def test_existing_lower_memory_cap_is_respected_and_restored(self):
        self.child("""
original = resource.getrlimit(resource.RLIMIT_AS)
soft = 96 * 1024 * 1024
if original[0] != resource.RLIM_INFINITY:
    soft = min(soft, original[0])
if original[1] != resource.RLIM_INFINITY:
    soft = min(soft, original[1])
resource.setrlimit(resource.RLIMIT_AS, (soft, original[1]))
with limits.identification_budget(128):
    assert resource.getrlimit(resource.RLIMIT_AS) == (soft, original[1])
assert resource.getrlimit(resource.RLIMIT_AS) == (soft, original[1])
resource.setrlimit(resource.RLIMIT_AS, original)
""")

    def test_normal_and_exceptional_exits_restore_previous_resource_caps(self):
        self.child("""
previous = resource.getrlimit(resource.RLIMIT_AS)
with limits.identification_budget(128):
    assert resource.getrlimit(resource.RLIMIT_AS)[0] <= 128 * 1024 * 1024
assert resource.getrlimit(resource.RLIMIT_AS) == previous
try:
    with limits.identification_budget(128):
        raise RuntimeError('ordinary failed work')
except RuntimeError:
    pass
assert resource.getrlimit(resource.RLIMIT_AS) == previous
""")

    def test_competing_job_fails_before_entering_work_and_lock_is_released(self):
        body = """
@limits.bounded_identification
def fake_work():
    (limits.lib.ROOT / 'entered-work').write_text('unexpected')
try:
    fake_work(memory_limit_mb=128)
except ValueError as error:
    assert 'Another identification job' in str(error)
else:
    raise AssertionError('competing identification entered work')
"""
        with patch.object(limits.lib, "ROOT", self.root):
            with limits.identification_budget(128):
                self.child(body)
            with limits.identification_budget(128):
                pass
        self.assertFalse((self.root / "entered-work").exists())

    def test_invalid_limits_are_rejected_before_creating_job_state(self):
        with patch.object(limits.lib, "ROOT", self.root):
            for value in (127, 0, -1, True, 128.0, "128"):
                with self.subTest(value=value):
                    with self.assertRaisesRegex(ValueError, "at least 128"):
                        with limits.identification_budget(value):
                            self.fail("invalid limit entered work")
        self.assertFalse((self.root / "build").exists())

    def test_lock_only_leaves_address_space_caps_unchanged(self):
        self.child("""
previous = resource.getrlimit(resource.RLIMIT_AS)
with limits.identification_lock():
    assert resource.getrlimit(resource.RLIMIT_AS) == previous
    try:
        with limits.identification_budget(128):
            raise AssertionError('budget entered a slot owned by the lock')
    except ValueError as error:
        assert 'Another identification job' in str(error)
assert resource.getrlimit(resource.RLIMIT_AS) == previous
with limits.identification_budget(128):
    pass
""")


if __name__ == "__main__":
    unittest.main()
