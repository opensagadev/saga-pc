#include "saga.h"
#include <shlobj.h>
#include <stdio.h>
#include <string.h>

extern "C" char *PcGetUserFolder(const char *unused_company, const char *unused_product) {
    i32 folder = CSIDL_LOCAL_APPDATA;
    if (PcUsePersonalFolder) {
        folder = CSIDL_PERSONAL;
    }
    HRESULT result = SHGetFolderPathA(NULL, folder, NULL, 0, PcUserFolder);
    if (FAILED(result)) {
        return NULL;
    }
    return PcUserFolder;
}

extern "C" i32 fun_006dd460(const char *name, const char *prefix, i32 use_user_folder, void *buffer, i32 size) {
    char path[256];
    if (use_user_folder == 1) {
        strcpy(path, PcGetUserFolder(PcCompanyName, PcProductName));
        strcat(path, "\\");
        strcat(path, PcCompanyName);
        strcat(path, "\\");
        strcat(path, PcProductName);
        strcat(path, "\\");
        strcat(path, prefix);
    } else {
        strcpy(path, prefix);
    }
    strcat(path, name);
    FILE *file = fopen(path, "rb");
    if (!file) {
        return 0;
    }
    fread(buffer, size, 1, file);
    fclose(file);
    return 1;
}

extern "C" i32 fun_006dd690(const char *name, const char *prefix, i32 use_user_folder, const void *buffer, i32 size) {
    char path[256];
    if (use_user_folder == 1) {
        strcpy(path, PcGetUserFolder(PcCompanyName, PcProductName));
        strcat(path, "\\");
        strcat(path, PcCompanyName);
        strcat(path, "\\");
        strcat(path, PcProductName);
        strcat(path, "\\");
        strcat(path, prefix);
    } else {
        strcpy(path, prefix);
    }
    strcat(path, name);
    FILE *file = fopen(path, "wb");
    if (!file) {
        return 0;
    }
    fwrite(buffer, size, 1, file);
    fclose(file);
    return 1;
}

extern "C" FILE *PcOpenPlatformFile(const char *name, const char *prefix, i32 use_user_folder, const char *mode) {
    if (use_user_folder == 1) {
        strcpy(PcFileDirectory, PcGetUserFolder(PcCompanyName, PcProductName));
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, PcCompanyName);
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, PcProductName);
        strcat(PcFileDirectory, "\\");
        strcat(PcFileDirectory, prefix);
    } else {
        strcpy(PcFileDirectory, prefix);
    }
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

extern "C" i32 fun_006ddd30(void) {
    return 1;
}
