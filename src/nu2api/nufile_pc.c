#include "gameapi/apisave.h"
#include "gameapi/edtools/edui.h"
#include "nu2api/bgproc_pc.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile_pc.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <windows.h>
#include <string.h>

i32 NuFileUseHiresPC(void) {
    return PcWindowOptions.usehires;
}

i32 bgproc_initialized = 0;
HANDLE bgproc_work_event = 0;
HANDLE bgproc_resume_event = 0;
void (*nuwarning_handler)(char *) = 0;
char *nufile = 0;
char PCWarningFormatBuffer[1024] = {0};
i32 nuline = 0;
CRITICAL_SECTION PcStartupCriticalSection = {0};
void (*nuerror_handler)(char *) = 0;
char PCWarningMessages[1024] = {0};
char PCWarningOutputBuffer[1024] = {0};
CRITICAL_SECTION bgproc_critical_section = {0};
char err_msg_filter[32] = {0};
i32 DisableDebugMsg = 0;
i32 bHaveErr = 0;
BGPROCINFO *bgproc_active = 0;
HANDLE bgproc_thread_handle = NULL;
DWORD bgproc_thread_id = 0;
i32 bgproc_frozen = 0;
i32 bgproc_freeze = 0;
i32 PcStartupCriticalSectionInitialized = 0;
i32 PCThreadCriticalSectionCount = 0;
