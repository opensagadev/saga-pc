#include "saga.h"

__declspec(noinline) void __cdecl edppPtlDestroy(i32 index) {
    if (edpp_ptls[index].instance_id != -1) {
        if (edpp_ptls[index].instance_id != 99999)
            DebFreeInstantly(&edpp_ptls[index].instance_id);
        --edpp_instances_used;
        edpp_ptls[index].instance_id = -1;
    }
}

void __cdecl edppPtlShelve(i32 index) {
    EDPP_PARTICLE_PC *particle = &edpp_ptls[index];
    if (particle->instance_id != -1 && particle->instance_id != 99999) {
        DebFreeInstantly(&particle->instance_id);
        particle->instance_id = 99999;
    }
}

void __cdecl cbCancelMessageMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(messagemenu);
    messagemenu = NULL;
}

static void edppShowMessagePc(eduimenu_s *parent, char *text, i32 ok) {
    u32 colours[4] = {0x800000c0, 0x80ff0000, 0x80808080, 0x80404040};
    if (ok == 1)
        colours[0] = 0x8000c000;
    messagemenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbCancelMessageMenu, "Message");
    if (messagemenu != NULL) {
        eduiMenuAddItem(messagemenu, eduiItemSelCreate(1, colours, 0, 0, NULL, text));
        eduiMenuAttach(parent, messagemenu);
        messagemenu->x = parent->x + 10;
        messagemenu->y = parent->y + 40;
    }
}

void __cdecl cbCancelChangeNameMenu(eduimenu_s *menu, eduimenu_s *unused1) {
    if (edpp_create_type == -1 || debtab[edpp_create_type] == NULL)
        return;
    if (debtab[edpp_create_type]->name[0] == '\0') {
        edppShowMessagePc(menu, "Name Duplicate", 0);
    } else {
        eduiMenuDestroy(namemenu);
        namemenu = NULL;
    }
}

void __cdecl cbChangeName(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpp_create_type != -1 && debtab[edpp_create_type] != NULL)
        strcpy(debtab[edpp_create_type]->name, ((edui_textpicker_s *)item)->value);
}

static void cbChangeNameMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct debris_info_pc_s *effect;
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edpp_create_type == -1) {
        return;
    }
    effect = debtab[edpp_create_type];
    namemenu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, cbCancelChangeNameMenu, "Type Name");
    if (namemenu != NULL) {
        eduiMenuAddItem(namemenu, eduiItemTextPickCreate(0, colours, cbChangeName, "Type Name"));
        strcpy(((edui_textpicker_s *)(edui_last_item))->value, effect->name);
        ((edui_textpicker_s *)(edui_last_item))->max_length = 15;
        eduiMenuAttach(menu, namemenu);
        namemenu->x = menu->x + 10;
        namemenu->y = menu->y + 40;
    }
}
static i32 edppProc(f32 delta_time, FRAME_PAD_PC *pad) {
    edptl_count += 5;
    if (edpp_active_menu != NULL) {
        eduiMenuProcess(edpp_active_menu, delta_time, pad);
        return 0;
    }
    edppDoInput(pad);
    edppDetermineNearest(1.0f);
    if (edpp_nearest != -1)
        DebrisStatusNormal(&edpp_ptls[edpp_nearest].instance_id);
    return (pad->digital_buttons_pressed >> 11) & 1;
}

void __cdecl edppStopPage(i8 page) {
    i32 index, key, handle;
    EDPP_PARTICLE_PC *particle;
    edpp_page_on[page] = 0;
    for (index = 0; index < 512; ++index) {
        particle = &edpp_ptls[index];
        if (particle->instance_id == 99999 || particle->instance_id == -1)
            continue;
        if (particle->page == page) {
            DebFreeInstantly(&particle->instance_id);
            particle->instance_id = 99999;
        } else if (particle->effect_index > 0 && debtab[particle->effect_index] != NULL &&
                   debtab[particle->effect_index]->page == (u8)page) {
            edppPtlDestroy(index);
        }
    }
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL || debtab[index]->page != (u8)page)
            continue;
        for (key = 0; key < maxdebkeys; ++key) {
            if (debkeydata[key].effect_index == index) {
                handle = key;
                DebFreeInstantly(&handle);
            }
        }
        DebFreeOrphansInstantly(debtab[index]);
    }
}

void __cdecl edppClearPage(i8 page) {
    i32 index, key, handle;
    edpp_page_on[page] = 0;
    edpp_page_used[page] = 0;
    for (index = 0; index < 512; ++index) {
        if (edpp_ptls[index].page == page && edpp_ptls[index].instance_id != -1) {
            if (edpp_ptls[index].instance_id != 99999)
                DebFreeInstantly(&edpp_ptls[index].instance_id);
            --edpp_instances_used;
            edpp_ptls[index].instance_id = -1;
        }
    }
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL || debtab[index]->page != (u8)page)
            continue;
        debtab[index]->disabled = 1;
        for (key = 0; key < maxdebkeys; ++key) {
            if (debkeydata[key].effect_index == index) {
                handle = key;
                DebFreeInstantly(&handle);
            }
        }
        DebFreeOrphansInstantly(debtab[index]);
        if (debtab[index]->native_data != NULL) {
            DmaDebTypes[--freeDmaDebType] = debtab[index]->native_data;
            debtab[index]->native_data = NULL;
        }
        debtab[index] = NULL;
        --edpp_types_used;
    }
}

extern NUGSCN *EdppPageScenesPc[8];
void __cdecl DebrisSetFacing(i32, u8, i16, i16);
void __cdecl DebrisSetRoomID(i32, NUGSCN *);
void __cdecl edppStartSingleEffect(i32 index) {
    EDPP_PARTICLE_PC *particle = &edpp_ptls[index];
    if (particle->instance_id != -1)
        return;
    AddDebrisEffect(&particle->instance_id, particle->effect_index, particle->position.x, particle->position.y,
                    particle->position.z);
    if (particle->instance_id == -1)
        return;
    debkeydata[particle->instance_id].field_2f9 = 0;
    DebrisOrientation(particle->instance_id, particle->rotation_z, particle->rotation_y);
    DebrisEmitterOrientation(particle->instance_id, particle->emitter_rotation_z, particle->emitter_rotation_y,
                             particle->emitter_rotation_x);
    DebrisStartOffsetEx(&debkeydata[particle->instance_id], particle->start_offset);
    DebrisSetTrigger(particle->instance_id, particle->switch_type, particle->switch_id, particle->switch_variable);
    DebrisReflectionOrientation(particle->instance_id, particle->reflection_rotation_z, particle->reflection_rotation_y,
                                particle->reflection_offset, particle->reflection_bounce);
    DebrisSetFacing(particle->instance_id, particle->facing_mode, particle->facing_rotation_x,
                    particle->facing_rotation_y);
    if (particle->instance_id != -1)
        debkeydata[particle->instance_id].render_group = particle->render_group;
    DebrisSetRoomID(particle->instance_id, EdppPageScenesPc[particle->page]);
    debkeydata[particle->instance_id].orientation_flags = (u8)particle->detail_levels;
}
void __cdecl edppStartPage(i32 page) {
    i32 index;
    for (index = 0; index < 512; ++index) {
        EDPP_PARTICLE_PC *particle = &edpp_ptls[index];
        if (particle->page == (i8)page && particle->instance_id == 99999) {
            particle->instance_id = -1;
            particle->effect_index = LookupDebrisEffectPageIgnore(particle->name, (i8)page, 0);
            edppStartSingleEffect(index);
            if (particle->instance_id == -1)
                particle->instance_id = 99999;
        }
    }
    edpp_page_on[(i8)page] = 1;
}
extern i32 DebrisProcessingEnabledPc, globalframes;
extern i32 debris_sort_delay, debris_sort_this_stack, debris_update_delay, debris_update_this_stack;
extern i32 DebrisDmaCleanupDelayPc, DebrisDmaCleanupIndexPc;
void __cdecl edppDeleteEffect(i32);
extern i32 edpp_nearest;
extern NUVEC edpp_cam_pos;
void __cdecl edppDetermineNearest(f32);
void __cdecl edppDetermineNearest(f32 max_distance_squared) {
    NUVEC delta;
    i32 i;
    f32 distance;
    if (edpp_nearest != -1) {
        EDPP_PARTICLE_PC *particle = &edpp_ptls[edpp_nearest];
        if (particle->instance_id != 99999 && particle->instance_id != -1) {
            NuVecSub(&delta, &edpp_cam_pos, &particle->position);
            distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (distance == 0.0f)
                return;
        }
    }
    edpp_nearest = -1;
    for (i = 0; i < 512; ++i) {
        if (edpp_ptls[i].instance_id == -1 || edpp_ptls[i].instance_id == 99999)
            continue;
        NuVecSub(&delta, &edpp_cam_pos, &edpp_ptls[i].position);
        distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (max_distance_squared < 0.0f || max_distance_squared > distance) {
            max_distance_squared = distance;
            edpp_nearest = i;
        }
    }
}

static void cbSelEffectList(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    struct eduimenu_s *parent;
    if (item->data != ((i8)(edpp_effect_list))) {
        i32 index;
        edpp_effect_list = ((i8)(item->data));
        edpp_create_type = -1;
        for (index = 1; index < debinf_count; ++index) {
            if (debtab[index] != NULL && debtab[index]->category == edpp_effect_list) {
                edpp_create_type = index;
                break;
            }
        }
    }
    parent = menu->parent;
    if (parent != NULL)
        eduiMenuDetach(menu);
    if (menu->callback != NULL)
        menu->callback(menu, parent);
}

void cbCancelEffectListMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(effectlistmenu);
    effectlistmenu = NULL;
}

void edptlcbSetDebrisThinning(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    debris_thinning_level = ((edui_slider_s *)(item))->value;
}

void edptlcbSetDebrisDetail(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    debris_detail_level = item->data;
}

void edptlcbCancelTestDetailMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edptl_testdetail_menu);
    edptl_testdetail_menu = NULL;
}

void edptlcbSetDpadMode(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edpp_dpad_mode = item->data;
}

void edptlcbCancelDpadModeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(dpadmodemenu);
    dpadmodemenu = NULL;
}

void edptlcbCancelOrphanListMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edptl_orphanlist_menu);
    edptl_orphanlist_menu = NULL;
}

static void cbPtlCancelTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(ptltypemenu);
    ptltypemenu = NULL;
}

void cbPtlCancelQuickDeleteMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edptl_quickdel_menu);
    edptl_quickdel_menu = NULL;
}

static void cbPtlJibMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        const DEBRIS_INFO_PC *effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
        const u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
        ptljibmenu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, cbPtlCancelJibMenu, "Particle Jibber");
        if (ptljibmenu == NULL)
            return;

        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, colours, 0, cbPtlApplyJib, 0.0f, 32.0f,
                                                         effect->jib_x_frequency, "Jibber X Freq"));
        grad_jib_x_freq_item = ((edui_slider_s *)(edui_last_item));
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, colours, 0, cbPtlApplyJib, 0.0f, 1000.0f,
                                                         effect->jib_x_amplitude, "Jibber X Amp"));
        grad_jib_x_amp_item = ((edui_slider_s *)(edui_last_item));
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, colours, 0, cbPtlApplyJib, 0.0f, 32.0f,
                                                         effect->jib_y_frequency, "Jibber Y Freq"));
        grad_jib_y_freq_item = ((edui_slider_s *)(edui_last_item));
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, colours, 0, cbPtlApplyJib, 0.0f, 1000.0f,
                                                         effect->jib_y_amplitude, "Jibber Y Amp"));
        grad_jib_y_amp_item = ((edui_slider_s *)(edui_last_item));
        eduiMenuAttach(parent, ptljibmenu);
        ptljibmenu->x = parent->x + 10;
        ptljibmenu->y = parent->y + 40;
    }
}

