#include "saga.h"

extern i32 PcSaveFolderRegisteredPc;
extern "C" void __cdecl PcRegisterSaveFolderPc(const char *company, const char *product, const char *parameter,
                                               i32 personal) {
    PcCompanyName = company;
    PcProductName = product;
    NuHwDefaultParameterPc = parameter;
    PcUsePersonalFolder = personal;
    PcSaveFolderRegisteredPc = 1;
}

extern "C" void __cdecl PcSetDefaultParameterPc(const char *parameter) {
    NuHwDefaultParameterPc = parameter;
}
