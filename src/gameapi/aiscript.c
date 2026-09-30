#include "saga.h"

#include <stdio.h>
#include <string.h>

extern const char AIScriptFalse[6];

extern VARIPTR *aiscript_load_buffer;
extern VARIPTR *aiscript_load_end_buffer;
extern AISCRIPT_PC *aiscript_load_script;
extern AISTATE_PC *aiscript_load_state;
extern NULISTHDR *aiscript_load_conditions;
extern NULISTHDR *aiscript_load_actions;
extern NULISTHDR *aiscript_load_refs;
extern i32 condition_has_no_goto;
extern const char AIScriptListFilename[11];
extern const char AIScriptListPathFormat[14];
extern const char AIScriptFileExtension[5];
extern const char AIScriptKeywordState[6];
extern const char AIScriptKeywordParam[6];
extern const char AIScriptKeywordConst[6];
extern const char AIScriptKeywordConditionMacro[15];
extern const char AIScriptKeywordActionMacro[12];
extern const char AIScriptKeywordReferenceScript[16];
extern const char AIScriptKeywordDeriveFromScript[17];
extern const char AIScriptKeywordInclude[8];
extern const char AIScriptKeywordConditions[11];
extern const char AIScriptKeywordActions[8];
extern const char AIScriptKeywordEnd[2];
extern const char AIScriptKeywordBase[5];
extern const char AIScriptKeywordScript[7];
extern const char AIScriptKeywordSource[7];
extern const char AIScriptKeywordGlobal[7];
extern const char AIScriptKeywordLevel[6];

void __cdecl xConditions(NUFPAR_PC *parser);
void __cdecl xActions(NUFPAR_PC *parser);
void __cdecl xConditionMacro(NUFPAR_PC *parser);
void __cdecl xActionMacro(NUFPAR_PC *parser);
void __cdecl xRefScript(NUFPAR_PC *parser);

AISCRIPT_PC *__cdecl AIScriptFind(AISYS_PC *sys, const char *name, i32 can_use_default, i32 check_level_scripts,
                                  i32 check_global_scripts) {
    AISCRIPT_PC *script;

    if (name != NULL) {
        if (check_level_scripts != 0 && sys != NULL) {
            script = (AISCRIPT_PC *)NuLinkedListGetHead(&sys->scripts);
            while (script != NULL) {
                if (NuStrICmp(name, script->name) == 0) {
                    return script;
                }
                script = (AISCRIPT_PC *)NuLinkedListGetNext(&sys->scripts, &script->link);
            }
        }

        if (check_global_scripts != 0) {
            script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts);
            while (script != NULL) {
                if (NuStrICmp(name, script->name) == 0) {
                    return script;
                }
                script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link);
            }
        }
    }

    if (can_use_default != 0) {
        script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts);
        while (script != NULL) {
            if (NuStrICmp(GizmoDefaultOutputName, script->name) == 0) {
                return script;
            }
            script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link);
        }
    }

    return NULL;
}

AISTATE_PC *__cdecl AIStateFind(const char *name, AISCRIPT_PC *script) {
    AISTATE_PC *state;

    state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);
    while (state != NULL) {
        if (NuStrICmp(name, state->name) == 0) {
            return state;
        }
        state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
    }

    return NULL;
}