static void edptlcbBounceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0xc479c000, 0xc479c000, 0xc479c000, 0xc479c000};
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        DEBRISKEY_PC *key = &debkeydata[edpp_ptls[edpp_nearest].instance_id];
        edptl_bounce_menu = eduiMenuCreate(70, 70, 200, 300, ed_fnt, edptlcbCancelBounceMenu, "Particle Bounce");
        if (edptl_bounce_menu != NULL) {
            eduiMenuAddItem(edptl_bounce_menu,
                            eduiItemSliderCreate(0, colours, 0, edptlcbApplyBounceOffset, -10.0f * edptl_superscale,
                                                 10.0f * edptl_superscale, key->collision_plane, "Plane Offset"));
            eduiMenuAddItem(edptl_bounce_menu, eduiItemSliderCreate(0, colours, 0, edptlcbApplyBounceFactor, 0.0f, 2.0f,
                                                                    key->reflection_scale, "Bounce Factor"));
            eduiMenuAttach(parent, edptl_bounce_menu);
            edptl_bounce_menu->x = parent->x + 10;
            edptl_bounce_menu->y = parent->y + 40;
        }
    }
}

static void edptlcbStarMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        DEBRIS_INFO_PC *effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
        u32 colours[4] = {0xc479c000, 0xc479c000, 0xc479c000, 0xc479c000};
        edptl_star_menu = eduiMenuCreate(70, 70, 200, 300, ed_fnt, edptlcbCancelStarMenu, "Star Settings");
        if (edptl_star_menu == NULL)
            return;
        eduiMenuAddItem(edptl_star_menu, eduiItemSliderCreateInt(0, colours, 0, edptlcbApplyStarPoints, 3, 17,
                                                                 ((i8)(effect->radial_segments)), "Number of Points"));
        eduiMenuAddItem(edptl_star_menu, eduiItemSliderCreate(0, colours, 0, edptlcbApplyStarRatio, 0.1f, 0.8f,
                                                              effect->radial_floor, "Radius Ratio"));
        eduiMenuAttach(parent, edptl_star_menu);
        edptl_star_menu->x = parent->x + 10;
        edptl_star_menu->y = parent->y + 40;
    }
}

static void cbPtlDamageFlagMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct debris_info_pc_s *effect;
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1) {
        return;
    }
    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    edptl_damageflag_menu = eduiMenuCreate(70, 70, 200, 250, ed_fnt, cbPtlCancelDamageFlagMenu, "Damage Flags");
    if (edptl_damageflag_menu != NULL) {
        eduiMenuAddItem(edptl_damageflag_menu, eduiItemToggleCreate(1, edblack, effect->collision_flags & 1, 1,
                                                                    cbPtlChangeDamageFlags, "Good"));
        eduiMenuAddItem(edptl_damageflag_menu, eduiItemToggleCreate(2, edblack, (effect->collision_flags >> 1) & 1, 2,
                                                                    cbPtlChangeDamageFlags, "Evil"));
    }
    eduiMenuAttach(menu, edptl_damageflag_menu);
    edptl_damageflag_menu->x = menu->x + 10;
    edptl_damageflag_menu->y = menu->y + 40;
}

void cbPtlCancelDamageMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edptl_damage_menu);
    edptl_damage_menu = NULL;
}

static void cbPtlCutOffMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    struct debris_info_pc_s *effect;
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)
        return;

    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    ptlcutoffmenu = eduiMenuCreate(70, 70, 250, 200, ed_fnt, cbPtlCancelCutOffMenu, "Repeat / Radii");
    if (ptlcutoffmenu == NULL)
        return;
    eduiMenuAddItem(ptlcutoffmenu, eduiItemToggleCreate(0, edblack, effect->use_explicit_clip_box != 0, 1,
                                                        cbPtlChangeRepeatFlag, "Repeat"));
    eduiMenuAddItem(ptlcutoffmenu, eduiItemSelCreate(1, edblack, 0, 0, cbPtlRepeatBoxMenu, "Repeat Box..."));
    eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, edblack, 0, cbPtlChangeCutOff, 0.0f,
                                                        25.0f * edptl_superscale, effect->clip_extent, "CutOff Rad"));
    eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, edblack, 0, cbPtlChangeCutOn, 0.0f, 25.0f * edptl_superscale,
                                                        effect->cut_on, "CutOn Rad"));
    eduiMenuAddItem(ptlcutoffmenu,
                    eduiItemSliderCreate(0, edblack, 0, cbPtlChangeDrawCutOff, 0.0f, 25.0f * edptl_superscale,
                                         effect->sound_range, "DrawCutOff Rad"));
    eduiMenuAddItem(ptlcutoffmenu,
                    eduiItemSliderCreate(0, edblack, 0, cbPtlChangeSoundCutOff, 0.0f, 25.0f * edptl_superscale,
                                         effect->sound_range_override, "SoundCutOff Rad"));
    eduiMenuAddItem(ptlcutoffmenu,
                    eduiItemSliderCreate(0, edblack, 0, cbPtlChangeCameraCutOff, 0.0f, 5.0f * edptl_superscale,
                                         effect->field_044, "CameraCutOff Rad"));
    eduiMenuAttach(parent, ptlcutoffmenu);
    ptlcutoffmenu->x = parent->x + 10;
    ptlcutoffmenu->y = parent->y + 40;
}

void cbPtlSelType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    ptltypemenu = NULL;
    edpp_active_menu = NULL;
    edpp_create_type = item->data;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}
void edppRender() {
    edcamSet();
    edppDrawCursor();
    edppHighlightNearest();
    if (edpp_active_menu != NULL)
        eduiMenuRender(edpp_active_menu);
}
void cbEffectListMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    effectlistmenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbCancelEffectListMenu, "Effect List");
    if (effectlistmenu != NULL) {
        eduiMenuAddItem(effectlistmenu,
                        eduiItemCheckCreate(0, colours, edpp_effect_list == 0, 1, cbSelEffectList, "General"));
        eduiMenuAddItem(effectlistmenu,
                        eduiItemCheckCreate(1, colours, edpp_effect_list == 1, 1, cbSelEffectList, "Level"));
        eduiMenuAddItem(effectlistmenu,
                        eduiItemCheckCreate(5, colours, edpp_effect_list == 5, 1, cbSelEffectList, "Char"));
        eduiMenuAttach(parent, effectlistmenu);
        effectlistmenu->x = parent->x + 10;
        effectlistmenu->y = parent->y + 40;
    }
}
static void edptlcbTestDetailMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    edptl_testdetail_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edptlcbCancelTestDetailMenu, "Detail Level Test");
    if (edptl_testdetail_menu == NULL)
        return;
    eduiMenuAddItem(edptl_testdetail_menu, eduiItemSliderCreate(0, edblack, 0, edptlcbSetDebrisThinning, 1.0f, 9.0f,
                                                                debris_thinning_level, "Thinning Level"));
    eduiMenuAddItem(edptl_testdetail_menu, eduiItemCheckCreate(4, edblack, debris_detail_level == 4, 1,
                                                               edptlcbSetDebrisDetail, "High Detail"));
    eduiMenuAddItem(edptl_testdetail_menu, eduiItemCheckCreate(2, edblack, debris_detail_level == 2, 1,
                                                               edptlcbSetDebrisDetail, "Medium Detail"));
    eduiMenuAddItem(edptl_testdetail_menu,
                    eduiItemCheckCreate(1, edblack, debris_detail_level == 1, 1, edptlcbSetDebrisDetail, "Low Detail"));
    eduiMenuAttach(parent, edptl_testdetail_menu);
    edptl_testdetail_menu->x = parent->x + 10;
    edptl_testdetail_menu->y = parent->y + 40;
}
void edptlcbDpadModeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    dpadmodemenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edptlcbCancelDpadModeMenu, "Dpad Mode");
    if (dpadmodemenu == NULL)
        return;
    eduiMenuAddItem(dpadmodemenu,
                    eduiItemCheckCreate(0, colours, edpp_dpad_mode == 0, 1, edptlcbSetDpadMode, "Emitter Rotate"));
    eduiMenuAddItem(dpadmodemenu,
                    eduiItemCheckCreate(1, colours, edpp_dpad_mode == 1, 1, edptlcbSetDpadMode, "Gravity Rotate"));
    eduiMenuAddItem(dpadmodemenu,
                    eduiItemCheckCreate(2, colours, edpp_dpad_mode == 2, 1, edptlcbSetDpadMode, "Offset"));
    eduiMenuAddItem(dpadmodemenu,
                    eduiItemCheckCreate(3, colours, edpp_dpad_mode == 3, 1, edptlcbSetDpadMode, "Reflections"));
    eduiMenuAddItem(dpadmodemenu,
                    eduiItemCheckCreate(4, colours, edpp_dpad_mode == 4, 1, edptlcbSetDpadMode, "Texture Facing"));
    eduiMenuAttach(parent, dpadmodemenu);
    dpadmodemenu->x = parent->x + 10;
    dpadmodemenu->y = parent->y + 40;
}
static void edptlcbOrphanListMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 index;
    edptl_orphanlist_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edptlcbCancelOrphanListMenu, "Orphan List");
    if (edptl_orphanlist_menu == NULL)
        return;
    for (index = 0; index < 512; ++index) {
        if (edpp_ptls[index].effect_index != -1)
            continue;
        eduiMenuAddItem(edptl_orphanlist_menu, eduiItemSelCreate(0, edblack, 0, 0, NULL, edpp_ptls[index].name));
        edui_last_item->text_alignment = 0x10;
    }
    eduiMenuAttach(parent, edptl_orphanlist_menu);
    edptl_orphanlist_menu->x = parent->x + 10;
    edptl_orphanlist_menu->y = parent->y + 40;
}
static void cbPtlDamageMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1) {
        return;
    }
    edptl_damage_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, cbPtlCancelDamageMenu, "Particle Damage");
    if (edptl_damage_menu != NULL) {
        eduiMenuAddItem(edptl_damage_menu, eduiItemSelCreate(1, edblack, 0, 0, cbPtlDamageFlagMenu, "Damage Flags..."));
        eduiMenuAddItem(edptl_damage_menu, eduiItemSelCreate(1, edblack, 0, 0, cbPtlCollMenu, "Collision Spheres..."));
        eduiMenuAddItem(edptl_damage_menu, eduiItemSelCreate(1, edblack, 0, 0, cbPtlTorusMenu, "Collision Torus..."));
    }
    eduiMenuAttach(menu, edptl_damage_menu);
    edptl_damage_menu->x = menu->x + 10;
    edptl_damage_menu->y = menu->y + 40;
}

void __cdecl edppDeleteEffect(i32 index) {
    i32 replacement, i;
    if (edpp_ptls[edpp_nearest].effect_index == index)
        edpp_nearest = -1;
    DebFreeOrphansInstantly(debtab[index]);
    replacement = LookupDebrisEffectPageIgnore(debtab[index]->name, 1, index);
    for (i = 0; i < 512; ++i) {
        EDPP_PARTICLE_PC *particle = &edpp_ptls[i];
        if (particle->effect_index != index)
            continue;
        if (replacement == -1) {
            if (particle->instance_id == -1)
                continue;
            if (particle->instance_id != 99999)
                DebFreeInstantly(&particle->instance_id);
            --edpp_instances_used;
            particle->instance_id = -1;
        } else {
            i32 handle = particle->instance_id;
            if (handle != 99999 && handle != -1)
                debkeydata[handle].effect_index = (i16)replacement;
            particle->effect_index = replacement;
        }
    }
    for (i = 0; i < maxdebkeys; ++i) {
        if (debkeydata[i].effect_index == index) {
            if (replacement == -1) {
                i32 handle = i;
                DebFreeInstantly(&handle);
            } else
                debkeydata[i].effect_index = (i16)replacement;
        }
    }
    debtab[index] = NULL;
    --edpp_types_used;
    edppDetermineNearest(1.0f);
}
void cbPtlDeleteEffect(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent;
    edppDeleteEffect(edpp_create_type);
    edpp_create_type = -1;
    parent = menu->parent;
    if (parent) {
        eduiMenuDetach(menu);
    }
    if (menu->callback) {
        menu->callback(menu, parent);
    }
}
void cbPtlQuickDeleteType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    struct eduimenu_s *parent;
    edppDeleteEffect(item->data);
    parent = menu->parent;
    if (parent) {
        eduiMenuDetach(menu);
    }
    if (menu->callback) {
        menu->callback(menu, parent);
    }
}
void edptlcbEmptyClipboard(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent;
    i32 create_type = edpp_create_type;
    edpp_create_type = edptl_clipboard_entry;
    edppDeleteEffect(edpp_create_type);
    edpp_create_type = -1;

    parent = menu->parent;
    if (parent != NULL) {
        eduiMenuDetach(menu);
    }
    if (menu->callback != NULL) {
        menu->callback(menu, parent);
    }

    edptl_clipboard_entry = -1;
    edpp_create_type = create_type;
}

