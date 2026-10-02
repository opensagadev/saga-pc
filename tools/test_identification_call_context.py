"""Finite synthetic controls. No original bodies or paired identities in fixtures."""
import resource
if __name__ == '__main__':
    resource.setrlimit(resource.RLIMIT_AS,(256*1024*1024,)*2)

import hashlib
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('call_context_fixture', Path(__file__).with_name('identification_call_context.py'))
c = importlib.util.module_from_spec(spec)
spec.loader.exec_module(c)


def body(raw, entry=0x1000, side='pc', **extra):
    raw = bytes.fromhex(raw)
    return dict(side=side, entry=entry, bytes=raw.hex(), sha256=hashlib.sha256(raw).hexdigest(), **extra)


def get(raw, **kw):
    return c.features(body(raw, **kw), {}, c.Ledger())


def capture_data():
    return {'schema': 'structural-call-context-capture-v1',
            'functions': [body('8b0083f800c3'), body('8b0983f900c3', entry=0x2000, side='android')]}


CAPSTONE_AVAILABLE = importlib.util.find_spec('capstone') is not None


@unittest.skipUnless(CAPSTONE_AVAILABLE, 'optional Capstone dependency unavailable')
class FeatureControls(unittest.TestCase):
    def test_names_and_labels_cannot_change_observations(self):
        a = get('8b0083f800c3', name='desired', paired_truth='target')
        b = get('8b0083f800c3', name='adversary', paired_truth='wrong')
        self.assertEqual(a, b)

    def test_register_reallocation_is_not_identity(self):
        a, b = get('8b0083f800c3'), get('8b0983f900c3')
        self.assertEqual(a['positive'], b['positive'])

    def test_address_substitution_does_not_change_features(self):
        a, b = get('a100204000c3'), get('a100308000c3', entry=0x5000)
        self.assertEqual(a['positive'], b['positive'])

    def test_field_offsets_are_not_features(self):
        a, b = get('8b4024c3'), get('8b4060c3')
        self.assertEqual(a['positive'], b['positive'])

    def test_lea_is_address_not_memory_read(self):
        self.assertNotIn('memory:read:4', get('8d4024c3')['positive'])
        self.assertIn('memory:read:4', get('8b4024c3')['positive'])

    def test_read_and_write_widths_separate_observed_roles(self):
        a, b = get('0fb600c3'), get('8901c3')
        self.assertIn('memory:read:1', a['positive'])
        self.assertIn('memory:write:4', b['positive'])

    def test_loop_and_multiple_return_positive_patterns(self):
        a = get('85c0740340ebf9c3')
        self.assertIn('CFG:natural-loop', a['positive'])
        b = get('85c07401c3c3')
        self.assertIn('CFG:multiple-returns', b['positive'])

    def test_unknown_indirect_flow_stays_held(self):
        a = get('8b00ffe0')
        self.assertFalse(a['control_capture_qualified'])
        row = c.compare(a, get('8b00c3'), c.Ledger())
        self.assertIn('control-flow-incomplete-observations-conditional', row['holds'])
        self.assertFalse(row['eligible'])

    def test_unknown_indirect_call_not_an_anchor(self):
        a = get('6a01ffd0c3')
        self.assertIn('unresolved-indirect-call', a['debts'])
        self.assertIsNone(a['calls'][0]['anchor'])

    def test_local_push_origin_survives_copy_not_partial_write(self):
        a = get('8b4424048bc850e800000000c3')
        self.assertEqual(a['calls'][0]['push_origins'], ['stack-value'])
        b = get('8b442404b00150e800000000c3')
        self.assertEqual(b['calls'][0]['push_origins'], ['unknown'])

    def test_call_barrier_invalidates_reused_register(self):
        a = get('8b442404e80000000050e800000000c3')
        self.assertEqual(a['calls'][1]['push_origins'], ['unknown'])

    def test_branch_join_cannot_inherit_argument_origin(self):
        a = get('8b44240485c074019050e800000000c3')
        self.assertEqual(a['calls'][0]['push_origins'], ['unknown'])

    def test_anchored_dominance_is_not_linear_address_order(self):
        # one call is bypassable; no positive dominance from first to second.
        r = body('85c07405e800000000e800000000c3')
        a = c.features(r, {0x1009:'r1', 0x100e:'r2'}, c.Ledger())
        self.assertNotIn(('r1','r2'), a['anchored_dominance'])
        b = c.features(body('e800000000e800000000c3'), {0x1005:'r1',0x100a:'r2'}, c.Ledger())
        self.assertIn(('r1','r2'), b['anchored_dominance'])

    def test_repeated_sites_do_not_mint_roots_or_context_support(self):
        a = get('6a01e8000000006a01e800000000c3')
        b = get('6a01e800000000c3')
        row = c.compare(a,b,c.Ledger())
        self.assertEqual(row['shared_local_push_contexts'], [('scalar',)])
        self.assertEqual(row['independent_roots'], 0)

    def test_equal_generic_signatures_remain_tied(self):
        data = capture_data()
        data['functions'].append(body('8b1283fa00c3',entry=0x3000,side='android'))
        rows = c.diagnose(data,c.Ledger())['pairs']
        self.assertEqual(rows[0]['advisory_tuple'], rows[1]['advisory_tuple'])
        self.assertTrue(all(not r['eligible'] for r in rows))

    def test_cohort_identity_cannot_be_external_anchor(self):
        data=capture_data()
        data['external_anchors']=[{'id':'r','pc':0x1000,'android':0x9000,'review_proof_sha256':'a'*64}]
        with self.assertRaisesRegex(c.Guard,'cohort truth'):
            c.diagnose(data,c.Ledger())

    def test_bad_hash_and_decode_extent_fail(self):
        r=body('c3');r['sha256']='0'*64
        with self.assertRaises(c.Guard):c.features(r,{},c.Ledger())
        with self.assertRaises(c.Guard):get('0f')

    def test_unreachable_padding_does_not_mint_flow_debt(self):
        a=get('c3cccc')
        self.assertTrue(a['control_capture_qualified'])
        self.assertEqual(a['debts'],[])
        self.assertFalse(get('cc')['control_capture_qualified'])

    def test_work_guard_fails_instead_of_truncating(self):
        with self.assertRaisesRegex(c.Guard,'work cap'):
            c.diagnose(capture_data(),c.Ledger(work=1))

    def test_partial_xor_mov_and_arithmetic_invalidate_whole_word(self):
        # Incoming EAX holds a locally observed stack-value origin. None of
        # these byte/word writes defines a replacement complete EAX origin.
        for partial in ('30c0', 'b001', '24fe', '0401', '66ffc0', '30e4'):
            with self.subTest(partial=partial):
                a=get('8b442404'+partial+'50e800000000c3')
                self.assertEqual(a['calls'][0]['push_origins'], ['unknown'])

    def test_full_word_copy_does_not_resurrect_partial_source(self):
        a=get('8b44240430c089c15051e800000000c3')
        self.assertEqual(a['calls'][0]['push_origins'], ['unknown','unknown'])
        b=get('8b44240431c050e800000000c3')
        self.assertEqual(b['calls'][0]['push_origins'], ['scalar'])

    def test_lea_pop_arithmetic_and_move_esp_clear_push_window(self):
        for cleanup in ('8d642404', '58', '83c404', '83ec04', '89c4', '66bc0100', 'c9'):
            with self.subTest(cleanup=cleanup):
                a=get('6a01'+cleanup+'e800000000c3')
                self.assertEqual(a['calls'][0]['push_origins'], [])

    def test_cleanup_then_new_push_records_only_new_observation(self):
        a=get('6a018d6424046a02e800000000c3')
        self.assertEqual(a['calls'][0]['push_origins'], ['scalar'])

    def test_unrelated_partial_word_cannot_mint_or_erase_other_origin(self):
        a=get('8b44240430db50e800000000c3')
        self.assertEqual(a['calls'][0]['push_origins'], ['stack-value'])

    def test_decoder_write_failure_is_strict_origin_and_window_barrier(self):
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
        ins=next(md.disasm(bytes.fromhex('30c0'),0x1000))
        real=c.written_gp
        class MissingAccess:
            regs_write=ins.regs_write
            operands=ins.operands
            def regs_access(self):raise ValueError('missing decoder access')
            def reg_name(self,x):return ins.reg_name(x)
        writes,uncertain=real(MissingAccess())
        self.assertIn('eax',writes);self.assertTrue(uncertain)
        def failed(i):
            writes,uncertain=real(i)
            return (writes,True) if i.mnemonic=='nop' else (writes,uncertain)
        with patch.object(c,'written_gp',failed):
            a=get('8b4424046a019050e800000000c3')
        self.assertEqual(a['calls'][0]['push_origins'], ['unknown'])