#define AISCRIPT_COPY_ARENA_STRING(source, destination, cursor, end)                                                   \
    do {                                                                                                               \
        const char *copy_source = (source);                                                                            \
        i32 copy_length;                                                                                               \
        (destination) = NULL;                                                                                          \
        if (copy_source != NULL) {                                                                                     \
            copy_length = NuStrLen(copy_source);                                                                       \
            if (copy_length != 0) {                                                                                    \
                (destination) = (char *)AISysBufferAlloc((cursor), (end), (u32)(copy_length + 1));                     \
                if ((destination) != NULL) {                                                                           \
                    NuStrCpy((destination), copy_source);                                                              \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

void __cdecl AIScriptCopyConditions(NULISTHDR *source, NULISTHDR *destination, VARIPTR *cursor, VARIPTR *end) {
    AICONDITION_PC *source_condition;

    source_condition = (AICONDITION_PC *)NuLinkedListGetHead(source);
    while (source_condition != NULL) {
        AICONDITION_PC *copy;
        copy = (AICONDITION_PC *)AISysBufferAlloc(cursor, end, sizeof(AICONDITION_PC));
        if (copy != NULL) {
            PCByteCopy_candidate(copy, source_condition, sizeof(AICONDITION_PC));
            copy->link.next = NULL;
            copy->link.prev = NULL;
            AISCRIPT_COPY_ARENA_STRING(source_condition->argument, copy->argument, cursor, end);
            AISCRIPT_COPY_ARENA_STRING(source_condition->next_state_name, copy->next_state_name, cursor, end);
            NuLinkedListAppend(destination, &copy->link);
        }
        source_condition = (AICONDITION_PC *)NuLinkedListGetNext(source, &source_condition->link);
    }
}

void __cdecl AIScriptCopyActions(NULISTHDR *source, NULISTHDR *destination, VARIPTR *cursor, VARIPTR *end) {
    AIACTION_PC *source_action;

    source_action = (AIACTION_PC *)NuLinkedListGetHead(source);
    while (source_action != NULL) {
        AIACTION_PC *copy;
        copy = (AIACTION_PC *)AISysBufferAlloc(cursor, end, sizeof(AIACTION_PC));
        if (copy != NULL) {
            i32 i;
            copy->params = (char **)AISysBufferAlloc(cursor, end, (u32)(source_action->param_count * sizeof(char *)));
            copy->param_count = source_action->param_count;
            if (copy->params != NULL) {
                for (i = 0; i < copy->param_count; i++) {
                    AISCRIPT_COPY_ARENA_STRING(source_action->params[i], copy->params[i], cursor, end);
                }
            }
            copy->definition = source_action->definition;
            NuLinkedListAppend(destination, &copy->link);
        }
        source_action = (AIACTION_PC *)NuLinkedListGetNext(source, &source_action->link);
    }
}

i32 __cdecl AIScriptBuildDerivedScript(AISCRIPT_PC *script, VARIPTR *buffer, VARIPTR *end_buffer, AISYS_PC *sys) {
    AISCRIPT_PC *source_script;
    AISTATE_PC *source_state;
    AISTATE_PC *state;
    AIREFSCRIPT_PC *source_reference;
    AICONDITION_PC *condition;
    i32 source_is_level;
    i32 i;

    if (script == NULL || script->derived_from == NULL || (script->flags_3c & 2) == 0) {
        return 1;
    }

    source_is_level = (script->flags_3c >> 2) & 1;
    source_script = AIScriptFind(sys, script->derived_from, 0, source_is_level, !source_is_level);
    if (source_script == NULL) {
        STUBBED_NAMED("AIScriptBuildDerivedScript: missing-source diagnostic");
        script->flags_3c &= ~2u;
        return 1;
    }
    if (source_script == script) {
        script->flags_3c &= ~2u;
        return 1;
    }
    if ((source_script->flags_3c & 2) != 0) {
        return 0;
    }

    source_state = (AISTATE_PC *)NuLinkedListGetHead(&source_script->states);
    while (source_state != NULL) {
        i32 should_copy = 1;
        state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);
        while (state != NULL) {
            if (NuStrICmp(source_state->name, state->name) == 0) {
                should_copy = 0;
                break;
            }
            state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
        }
        if (should_copy) {
            AISTATE_PC *copy;
            copy = (AISTATE_PC *)AISysBufferAlloc(buffer, end_buffer, sizeof(AISTATE_PC));
            if (copy != NULL) {
                AISCRIPT_COPY_ARENA_STRING(source_state->name, copy->name, buffer, end_buffer);
                AIScriptCopyConditions(&source_state->conditions, &copy->conditions, buffer, end_buffer);
                AIScriptCopyActions(&source_state->actions, &copy->actions, buffer, end_buffer);
                source_reference = (AIREFSCRIPT_PC *)NuLinkedListGetHead(&source_state->reference_scripts);
                while (source_reference != NULL) {
                    AIREFSCRIPT_PC *reference_copy;
                    reference_copy = (AIREFSCRIPT_PC *)AISysBufferAlloc(buffer, end_buffer, sizeof(AIREFSCRIPT_PC));
                    if (reference_copy != NULL) {
                        AISCRIPT_COPY_ARENA_STRING(source_reference->name, reference_copy->name, buffer, end_buffer);
                        AISCRIPT_COPY_ARENA_STRING(source_reference->return_state_name,
                                                   reference_copy->return_state_name, buffer, end_buffer);
                        reference_copy->check_global_scripts = source_reference->check_global_scripts;
                        reference_copy->check_level_scripts = source_reference->check_level_scripts;
                        AIScriptCopyConditions(&source_reference->conditions, &reference_copy->conditions, buffer,
                                               end_buffer);
                        NuLinkedListAppend(&copy->reference_scripts, &reference_copy->link);
                    }
                    source_reference = (AIREFSCRIPT_PC *)NuLinkedListGetNext(&source_state->reference_scripts,
                                                                             &source_reference->link);
                }
                NuLinkedListAppend(&script->states, &copy->link);
            }
        }
        source_state = (AISTATE_PC *)NuLinkedListGetNext(&source_script->states, &source_state->link);
    }

    for (i = 0; i < 4; i++) {
        if (source_script->params[i].name != NULL && script->params[i].name == NULL) {
            AISCRIPT_COPY_ARENA_STRING(source_script->params[i].name, script->params[i].name, buffer, end_buffer);
            script->params[i].default_value = source_script->params[i].default_value;
        }
    }

    state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);
    while (state != NULL) {
        condition = (AICONDITION_PC *)NuLinkedListGetHead(&state->conditions);
        while (condition != NULL) {
            condition->next_state = AIStateFind(condition->next_state_name, script);
            if (condition->next_state_name != NULL && condition->next_state == NULL) {
                STUBBED_NAMED("AIScriptBuildDerivedScript: unresolved inherited next-state diagnostic");
            }
            condition = (AICONDITION_PC *)NuLinkedListGetNext(&state->conditions, &condition->link);
        }
        state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
    }

    script->flags_3c &= ~2u;
    return 1;
}

void __cdecl xEndState(NUFPAR_PC *parser) {
    NuFParPopCom(parser);
}

void __cdecl xState(NUFPAR_PC *parser) {
    NUFPCOMJMP state_commands[5];
    AISTATE_PC *state;

    if (aiscript_load_script == NULL || NuFParGetWord(parser) == 0) {
        return;
    }

    state = (AISTATE_PC *)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer, sizeof(AISTATE_PC));
    if (state == NULL) {
        STUBBED_NAMED("xState: state-arena allocation diagnostic");
        return;
    }
    aiscript_load_state = state;
    AISCRIPT_COPY_ARENA_STRING(parser->word_buffer, state->name, aiscript_load_buffer, aiscript_load_end_buffer);
    if (NuStrICmp(state->name, AIScriptKeywordBase) == 0) {
        NULISTHDR *states = &aiscript_load_script->states;
        state->link.next = states->head;
        state->link.prev = NULL;
        if (states->head != NULL) {
            states->head->prev = &state->link;
        } else {
            states->tail = &state->link;
        }
        states->head = &state->link;
    } else {
        NuLinkedListAppend(&aiscript_load_script->states, &state->link);
    }

    state_commands[0].keyword = (char *)AIScriptKeywordConditions;
    state_commands[0].callback = xConditions;
    state_commands[1].keyword = (char *)AIScriptKeywordActions;
    state_commands[1].callback = xActions;
    state_commands[2].keyword = (char *)AIScriptKeywordReferenceScript;
    state_commands[2].callback = xRefScript;
    state_commands[3].keyword = (char *)AIScriptKeywordEnd;
    state_commands[3].callback = xEndState;
    state_commands[4].keyword = NULL;
    state_commands[4].callback = NULL;

    NuFParPushCom(parser, state_commands);
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        aiscript_load_refs = &state->reference_scripts;
        aiscript_load_conditions = &state->conditions;
        aiscript_load_actions = &state->actions;
        NuFParInterpretWord(parser);
    }
    NuFParPopCom(parser);
    aiscript_load_state = NULL;
    aiscript_load_actions = NULL;
    aiscript_load_conditions = NULL;
    aiscript_load_refs = NULL;
}