extern PC_SCENE_MTL_VIEW *edpp_mtl, *edpp_boxmtl;
extern i32 edpp_copy_mode, edpp_copy_source_count, edpp_copy_enclosed, edpp_copyrotz, edpp_copyroty;
extern f32 edpp_offset;
void __cdecl edbitsDrawCube(f32 x, f32 y, f32 z, f32 half_x, f32 half_y, f32 half_z, i32 rotation_z, i32 rotation_y,
                            i32 rotation_x, i32 outer_z, i32 outer_y, i32 colour, NUMTL *material);
void __cdecl edbitsDrawBasicCube(f32 x, f32 y, f32 z, f32 half_x, f32 half_y, f32 half_z, i32 rotation_x,
                                 i32 rotation_y, i32 rotation_z, i32 colour, NUMTL *material);
void __cdecl edbitsDrawSphere(NUVEC *centre, f32 radius, i32 colour, NUMTL *material);
void __cdecl edbitsDrawTorus(NUVEC *centre, f32 radius, f32 radial_extent, f32 vertical_extent, i32 colour,
                             NUMTL *material);

void __cdecl edppDrawSpheres(DEBRIS_INFO_PC *effect, i32 key_index) {
    i32 sphere_index, first, next;
    f32 time, radius;
    NUVEC position;
    if (effect->process_spheres == 0)
        return;
    for (sphere_index = 0; sphere_index < effect->process_spheres; ++sphere_index) {
        time = globaltime - debkeydata[key_index].process_spheres[sphere_index].time;
        if (time >= 0.0f && time <= effect->particle_lifetime) {
            position.x = debkeydata[key_index].process_spheres[sphere_index].momentum.x * time;
            position.y = debkeydata[key_index].process_spheres[sphere_index].momentum.y * time;
            position.z = debkeydata[key_index].process_spheres[sphere_index].momentum.z * time;
            position.y += time * time * effect->gravity;
            position.x += debkeydata[key_index].process_spheres[sphere_index].position.x;
            position.y += debkeydata[key_index].process_spheres[sphere_index].position.y;
            position.z += debkeydata[key_index].process_spheres[sphere_index].position.z;
            time /= effect->particle_lifetime;
            for (first = 0; first < 7; ++first) {
                if (time >= effect->collision_keys[first].time && time <= effect->collision_keys[first + 1].time)
                    break;
            }
            next = first + 1;
            radius = ((time - effect->collision_keys[first].time) /
                      (effect->collision_keys[next].time - effect->collision_keys[first].time)) *
                         (effect->collision_keys[next].value - effect->collision_keys[first].value) +
                     effect->collision_keys[first].value;
            edbitsDrawSphere(&position, radius, 0xffff0000, (NUMTL *)edpp_mtl);
        }
    }
}

void __cdecl edppDrawTorus(DEBRIS_INFO_PC *effect, i32 key_index) {
    f32 time, radius, radial_extent, vertical_extent, delta, range;
    i32 index;
    NUVEC position;
    if (effect->torus_lifetime == 0.0f)
        return;
    radius = 0.0f;
    radial_extent = 0.0f;
    vertical_extent = 0.0f;
    if (globaltime > debkeydata[key_index].emission_time)
        time = globaltime - debkeydata[key_index].emission_time;
    else
        time = globaltime - debkeydata[key_index].previous_emission_time;
    if (!(time > 0.0f && time < effect->torus_lifetime))
        return;
    time /= effect->torus_lifetime;
    for (index = 0; index < 7; ++index) {
        if (time >= effect->torus_keys1[index].time && time <= effect->torus_keys1[index + 1].time) {
            delta = time - effect->torus_keys1[index].time;
            range = effect->torus_keys1[index + 1].time - effect->torus_keys1[index].time;
            radius = delta / range * (effect->torus_keys1[index + 1].value - effect->torus_keys1[index].value) +
                     effect->torus_keys1[index].value;
            break;
        }
    }
    for (index = 0; index < 7; ++index) {
        if (time >= effect->torus_keys2[index].time && time <= effect->torus_keys2[index + 1].time) {
            delta = time - effect->torus_keys2[index].time;
            range = effect->torus_keys2[index + 1].time - effect->torus_keys2[index].time;
            radial_extent = delta / range * (effect->torus_keys2[index + 1].value - effect->torus_keys2[index].value) +
                            effect->torus_keys2[index].value;
            break;
        }
    }
    for (index = 0; index < 7; ++index) {
        if (time >= effect->torus_keys3[index].time && time <= effect->torus_keys3[index + 1].time) {
            delta = time - effect->torus_keys3[index].time;
            range = effect->torus_keys3[index + 1].time - effect->torus_keys3[index].time;
            vertical_extent =
                delta / range * (effect->torus_keys3[index + 1].value - effect->torus_keys3[index].value) +
                effect->torus_keys3[index].value;
            break;
        }
    }
    position.x = debkeydata[key_index].position.x;
    position.y = debkeydata[key_index].position.y;
    position.z = debkeydata[key_index].position.z;
    edbitsDrawTorus(&position, radius * effect->torus_radius1, radial_extent * effect->torus_radius2,
                    vertical_extent * effect->torus_radius2, 0xffff0000, (NUMTL *)edpp_mtl);
}

#define EDPP_ROTATE()                                                                                                  \
    NuVecRotateZ(&vector, &vector, rotation_z);                                                                        \
    NuVecRotateY(&vector, &vector, rotation_y)
#define EDPP_LINE_FROM_CAM(colour_value)                                                                               \
    line[0].position.x = edpp_cam_pos.x;                                                                               \
    line[0].position.y = edpp_cam_pos.y;                                                                               \
    line[0].position.z = edpp_cam_pos.z;                                                                               \
    line[1].position.x = edpp_cam_pos.x + vector.x;                                                                    \
    line[1].position.y = edpp_cam_pos.y + vector.y;                                                                    \
    line[1].position.z = edpp_cam_pos.z + vector.z;                                                                    \
    line[0].colour = colour_value;                                                                                     \
    line[1].colour = colour_value;                                                                                     \
    NuRndrLine3d(line, (NUMTL *)edpp_mtl, NULL)
#define EDPP_DRAW_AXIS()                                                                                               \
    EDPP_ROTATE();                                                                                                     \
    line[0].position.x = edpp_cam_pos.x - vector.x;                                                                    \
    line[0].position.y = edpp_cam_pos.y - vector.y;                                                                    \
    line[0].position.z = edpp_cam_pos.z - vector.z;                                                                    \
    line[0].colour = 0xffffffff;                                                                                       \
    line[1].position.x = edpp_cam_pos.x;                                                                               \
    line[1].position.y = edpp_cam_pos.y;                                                                               \
    line[1].position.z = edpp_cam_pos.z;                                                                               \
    line[1].colour = 0xffffffff;                                                                                       \
    NuRndrLine3d(line, (NUMTL *)edpp_mtl, NULL);                                                                       \
    line[0].position.x = edpp_cam_pos.x;                                                                               \
    line[0].position.y = edpp_cam_pos.y;                                                                               \
    line[0].position.z = edpp_cam_pos.z;                                                                               \
    line[0].colour = 0xff00ff00;                                                                                       \
    line[1].position.x = edpp_cam_pos.x + vector.x;                                                                    \
    line[1].position.y = edpp_cam_pos.y + vector.y;                                                                    \
    line[1].position.z = edpp_cam_pos.z + vector.z;                                                                    \
    line[1].colour = 0xff00ff00;                                                                                       \
    NuRndrLine3d(line, (NUMTL *)edpp_mtl, NULL)
#define EDPP_DRAW_MARK(sx, sy, sz, ex, ey, ez)                                                                         \
    vector.x = sx;                                                                                                     \
    vector.y = sy;                                                                                                     \
    vector.z = sz;                                                                                                     \
    EDPP_ROTATE();                                                                                                     \
    line[0].position.x = edpp_cam_pos.x + vector.x;                                                                    \
    line[0].position.y = edpp_cam_pos.y + vector.y;                                                                    \
    line[0].position.z = edpp_cam_pos.z + vector.z;                                                                    \
    line[0].colour = 0xff00ff00;                                                                                       \
    vector.x = ex;                                                                                                     \
    vector.y = ey;                                                                                                     \
    vector.z = ez;                                                                                                     \
    EDPP_ROTATE();                                                                                                     \
    line[1].position.x = edpp_cam_pos.x + vector.x;                                                                    \
    line[1].position.y = edpp_cam_pos.y + vector.y;                                                                    \
    line[1].position.z = edpp_cam_pos.z + vector.z;                                                                    \
    line[1].colour = 0xff00ff00;                                                                                       \
    NuRndrLine3d(line, (NUMTL *)edpp_mtl, NULL)

