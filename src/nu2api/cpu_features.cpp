#include "saga.h"
#include <excpt.h>

/* Detects the CPU with a hand-written inline assembly block; the shifts by
   zero come from its uniform field-extraction pattern. */
extern "C" void CpuDetectFeatures(void) {
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
        mov CpuHasCpuid, 0
        mov CpuBasicLeafMax, 0
        mov ebx, CpuVendorNoCpuid.word0
        mov edx, CpuVendorNoCpuid.word1
        mov ecx, CpuVendorNoCpuid.word2
        mov CpuVendorName.word0, ebx
        mov CpuVendorName.word1, edx
        mov CpuVendorName.word2, ecx
        jmp done
    has_cpuid:
        mov CpuHasCpuid, 1
        mov eax, 0
        cpuid
        mov CpuBasicLeafMax, eax
        mov CpuVendorName.word0, ebx
        mov CpuVendorName.word1, edx
        mov CpuVendorName.word2, ecx
        cmp ebx, CpuVendorIntel.word0
        jne not_intel
        cmp edx, CpuVendorIntel.word1
        jne not_intel
        cmp ecx, CpuVendorIntel.word2
        jne not_intel
        mov CpuVendor, 1
        jmp vendor_known
    not_intel:
        cmp ebx, CpuVendorAmd.word0
        jne not_amd
        cmp edx, CpuVendorAmd.word1
        jne not_amd
        cmp ecx, CpuVendorAmd.word2
        jne not_amd
        mov CpuVendor, 2
        jmp vendor_known
    not_amd:
        cmp ebx, CpuVendorCyrix.word0
        jne not_cyrix
        cmp edx, CpuVendorCyrix.word1
        jne not_cyrix
        cmp ecx, CpuVendorCyrix.word2
        jne not_cyrix
        mov CpuVendor, 3
        jmp vendor_known
    not_cyrix:
        mov CpuVendor, 0
    vendor_known:
        cmp eax, 0
        je done
        mov eax, 1
        cpuid
        mov edi, eax
        and edi, 0x3000
        shr edi, 12
        mov CpuProcessorType, edi
        mov edi, eax
        and edi, 0xf00
        shr edi, 8
        mov CpuFamily, edi
        mov edi, eax
        and edi, 0xf0
        shr edi, 4
        mov CpuModel, edi
        mov edi, eax
        and edi, 0xf
        shr edi, 0
        mov CpuStepping, edi
        mov CpuBrandIndex, bl
        mov edi, edx
        and edi, 0x1
        shr edi, 0
        mov CpuHasFpu, edi
        mov edi, edx
        and edi, 0x2
        shr edi, 1
        mov CpuHasVme, edi
        mov edi, edx
        and edi, 0x1
        shr edi, 0
        mov CpuHasFpu, edi
        mov edi, edx
        and edi, 0x4
        shr edi, 2
        mov CpuHasDe, edi
        mov edi, edx
        and edi, 0x8
        shr edi, 3
        mov CpuHasPse, edi
        mov edi, edx
        and edi, 0x10
        shr edi, 4
        mov CpuHasTsc, edi
        mov edi, edx
        and edi, 0x20
        shr edi, 5
        mov CpuHasMsr, edi
        mov edi, edx
        and edi, 0x40
        shr edi, 6
        mov CpuHasPae, edi
        mov edi, edx
        and edi, 0x80
        shr edi, 7
        mov CpuHasMce, edi
        mov edi, edx
        and edi, 0x100
        shr edi, 8
        mov CpuHasCmpxchg8b, edi
        mov edi, edx
        and edi, 0x200
        shr edi, 9
        mov CpuHasApic, edi
        mov edi, edx
        and edi, 0x1000
        shr edi, 12
        mov CpuHasPge, edi
        mov edi, edx
        and edi, 0x2000
        shr edi, 13
        mov CpuHasMca, edi
        mov edi, edx
        and edi, 0x4000
        shr edi, 14
        mov CpuHasCmov, edi
        mov edi, edx
        and edi, 0x8000
        shr edi, 15
        mov CpuHasPat, edi
        mov edi, edx
        and edi, 0x800000
        shr edi, 23
        mov CpuHasMmx, edi
        mov eax, CpuVendor
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
        mov CpuHasSse2, edi
        jmp done
    amd_features:
        mov edi, edx
        and edi, 0x2000000
        shr edi, 25
        mov PCClipUseSSE, edi
        mov eax, 0x80000000
        cpuid
        mov CpuExtendedLeafMax, eax
        test eax, eax
        je done
        mov eax, 0x80000001
        cpuid
        mov CpuExtendedFeatures, edx
        mov edi, edx
        and edi, 0x80000000
        shr edi, 31
        mov CpuHas3dnow, edi
        jmp done
    cyrix_features:
        mov eax, 0x80000000
        cpuid
        mov CpuExtendedLeafMax, eax
        cmp eax, 0x80000000
        jl done
        mov eax, 0x80000001
        cmp eax, CpuExtendedLeafMax
        ja done
        cpuid
        mov CpuExtendedFeatures, edx
        jmp done
    done:
    }
    if (CpuVendor == 2 && PCClipUseSSE != 0) {
        // Executing an SSE instruction faults when the OS has not enabled SSE state saving.
        __try {
            __asm orps xmm0, xmm0
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            PCClipUseSSE = 0;
        }
    }
    CpuInitialized = 1;
}

/* Lazy accessors for the detected CPU fields. None has a caller. */
extern "C" u32 CpuGetHasCpuid(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasCpuid;
}

extern "C" u32 CpuGetFamily(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuFamily;
}

extern "C" u32 CpuGetProcessorType(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuProcessorType;
}

extern "C" u32 CpuGetModel(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuModel;
}

extern "C" u32 CpuGetStepping(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuStepping;
}

extern "C" u32 CpuGetUnsetValue0(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetValue0;
}

extern "C" u32 CpuGetBasicLeafMax(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuBasicLeafMax;
}

extern "C" u32 CpuGetUnsetValue1(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetValue1;
}

extern "C" u32 CpuGetHasFpu(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasFpu;
}

extern "C" u32 CpuGetHasMmx(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMmx;
}

extern "C" u32 CpuGetHasTsc(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasTsc;
}

extern "C" u32 CpuGetHasDe(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasDe;
}

extern "C" u32 CpuGetHasApic(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasApic;
}

extern "C" u32 CpuGetHasPat(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPat;
}

extern "C" u32 CpuGetHasVme(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasVme;
}

extern "C" u32 CpuGetHasPse(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPse;
}

extern "C" u32 CpuGetHasMsr(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMsr;
}

extern "C" u32 CpuGetHasPae(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPae;
}

extern "C" u32 CpuGetHasMce(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMce;
}

extern "C" u32 CpuGetHasCmpxchg8b(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasCmpxchg8b;
}

extern "C" u32 CpuGetHasPge(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPge;
}

extern "C" u32 CpuGetHasMca(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMca;
}

extern "C" u32 CpuGetHasCmov(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasCmov;
}

extern "C" u32 CpuGetUnsetFeature0(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature0;
}

extern "C" u32 CpuGetUnsetFeature1(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature1;
}

extern "C" u32 CpuGetUnsetFeature2(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature2;
}

extern "C" u32 CpuGetUnsetFeature3(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature3;
}

extern "C" u32 CpuGetHas3dnow(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHas3dnow;
}

extern "C" i32 CpuGetHasSse(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return PCClipUseSSE;
}

extern "C" u32 CpuGetHasSse2(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasSse2;
}