void __cdecl xParam(NUFPAR_PC *parser) {
    i32 index;

    if (aiscript_load_script == NULL) {
        return;
    }
    index = NuFParGetInt(parser);
    if (index < 0 || index >= 4 || NuFParGetWord(parser) == 0) {
        return;
    }
    AISCRIPT_COPY_ARENA_STRING(parser->word_buffer, aiscript_load_script->params[index].name, aiscript_load_buffer,
                               aiscript_load_end_buffer);
    aiscript_load_script->params[index].default_value = NuFParGetFloat(parser);
}

void __cdecl xDeriveFromScript(NUFPAR_PC *parser) {
    i32 done;

    if (aiscript_load_script == NULL || aiscript_load_script->derived_from != NULL) {
        return;
    }
    aiscript_load_script->flags_3c &= ~4u;
    done = 0;
    while (!done && NuFParGetLine(parser) != 0) {
        while (!done && NuFParGetWord(parser) != 0) {
            char *cursor;
            if (NuStrICmp(parser->word_buffer, AIScriptKeywordEnd) == 0) {
                done = 1;
            } else if ((cursor = NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordScript)) != NULL) {
                AISCRIPT_COPY_ARENA_STRING(cursor + NuStrLen(AIScriptKeywordScript) + 1,
                                           aiscript_load_script->derived_from, aiscript_load_buffer,
                                           aiscript_load_end_buffer);
            } else if (NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordSource) != NULL) {
                if (NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordGlobal) != NULL) {
                    aiscript_load_script->flags_3c &= ~4u;
                } else if (NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordLevel) != NULL) {
                    aiscript_load_script->flags_3c |= 4u;
                }
            }
        }
    }
    if (aiscript_load_script->derived_from != NULL) {
        aiscript_load_script->flags_3c |= 2u;
    }
}

