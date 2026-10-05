#include "saga.h"
#include <intrin.h>
#include <excpt.h>

/* Partial supported-platform reconstruction: CPUID must be available. Native
   EFLAGS.ID probing, its side effect and the no-support path remain omitted. */
extern "C" void __cdecl CpuDetectFeaturesPc(void) {
    i32 registers[4];
    STUBBED_NAMED(0x006e5f93); /* Native EFLAGS.ID toggle and unavailable-CPUID path. */
    __cpuid(registers, 0);
    CpuHasCpuidPc = 1;
    CpuBasicLeafMaxPc = (u32)registers[0];
    CpuVendorNamePc.word0 = (u32)registers[1];
    CpuVendorNamePc.word1 = (u32)registers[3];
    CpuVendorNamePc.word2 = (u32)registers[2];
    if (CpuVendorNamePc.word0 == CpuVendorIntelPc.word0 && CpuVendorNamePc.word1 == CpuVendorIntelPc.word1 &&
        CpuVendorNamePc.word2 == CpuVendorIntelPc.word2) {
        CpuVendorPc = 1;
    } else if (CpuVendorNamePc.word0 == CpuVendorAmdPc.word0 && CpuVendorNamePc.word1 == CpuVendorAmdPc.word1 &&
               CpuVendorNamePc.word2 == CpuVendorAmdPc.word2) {
        CpuVendorPc = 2;
    } else if (CpuVendorNamePc.word0 == CpuVendorCyrixPc.word0 && CpuVendorNamePc.word1 == CpuVendorCyrixPc.word1 &&
               CpuVendorNamePc.word2 == CpuVendorCyrixPc.word2) {
        CpuVendorPc = 3;
    } else {
        CpuVendorPc = 0;
    }
    if (CpuBasicLeafMaxPc != 0) {
        __cpuid(registers, 1);
        u32 signature = (u32)registers[0];
        u32 features = (u32)registers[3];
        CpuProcessorTypePc = (signature & 0x3000) >> 12;
        CpuFamilyPc = (signature & 0xf00) >> 8;
        CpuModelPc = (signature & 0xf0) >> 4;
        CpuSteppingPc = signature & 0xf;
        CpuBrandIndexPc = (u8)registers[1];
        CpuHasFpuPc = (features >> 0) & 1;
        CpuHasVmePc = (features >> 1) & 1;
        CpuHasFpuPc = features & 1;
        CpuHasDePc = (features >> 2) & 1;
        CpuHasPsePc = (features >> 3) & 1;
        CpuHasTscPc = (features >> 4) & 1;
        CpuHasMsrPc = (features >> 5) & 1;
        CpuHasPaePc = (features >> 6) & 1;
        CpuHasMcePc = (features >> 7) & 1;
        CpuHasCmpxchg8bPc = (features >> 8) & 1;
        CpuHasApicPc = (features >> 9) & 1;
        CpuHasPgePc = (features >> 12) & 1;
        CpuHasMcaPc = (features >> 13) & 1;
        CpuHasCmovPc = (features >> 14) & 1;
        CpuHasPatPc = (features >> 15) & 1;
        CpuHasMmxPc = (features >> 23) & 1;
        if (CpuVendorPc == 1) {
            PCClipUseSSE = (features >> 25) & 1;
            CpuHasSse2Pc = (features >> 26) & 1;
        } else if (CpuVendorPc == 2) {
            PCClipUseSSE = (features >> 25) & 1;
            __cpuid(registers, (i32)0x80000000);
            CpuExtendedLeafMaxPc = (u32)registers[0];
            if (CpuExtendedLeafMaxPc != 0) {
                __cpuid(registers, (i32)0x80000001);
                CpuExtendedFeaturesPc = (u32)registers[3];
                CpuHas3dnowPc = CpuExtendedFeaturesPc >> 31;
            }
        } else if (CpuVendorPc == 3) {
            __cpuid(registers, (i32)0x80000000);
            CpuExtendedLeafMaxPc = (u32)registers[0];
            if (CpuExtendedLeafMaxPc >= 0x80000001U) {
                __cpuid(registers, (i32)0x80000001);
                CpuExtendedFeaturesPc = (u32)registers[3];
            }
        }
    }
    if (CpuVendorPc == 2 && PCClipUseSSE != 0) {
        __try {
            volatile __m128 probe = _mm_setzero_ps();
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            PCClipUseSSE = 0;
        }
    }
    CpuInitializedPc = 1;
}
