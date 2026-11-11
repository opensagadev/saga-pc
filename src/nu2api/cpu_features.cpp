#include "saga.h"
#include <excpt.h>

/* Retail detects the CPU with a hand-written inline assembly block, ported as is;
   the shifts by zero come from its uniform field-extraction pattern. */
extern "C" void __cdecl CpuDetectFeaturesPc(void) {
    __asm {
        pushfd
        pop eax
        mov ecx, eax
        xor eax, 0x200000
        push eax
        popfd
        pushfd
        pop eax
        xor eax, ecx
        jne has_cpuid
        mov CpuHasCpuidPc, 0
        mov CpuBasicLeafMaxPc, 0
        mov ebx, CpuVendorNoCpuidPc.word0
        mov edx, CpuVendorNoCpuidPc.word1
        mov ecx, CpuVendorNoCpuidPc.word2
        mov CpuVendorNamePc.word0, ebx
        mov CpuVendorNamePc.word1, edx
        mov CpuVendorNamePc.word2, ecx
        jmp done
    has_cpuid:
        mov CpuHasCpuidPc, 1
        mov eax, 0
        cpuid
        mov CpuBasicLeafMaxPc, eax
        mov CpuVendorNamePc.word0, ebx
        mov CpuVendorNamePc.word1, edx
        mov CpuVendorNamePc.word2, ecx
        cmp ebx, CpuVendorIntelPc.word0
        jne not_intel
        cmp edx, CpuVendorIntelPc.word1
        jne not_intel
        cmp ecx, CpuVendorIntelPc.word2
        jne not_intel
        mov CpuVendorPc, 1
        jmp vendor_known
    not_intel:
        cmp ebx, CpuVendorAmdPc.word0
        jne not_amd
        cmp edx, CpuVendorAmdPc.word1
        jne not_amd
        cmp ecx, CpuVendorAmdPc.word2
        jne not_amd
        mov CpuVendorPc, 2
        jmp vendor_known
    not_amd:
        cmp ebx, CpuVendorCyrixPc.word0
        jne not_cyrix
        cmp edx, CpuVendorCyrixPc.word1
        jne not_cyrix
        cmp ecx, CpuVendorCyrixPc.word2
        jne not_cyrix
        mov CpuVendorPc, 3
        jmp vendor_known
    not_cyrix:
        mov CpuVendorPc, 0
    vendor_known:
        cmp eax, 0
        je done
        mov eax, 1
        cpuid
        mov edi, eax
        and edi, 0x3000
        shr edi, 12
        mov CpuProcessorTypePc, edi
        mov edi, eax
        and edi, 0xf00
        shr edi, 8
        mov CpuFamilyPc, edi
        mov edi, eax
        and edi, 0xf0
        shr edi, 4
        mov CpuModelPc, edi
        mov edi, eax
        and edi, 0xf
        shr edi, 0
        mov CpuSteppingPc, edi
        mov CpuBrandIndexPc, bl
        mov edi, edx
        and edi, 0x1
        shr edi, 0
        mov CpuHasFpuPc, edi
        mov edi, edx
        and edi, 0x2
        shr edi, 1
        mov CpuHasVmePc, edi
        mov edi, edx
        and edi, 0x1
        shr edi, 0
        mov CpuHasFpuPc, edi
        mov edi, edx
        and edi, 0x4
        shr edi, 2
        mov CpuHasDePc, edi
        mov edi, edx
        and edi, 0x8
        shr edi, 3
        mov CpuHasPsePc, edi
        mov edi, edx
        and edi, 0x10
        shr edi, 4
        mov CpuHasTscPc, edi
        mov edi, edx
        and edi, 0x20
        shr edi, 5
        mov CpuHasMsrPc, edi
        mov edi, edx
        and edi, 0x40
        shr edi, 6
        mov CpuHasPaePc, edi
        mov edi, edx
        and edi, 0x80
        shr edi, 7
        mov CpuHasMcePc, edi
        mov edi, edx
        and edi, 0x100
        shr edi, 8
        mov CpuHasCmpxchg8bPc, edi
        mov edi, edx
        and edi, 0x200
        shr edi, 9
        mov CpuHasApicPc, edi
        mov edi, edx
        and edi, 0x1000
        shr edi, 12
        mov CpuHasPgePc, edi
        mov edi, edx
        and edi, 0x2000
        shr edi, 13
        mov CpuHasMcaPc, edi
        mov edi, edx
        and edi, 0x4000
        shr edi, 14
        mov CpuHasCmovPc, edi
        mov edi, edx
        and edi, 0x8000
        shr edi, 15
        mov CpuHasPatPc, edi
        mov edi, edx
        and edi, 0x800000
        shr edi, 23
        mov CpuHasMmxPc, edi
        mov eax, CpuVendorPc
        cmp eax, 1
        je intel_features
        cmp eax, 2
        je amd_features
        cmp eax, 3
        je cyrix_features
        jmp done
    intel_features:
        mov edi, edx
        and edi, 0x2000000
        shr edi, 25
        mov PCClipUseSSE, edi
        mov edi, edx
        and edi, 0x4000000
        shr edi, 26
        mov CpuHasSse2Pc, edi
        jmp done
    amd_features:
        mov edi, edx
        and edi, 0x2000000
        shr edi, 25
        mov PCClipUseSSE, edi
        mov eax, 0x80000000
        cpuid
        mov CpuExtendedLeafMaxPc, eax
        test eax, eax
        je done
        mov eax, 0x80000001
        cpuid
        mov CpuExtendedFeaturesPc, edx
        mov edi, edx
        and edi, 0x80000000
        shr edi, 31
        mov CpuHas3dnowPc, edi
        jmp done
    cyrix_features:
        mov eax, 0x80000000
        cpuid
        mov CpuExtendedLeafMaxPc, eax
        cmp eax, 0x80000000
        jl done
        mov eax, 0x80000001
        cmp eax, CpuExtendedLeafMaxPc
        ja done
        cpuid
        mov CpuExtendedFeaturesPc, edx
        jmp done
    done:
    }
    if (CpuVendorPc == 2 && PCClipUseSSE != 0) {
        // Executing an SSE instruction faults when the OS has not enabled SSE state saving.
        __try {
            __asm orps xmm0, xmm0
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            PCClipUseSSE = 0;
        }
    }
    CpuInitializedPc = 1;
}