void __cdecl xInclude(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }
    NuFParSuspend(parser);
    STUBBED_NAMED("xInclude: recursive AIScriptOpenPakFileParse (0x52bcb0)");
    NuFParResume(parser);
}

void __cdecl AIScriptLoadAllPakFile(void *pak, char *path, VARIPTR *buffer, VARIPTR *end_buffer, AISYS_PC *sys) {
    NULISTHDR *scripts;
    NUFPAR_PC *parser;
    NUFILE file;
    void *memfile;
    i32 memfile_size;
    i32 item_index;
    i32 file_count;
    i32 i;
    i32 done;
    char filepath[0x80];
    char script_files[0x80][0x18];

    scripts = (sys != NULL) ? &sys->scripts : &global_aiscripts;
    file = 0;
    if (pak != NULL) {
        item_index = NuFilePakGetItem(pak, (char *)AIScriptListFilename);
        if (item_index != 0) {
            NuFilePakGetItemInfo(pak, item_index, &memfile, &memfile_size);
            file = NuMemFileOpen(memfile, memfile_size, 0);
            if (file == 0) {
                STUBBED_NAMED("AIScriptLoadAllPakFile: package script-list open diagnostic");
            }
        } else {
            STUBBED_NAMED("AIScriptLoadAllPakFile: missing package script-list diagnostic");
        }
    }

    if (file == 0) {
        sprintf(filepath, AIScriptListPathFormat, path);
        file = NuFileOpen(filepath, 0);
        if (file == 0) {
            STUBBED_NAMED("AIScriptLoadAllPakFile: script-list file open diagnostic");
        }
    }

    file_count = 0;
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            while (NuFParGetLine(parser) != 0) {
                NuFParGetWord(parser);
                if (NuStrLen(parser->word_buffer) > 0x0f) {
                    STUBBED_NAMED("AIScriptLoadAllPakFile: overlong script filename diagnostic");
                }
                NuStrCpy(script_files[file_count], parser->word_buffer);
                NuStrCat(script_files[file_count], AIScriptFileExtension);
                file_count++;
            }
            NuFParDestroy(parser);
        }
        NuFileClose(file);

        for (i = 0; i < file_count; i++) {
            /* PC delegates parsing, state linking, and construction to 0x52c320. */
            STUBBED_NAMED("AIScriptLoadAllPakFile: per-file AIScriptLoadPakFile (0x52c320)");
        }
    }

    if (sys == NULL) {
        return;
    }

    do {
        AISCRIPT_PC *script;
        done = 1;
        script = (AISCRIPT_PC *)NuLinkedListGetHead(scripts);
        while (script != NULL) {
            if (AIScriptBuildDerivedScript(script, buffer, end_buffer, sys) == 0) {
                done = 0;
            }
            script = (AISCRIPT_PC *)NuLinkedListGetNext(scripts, &script->link);
        }
    } while (done == 0);
}