void __cdecl edppDrawCursor(void) {
    i32 rotation_z, rotation_y;
    NUVEC vector;
    NURND_VERTEX3D_PC line[2];
    DEBRISKEY_PC *key;
    DEBRIS_INFO_PC *effect;
    i32 group;

    if (edpp_copy_mode != 0) {
        rotation_z = edpp_copyrotz;
        rotation_y = edpp_copyroty;
    } else {
        rotation_z = edpp_rotz;
        rotation_y = edpp_roty;
    }
    vector.x = 0.5f;
    vector.y = 0.0f;
    vector.z = 0.0f;
    EDPP_DRAW_AXIS();
    vector.x = 0.0f;
    vector.y = 0.5f;
    vector.z = 0.0f;
    EDPP_DRAW_AXIS();
    vector.x = 0.0f;
    vector.y = 0.0f;
    vector.z = 0.5f;
    EDPP_DRAW_AXIS();
    EDPP_DRAW_MARK(0.55f, 0.05f, 0.0f, 0.6f, -0.05f, 0.0f);
    EDPP_DRAW_MARK(0.6f, 0.05f, 0.0f, 0.55f, -0.05f, 0.0f);
    EDPP_DRAW_MARK(0.0f, 0.65f, -0.025f, 0.0f, 0.6f, 0.0f);
    EDPP_DRAW_MARK(0.0f, 0.65f, 0.025f, 0.0f, 0.6f, 0.0f);
    EDPP_DRAW_MARK(0.0f, 0.6f, 0.0f, 0.0f, 0.55f, 0.0f);
    EDPP_DRAW_MARK(0.0f, 0.05f, 0.55f, 0.0f, 0.05f, 0.6f);
    EDPP_DRAW_MARK(0.0f, 0.05f, 0.6f, 0.0f, -0.05f, 0.55f);
    EDPP_DRAW_MARK(0.0f, -0.05f, 0.55f, 0.0f, -0.05f, 0.6f);

    if (edpp_copy_mode != 0) {
        edbitsDrawCube(edpp_cam_pos.x, edpp_cam_pos.y, edpp_cam_pos.z, edpp_copy_size, edpp_copy_size, edpp_copy_size,
                       0, 0, 0, 0, 0, 0xffffffff, (NUMTL *)edpp_mtl);
    } else {
        vector.x = 0.0f;
        vector.y = 0.375f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, edpp_emitrotz);
        NuVecRotateY(&vector, &vector, edpp_emitroty);
        NuVecRotateX(&vector, &vector, edpp_emitrotx);
        EDPP_ROTATE();
        EDPP_LINE_FROM_CAM(0xff0000ff);
        vector.x = 0.0f;
        vector.y = 0.375f;
        vector.z = 0.125f;
        NuVecRotateZ(&vector, &vector, edpp_emitrotz);
        NuVecRotateY(&vector, &vector, edpp_emitroty);
        NuVecRotateX(&vector, &vector, edpp_emitrotx);
        EDPP_ROTATE();
        line[0].position.x = edpp_cam_pos.x + vector.x;
        line[0].position.y = edpp_cam_pos.y + vector.y;
        line[0].position.z = edpp_cam_pos.z + vector.z;
        line[0].colour = 0xff0000ff;
        NuRndrLine3d(line, (NUMTL *)edpp_mtl, NULL);
        if (edpp_dpad_mode == 3) {
            vector.x = 0.0f;
            vector.y = 0.25f;
            vector.z = 0.0f;
            NuVecRotateZ(&vector, &vector, edpp_refrotz);
            NuVecRotateY(&vector, &vector, edpp_refroty);
            EDPP_ROTATE();
            EDPP_LINE_FROM_CAM(0xffff0000);
            if (edpp_nearest != -1) {
                vector.x = 0.0f;
                vector.y = edpp_ptls[edpp_nearest].reflection_offset;
                vector.z = 0.0f;
                NuVecRotateZ(&vector, &vector, edpp_refrotz);
                NuVecRotateY(&vector, &vector, edpp_refroty);
                EDPP_ROTATE();
                edbitsDrawCube(edpp_cam_pos.x + vector.x, edpp_cam_pos.y + vector.y, edpp_cam_pos.z + vector.z, 0.25f,
                               0.0f, 0.25f, edpp_refrotz, edpp_refroty, 0, rotation_z, rotation_y, 0xffff0000,
                               (NUMTL *)edpp_mtl);
            }
        }
        if (edpp_dpad_mode == 4) {
            vector.x = 0.0f;
            vector.y = 0.0f;
            vector.z = 0.25f;
            NuVecRotateX(&vector, &vector, edpp_facrotx);
            NuVecRotateY(&vector, &vector, edpp_facroty);
            EDPP_LINE_FROM_CAM(0xffff0000);
            edbitsDrawBasicCube(edpp_cam_pos.x, edpp_cam_pos.y, edpp_cam_pos.z, 0.25f, 0.25f, 0.0f, edpp_facrotx,
                                edpp_facroty, 0, 0xffff0000, (NUMTL *)edpp_mtl);
        }
    }

    NuRndrRect2di(0x1680, 0x9b0, 4000, 0x410, 0x80808080, (NUMTL *)edpp_boxmtl);
    NuRndrRect2di(0x1670, 0x8f0, 0xfc0, 0xc0, 0x80000000, (NUMTL *)edpp_boxmtl);
    NuRndrLine2di(0x1670, 0x9b0, 0x1670, 0xdc8, 0x80000000, (NUMTL *)edpp_boxmtl);
    NuRndrLine2di(0x2630, 0x9b0, 0x2630, 0xdc8, 0x80000000, (NUMTL *)edpp_boxmtl);
    NuRndrLine2di(0x1670, 0xdc8, 0x2630, 0xdc8, 0x80000000, (NUMTL *)edpp_boxmtl);
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(NUQFNT_CSMODE_PS2);
    NuQFntSet(system_qfont);
    if (edpp_readout == 0) {
        if (globalframes % 200 > 100) {
            if (edpp_num_orphans > 0) {
                NuQFntSetColour(system_qfont, 0x80202080);
                NuQFntPrintEx(system_qfont, 0x1720, 0x988, 0x10, "WARNING: %d Orphan(s)", edpp_num_orphans);
            } else {
                NuQFntSetColour(system_qfont, 0xe0e0e0e0);
                NuQFntPrintEx(system_qfont, 0x1720, 0x988, 0x10, "Info Box");
            }
        } else {
            NuQFntSetColour(system_qfont, 0xe0e0e0e0);
            NuQFntPrintEx(system_qfont, 0x1720, 0x988, 0x10, "Info Box");
        }
    } else if (edpp_readout == 1) {
        NuQFntSetColour(system_qfont, 0x80808080);
        NuQFntPrintEx(system_qfont, 0x1720, 0x988, 0x10, "Co-ordinates");
    }
    NuQFntSetColour(system_qfont, 0x80000000);
    if (edpp_readout == 0) {
        if (edpp_copy_mode == 0) {
            if (edpp_effect_list == 1)
                NuQFntPrintEx(system_qfont, 0x1720, 0xa50, 0x10, "Current List: Level");
            else if (edpp_effect_list == 0)
                NuQFntPrintEx(system_qfont, 0x1720, 0xa50, 0x10, "Current List: General");
            else if (edpp_effect_list == 5)
                NuQFntPrintEx(system_qfont, 0x1720, 0xa50, 0x10, "Current List: Character");
            if (edpp_create_type == -1)
                NuQFntPrintEx(system_qfont, 0x1720, 0xaf0, 0x10, "Current Type: <none>");
            else
                NuQFntPrintEx(system_qfont, 0x1720, 0xaf0, 0x10, "Current Type: %s", debtab[edpp_create_type]->name);
        }
        if (edpp_copy_mode != 0) {
            if (edpp_copy_source_count == 0)
                NuQFntPrintEx(system_qfont, 0x1720, 0xb90, 0x10, "Clipboard: <none>");
            else
                NuQFntPrintEx(system_qfont, 0x1720, 0xb90, 0x10, "Clipboard: %d items", edpp_copy_source_count);
        } else {
            if (edptl_clipboard_entry == -1)
                NuQFntPrintEx(system_qfont, 0x1720, 0xb90, 0x10, "Clipboard: <none>");
            else
                NuQFntPrintEx(system_qfont, 0x1720, 0xb90, 0x10, "Clipboard: %s", debtab[edptl_clipboard_entry]->name);
        }
        if (edpp_copy_mode != 0) {
            if (edpp_copy_enclosed > 8)
                NuQFntSetColour(system_qfont, 0x80000080);
            NuQFntPrintEx(system_qfont, 0x1720, 0xc30, 0x10, "Enclosed: %d", edpp_copy_enclosed);
            NuQFntSetColour(system_qfont, 0x80000000);
        } else {
            if (edpp_nearest == -1) {
                NuQFntPrintEx(system_qfont, 0x1720, 0xc30, 0x10, "Highlight: <none>");
            } else {
                effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
                if (effect->generator_type == 0) {
                    edbitsDrawCube(edpp_cam_pos.x, edpp_cam_pos.y, edpp_cam_pos.z, effect->emitter_random_extent.x,
                                   effect->emitter_random_extent.y, effect->emitter_random_extent.z, edpp_emitrotz,
                                   edpp_emitroty, edpp_emitrotx, rotation_z, rotation_y, 0xff0000ff, (NUMTL *)edpp_mtl);
                    edppDrawSpheres(effect, edpp_ptls[edpp_nearest].instance_id);
                }
                edppDrawTorus(effect, edpp_ptls[edpp_nearest].instance_id);
                NuQFntSet(system_qfont);
                NuQFntPrintEx(system_qfont, 0x1720, 0xc30, 0x10, "Highlight: %s", effect->name);
                NuQFntPrintEx(system_qfont, 0x1720, 0xcd0, 0x10, "Particles:");
                group = (effect->particle_type != 7) ? 32 : 12;
                if (effect->max_particles > group * 32)
                    NuQFntSetColour(system_qfont, 0x80000080);
                NuQFntPrintEx(system_qfont, 0x1cc0, 0xcd0, 0x10, "%d (%d)", effect->max_particles,
                              ((effect->max_particles - 1) / group + 1) * group);
                NuQFntSetColour(system_qfont, 0x80000000);
            }
        }
        if (edpp_copy_mode != 0)
            NuQFntPrintEx(system_qfont, 0x1720, 0xd70, 0x10, "Multiple Copy Mode");
        else if (edpp_dpad_mode == 0)
            NuQFntPrintEx(system_qfont, 0x1720, 0xd70, 0x10, "Dpad Mode: Emit Rotate");
        else if (edpp_dpad_mode == 1)
            NuQFntPrintEx(system_qfont, 0x1720, 0xd70, 0x10, "Dpad Mode: Grav Rotate");
        else if (edpp_dpad_mode == 2)
            NuQFntPrintEx(system_qfont, 0x1720, 0xd70, 0x10, "Dpad Mode: Offset (%0.2f)", edpp_offset);
        else if (edpp_dpad_mode == 3)
            NuQFntPrintEx(system_qfont, 0x1720, 0xd70, 0x10, "Dpad Mode: Reflections");
        else if (edpp_dpad_mode == 4)
            NuQFntPrintEx(system_qfont, 0x1720, 0xd70, 0x10, "Dpad Mode: Facing");
    } else if (edpp_readout == 1) {
        if (edpp_nearest == -1) {
            NuQFntPrintEx(system_qfont, 0x1720, 0xa50, 0x10, "Highlight: <none>");
        } else {
            key = &debkeydata[edpp_ptls[edpp_nearest].instance_id];
            NuQFntPrintEx(system_qfont, 0x1720, 0xa50, 0x10, "Highlight: %s", debtab[key->effect_index]->name);
            NuQFntPrintEx(system_qfont, 0x1720, 0xaf0, 0x10, "XYZ: %0.2f %0.2f %0.2f", key->position.x, key->position.y,
                          key->position.z);
            NuQFntPrintEx(system_qfont, 0x1720, 0xb90, 0x10, "RotZ: %d", edpp_ptls[edpp_nearest].rotation_z);
            NuQFntPrintEx(system_qfont, 0x1720, 0xc30, 0x10, "RotY: %d", edpp_ptls[edpp_nearest].rotation_y);
            NuQFntPrintEx(system_qfont, 0x1720, 0xcd0, 0x10, "EmitRotZ: %d",
                          edpp_ptls[edpp_nearest].emitter_rotation_z);
            NuQFntPrintEx(system_qfont, 0x1720, 0xd70, 0x10, "EmitRotY: %d",
                          edpp_ptls[edpp_nearest].emitter_rotation_y);
        }
    }
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
}

extern i32 edpp_copy_source[8];
extern NUVEC edpp_copy_source_vec;
extern i32 edpp_snap_enabled, edpp_cam_ax, edpp_cam_ay;
extern eduimenu_s *ptloptmenu;
void __cdecl edcamMove(FRAME_PAD_PC *pad);
void __cdecl edcamGetPosAng(NUVEC *position, i32 *angle_x, i32 *angle_y);
void __cdecl edcamGetPosAngSnap(NUVEC *position, i32 *angle_x, i32 *angle_y);

typedef union EDPP_FLOAT_BITS_PC {
    f32 value;
    u32 bits;
} EDPP_FLOAT_BITS_PC;

static __inline f32 edppFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    f32 result = *(f32 *)&bits;
    return result;
}

static __inline void edppDebrisEmitterPos(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].position.x = x;
        debkeydata[handle].position.y = y;
        debkeydata[handle].position.z = z;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

static __inline void edppDebrisSetGroupID(i32 handle, i16 group) {
    if (handle != -1)
        debkeydata[handle].render_group = group;
}

static __inline void edppDebrisSetPriority(i32 handle, u16 priority, u8 automatic) {
    if (handle != -1) {
        debkeydata[handle].timed_flags = automatic;
        if (automatic == 0)
            debkeydata[handle].render_priority = priority;
    }
}

static __inline void edppDebrisClearRoomID(i32 handle) {
    debkeydata[handle].field_2f2 = -1;
    debkeydata[handle].gscene = NULL;
}

static __inline void edppDebrisSetDetailLevels(i32 handle, i32 detail_levels) {
    debkeydata[handle].orientation_flags = (u8)detail_levels;
}

static __inline void edppDebrisSetTrigger(i32 handle, i32 first, i32 second, f32 variable) {
    if (handle != -1) {
        debkeydata[handle].trigger_first = first;
        debkeydata[handle].trigger_second = second;
        debkeydata[handle].switch_variable = variable;
    }
}

static __inline void edppDebrisReflectionOrientation(i32 handle, i16 x, i16 y, f32 plane, f32 scale) {
    if (handle != -1) {
        debkeydata[handle].reflection_x = x;
        debkeydata[handle].reflection_y = y;
        debkeydata[handle].collision_plane = plane;
        debkeydata[handle].reflection_scale = scale;
    }
}

