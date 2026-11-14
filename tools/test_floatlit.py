import unittest

from tools.floatlit import rewrite, shortest


class FloatLiteralTests(unittest.TestCase):
    def test_float_literals_take_their_shortest_spelling(self):
        self.assertEqual(shortest("0.10000000149011612", "f"), "0.1")
        self.assertEqual(shortest("0.800000011920929", "f"), "0.8")
        self.assertEqual(shortest("1.3333333730697632", "f"), "1.3333334")
        self.assertEqual(shortest("0.10000000000000001", ""), "0.1")
        # 3.4028235e38 is above FLT_MAX as a double, which VC8 rejects
        self.assertEqual(shortest("3.4028234663852886e+38", "f"), "3.402823466e38")  # float.h spells FLT_MAX the same way

    def test_values_that_would_change_keep_their_spelling(self):
        self.assertIsNone(shortest("1e-10", "f"))  # not a float32 value as written
        self.assertIsNone(shortest("0.5", "f"))  # already shortest

    def test_comments_and_strings_are_left_alone(self):
        source = 'x = 0.10000000149011612f; /* 0.10000000149011612f */ s = "0.25000f";'
        new, changes = rewrite(source)
        self.assertEqual(new, 'x = 0.1f; /* 0.10000000149011612f */ s = "0.25000f";')
        self.assertEqual(len(changes), 1)


if __name__ == "__main__":
    unittest.main()