#define AISCRIPT_COPY_STRING(source, destination)                                                                      \
    do {                                                                                                               \
        const char *copy_source = (source);                                                                            \
        i32 copy_length;                                                                                               \
        (destination) = NULL;                                                                                          \
        if (copy_source != NULL) {                                                                                     \
            copy_length = NuStrLen(copy_source);                                                                       \
            if (copy_length != 0) {                                                                                    \
                (destination) =                                                                                        \
                    (char *)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer, (u32)(copy_length + 1));  \
                if ((destination) != NULL) {                                                                           \
                    NuStrCpy((destination), copy_source);                                                              \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

void __cdecl xActions(NUFPAR_PC *parser) {
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer, "}") == 0) {
            return;
        }

        if (NuStrICmp(parser->word_buffer, "DoMacro") == 0) {
            AIACTIONMACRO_PC *macro;
            NuFParGetWord(parser);
            macro = (AIACTIONMACRO_PC *)NuLinkedListGetHead(&aiscript_load_script->action_macros);
            while (macro != NULL) {
                if (NuStrICmp(macro->name, parser->word_buffer) == 0) {
                    AIACTION_PC *action;
                    action = (AIACTION_PC *)NuLinkedListGetHead(&macro->actions);
                    while (action != NULL) {
                        AIACTION_PC *copy;
                        i32 i;
                        copy = (AIACTION_PC *)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer,
                                                               sizeof(AIACTION_PC));
                        if (copy != NULL) {
                            copy->params = (char **)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer,
                                                                     (u32)(action->param_count * sizeof(char *)));
                            copy->param_count = action->param_count;
                            if (copy->params != NULL) {
                                for (i = 0; i < copy->param_count; i++) {
                                    AISCRIPT_COPY_STRING(action->params[i], copy->params[i]);
                                }
                            }
                            copy->definition = action->definition;
                            NuLinkedListAppend(aiscript_load_actions, &copy->link);
                        }
                        action = (AIACTION_PC *)NuLinkedListGetNext(&macro->actions, &action->link);
                    }
                    break;
                }
                macro = (AIACTIONMACRO_PC *)NuLinkedListGetNext(&aiscript_load_script->action_macros, &macro->link);
            }
        } else {
            STUBBED_NAMED("xActions: PC action-definition lookup table not yet mapped");
            NuStrChr(parser->word_buffer, '{');
            while (NuFParGetWord(parser) != 0) {
                /* The source line is consumed while its unresolved action is skipped. */
            }
            continue;
        }
    }
}

