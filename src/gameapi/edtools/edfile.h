#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct FRAME_PAD;
struct edui_slider_s;
struct eduiitem_s;
struct eduimenu_s;

#ifdef __cplusplus
extern "C" {
#endif
    void EdFileWriteFloat(f32 value);
    void EdFileSetMedia(i32 media);
    void EdFileSetPakFile(void *pak);
    void EdFileSetReadWrongEndianess(i32 value);
    i32 EdFileOpen(char *filepath, i32 mode);
    i32 EdFileClose(void);
    void EdFileRead(void *buffer, i32 length);
    i32 EdFileReadInt(void);
    f32 EdFileReadFloat(void);
    i16 EdFileReadShort(void);
    void EdFileReadNuVec(NUVEC *out);
    char EdFileReadChar(void);
    void EdFileSwapEndianess16(void *);
    void EdFileSwapEndianess32(void *);

    u16 EdFileReadUnsignedShort(void);
    u8 EdFileReadUnsignedChar(void);
    /* Static DisplayClipFallbackPC logical source signature remains local:
     * native/private incoming ESI scene and one caller-clean stack visibility word.
     * Blank CSV ctype prevents an external plain two-stack-argument call. */

    u32 EdFileReadUnsignedInt(void);
    i32 cbProcessColourPick(struct eduimenu_s *menu, struct eduiitem_s *item, f32 delta_time, struct FRAME_PAD *pad);
    void cbToggleIndicatorMode(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void cbCopy(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPaste(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbmcfgCameraSpeed(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void EdFileWrite(void *data, i32 len);
    void EdFileWriteNuVec(NUVEC *value);
    void edmainSetLocVec(NUVEC *location);
    struct eduiitem_s *eduiItemTexturePickCreate(u32 data, void *colours, void *callback, char *text);
    extern f32 edui_font_scale_x;
    extern f32 edui_font_scale_y;
    void EdFileWriteUnsignedChar(u8);
    i32 EdFileBackup(char *source, char *destination);
    i32 EduiRenderItem(struct eduiitem_s *, struct eduimenu_s *, i32, i32, i32, i32);
    void EdFileWriteUnsignedShort(u16 value);
    void eduiItemSliderSetValEx(struct edui_slider_s *item, f32 value, i32 reset_timer, i32 notify);
#ifdef __cplusplus
}
#endif