@unittest.skipUnless(CAPSTONE_AVAILABLE, 'optional Capstone dependency unavailable')
class PublicationControls(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.base=Path(self.temp.name)
        self.input=self.base/'capture.json'
        self.input.write_text(json.dumps(capture_data()))
        self.pin=hashlib.sha256(self.input.read_bytes()).hexdigest()
        self.output=self.base/'result.json'

    def test_pins_and_exclusive_publish(self):
        result=c.run(self.input,self.pin,self.output)
        self.assertEqual(len(result['pairs']),1)
        with self.assertRaises(c.Guard):c.run(self.input,self.pin,self.output)

    def test_wrong_input_pin_has_no_output(self):
        with self.assertRaises(c.Guard):c.run(self.input,'0'*64,self.output)
        self.assertFalse(self.output.exists())

    def test_low_metadata_cap_reads_remaining_plus_one(self):
        ledger=c.Ledger(metadata=1)
        with self.assertRaisesRegex(c.Guard,'metadata cap'):
            c.run(self.input,self.pin,self.output,ledger)
        self.assertEqual(ledger.metadata,2)
        self.assertFalse(self.output.exists())

    def test_input_mutation_during_work_aborts_and_cleans_stage(self):
        original=c.diagnose
        def change(data,ledger):
            result=original(data,ledger)
            self.input.write_text('{}')
            return result
        with patch.object(c,'diagnose',change),self.assertRaises(c.Guard):
            c.run(self.input,self.pin,self.output)
        self.assertFalse(self.output.exists())
        self.assertEqual(list(self.base.glob('.call-context-*')),[])

    def test_producer_mutation_during_final_input_hash_aborts(self):
        producer=self.base/'producer.py';producer.write_text('original')
        real=c.integrity
        count=0
        def integrity(path):
            nonlocal count
            result=real(path)
            if Path(path)==self.input:
                count+=1
                if count==3:producer.write_text('changed')
            return result
        with patch.object(c,'__file__',str(producer)),patch.object(c,'integrity',integrity),self.assertRaises(c.Guard):
            c.run(self.input,self.pin,self.output)
        self.assertFalse(self.output.exists())
        self.assertEqual(list(self.base.glob('.call-context-*')),[])

    def test_output_overflow_fails_without_publication(self):
        with patch.object(c,'diagnose',return_value={'large':'x'*(2*1024*1024)}),self.assertRaisesRegex(c.Guard,'output cap'):
            c.run(self.input,self.pin,self.output)
        self.assertFalse(self.output.exists())

    def test_import_does_not_mutate_resource_limits(self):
        before=resource.getrlimit(resource.RLIMIT_AS)
        new=importlib.util.module_from_spec(spec);spec.loader.exec_module(new)
        self.assertEqual(before,resource.getrlimit(resource.RLIMIT_AS))


if __name__=='__main__':
    resource.setrlimit(resource.RLIMIT_AS,(256*1024*1024,)*2)
    unittest.main()