i32 __cdecl edppPtlCreate(NUVEC *position, i32 effect_index) {
    i32 index = 0;
    u8 particle_type;
    if (edpp_instances_used == 512)
        return -1;
    while (edpp_ptls[index].instance_id != -1)
        ++index;
    AddDebrisEffect(&edpp_ptls[index].instance_id, effect_index, position->x, position->y, position->z);
    if (edpp_ptls[index].instance_id != -1) {
        debkeydata[edpp_ptls[index].instance_id].field_2f9 = 0;
        edpp_ptls[index].reflection_offset = 0.0f;
        edpp_ptls[index].render_group = 0;
        edpp_ptls[index].reflection_bounce = 0.9f;
        edpp_ptls[index].position = *position;
        edpp_ptls[index].start_offset = edpp_offset;
        edpp_ptls[index].switch_variable = 0.0f;
        edpp_ptls[index].rotation_z = edpp_rotz;
        edpp_ptls[index].rotation_y = edpp_roty;
        edpp_ptls[index].emitter_rotation_z = edpp_emitrotz;
        edpp_ptls[index].emitter_rotation_y = edpp_emitroty;
        edpp_ptls[index].effect_index = effect_index;
        edpp_ptls[index].emitter_rotation_x = edpp_emitrotx;
        edpp_ptls[index].page = edbits_particle_level_page;
        edpp_ptls[index].switch_type = 0;
        edpp_ptls[index].switch_id = -1;
        edpp_ptls[index].detail_levels = 7;
        particle_type = debtab[effect_index]->particle_type;
        if (particle_type == 2)
            edpp_ptls[index].render_priority = (i16)40000;
        else if (particle_type == 3)
            edpp_ptls[index].render_priority = 30000;
        else if (particle_type == 0)
            edpp_ptls[index].render_priority = 20000;
        else if (particle_type == 7)
            edpp_ptls[index].render_priority = 10000;
        edpp_ptls[index].dynamic_priority = 0;
        edpp_ptls[index].facing_mode = 0;
        edpp_ptls[index].facing_rotation_x = 0;
        edpp_ptls[index].facing_rotation_y = 0;
        strcpy(edpp_ptls[index].name, debtab[edpp_ptls[index].effect_index]->name);
        DebrisOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].rotation_z, edpp_ptls[index].rotation_y);
        DebrisEmitterOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].emitter_rotation_z,
                                 edpp_ptls[index].emitter_rotation_y, edpp_ptls[index].emitter_rotation_x);
        DebrisStartOffsetEx(&debkeydata[edpp_ptls[index].instance_id], edpp_ptls[index].start_offset);
        DebrisReflectionOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].reflection_rotation_z,
                                    edpp_ptls[index].reflection_rotation_y, edpp_ptls[index].reflection_offset,
                                    edpp_ptls[index].reflection_bounce);
        DebrisSetFacing(edpp_ptls[index].instance_id, edpp_ptls[index].facing_mode, edpp_ptls[index].facing_rotation_x,
                        edpp_ptls[index].facing_rotation_y);
        edppDebrisSetGroupID(edpp_ptls[index].instance_id, edpp_ptls[index].render_group);
        edppDebrisSetPriority(edpp_ptls[index].instance_id, edpp_ptls[index].render_priority,
                              edpp_ptls[index].dynamic_priority);
        edppDebrisClearRoomID(edpp_ptls[index].instance_id);
        edppDebrisSetDetailLevels(edpp_ptls[index].instance_id, edpp_ptls[index].detail_levels);
        ++edpp_instances_used;
        edpp_page_used[edbits_particle_level_page] = 1;
        edpp_page_on[edbits_particle_level_page] = 1;
        if (EdppPageScenesPc[edbits_particle_level_page] == NULL)
            EdppPageScenesPc[edbits_particle_level_page] = edbits_base_scene;
        return index;
    }
    return -1;
}

i32 __cdecl edppPtlCreateCopy(NUVEC *position, i32 source_index) {
    i32 index = 0;
    if (edpp_instances_used == 512)
        return -1;
    while (edpp_ptls[index].instance_id != -1)
        ++index;
    AddDebrisEffect(&edpp_ptls[index].instance_id, edpp_ptls[source_index].effect_index, position->x, position->y,
                    position->z);
    if (edpp_ptls[index].instance_id != -1) {
        debkeydata[edpp_ptls[index].instance_id].field_2f9 = 0;
        edpp_ptls[index].render_group = edpp_ptls[source_index].render_group;
        edpp_ptls[index].page = edpp_ptls[source_index].page;
        edpp_ptls[index].reflection_offset = edpp_ptls[source_index].reflection_offset;
        edpp_ptls[index].reflection_bounce = edpp_ptls[source_index].reflection_bounce;
        edpp_ptls[index].position = *position;
        edpp_ptls[index].effect_index = edpp_ptls[source_index].effect_index;
        edpp_ptls[index].rotation_z = edpp_ptls[source_index].rotation_z;
        edpp_ptls[index].rotation_y = edpp_ptls[source_index].rotation_y;
        edpp_ptls[index].emitter_rotation_z = edpp_ptls[source_index].emitter_rotation_z;
        edpp_ptls[index].emitter_rotation_y = edpp_ptls[source_index].emitter_rotation_y;
        edpp_ptls[index].start_offset = edpp_ptls[source_index].start_offset;
        edpp_ptls[index].switch_type = edpp_ptls[source_index].switch_type;
        edpp_ptls[index].switch_id = edpp_ptls[source_index].switch_id;
        edpp_ptls[index].switch_variable = edpp_ptls[source_index].switch_variable;
        edpp_ptls[index].detail_levels = edpp_ptls[source_index].detail_levels;
        edpp_ptls[index].facing_mode = edpp_ptls[source_index].facing_mode;
        edpp_ptls[index].facing_rotation_x = edpp_ptls[source_index].facing_rotation_x;
        edpp_ptls[index].facing_rotation_y = edpp_ptls[source_index].facing_rotation_y;
        strcpy(edpp_ptls[index].name, debtab[edpp_ptls[index].effect_index]->name);
        DebrisOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].rotation_z, edpp_ptls[index].rotation_y);
        DebrisEmitterOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].emitter_rotation_z,
                                 edpp_ptls[index].emitter_rotation_y, edpp_ptls[index].emitter_rotation_x);
        DebrisStartOffsetEx(&debkeydata[edpp_ptls[index].instance_id], edpp_ptls[index].start_offset);
        DebrisReflectionOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].reflection_rotation_z,
                                    edpp_ptls[index].reflection_rotation_y, edpp_ptls[index].reflection_offset,
                                    edpp_ptls[index].reflection_bounce);
        DebrisSetFacing(edpp_ptls[index].instance_id, edpp_ptls[index].facing_mode, edpp_ptls[index].facing_rotation_x,
                        edpp_ptls[index].facing_rotation_y);
        edppDebrisSetGroupID(edpp_ptls[index].instance_id, edpp_ptls[index].render_group);
        DebrisSetRoomID(edpp_ptls[index].instance_id, EdppPageScenesPc[edpp_ptls[index].page]);
        edppDebrisSetDetailLevels(edpp_ptls[index].instance_id, edpp_ptls[index].detail_levels);
        ++edpp_instances_used;
        return index;
    }
    return -1;
}

i32 __cdecl edppPtlPlace(i32 index, NUVEC *position) {
    edppDebrisEmitterPos(edpp_ptls[index].instance_id, position->x, position->y, position->z);
    DebrisOrientation(edpp_ptls[index].instance_id, edpp_rotz, edpp_roty);
    DebrisEmitterOrientation(edpp_ptls[index].instance_id, edpp_emitrotz, edpp_emitroty, edpp_emitrotx);
    DebrisReflectionOrientation(edpp_ptls[index].instance_id, edpp_refrotz, edpp_refroty,
                                edpp_ptls[index].reflection_offset, edpp_ptls[index].reflection_bounce);
    if (edpp_ptls[index].facing_mode != 0) {
        edpp_ptls[index].facing_rotation_x = edpp_facrotx;
        edpp_ptls[index].facing_rotation_y = edpp_facroty;
        DebrisSetFacing(edpp_ptls[index].instance_id, edpp_ptls[index].facing_mode, edpp_ptls[index].facing_rotation_x,
                        edpp_ptls[index].facing_rotation_y);
    }
    edpp_ptls[index].position = *position;
    edpp_ptls[index].emitter_rotation_z = edpp_emitrotz;
    edpp_ptls[index].emitter_rotation_y = edpp_emitroty;
    edpp_ptls[index].emitter_rotation_x = edpp_emitrotx;
    edpp_ptls[index].rotation_z = edpp_rotz;
    edpp_ptls[index].rotation_y = edpp_roty;
    edpp_ptls[index].reflection_rotation_z = edpp_refrotz;
    edpp_ptls[index].reflection_rotation_y = edpp_refroty;
    return index;
}

void __cdecl edppMultipleCopyCopy(void) {
    i32 index;
    NUVEC delta;
    edpp_copy_source_count = 0;
    for (index = 0; index < 512; ++index) {
        if (edpp_ptls[index].instance_id == -1 || edpp_ptls[index].instance_id == 99999)
            continue;
        NuVecSub(&delta, &edpp_cam_pos, &edpp_ptls[index].position);
        if (edppFabs(delta.x) <= edpp_copy_size && edppFabs(delta.y) <= edpp_copy_size &&
            edppFabs(delta.z) <= edpp_copy_size) {
            edpp_copy_source[edpp_copy_source_count++] = index;
            if (edpp_copy_source_count == 8)
                break;
        }
    }
    edpp_copy_source_vec = edpp_cam_pos;
    edpp_copyroty = 0;
}

void __cdecl edppMultipleCopyPaste(void) {
    i32 source, index;
    NUVEC offset, position;
    if (edpp_copy_source_count == 0)
        return;
    for (source = 0; source < edpp_copy_source_count; ++source) {
        NuVecSub(&offset, &edpp_ptls[edpp_copy_source[source]].position, &edpp_copy_source_vec);
        NuVecRotateY(&offset, &offset, edpp_copyroty);
        NuVecAdd(&position, &edpp_cam_pos, &offset);
        index = edppPtlCreateCopy(&position, edpp_copy_source[source]);
        if (index != -1) {
            edpp_ptls[index].emitter_rotation_y += (i16)edpp_copyroty;
            DebrisEmitterOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].emitter_rotation_z,
                                     edpp_ptls[index].emitter_rotation_y, edpp_ptls[index].emitter_rotation_x);
        }
    }
}

