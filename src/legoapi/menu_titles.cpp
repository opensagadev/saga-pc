#include <string.h>
#include <stdio.h>
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nuwin32/Stubs.h"
#include "gameapi/edptl.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/core/rtl.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "gameapi/edpart.h"

extern "C" {
    MENU_SETTINGS_STRING_ENTRY dat_00827800[40] = {
        {0,  "Quit"                                                       },
        {1,  "Quit to Windows"                                            },
        {2,  "Quit to Demos Menu"                                         },
        {3,  "Return to game"                                             },
        {4,  "Low"                                                        },
        {5,  "Medium"                                                     },
        {6,  "High"                                                       },
        {7,  "Resume"                                                     },
        {8,  "Accept"                                                     },
        {10, "Back"                                                       },
        {9,  "Decline"                                                    },
        {11, dat_0075f2c4                                                 },
        {12, dat_0075f2c0                                                 },
        {13, "OK"                                                         },
        {14, "Cancel"                                                     },
        {15, "On"                                                         },
        {16, dat_0076b274                                                 },
        {17, "Reset To Defaults"                                          },
        {18, "Video Settings"                                             },
        {19, "Screen Resolution"                                          },
        {20, "Refresh Rate"                                               },
        {21, "Vertical Sync"                                              },
        {22, "Aspect Ratio"                                               },
        {23, "Enhanced Graphics"                                          },
        {24, "From Screen-Res"                                            },
        {25, "Use current settings"                                       },
        {26, "Texture Quality"                                            },
        {27, "Revert"                                                     },
        {28, "You must restart the game for these changes to take effect!"},
        {29, "Effects Settings"                                           },
        {30, "FSAA"                                                       },
        {31, "Externally Set"                                             },
        {32, "Shadows"                                                    },
        {33, "Bloom"                                                      },
        {34, "Depth of Field"                                             },
        {35, "Speed Blur"                                                 },
        {36, "Dynamic Light Quality"                                      },
        {37, "Edge AA"                                                    },
        {38, "Show Game Controls"                                         },
        {39, NULL                                                         },
    };
    MENU_SETTINGS_STRING_ENTRY dat_00827940[40] = {
        {0,  "Quit"                                                       },
        {1,  "Quit to Windows"                                            },
        {2,  "Quit to Demos Menu"                                         },
        {3,  "Return to game"                                             },
        {4,  "Low"                                                        },
        {5,  "Medium"                                                     },
        {6,  "High"                                                       },
        {7,  "Resume"                                                     },
        {8,  "Accept"                                                     },
        {9,  "Decline"                                                    },
        {10, "Back"                                                       },
        {11, dat_0075f2c4                                                 },
        {12, dat_0075f2c0                                                 },
        {13, "OK"                                                         },
        {14, "Cancel"                                                     },
        {15, "On"                                                         },
        {16, dat_0076b274                                                 },
        {17, "Reset To Defaults"                                          },
        {18, "Video Settings"                                             },
        {19, "Screen Resolution"                                          },
        {20, "Refresh Rate"                                               },
        {21, "Vertical Sync"                                              },
        {22, "Aspect Ratio"                                               },
        {23, "Enhanced Graphics"                                          },
        {24, "From Screen-Res"                                            },
        {25, "Use current settings"                                       },
        {26, "Texture Quality"                                            },
        {27, "Revert"                                                     },
        {28, "You must restart the game for these changes to take effect!"},
        {29, "Effects Settings"                                           },
        {30, "FSAA"                                                       },
        {31, "Externally Set"                                             },
        {32, "Shadows"                                                    },
        {33, "Bloom"                                                      },
        {34, "Depth of Field"                                             },
        {35, "Speed Blur"                                                 },
        {36, "Dynamic Light Quality"                                      },
        {37, "Edge AA"                                                    },
        {38, "Show Game Controls"                                         },
        {39, NULL                                                         },
    };
    MENU_SETTINGS_STRING_ENTRY dat_00827a80[40] = {
        {0,  "Quitter"                                                                     },
        {1,  "Revenir sous Windows"                                                        },
        {2,  "Quitter au menu D\303\251mo"                                                 },
        {3,  "Reprendre la partie"                                                         },
        {4,  "Faible"                                                                      },
        {5,  "Moyenne"                                                                     },
        {6,  "Elev\303\251e"                                                               },
        {7,  "Reprendre"                                                                   },
        {8,  "Accepter"                                                                    },
        {9,  "Refuser"                                                                     },
        {10, "Retour"                                                                      },
        {11, "Oui"                                                                         },
        {12, "Non"                                                                         },
        {13, "OK"                                                                          },
        {14, "Annuler"                                                                     },
        {15, "Avec"                                                                        },
        {16, "Sans"                                                                        },
        {17, "Param\303\250tres par d\303\251faut"                                         },
        {18, "Param\303\250tres vid\303\251o"                                              },
        {19, "R\303\251solution"                                                           },
        {20, "Taux de rafra\303\256chissement"                                             },
        {21, "Synchronisation verticale"                                                   },
        {22, "Format"                                                                      },
        {23, "Graphismes am\303\251lior\303\251s"                                          },
        {24, "R\303\251solution \303\251cran"                                              },
        {25, "Utiliser les param\303\250tres actuels"                                      },
        {26, "Qualit\303\251 Textures"                                                     },
        {27, "Inverser"                                                                    },
        {28, "Vous devez red\303\251marrer le jeu pour que ces changements prennent effet."},
        {29, "Param\303\250tres d'effets"                                                  },
        {30, "FSAA"                                                                        },
        {31, "D\303\251fini en externe"                                                    },
        {32, "Ombres"                                                                      },
        {33, "Luminosit\303\251"                                                           },
        {34, "Distance d'affichage"                                                        },
        {35, "Flou (effet de vitesse)"                                                     },
        {36, "Qualit\303\251 lumi\303\250re dynamique"                                     },
        {37, "Lissage des ar\303\252tes"                                                   },
        {38, "Show Game Controls"                                                          },
        {39, NULL                                                                          },
    };
    MENU_SETTINGS_STRING_ENTRY dat_00827bc0[40] = {
        {0,  "Ende"                                                                             },
        {1,  "Zur\303\274ck zu Windows"                                                         },
        {2,  "Schlie\303\237en und zum Demo-Men\303\274"                                        },
        {3,  "Zur\303\274ck zum Spiel"                                                          },
        {4,  "Gering"                                                                           },
        {5,  "Mittel"                                                                           },
        {6,  "Hoch"                                                                             },
        {7,  "Fortsetzen"                                                                       },
        {8,  "Annehmen"                                                                         },
        {9,  "Ablehnen"                                                                         },
        {10, "Zur\303\274ck"                                                                    },
        {11, "Ja"                                                                               },
        {12, "Nein"                                                                             },
        {13, "OK"                                                                               },
        {14, "Abbrechen"                                                                        },
        {15, "Ein"                                                                              },
        {16, "Aus"                                                                              },
        {17, "Zur\303\274cksetzen auf Standardwerte"                                            },
        {18, "Grafikeinstellungen"                                                              },
        {19, "Bildschirmaufl\303\266sung"                                                       },
        {20, "Bildwiederholfrequenz"                                                            },
        {21, "Vertikales Synchr."                                                               },
        {22, "Seitenverh\303\244ltnis"                                                          },
        {23, "Erh\303\266hte Grafiken"                                                          },
        {24, "Aufl\303\266sung"                                                                 },
        {25, "Aktuelle Einstellungen verwenden"                                                 },
        {26, "Qualit\303\244t der Texturen"                                                     },
        {27, "Umkehren"                                                                         },
        {28, "Du musst das Spiel erneut starten, damit diese \303\204nderungen in Kraft treten."},
        {29, "Effekteinstellungen"                                                              },
        {30, "FSAA"                                                                             },
        {31, "Extern konfiguriert"                                                              },
        {32, "Schatten"                                                                         },
        {33, "Bloomeffekt"                                                                      },
        {34, "Sch\303\244rfentiefe"                                                             },
        {35, "Geschwindigkeitsunsch\303\244rfe"                                                 },
        {36, "Qualit\303\244t der dynamischen Beleuchtungsverh\303\244ltnisse"                  },
        {37, "Kantengl\303\244ttung"                                                            },
        {38, "Show Game Controls"                                                               },
        {39, NULL                                                                               },
    };
    MENU_SETTINGS_STRING_ENTRY dat_00827d00[40] = {
        {0,  "Esci"                                                                 },
        {1,  "Torna a Windows"                                                      },
        {2,  "Esci al Menu Demo"                                                    },
        {3,  "Torna al gioco"                                                       },
        {4,  "Bassa"                                                                },
        {5,  "Media"                                                                },
        {6,  "Alta"                                                                 },
        {7,  "Riprendi"                                                             },
        {8,  "Accetta"                                                              },
        {9,  "Annulla"                                                              },
        {10, "Indietro"                                                             },
        {11, "S\303\254"                                                            },
        {12, dat_0075f2c0                                                           },
        {13, "OK"                                                                   },
        {14, "Annulla"                                                              },
        {15, "On"                                                                   },
        {16, dat_0076b274                                                           },
        {17, "Impostazioni predefinite"                                             },
        {18, "Video"                                                                },
        {19, "Risoluzione schermo"                                                  },
        {20, "Frequenza di aggiornamento"                                           },
        {21, "Sincronizzazione verticale"                                           },
        {22, "Rapporto"                                                             },
        {23, "Grafica migliorata"                                                   },
        {24, "Da risoluzione"                                                       },
        {25, "Usa impostazioni attuali"                                             },
        {26, "Qualit\303\240 texture"                                               },
        {27, "Ripristina"                                                           },
        {28, "Devi riavviare il gioco affinch\303\251 le modifiche abbiano effetto."},
        {29, "Effetti"                                                              },
        {30, "FSAA"                                                                 },
        {31, "Impostato esternamente"                                               },
        {32, "Ombre"                                                                },
        {33, "Luminosit\303\240"                                                    },
        {34, "Profondit\303\240 di campo"                                           },
        {35, "Sfumatura movimento"                                                  },
        {36, "Qualit\303\240 luci dinamiche"                                        },
        {37, "Edge AA"                                                              },
        {38, "Show Game Controls"                                                   },
        {39, NULL                                                                   },
    };
    MENU_SETTINGS_STRING_ENTRY dat_00827e40[40] = {
        {0,  "Salir"                                                        },
        {1,  "Salir a Windows"                                              },
        {2,  "Salir al menu del demo"                                       },
        {3,  "Volver al juego"                                              },
        {4,  "Baja"                                                         },
        {5,  "Media"                                                        },
        {6,  "Alta"                                                         },
        {7,  "Reanudar"                                                     },
        {8,  "Aceptar"                                                      },
        {9,  "Rechazar"                                                     },
        {10, "Atr\303\241s"                                                 },
        {11, "S\303\255"                                                    },
        {12, dat_0075f2c0                                                   },
        {13, "Aceptar"                                                      },
        {14, "Cancelar"                                                     },
        {15, "S\303\255"                                                    },
        {16, dat_0075f2c0                                                   },
        {17, "Restablecer predeterminados"                                  },
        {18, "Ajustes de v\303\255deo"                                      },
        {19, "Resoluci\303\263n"                                            },
        {20, "Tasa de actualizaci\303\263n"                                 },
        {21, "Sincronizaci\303\263n vertical"                               },
        {22, "Relaci\303\263n de aspecto"                                   },
        {23, "Gr\303\241ficos mejorados"                                    },
        {24, "R. pantalla"                                                  },
        {25, "Usar ajustes de actuales"                                     },
        {26, "Calidad de textura"                                           },
        {27, "Revertir"                                                     },
        {28, "Debes reiniciar el juego para que estos cambios tengan efecto"},
        {29, "Ajustes de efectos"                                           },
        {30, "AAPC"                                                         },
        {31, "Ajustado externamente"                                        },
        {32, "Sombras"                                                      },
        {33, "Bloom"                                                        },
        {34, "Profundidad de campo"                                         },
        {35, "Distorsi\303\263n de velocidad"                               },
        {36, "Calidad de luz din\303\241mica"                               },
        {37, "Suavizado de bordes"                                          },
        {38, "Show Game Controls"                                           },
        {39, NULL                                                           },
    };
    MENU_SETTINGS_STRING_ENTRY dat_00827f80[40] = {
        {0,  "Forlad"                                                                    },
        {1,  "Forlad til Windows"                                                        },
        {2,  "Slut og g\303\245 til Demo-menuen"                                         },
        {3,  "Vend tilbage til spillet"                                                  },
        {4,  "Lav"                                                                       },
        {5,  "Mellem"                                                                    },
        {6,  "H\303\270j"                                                                },
        {7,  "Genoptag"                                                                  },
        {8,  "Accepter"                                                                  },
        {9,  "Afsl\303\245"                                                              },
        {10, "Tilbage"                                                                   },
        {11, "Ja"                                                                        },
        {12, "Nej"                                                                       },
        {13, "OK"                                                                        },
        {14, "Annuller"                                                                  },
        {15, "Til"                                                                       },
        {16, "Fra"                                                                       },
        {17, "Nulstil"                                                                   },
        {18, "Grafikindstillinger"                                                       },
        {19, "Sk\303\246rmopl\303\270sning"                                              },
        {20, "Billedfrekvens"                                                            },
        {21, "Sk\303\246rmopdatering"                                                    },
        {22, "St\303\270rrelsesforhold"                                                  },
        {23, "Forbedret grafik"                                                          },
        {24, "Fra sk\303\246rmopl\303\270sning"                                          },
        {25, "Brug nuv\303\246rende grafikindstillinger"                                 },
        {26, "Teksturkvalitet"                                                           },
        {27, "Vend tilbage"                                                              },
        {28, "Du skal genstarte spillet for at \303\246ndringerne tr\303\246der i kraft."},
        {29, "Effektindstilling"                                                         },
        {30, "Anti-aliasing, fuld sk\303\246rm"                                          },
        {31, "Ekstern"                                                                   },
        {32, "Skygger"                                                                   },
        {33, "Bloom"                                                                     },
        {34, "Synsdybde"                                                                 },
        {35, "Sl\303\270ring"                                                            },
        {36, "Dynamisk lys-kvalitet"                                                     },
        {37, "Kant-AA"                                                                   },
        {38, "Show Game Controls"                                                        },
        {39, NULL                                                                        },
    };
    const char *MenuSettingsStrings[39] = {0};
    i32 MenuSettingsStringsInitialized = 0;
}

