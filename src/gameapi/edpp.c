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