void __cdecl edppDoInput(FRAME_PAD_PC *pad) {
    if ((pad->digital_buttons & 0x100) == 0)
        edcamMove(pad);
    if (pad->digital_buttons & 0x100) {
        if (pad->digital_buttons_pressed & 0x20) {
            if (edpp_copy_mode) {
                edpp_copy_mode = 0;
            } else {
                edpp_copy_mode = 1;
                edpp_copy_source_count = 0;
            }
        }
        if (edpp_nearest != -1) {
            if (pad->digital_buttons_pressed & 8) {
                do {
                    ++edpp_nearest;
                    if (edpp_nearest == 512)
                        edpp_nearest = 0;
                } while (edpp_ptls[edpp_nearest].instance_id == -1 || edpp_ptls[edpp_nearest].instance_id == 99999);
            }
            if (pad->digital_buttons_pressed & 2) {
                do {
                    --edpp_nearest;
                    if (edpp_nearest == -1)
                        edpp_nearest = 511;
                } while (edpp_ptls[edpp_nearest].instance_id == -1 || edpp_ptls[edpp_nearest].instance_id == 99999);
            }
        } else {
            edppDetermineNearest(-1.0f);
        }
        if (edpp_nearest != -1) {
            edcamSetPos(&edpp_ptls[edpp_nearest].position);
            edpp_rotz = edpp_ptls[edpp_nearest].rotation_z;
            edpp_roty = edpp_ptls[edpp_nearest].rotation_y;
            edpp_emitrotz = edpp_ptls[edpp_nearest].emitter_rotation_z;
            edpp_emitroty = edpp_ptls[edpp_nearest].emitter_rotation_y;
            edpp_emitrotx = edpp_ptls[edpp_nearest].emitter_rotation_x;
            edpp_offset = edpp_ptls[edpp_nearest].start_offset;
            edpp_create_type = edpp_ptls[edpp_nearest].effect_index;
            edpp_effect_list = debtab[edpp_ptls[edpp_nearest].effect_index]->category;
        }
    }

    if (edpp_snap_enabled)
        edcamGetPosAngSnap(&edpp_cam_pos, &edpp_cam_ax, &edpp_cam_ay);
    else
        edcamGetPosAng(&edpp_cam_pos, &edpp_cam_ax, &edpp_cam_ay);

    if ((pad->digital_buttons & 0x100) == 0) {
        if (pad->digital_buttons_pressed & 0x80)
            edpp_active_menu = ptloptmenu;
        if (pad->digital_buttons_pressed & 0x40) {
            if (edpp_copy_mode != 0)
                edppMultipleCopyPaste();
            else if (edpp_create_type != -1)
                edppPtlCreate(&edpp_cam_pos, edpp_create_type);
        }
        if (pad->digital_buttons & 0x20) {
            if (edpp_copy_mode != 0)
                edppMultipleCopyCopy();
            else if (edpp_nearest != -1)
                edppPtlPlace(edpp_nearest, &edpp_cam_pos);
        }
        if (pad->digital_buttons & 0x400) {
            if (edpp_copy_mode == 0 && edpp_nearest != -1)
                edppPtlPlace(edpp_nearest, &edpp_cam_pos);
        }
        if (pad->digital_buttons_pressed & 0x10) {
            if (edpp_copy_mode != 0) {
                edpp_copy_source_count = 0;
                edpp_copyroty = 0;
            } else {
                if (edpp_nearest != -1)
                    edppPtlDestroy(edpp_nearest);
                edpp_nearest = -1;
            }
        }
    }

    if (edpp_copy_mode) {
        edpp_copy_size += (f32)pad->analog_left_pad_up / 5000.0;
        edpp_copy_size -= (f32)pad->analog_left_pad_down / 5000.0;
        if (edpp_copy_size < 0.05f)
            edpp_copy_size = 0.05f;
        else if (edpp_copy_size > 2.0f)
            edpp_copy_size = 2.0f;
        edpp_copyroty += pad->analog_left_pad_right;
        edpp_copyroty -= pad->analog_left_pad_left;
    } else {
        if (edpp_dpad_mode == 1) {
            if (pad->digital_buttons & 0x200)
                edpp_roty = edpp_rotz = 0;
            edpp_roty += pad->analog_left_pad_right;
            edpp_roty -= pad->analog_left_pad_left;
            edpp_rotz += pad->analog_left_pad_up;
            if (edpp_rotz > 0)
                edpp_rotz = 0;
            edpp_rotz -= pad->analog_left_pad_down;
            if (edpp_rotz < -0x8000)
                edpp_rotz = -0x8000;
        } else if (edpp_dpad_mode == 0) {
            if (pad->digital_buttons & 0x200) {
                edpp_emitrotz = 0;
                edpp_emitroty = 0;
                edpp_emitrotx = 0;
            }
            if (pad->digital_buttons & 0x400) {
                edpp_emitrotx += pad->analog_left_pad_right;
                edpp_emitrotx -= pad->analog_left_pad_left;
            } else {
                edpp_emitroty += pad->analog_left_pad_right;
                edpp_emitroty -= pad->analog_left_pad_left;
                edpp_emitrotz += pad->analog_left_pad_up;
                if (edpp_emitrotz > 0x8000)
                    edpp_emitrotz = 0x8000;
                edpp_emitrotz -= pad->analog_left_pad_down;
                if (edpp_emitrotz < -0x8000)
                    edpp_emitrotz = -0x8000;
            }
        } else if (edpp_dpad_mode == 3) {
            edpp_refroty += pad->analog_left_pad_right;
            edpp_refroty -= pad->analog_left_pad_left;
            edpp_refrotz += pad->analog_left_pad_up;
            if (edpp_refrotz > 0)
                edpp_refrotz = 0;
            edpp_refrotz -= pad->analog_left_pad_down;
            if (edpp_refrotz < -0x8000)
                edpp_refrotz = -0x8000;
        } else if (edpp_dpad_mode == 2) {
            if (pad->analog_left_pad_up == 255 || (pad->digital_buttons_pressed & 0x1000))
                edpp_offset += 0.01f;
            if (pad->analog_left_pad_down == 255 || (pad->digital_buttons_pressed & 0x4000))
                edpp_offset -= 0.01f;
            if (edpp_offset < 0.0f)
                edpp_offset = 0.0f;
        } else if (edpp_dpad_mode == 4) {
            edpp_facroty += pad->analog_left_pad_right;
            edpp_facroty -= pad->analog_left_pad_left;
            edpp_facrotx -= pad->analog_left_pad_up;
            if (edpp_facrotx < -0x4000)
                edpp_facrotx = -0x4000;
            edpp_facrotx += pad->analog_left_pad_down;
            if (edpp_facrotx > 0x4000)
                edpp_facrotx = 0x4000;
        }
    }
}

void __cdecl edppHighlightNearest(void) {
    i32 index, source;
    NUVEC delta, offset, position;
    if (edpp_copy_mode != 0) {
        edpp_copy_enclosed = 0;
        for (index = 0; index < 512; ++index) {
            if (edpp_ptls[index].instance_id == -1)
                continue;
            if (edpp_ptls[index].instance_id == 99999)
                continue;
            NuVecSub(&delta, &edpp_cam_pos, &edpp_ptls[index].position);
            if (edppFabs(delta.x) <= edpp_copy_size && edppFabs(delta.y) <= edpp_copy_size &&
                edppFabs(delta.z) <= edpp_copy_size) {
                ++edpp_copy_enclosed;
                edbitsDrawDiagonalCross(edpp_ptls[index].position.x, edpp_ptls[index].position.y,
                                        edpp_ptls[index].position.z, 0.05f, 0xffffffff, (PC_SCENE_MTL_VIEW *)edpp_mtl);
            }
        }
        if (edpp_copy_source_count > 0) {
            for (source = 0; source < edpp_copy_source_count; ++source) {
                NuVecSub(&offset, &edpp_ptls[edpp_copy_source[source]].position, &edpp_copy_source_vec);
                NuVecRotateY(&offset, &offset, edpp_copyroty);
                NuVecAdd(&position, &edpp_cam_pos, &offset);
                edbitsDrawDiagonalCross(position.x, position.y, position.z, 0.05f, 0xff808080,
                                        (PC_SCENE_MTL_VIEW *)edpp_mtl);
            }
        }
    } else {
        if (edpp_nearest != -1) {
            edbitsDrawCube(edpp_ptls[edpp_nearest].position.x, edpp_ptls[edpp_nearest].position.y,
                           edpp_ptls[edpp_nearest].position.z, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, 0, 0xffffffff,
                           (NUMTL *)edpp_mtl);
        }
        if (edpp_showAllPlaced != 0) {
            for (index = 0; index < 512; ++index) {
                if (edpp_ptls[index].effect_index > 0 && index != edpp_nearest) {
                    edbitsDrawCube(edpp_ptls[index].position.x, edpp_ptls[index].position.y,
                                   edpp_ptls[index].position.z, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, 0, 0xff000000,
                                   (NUMTL *)edpp_mtl);
                }
            }
        }
    }
}

void EdFileWrite(void *data, i32 len);
void EdFileWriteInt(i32 value);
void EdFileWriteShort(i16 value);
void EdFileWriteUnsignedShort(u16 value);
void EdFileWriteChar(char value);
void EdFileWriteUnsignedChar(u8 value);
void EdFileWriteNuVec(NUVEC *value);
i32 EdFileBackup(char *source, char *destination);

i32 __cdecl edppSaveEffects(char *filename, u8 page) {
    i32 effect_count = 0;
    i32 instance_count = 0;
    u8 list = page;
    i32 index, key, sound_count;
    if (page == 6)
        page = 1;
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL)
            continue;
        if (page == 2) {
            ++effect_count;
        } else if (page == 1) {
            if (effecttypes[index].category == page && edbits_particle_level_page == effecttypes[index].page)
                ++effect_count;
        } else if (effecttypes[index].category == page) {
            ++effect_count;
        }
    }
    if (EdppUseMemcardPc != 0)
        EdFileSetMedia(2);
    else
        EdFileSetMedia(1);
    if (EdFileOpen(filename, NUFILE_WRITE) != 0) {
        EdFileSetReadWrongEndianess(1);
        EdFileWriteInt(0x29);
        EdFileWriteInt(effect_count);
        for (index = 1; index < debinf_count; ++index) {
            if (debtab[index] == NULL)
                continue;
            if (page != 2) {
                if (page == 1) {
                    if (effecttypes[index].category != page)
                        continue;
                    if (edbits_particle_level_page != effecttypes[index].page)
                        continue;
                } else if (effecttypes[index].category != page) {
                    continue;
                }
            }
            EdFileWrite(effecttypes[index].name, 16);
            EdFileWriteShort(effecttypes[index].frequency);
            EdFileWriteShort(effecttypes[index].max_particles);
            EdFileWriteFloat(effecttypes[index].emission_start_offset);
            EdFileWriteFloat(effecttypes[index].emission_period);
            EdFileWriteFloat(effecttypes[index].emission_period_random);
            EdFileWriteFloat(effecttypes[index].emission_pause);
            EdFileWriteFloat(effecttypes[index].emission_pause_random);
            EdFileWriteChar(effecttypes[index].generator_type);
            EdFileWriteChar(effecttypes[index].momentum_adjustment_type);
            EdFileWriteChar(effecttypes[index].cutscene_only);
            EdFileWriteChar(effecttypes[index].particle_type);
            EdFileWriteChar(effecttypes[index].camera_facing);
            EdFileWriteFloat(effecttypes[index].field_030);
            EdFileWriteFloat(effecttypes[index].cut_on);
            EdFileWriteFloat(effecttypes[index].clip_extent);
            EdFileWriteFloat(effecttypes[index].sound_range);
            EdFileWriteFloat(effecttypes[index].sound_range_override);
            EdFileWriteFloat(effecttypes[index].field_044);
            EdFileWriteFloat(effecttypes[index].momentum_base);
            EdFileWriteNuVec((NUVEC *)&effecttypes[index].momentum_random_x);
            EdFileWriteNuVec(&effecttypes[index].emitter_random_extent);
            EdFileWriteNuVec(&effecttypes[index].emitter_velocity);
            EdFileWriteFloat(effecttypes[index].unknown_070[0]);
            EdFileWriteFloat(effecttypes[index].unknown_070[1]);
            EdFileWriteFloat(effecttypes[index].unknown_070[2]);
            EdFileWriteFloat(effecttypes[index].unknown_070[3]);
            EdFileWriteFloat(effecttypes[index].unknown_070[4]);
            EdFileWriteFloat(effecttypes[index].unknown_070[5]);
            EdFileWriteFloat(effecttypes[index].unknown_070[6]);
            EdFileWriteFloat(effecttypes[index].unknown_070[7]);
            EdFileWriteFloat(effecttypes[index].unknown_070[8]);
            EdFileWriteFloat(effecttypes[index].unknown_070[9]);
            EdFileWriteFloat(effecttypes[index].unknown_070[10]);
            EdFileWriteFloat(effecttypes[index].unknown_070[11]);
            EdFileWriteFloat(effecttypes[index].gravity);
            EdFileWriteFloat(effecttypes[index].particle_lifetime);
            EdFileWriteShort(effecttypes[index].field_0a8);
            EdFileWriteChar(effecttypes[index].field_0aa);
            EdFileWriteChar(effecttypes[index].field_0ab);
            EdFileWriteFloat(effecttypes[index].field_0ac);
            EdFileWriteFloat(effecttypes[index].jib_x_frequency);
            EdFileWriteFloat(effecttypes[index].jib_x_amplitude);
            EdFileWriteFloat(effecttypes[index].jib_y_frequency);
            EdFileWriteFloat(effecttypes[index].jib_y_amplitude);
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].colour_keys[key].time);
                EdFileWriteUnsignedChar(effecttypes[index].colour_keys[key].red);
                EdFileWriteUnsignedChar(effecttypes[index].colour_keys[key].green);
                EdFileWriteUnsignedChar(effecttypes[index].colour_keys[key].blue);
                EdFileWriteUnsignedChar(effecttypes[index].colour_keys[key].unknown_7);
            }
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].alpha_keys[key].time);
                EdFileWriteFloat(effecttypes[index].alpha_keys[key].value);
            }
            EdFileWriteFloat(effecttypes[index].particle_parameter_a);
            EdFileWriteFloat(effecttypes[index].particle_parameter_b);
            EdFileWriteFloat(effecttypes[index].min_size);
            EdFileWriteFloat(effecttypes[index].max_size);
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].width_keys[key].time);
                EdFileWriteFloat(effecttypes[index].width_keys[key].value);
            }
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].height_keys[key].time);
                EdFileWriteFloat(effecttypes[index].height_keys[key].value);
            }
            EdFileWriteFloat(effecttypes[index].min_rotation);
            EdFileWriteFloat(effecttypes[index].max_rotation);
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].rotation_keys[key].time);
                EdFileWriteFloat(effecttypes[index].rotation_keys[key].value);
            }
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].field_218_keys[key].time);
                EdFileWriteFloat(effecttypes[index].field_218_keys[key].value);
            }
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].field_258_keys[key].time);
                EdFileWriteFloat(effecttypes[index].field_258_keys[key].value);
            }
            EdFileWriteFloat(effecttypes[index].texture_u0);
            EdFileWriteFloat(effecttypes[index].texture_v0);
            EdFileWriteFloat(effecttypes[index].texture_u1);
            EdFileWriteFloat(effecttypes[index].texture_v1);
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].collision_keys[key].time);
                EdFileWriteFloat(effecttypes[index].collision_keys[key].value);
            }
            EdFileWriteChar(effecttypes[index].process_spheres);
            EdFileWriteChar(effecttypes[index].time_group);
            EdFileWriteChar(effecttypes[index].collision_flags);
            EdFileWriteChar(effecttypes[index].use_explicit_clip_box);
            EdFileWriteNuVec(&effecttypes[index].particle_box);
            EdFileWriteFloat(effecttypes[index].thinning);
            EdFileWriteFloat(effecttypes[index].torus_radius1);
            EdFileWriteFloat(effecttypes[index].torus_radius2);
            EdFileWriteFloat(effecttypes[index].torus_lifetime);
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].torus_keys1[key].time);
                EdFileWriteFloat(effecttypes[index].torus_keys1[key].value);
            }
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].torus_keys2[key].time);
                EdFileWriteFloat(effecttypes[index].torus_keys2[key].value);
            }
            for (key = 0; key < 8; ++key) {
                EdFileWriteFloat(effecttypes[index].torus_keys3[key].time);
                EdFileWriteFloat(effecttypes[index].torus_keys3[key].value);
            }
            sound_count = 0;
            if (effecttypes[index].sound_data[0] != -1)
                ++sound_count;
            if (effecttypes[index].sound_data[3] != -1)
                ++sound_count;
            if (effecttypes[index].sound_data[6] != -1)
                ++sound_count;
            if (effecttypes[index].sound_data[9] != -1)
                ++sound_count;
            EdFileWriteInt(sound_count);
            for (key = 0; key < 4; ++key) {
                if (effecttypes[index].sound_data[key * 3] != -1) {
                    EdFileWrite(SfxInfo[effecttypes[index].sound_data[key * 3]].name, 16);
                    EdFileWriteInt(effecttypes[index].sound_data[key * 3 + 1]);
                    EdFileWriteInt(effecttypes[index].sound_data[key * 3 + 2]);
                }
            }
            EdFileWriteChar(effecttypes[index].trail_count);
            EdFileWriteFloat(effecttypes[index].trail_time);
            EdFileWriteChar(effecttypes[index].radial_segments);
            EdFileWriteFloat(effecttypes[index].radial_floor);
            EdFileWriteFloat(effecttypes[index].scale_in_time);
        }
        if (list == 1 || list == 2) {
            for (index = 0; index < 512; ++index) {
                if (edpp_ptls[index].instance_id != -1)
                    ++instance_count;
            }
            EdFileWriteInt(instance_count);
            for (index = 0; index < 512; ++index) {
                if (edpp_ptls[index].instance_id == -1)
                    continue;
                EdFileWriteNuVec(&edpp_ptls[index].position);
                EdFileWriteShort(edpp_ptls[index].rotation_z);
                EdFileWriteShort(edpp_ptls[index].rotation_y);
                EdFileWriteShort(edpp_ptls[index].emitter_rotation_z);
                EdFileWriteShort(edpp_ptls[index].emitter_rotation_y);
                EdFileWriteShort(edpp_ptls[index].emitter_rotation_x);
                EdFileWriteFloat(edpp_ptls[index].start_offset);
                if (edpp_ptls[index].effect_index != -1)
                    EdFileWrite(debtab[edpp_ptls[index].effect_index]->name, 16);
                else
                    EdFileWrite(edpp_ptls[index].name, 16);
                EdFileWriteInt(edpp_ptls[index].switch_type);
                EdFileWriteInt(edpp_ptls[index].switch_id);
                EdFileWriteFloat(edpp_ptls[index].switch_variable);
                EdFileWriteShort(edpp_ptls[index].reflection_rotation_z);
                EdFileWriteShort(edpp_ptls[index].reflection_rotation_y);
                EdFileWriteFloat(edpp_ptls[index].reflection_offset);
                EdFileWriteFloat(edpp_ptls[index].reflection_bounce);
                EdFileWriteShort(edpp_ptls[index].render_group);
                EdFileWriteUnsignedShort(edpp_ptls[index].render_priority);
                EdFileWriteChar(edpp_ptls[index].dynamic_priority);
                EdFileWriteChar(edpp_ptls[index].detail_levels);
                EdFileWriteChar(edpp_ptls[index].facing_mode);
                EdFileWriteShort(edpp_ptls[index].facing_rotation_x);
                EdFileWriteShort(edpp_ptls[index].facing_rotation_y);
            }
        }
        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        return 1;
    }
    return 0;
}

