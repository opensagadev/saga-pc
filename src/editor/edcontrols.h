#pragma once

/* The editor's property controls that both the level editor and the light registration (rtlhelper.cpp)
 * create; EdValueControl's members are defined in legoapi/rtlhelper.cpp. */

#include "editor/edlevelall.h"
#include "gameapi/edtools/edui.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"

#ifdef __cplusplus
/* Property control for EdType_String members: the text is edited in place
 * and the button opens a text picker on it. */
class EdStringControl : public EdControl { // size 0x10
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdStringControl));
    }

    EdStringControl();
    virtual void AddMenuItem(eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    void GetVal(char *value, i32 capacity);
    void SetVal(char const *value);
    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbPress(eduimenu_s *menu, eduiitem_s *item, u32);
};

/* Property control for EdType_Colour3 members: "r g b" text, the property
 * button shows the colour and opens a colour picker. */
class EdColourControl : public EdControl { // size 0x10
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdColourControl));
    }

    EdColourControl();
    virtual ~EdColourControl();
    virtual void AddMenuItem(eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    void GetVal(NUVEC &value);
    void SetVal(NUVEC &value);
    static void cbButton(eduimenu_s *menu, eduiitem_s *item, u32);
    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbColourSelected(eduimenu_s *menu, eduiitem_s *item, u32 flags);
};

/* Three "%.2f" rows (x, y, z) under an expander for a VuVec member. */
class EdVectorControl : public EdControl { // size 0x1c
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdVectorControl));
    }

    EdVectorControl();

    virtual ~EdVectorControl() {
        Destroy();
    }

    virtual void AddMenuItem(eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    void Destroy();
    void GetVal(VuVec &value);
    void SetVal(VuVec &value);

    /* Drags the row's value by the cursor and analogue pad. */
    static void Drag(eduiitem_s *item) {
        edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
        property->button_latched = 1;
        FRAME_PAD *pad = EdControl::Input->pad;
        f32 value = NuAToF(property->property_text);
        f32 dx;
        f32 dy;
        eduiGetCursorDelta(&dx, &dy);
        value -= dy * 100.0;
        dy = eduiGetAnalougePadValue(pad);
        value -= dy;
        char text[128];
        NuSPrintf(text, "%.2f", value);
        eduiItemPropSetText(property, text);
    }

    static void cbButton(eduimenu_s *menu, eduiitem_s *item, u32 value);
    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbSelected(eduimenu_s *menu, eduiitem_s *item, u32);

    eduiitem_s *components[3]; // 0x10
};

/* Position, rotation and scale rows (x, y, z each) under an expander for a
 * VuMtx member; the member's attributes 8, 0x10 and 0x20 pick the groups. */
class EdMatrixControl : public EdControl { // size 0x34
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdMatrixControl));
    }

    EdMatrixControl();

    virtual ~EdMatrixControl() {
        Destroy();
    }

    virtual void AddMenuItem(eduimenu_s *menu, EdRef *reference, void *object);
    virtual void Refresh();
    virtual void SetMenuItemAttr(i32 mask, eduiitem_s *menu_item, eduiiattr_s *selected, eduiiattr_s *unselected);
    void Destroy();
    void GetVal(VuMtx &value);
    void SetVal(VuMtx &value);

    /* Drags the row's value by the cursor and analogue pad. */
    static void Drag(eduiitem_s *item) {
        edui_prop_s *property = reinterpret_cast<edui_prop_s *>(item);
        property->button_latched = 1;
        FRAME_PAD *pad = EdControl::Input->pad;
        f32 value = NuAToF(property->property_text);
        f32 dx;
        f32 dy;
        eduiGetCursorDelta(&dx, &dy);
        value -= dy * 100.0;
        dy = eduiGetAnalougePadValue(pad);
        value -= dy;
        char text[128];
        NuSPrintf(text, "%.2f", value);
        eduiItemPropSetText(property, text);
    }

    static void cbButton(eduimenu_s *menu, eduiitem_s *item, u32 value);
    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbSelected(eduimenu_s *menu, eduiitem_s *item, u32);

    eduiitem_s *components[9]; // 0x10
};

/* Numeric property row clamped to [minimum, maximum]; the button drags the
 * value with the cursor or the analogue sticks. */
/* Layout with T = f32 or i32: value_type at 0x10, format at 0x14, minimum at 0x18, maximum at 0x1c, size 0x20. */
template <class T> class EdValueControl : public EdControl {
  public:
    static void *operator new(size_t size) {
        return theMemoryManager.AllocPool(size, 1);
    }

    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdValueControl));
    }

    EdValueControl() : minimum(0), maximum(0) {
    }

    EdValueControl(T minimum, T maximum) : minimum(minimum), maximum(maximum) {
    }

    virtual void AddMenuItem(eduimenu_s *menu, EdRef *member, void *target);
    virtual void Refresh();

    void GetVal(T *value) {
        reference->GetMemberData(object, value_type, value, 0);
    }

    void SetVal(T *value) {
        reference->SetMemberData(object, value_type, value, 0, NULL);
    }

    /* Drag step per unit of stick or cursor travel; finer for narrow ranges. */
    static f32 GetSensitivity(eduiitem_s *item) {
        EdValueControl *control = static_cast<EdValueControl *>(item->data_ptr);
        f32 sensitivity = 0.01f;
        T range = control->maximum - control->minimum;
        if (range < 5) {
            sensitivity = 0.001f;
        }
        return sensitivity;
    }

    static void cbChanged(eduimenu_s *, eduiitem_s *item, u32);
    static void cbButton(eduimenu_s *menu, eduiitem_s *item, u32 flags);

    static f32 MouseScale;

    i32 value_type;
    char *format;
    T minimum;
    T maximum;
};

class EdIntControl : public EdValueControl<i32> {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdIntControl));
    }

    EdIntControl(i32 minimum, i32 maximum) : EdValueControl<i32>(minimum, maximum) {
        value_type = EdType_Int;
        format = "%d";
    }
};

class EdFloatControl : public EdValueControl<f32> {
  public:
    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(EdFloatControl));
    }

    EdFloatControl() : EdValueControl<f32>(-1000000000.0f, 1000000000.0f) {
        value_type = EdType_Float;
        format = "%.2f";
    }

    EdFloatControl(f32 minimum, f32 maximum) : EdValueControl<f32>(minimum, maximum) {
        value_type = EdType_Float;
        format = "%.2f";
    }
};
#endif