/* Native 0x6e62d0-0x6e668d: lazy accessors for the detected CPU fields. None has a caller in retail. */
extern "C" u32 __cdecl CpuGetHasCpuidPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasCpuidPc;
}

extern "C" u32 __cdecl CpuGetFamilyPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuFamilyPc;
}

extern "C" u32 __cdecl CpuGetProcessorTypePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuProcessorTypePc;
}

extern "C" u32 __cdecl CpuGetModelPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuModelPc;
}

extern "C" u32 __cdecl CpuGetSteppingPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuSteppingPc;
}

extern "C" u32 __cdecl CpuGetUnsetValue0Pc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuUnsetValue0Pc;
}

extern "C" u32 __cdecl CpuGetBasicLeafMaxPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuBasicLeafMaxPc;
}

extern "C" u32 __cdecl CpuGetUnsetValue1Pc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuUnsetValue1Pc;
}

extern "C" u32 __cdecl CpuGetHasFpuPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasFpuPc;
}

extern "C" u32 __cdecl CpuGetHasMmxPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasMmxPc;
}

extern "C" u32 __cdecl CpuGetHasTscPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasTscPc;
}

extern "C" u32 __cdecl CpuGetHasDePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasDePc;
}

extern "C" u32 __cdecl CpuGetHasApicPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasApicPc;
}

extern "C" u32 __cdecl CpuGetHasPatPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasPatPc;
}

extern "C" u32 __cdecl CpuGetHasVmePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasVmePc;
}

extern "C" u32 __cdecl CpuGetHasPsePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasPsePc;
}

extern "C" u32 __cdecl CpuGetHasMsrPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasMsrPc;
}

extern "C" u32 __cdecl CpuGetHasPaePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasPaePc;
}

extern "C" u32 __cdecl CpuGetHasMcePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasMcePc;
}

extern "C" u32 __cdecl CpuGetHasCmpxchg8bPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasCmpxchg8bPc;
}

extern "C" u32 __cdecl CpuGetHasPgePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasPgePc;
}

extern "C" u32 __cdecl CpuGetHasMcaPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasMcaPc;
}

extern "C" u32 __cdecl CpuGetHasCmovPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasCmovPc;
}

extern "C" u32 __cdecl CpuGetUnsetFeature0Pc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuUnsetFeature0Pc;
}

extern "C" u32 __cdecl CpuGetUnsetFeature1Pc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuUnsetFeature1Pc;
}

extern "C" u32 __cdecl CpuGetUnsetFeature2Pc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuUnsetFeature2Pc;
}

extern "C" u32 __cdecl CpuGetUnsetFeature3Pc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuUnsetFeature3Pc;
}

extern "C" u32 __cdecl CpuGetHas3dnowPc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHas3dnowPc;
}

extern "C" i32 __cdecl CpuGetHasSsePc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return PCClipUseSSE;
}

extern "C" u32 __cdecl CpuGetHasSse2Pc(void) {
    if (CpuInitializedPc == 0) {
        CpuDetectFeaturesPc();
        CpuInitializedPc = 1;
    }
    return CpuHasSse2Pc;
}