void __cdecl cbFileSaveEffects(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 valid_page = 1;
    i32 backup_succeeded = 1;
    char original_directory[64] = "";
    char filename[256];
    char backup[256];
    char general_directory[256];
    char general_name[256];
    char general_extension[256];
    char level_directory[256];
    char level_name[256];
    char level_extension[256];
    if (edbits_datapath[0] != '\0') {
        NuFileGetCurrentDirectory(original_directory);
        NuFileSetCurrentDirectory(edbits_datapath);
    }
    if (edbits_general_save_directory[0] == '\0')
        sprintf(general_directory, ".");
    else
        strcpy(general_directory, edbits_general_save_directory);
    if (edbits_general_save_name[0] == '\0')
        sprintf(general_name, "particle");
    else
        strcpy(general_name, edbits_general_save_name);
    if (edbits_general_save_extension[0] == '\0')
        sprintf(general_extension, "ptl");
    else
        strcpy(general_extension, edbits_general_save_extension);
    if (edbits_level_save_directory[0] == '\0')
        sprintf(level_directory, ".");
    else
        strcpy(level_directory, edbits_level_save_directory);
    if (edbits_level_save_name[0] == '\0')
        sprintf(level_name, "particle");
    else
        strcpy(level_name, edbits_level_save_name);
    if (edbits_level_save_extension[0] == '\0')
        sprintf(level_extension, "ptl");
    else
        strcpy(level_extension, edbits_level_save_extension);
    if (edpp_effect_list == 0) {
        if (edpp_save_names[0] != NULL) {
            NuStrCpy(filename, edpp_save_names[0]);
            NuStrCpy(backup, filename);
            NuStrCat(backup, ".bak");
        } else {
            sprintf(filename, "%s\\%s.%s", general_directory, general_name, general_extension);
            sprintf(backup, "%s\\%s.%s.bak", general_directory, general_name, general_extension);
        }
    } else if (edpp_effect_list == 1) {
        if (edpp_save_names[1] != NULL) {
            NuStrCpy(filename, edpp_save_names[1]);
            NuStrCpy(backup, filename);
            NuStrCat(backup, ".bak");
        } else {
            sprintf(filename, "%s\\%s.%s", level_directory, level_name, level_extension);
            sprintf(backup, "%s\\%s.%s.bak", level_directory, level_name, level_extension);
        }
    } else if (edpp_effect_list == 5) {
        if (edpp_save_names[5] != NULL) {
            NuStrCpy(filename, edpp_save_names[5]);
            NuStrCpy(backup, filename);
            NuStrCat(backup, ".bak");
        } else {
            sprintf(filename, "%s\\char.%s", general_directory, general_extension);
            sprintf(backup, "%s\\char.%s.bak", general_directory, general_extension);
        }
    } else {
        valid_page = 0;
    }
    if (EdppUseMemcardPc == 0 && edbits_override_backups == 0 && valid_page != 0)
        backup_succeeded = EdFileBackup(filename, backup);
    if (valid_page != 0 && edppSaveEffects(filename, edpp_effect_list) != 0) {
        if (backup_succeeded != 0)
            edppShowMessagePc(parent, "Saved OK", 1);
        else
            edppShowMessagePc(parent, "Saved OK - Backup Failed", 1);
    } else {
        edppShowMessagePc(parent, "File Save Error", 0);
    }
    if (original_directory[0] != '\0')
        NuFileSetCurrentDirectory(original_directory);
}

void __cdecl cbPtlQuickDeleteMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2);
void __cdecl cbPtlReadoutMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2);

void __cdecl cbPtlCancelDataMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(ptldatamenu);
    ptldatamenu = NULL;
}

void __cdecl cbPtlShowAll(eduimenu_s *unused0, eduiitem_s *unused1, u32 unused2) {
    edpp_showAllPlaced = !edpp_showAllPlaced;
}

void __cdecl cbPtlCopyEffect(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    eduimenu_s *parent = NULL;
    i32 index;
    char short_name[16];
    char *source_name;
    size_t name_length;
    if (edpp_types_used >= debinf_count) {
        eduiCreateMessageMenu(menu, "Out of particle slots", 0);
        return;
    }
    if (edpp_create_type == -1) {
        eduiCreateMessageMenu(menu, "No particle to copy", 0);
        return;
    }
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL) {
            debtab[index] = &effecttypes[index];
            memcpy(debtab[index], debtab[edpp_create_type], sizeof(DEBRIS_INFO_PC));
            debtab[index]->native_data = NULL;
            debtab[index]->last_render_time = 0.0f;
            source_name = debtab[edpp_create_type]->name;
            name_length = strlen(source_name);
            if (name_length <= 12) {
                sprintf(debtab[index]->name, "%s%03d", source_name, index);
            } else {
                sprintf(short_name, "%s", source_name);
                short_name[12] = '\0';
                sprintf(debtab[index]->name, "%s%03d", short_name, index);
            }
            ++edpp_types_used;
            edpp_create_type = index;
            break;
        }
    }
    if (menu->parent != NULL) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback != NULL)
        menu->callback(menu, parent);
}

void __cdecl edptlcbDeleteOrphans(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    eduimenu_s *parent = NULL;
    i32 index;
    for (index = 0; index < 512; ++index) {
        if (edpp_ptls[index].effect_index == -1) {
            if (edpp_ptls[index].instance_id != -1) {
                if (edpp_ptls[index].instance_id != 99999)
                    DebFreeInstantly(&edpp_ptls[index].instance_id);
                --edpp_instances_used;
                edpp_ptls[index].instance_id = -1;
            }
        }
    }
    edpp_num_orphans = 0;
    if (menu->parent != NULL) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback != NULL)
        menu->callback(menu, parent);
}

void __cdecl edppRestartAllEffectsInLevel(void) {
    i32 index;
    for (index = 0; index < 512; ++index) {
        if (edpp_ptls[index].instance_id == -1)
            continue;
        edpp_ptls[index].instance_id = -1;
        edpp_ptls[index].effect_index = LookupDebrisEffectPageIgnore(edpp_ptls[index].name, 0, 0);
        AddDebrisEffect(&edpp_ptls[index].instance_id, edpp_ptls[index].effect_index, edpp_ptls[index].position.x,
                        edpp_ptls[index].position.y, edpp_ptls[index].position.z);
        if (edpp_ptls[index].instance_id != -1) {
            debkeydata[edpp_ptls[index].instance_id].field_2f9 = 0;
            DebrisOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].rotation_z, edpp_ptls[index].rotation_y);
            DebrisEmitterOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].emitter_rotation_z,
                                     edpp_ptls[index].emitter_rotation_y, edpp_ptls[index].emitter_rotation_x);
            DebrisStartOffsetEx(&debkeydata[edpp_ptls[index].instance_id], edpp_ptls[index].start_offset);
            edppDebrisSetTrigger(edpp_ptls[index].instance_id, edpp_ptls[index].switch_type, edpp_ptls[index].switch_id,
                                 edpp_ptls[index].switch_variable);
            edppDebrisReflectionOrientation(edpp_ptls[index].instance_id, edpp_ptls[index].reflection_rotation_z,
                                            edpp_ptls[index].reflection_rotation_y, edpp_ptls[index].reflection_offset,
                                            edpp_ptls[index].reflection_bounce);
            DebrisSetFacing(edpp_ptls[index].instance_id, edpp_ptls[index].facing_mode,
                            edpp_ptls[index].facing_rotation_x, edpp_ptls[index].facing_rotation_y);
            edppDebrisSetGroupID(edpp_ptls[index].instance_id, edpp_ptls[index].render_group);
            DebrisSetRoomID(edpp_ptls[index].instance_id, EdppPageScenesPc[edpp_ptls[index].page]);
        } else {
            edpp_ptls[index].instance_id = 99999;
        }
        edpp_page_on[edpp_ptls[index].page] = 1;
    }
    edpp_create_type = -1;
    edppDetermineNearest(1.0f);
}