extern "C" void MenuSettingsStringsInit(void) {
    MENU_SETTINGS_STRING_ENTRY *entry = dat_00827940;
    switch (NuLanguageGet()) {
        case NULANGUAGE_US_ENGLISH:
            entry = dat_00827800;
            break;
        case NULANGUAGE_FRENCH:
            entry = dat_00827a80;
            break;
        case NULANGUAGE_GERMAN:
            entry = dat_00827bc0;
            break;
        case NULANGUAGE_ITALIAN:
            entry = dat_00827d00;
            break;
        case NULANGUAGE_SPANISH:
            entry = dat_00827e40;
            break;
        case NULANGUAGE_DANISH:
            entry = dat_00827f80;
            break;
    }
    memset(MenuSettingsStrings, 0, sizeof(MenuSettingsStrings));
    while (entry->text != NULL) {
        MenuSettingsStrings[entry->id] = entry->text;
        ++entry;
    }
    MenuSettingsStringsInitialized = 1;
}

extern "C" {
    i32 PauseMenus_Align = 0;
    f32 PauseMenus_X = 0;
}

extern "C" void GameDrawMenuEntry(MENU *menu, char *text) {
    if (Paused != 0) {
        dme_align = PauseMenus_Align;
        menu->draw_x = PauseMenus_X;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}
