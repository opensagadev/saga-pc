#include "saga.h"
#include <shlobj.h>
#include <stdio.h>
#include <string.h>
extern const char *PcCompanyName, *PcProductName;
extern i32 PcUsePersonalFolder;
extern char PcUserFolder[], PcFileDirectory[], PcFilePath[];
extern "C" char *__cdecl PcGetUserFolder(const char *unused_company, const char *unused_product) {
    i32 folder = CSIDL_LOCAL_APPDATA;
    if (PcUsePersonalFolder)
        folder = CSIDL_PERSONAL;
    HRESULT result = SHGetFolderPathA(NULL, folder, NULL, 0, PcUserFolder);
    if (FAILED(result))
        return NULL;
    return PcUserFolder;
}
extern "C" FILE *__cdecl PcOpenPlatformFile(const char *name, const char *prefix, i32 use_user_folder,
                                            const char *mode) {
    if (use_user_folder == 1) {
        strcpy(PcFileDirectory, PcGetUserFolder(PcCompanyName, PcProductName));
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, PcCompanyName);
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, PcProductName);
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, prefix);
    } else
        strcpy(PcFileDirectory, prefix);
    strcpy(PcFilePath, PcFileDirectory);
    strcat(PcFilePath, name);
    FILE *file = fopen(PcFilePath, mode);
    if (!file) {
        strcpy(PcFileDirectory, PcGetUserFolder(PcCompanyName, PcProductName));
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, PcCompanyName);
        CreateDirectoryA(PcFileDirectory, NULL);
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, PcProductName);
        CreateDirectoryA(PcFileDirectory, NULL);
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, prefix);
        CreateDirectoryA(PcFileDirectory, NULL);
        file = fopen(PcFilePath, mode);
    }
    return file;
}