void __cdecl cbFileLoadEffects(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    char original_directory[64] = "";
    char path[256];
    char general_directory[256];
    char general_name[256];
    char general_extension[256];
    char level_directory[256];
    char level_name[256];
    char level_extension[256];
    i32 page;
    for (page = 0; page < 8; ++page) {
        if (edpp_page_used[page] != 0)
            edppClearPage((i8)page);
    }
    if (edbits_datapath[0] != '\0') {
        NuFileGetCurrentDirectory(original_directory);
        NuFileSetCurrentDirectory(edbits_datapath);
    }
    if (edbits_general_save_directory[0] == '\0')
        sprintf(general_directory, ".");
    else
        strcpy(general_directory, edbits_general_save_directory);
    if (edbits_general_save_name[0] == '\0')
        sprintf(general_name, "particle");
    else
        strcpy(general_name, edbits_general_save_name);
    if (edbits_general_save_extension[0] == '\0')
        sprintf(general_extension, "ptl");
    else
        strcpy(general_extension, edbits_general_save_extension);
    if (edbits_level_save_directory[0] == '\0')
        sprintf(level_directory, ".");
    else
        strcpy(level_directory, edbits_level_save_directory);
    if (edbits_level_save_name[0] == '\0')
        sprintf(level_name, "particle");
    else
        strcpy(level_name, edbits_level_save_name);
    if (edbits_level_save_extension[0] == '\0')
        sprintf(level_extension, "ptl");
    else
        strcpy(level_extension, edbits_level_save_extension);
    sprintf(path, "%s\\%s.%s", general_directory, general_name, general_extension);
    if (NuFileExists(path) != 0)
        edppLoadPage(path, 0, NULL);
    sprintf(path, "%s\\char.%s", general_directory, general_extension);
    if (NuFileExists(path) != 0)
        edppLoadPage(path, 5, NULL);
    sprintf(path, "%s\\%s.%s", level_directory, level_name, level_extension);
    if (NuFileExists(path) > 0)
        edppLoadPage(path, 1, edbits_base_scene);
    edppRestartAllEffectsInLevel();
    edppShowMessagePc(parent, "Loaded OK", 1);
    if (original_directory[0] != '\0')
        NuFileSetCurrentDirectory(original_directory);
}

void __cdecl cbPtlDataMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    ptldatamenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelDataMenu, "Data Menu");
    if (ptldatamenu != NULL) {
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbChangeNameMenu, "Type Name..."));
        if (edpp_create_type != -1) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlDeleteEffect, "Delete Effect"));
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlCopyEffect, "Copy Effect"));
        }
        if (edpp_effect_list == 0)
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbFileSaveEffects, "Save General list"));
        else if (edpp_effect_list == 1)
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbFileSaveEffects, "Save Level list"));
        else if (edpp_effect_list == 5)
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbFileSaveEffects, "Save Char list"));
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbFileLoadEffects, "Load all from file"));
        if (edpp_create_type != -1)
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, edptlcbClipboardMenu, "Clipboard..."));
        if (edpp_num_orphans != 0) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, edptlcbOrphanListMenu, "List Orphans..."));
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, edptlcbDeleteOrphans, "Delete Orphans"));
        }
        eduiMenuAddItem(ptldatamenu,
                        eduiItemSelCreate(1, colours, 0, 0, cbPtlQuickDeleteMenu, "Quick Delete Types..."));
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlReadoutMenu, "Info Box Style..."));
        eduiMenuAddItem(ptldatamenu,
                        eduiItemToggleCreate(1, colours, edpp_showAllPlaced, 2, cbPtlShowAll, "Show All Placed"));
        eduiMenuAttach(parent, ptldatamenu);
        ptldatamenu->x = parent->x + 10;
        ptldatamenu->y = parent->y + 40;
    }
}

void __cdecl cbPtlAddEffect(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2);

void __cdecl cbPtlTypeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 count = 0;
    i32 found_selected;
    i32 index;
    char title[32];
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL)
            continue;
        if (edpp_effect_list == 1) {
            if (debtab[index]->category == edpp_effect_list && debtab[index]->page == edbits_particle_level_page)
                ++count;
        } else if (debtab[index]->category == edpp_effect_list) {
            ++count;
        }
    }
    sprintf(title, "Particle Types (%d)", count);
    ptltypemenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelTypeMenu, title);
    if (ptltypemenu == NULL)
        return;
    found_selected = 0;
    for (index = 1; index < debinf_count; ++index) {
        i32 placed;
        i32 selected;
        i32 i;
        if (debtab[index] == NULL)
            continue;
        if (edpp_effect_list == 1) {
            if (debtab[index]->category != edpp_effect_list)
                continue;
            if (debtab[index]->page != edbits_particle_level_page)
                continue;
        } else if (debtab[index]->category != edpp_effect_list) {
            continue;
        }
        placed = 0;
        for (i = 0; i < 512; ++i) {
            if (edpp_ptls[i].instance_id != -1 && edpp_ptls[i].instance_id != 99999 &&
                edpp_ptls[i].effect_index == index) {
                placed = 1;
                break;
            }
        }
        selected = edpp_create_type == index;
        if (placed)
            eduiMenuAddItem(ptltypemenu,
                            eduiItemCheckCreate(index, edblack, selected, 1, cbPtlSelType, debtab[index]->name));
        else
            eduiMenuAddItem(ptltypemenu,
                            eduiItemCheckCreate(index, eddarkred, selected, 1, cbPtlSelType, debtab[index]->name));
        if (selected == 1) {
            ptltypemenu->selected = edui_last_item;
            found_selected = 1;
        }
    }
    eduiMenuSortItemsByTxt(ptltypemenu);
    eduiMenuAddItemFirst(ptltypemenu, eduiItemSelCreate(1, edblack, 0, 0, cbPtlAddEffect, "Add Effect"));
    eduiMenuAttach(parent, ptltypemenu);
    ptltypemenu->x = parent->x + 10;
    ptltypemenu->y = parent->y + 40;
    if (found_selected)
        ptltypemenu->field_0c = ptltypemenu->selected;
}

void __cdecl cbPtlCancelEmitMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(ptlemitmenu);
    ptlemitmenu = NULL;
}

void __cdecl cbPtlEmitMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    u32 disabled_colours[4] = {0x80303030, 0x80ff0000, 0x80808080, 0x80404040};
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        DEBRIS_INFO_PC *effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
        ptlemitmenu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, cbPtlCancelEmitMenu, "Emitter Settings");
        if (ptlemitmenu != NULL) {
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlEmitVelMenu, "Emitter Vel..."));
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlGravMenu, "Gravity..."));
            if (effect->generator_type == 0 || effect->generator_type == 8 || effect->generator_type == 9 ||
                effect->generator_type == 10) {
                eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlVarStartMenu, "Random Start..."));
            } else if (effect->generator_type == 6 || effect->generator_type == 7 || effect->generator_type == 11 ||
                       effect->generator_type == 12) {
                eduiMenuAddItem(ptlemitmenu,
                                eduiItemSelCreate(1, colours, 0, 0, cbPtlVarStartMenu, "Base Emit Vector..."));
            }
            if (effect->generator_type == 0) {
                eduiMenuAddItem(ptlemitmenu,
                                eduiItemSelCreate(1, colours, 0, 0, cbPtlStartVelMenu, "Start Velocity..."));
            } else if (effect->generator_type == 7 || effect->generator_type == 11) {
                eduiMenuAddItem(ptlemitmenu,
                                eduiItemSelCreate(1, colours, 0, 0, cbPtlStartVelMenu, "Emit Point Velocity..."));
            }
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlVarEmitMenu, "Emitter Variation..."));
            eduiMenuAddItem(ptlemitmenu,
                            eduiItemSelCreate(1, colours, 0, 0, cbChangeGenRateMenu, "Particles per Sec..."));
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, cbChangeETimeMenu, "Particle Life..."));
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlEmitTimeMenu, "Emitter Timing..."));
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, cbPtlCutOffMenu, "Repeat / Radii..."));
            if (effect->generator_type == 0 || effect->generator_type == 6 || effect->generator_type == 7 ||
                effect->generator_type == 8) {
                eduiMenuAddItem(ptlemitmenu,
                                eduiItemSelCreate(1, colours, 0, 0, edptlcbGhostMenu, "Particle Ghosts..."));
            } else {
                eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, disabled_colours, 0, 0, NULL, "Particle Ghosts..."));
            }
            if (effect->generator_type == 12) {
                eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, colours, 0, 0, edptlcbStarMenu, "Star Settings..."));
            } else {
                eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, disabled_colours, 0, 0, NULL, "Star Settings..."));
            }
        }
        eduiMenuAttach(parent, ptlemitmenu);
        ptlemitmenu->x = parent->x + 10;
        ptlemitmenu->y = parent->y + 40;
    }
}

extern struct eduimenu_s *ptlemitvelmenu;
extern struct eduimenu_s *ptlgravmenu;
extern struct eduimenu_s *edptl_ghost_menu;
void __cdecl edptlcbApplyNumGhosts(eduimenu_s *unused0, eduiitem_s *item, u32 unused2);

static void cbPtlCancelEmitVelMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(ptlemitvelmenu);
    ptlemitvelmenu = NULL;
}

static void cbPtlCancelGravMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(ptlgravmenu);
    ptlgravmenu = NULL;
}

static void edptlcbCancelGhostMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edptl_ghost_menu);
    edptl_ghost_menu = NULL;
}

static void cbPtlChangeEmitVel(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    if (edpp_nearest == -1)
        return;
    if (edpp_ptls[edpp_nearest].instance_id == -1)
        return;
    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    effect->momentum_base = ((edui_slider_s *)item)->value;
}

static void cbPtlChangeGrav(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    if (edpp_nearest == -1)
        return;
    if (edpp_ptls[edpp_nearest].instance_id == -1)
        return;
    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    effect->gravity = ((edui_slider_s *)item)->value;
    GenericDebinfoDmaTypeUpdate(debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index]);
}

static void edptlcbApplyGhostTime(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->trail_time = ((edui_slider_s *)item)->value;
}

void __cdecl cbPtlEmitVelMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    DEBRIS_INFO_PC *effect;
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)
        return;
    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    ptlemitvelmenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelEmitVelMenu, "Emitter Vel");
    if (ptlemitvelmenu != NULL) {
        eduiMenuAddItem(ptlemitvelmenu,
                        eduiItemSliderCreate(0, colours, 0, cbPtlChangeEmitVel, -(10.0f * edptl_superscale),
                                             20.0f * edptl_superscale, effect->momentum_base, "Emitter Vel"));
        eduiMenuAttach(menu, ptlemitvelmenu);
        ptlemitvelmenu->x = menu->x + 10;
        ptlemitvelmenu->y = menu->y + 40;
    }
}

void __cdecl cbPtlGravMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    DEBRIS_INFO_PC *effect;
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)
        return;
    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    ptlgravmenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelGravMenu, "Gravity");
    if (ptlgravmenu != NULL) {
        eduiMenuAddItem(ptlgravmenu, eduiItemSliderCreate(0, colours, 0, cbPtlChangeGrav, -10.0f * edptl_superscale,
                                                          20.0f * edptl_superscale, effect->gravity, "Gravity"));
        eduiMenuAttach(menu, ptlgravmenu);
        ptlgravmenu->x = menu->x + 10;
        ptlgravmenu->y = menu->y + 40;
    }
}

void __cdecl edptlcbGhostMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        DEBRIS_INFO_PC *effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
        edptl_ghost_menu = eduiMenuCreate(70, 70, 200, 300, ed_fnt, edptlcbCancelGhostMenu, "Particle Ghosts");
        if (edptl_ghost_menu != NULL) {
            eduiMenuAddItem(edptl_ghost_menu, eduiItemSliderCreateInt(0, colours, 0, edptlcbApplyNumGhosts, 0, 10,
                                                                      effect->trail_count, "Number of Ghosts"));
            eduiMenuAddItem(edptl_ghost_menu, eduiItemSliderCreate(0, colours, 0, edptlcbApplyGhostTime, 0.0f, 1.0f,
                                                                   (f32)(i32)effect->trail_time, "Ghost Separation"));
            eduiMenuAttach(parent, edptl_ghost_menu);
            edptl_ghost_menu->x = parent->x + 10;
            edptl_ghost_menu->y = parent->y + 40;
        }
    }
}