void __cdecl xConditions(NUFPAR_PC *parser) {
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer, "}") == 0) {
            return;
        }
        if (NuStrICmp(parser->word_buffer, "IF") != 0) {
            if (NuStrICmp(parser->word_buffer, "OnMacro") == 0) {
                AICONDITIONMACRO_PC *macro;
                AICONDITION_PC *last;
                NuFParGetWord(parser);
                macro = (AICONDITIONMACRO_PC *)NuLinkedListGetHead(&aiscript_load_script->condition_macros);
                while (macro != NULL && NuStrICmp(macro->name, parser->word_buffer) != 0) {
                    macro = (AICONDITIONMACRO_PC *)NuLinkedListGetNext(&aiscript_load_script->condition_macros,
                                                                       &macro->link);
                }
                if (macro != NULL) {
                    AICONDITION_PC *condition;
                    condition = (AICONDITION_PC *)NuLinkedListGetHead(&macro->conditions);
                    while (condition != NULL) {
                        AICONDITION_PC *copy;
                        copy = (AICONDITION_PC *)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer,
                                                                  sizeof(AICONDITION_PC));
                        if (copy != NULL) {
                            memcpy(copy, condition, sizeof(AICONDITION_PC));
                            copy->link.next = NULL;
                            copy->link.prev = NULL;
                            AISCRIPT_COPY_STRING(condition->argument, copy->argument);
                            AISCRIPT_COPY_STRING(condition->next_state_name, copy->next_state_name);
                            NuLinkedListAppend(aiscript_load_conditions, &copy->link);
                        }
                        condition = (AICONDITION_PC *)NuLinkedListGetNext(&macro->conditions, &condition->link);
                    }
                    last = (AICONDITION_PC *)aiscript_load_conditions->tail;
                    NuFParGetWord(parser);
                    if (NuStrICmp(parser->word_buffer, "goto") == 0) {
                        last->flags &= (u16)~2;
                        NuFParGetWord(parser);
                        AISCRIPT_COPY_STRING(parser->word_buffer, last->next_state_name);
                        NuFParGetWord(parser);
                        if (NuStrICmp(parser->word_buffer, "KeepBlockedMessages") == 0) {
                            last->flags |= 2;
                        }
                    } else if (NuStrICmp(parser->word_buffer, "and") == 0) {
                        last->flags |= 1;
                    }
                }
            } else {
                NuStrChr(parser->word_buffer, '{');
            }
            continue;
        }

        NuFParGetWord(parser);
        STUBBED_NAMED("xConditions: PC condition-definition table not yet mapped");
        while (NuFParGetWord(parser) != 0) {
        }
        continue;
    }
}

void __cdecl xConditionMacro(NUFPAR_PC *parser) {
    AICONDITIONMACRO_PC *macro;

    if (aiscript_load_script == NULL || NuFParGetWord(parser) == 0) {
        return;
    }
    macro = (AICONDITIONMACRO_PC *)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer,
                                                    sizeof(AICONDITIONMACRO_PC));
    if (macro == NULL) {
        return;
    }
    macro->link.next = NULL;
    macro->link.prev = NULL;
    AISCRIPT_COPY_STRING(parser->word_buffer, macro->name);
    NuLinkedListAppend(&aiscript_load_script->condition_macros, &macro->link);
    aiscript_load_conditions = &macro->conditions;
    condition_has_no_goto = 1;
    xConditions(parser);
    aiscript_load_conditions = NULL;
    condition_has_no_goto = 0;
}

void __cdecl xActionMacro(NUFPAR_PC *parser) {
    AIACTIONMACRO_PC *macro;

    if (aiscript_load_script == NULL || NuFParGetWord(parser) == 0) {
        return;
    }
    macro =
        (AIACTIONMACRO_PC *)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer, sizeof(AIACTIONMACRO_PC));
    if (macro == NULL) {
        return;
    }
    macro->link.next = NULL;
    macro->link.prev = NULL;
    AISCRIPT_COPY_STRING(parser->word_buffer, macro->name);
    NuLinkedListAppend(&aiscript_load_script->action_macros, &macro->link);
    aiscript_load_actions = &macro->actions;
    xActions(parser);
    aiscript_load_actions = NULL;
}

void __cdecl xRefScript(NUFPAR_PC *parser) {
    AIREFSCRIPT_PC *reference;
    VARIPTR saved_buffer;
    i32 done;

    if (aiscript_load_refs == NULL) {
        return;
    }
    saved_buffer = *aiscript_load_buffer;
    reference =
        (AIREFSCRIPT_PC *)AISysBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer, sizeof(AIREFSCRIPT_PC));
    if (reference == NULL) {
        return;
    }
    memset(reference, 0, sizeof(AIREFSCRIPT_PC));
    done = 0;
    while (!done && NuFParGetLine(parser) != 0) {
        while (!done && NuFParGetWord(parser) != 0) {
            char *cursor;
            if (NuStrICmp(parser->word_buffer, "}") == 0) {
                done = 1;
            } else if ((cursor = NuStrIStr(parser->word_buffer, "Script")) != NULL) {
                AISCRIPT_COPY_STRING(cursor + NuStrLen("Script") + 1, reference->name);
            } else if ((cursor = NuStrIStr(parser->word_buffer, "ReturnState")) != NULL) {
                AISCRIPT_COPY_STRING(cursor + NuStrLen("ReturnState") + 1, reference->return_state_name);
            } else if (NuStrICmp(parser->word_buffer, "CONDITIONS") == 0) {
                aiscript_load_conditions = &reference->conditions;
                condition_has_no_goto = 1;
                xConditions(parser);
                aiscript_load_conditions = NULL;
                condition_has_no_goto = 0;
            } else if (NuStrIStr(parser->word_buffer, "Source") != NULL) {
                if (NuStrIStr(parser->word_buffer, "Global") != NULL) {
                    reference->check_global_scripts = 1;
                }
            }
        }
    }
    if (reference->name != NULL) {
        if (!reference->check_global_scripts && !reference->check_level_scripts) {
            reference->check_global_scripts = 1;
            reference->check_level_scripts = 1;
        }
        NuLinkedListAppend(aiscript_load_refs, &reference->link);
    } else {
        *aiscript_load_buffer = saved_buffer;
    }
}

