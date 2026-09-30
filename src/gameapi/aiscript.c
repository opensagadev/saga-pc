#include "saga.h"

#include <string.h>

extern VARIPTR *aiscript_load_buffer;
extern VARIPTR *aiscript_load_end_buffer;
extern AISCRIPT_PC *aiscript_load_script;
extern AISTATE_PC *aiscript_load_state;
extern NULISTHDR *aiscript_load_conditions;
extern NULISTHDR *aiscript_load_actions;
extern NULISTHDR *aiscript_load_refs;
extern i32 condition_has_no_goto;

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