#undef AISCRIPT_COPY_ARENA_STRING

#define AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, mask, initially_set, set_on_false)                  \
    do {                                                                                                               \
        u32 *flags;                                                                                                    \
        i32 i;                                                                                                         \
        if ((packet) != NULL && (packet)->owner != NULL && (first_time) != 0) {                                        \
            flags = ((mask) == 0x2000u) ? &(packet)->owner->flags_1f8 : &(packet)->owner->flags_1fc;                   \
            if (initially_set) {                                                                                       \
                *flags |= (mask);                                                                                      \
            } else {                                                                                                   \
                *flags &= ~(mask);                                                                                     \
            }                                                                                                          \
            for (i = 0; i < (count); i++) {                                                                            \
                if (NuStrICmp((params)[i], AIScriptFalse) == 0) {                                                      \
                    if (set_on_false) {                                                                                \
                        *flags |= (mask);                                                                              \
                    } else {                                                                                           \
                        *flags &= ~(mask);                                                                             \
                    }                                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define AISCRIPT_SET_PACKET_FLAG(packet, params, count, first_time, mask)                                              \
    do {                                                                                                               \
        i32 i;                                                                                                         \
        if ((packet) != NULL && (first_time) != 0) {                                                                   \
            (packet)->flags |= (mask);                                                                                 \
            for (i = 0; i < (count); i++) {                                                                            \
                if (NuStrICmp((params)[i], AIScriptFalse) == 0) {                                                      \
                    (packet)->flags &= ~(mask);                                                                        \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

i32 __cdecl Action_NoTerrain(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count, i32 first_time,
                             f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x20u, 1, 0);
    return 1;
}

i32 __cdecl Action_NoLosCheck(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count, i32 first_time,
                              f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x400u, 1, 0);
    return 1;
}

i32 __cdecl Action_FlatTerrain(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count,
                               i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x10u, 1, 0);
    return 1;
}

i32 __cdecl Action_DontUseShadowTerrain(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count,
                                        i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x40u, 1, 0);
    return 1;
}

i32 __cdecl Action_DontPush(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count, i32 first_time,
                            f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x2u, 1, 0);
    return 1;
}

i32 __cdecl Action_CanSeeBehind(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count,
                                i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x800u, 1, 0);
    return 1;
}

i32 __cdecl Action_RequiresLOS(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count,
                               i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_PACKET_FLAG(packet, params, count, first_time, 0x20000u);
    return 1;
}

i32 __cdecl Action_SetFullPathSearch(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count,
                                     i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x8u, 0, 1);
    return 1;
}

i32 __cdecl Action_NoShadows(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 count, i32 first_time,
                             f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, 0x2000u, 1, 0);
    return 1;
}

f32 __cdecl Condition_Timer(void *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *argument,
                            void *initialized_argument) {
    (void)sys;
    (void)packet;
    (void)argument;
    (void)initialized_argument;
    return processor->script_timer;
}

f32 __cdecl Condition_Random(void *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *argument,
                             void *initialized_argument) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)argument;
    (void)initialized_argument;
    return NuRandFloat();
}

#undef AISCRIPT_SET_OWNER_FLAG
#undef AISCRIPT_SET_PACKET_FLAG

f32 __cdecl Condition_AlwaysTrue(void *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *argument,
                                 void *initialized_argument) {
    union {
        void *pointer;
        f32 value;
    } result;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)argument;
    result.pointer = initialized_argument;
    return result.value;
}
