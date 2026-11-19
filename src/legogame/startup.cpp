#include "gameapi/apitext.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "gamelib/util/Refpack.h"
#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/startup/text.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/things.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/menus/screens/movies.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "legogame/pcbatman.h"
#include "legogame/startup.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nuonline_gen.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/movement/carrying.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/grabber.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/world/levels/episodeII.h"
#include "legoapi/world/levels/episodeV.h"
#include "legogame/batman_gameai.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/episodeVI.h"
#include "legoapi/items/collect/minikits.h"
#include "legogame/gamestatus.h"
#include "nu2api/nucore/nucore_includes.h"

extern "C" char *lsw_memcard_objname;
extern "C" void Alt_registerStatusScreen(void);

static i32 PcInputCommandAllowed(i32 port, i32 command) {
    if (PcInput_PortIsKeyboard(port) && ((command >= 4 && command <= 7) || (u32)(command - 8) <= 3)) {
        return 0;
    }
    return 1;
}

class Fade;
class FadeWipe;
class FadeStillWipe;
class FadeStill;
extern "C" {
    extern FADEINFO_s *pFadeInfo;
    extern Fade fade;
    extern FadeWipe fadeWipe;
    extern FadeStillWipe fadeStillWipe;
    extern FadeStill fadeStill;
}

extern "C" {

    void LoadPermLoadFonts(void);
    void LoadPermLoadSaveIconScene(void);
    void RegisterHelpers(void);

    extern AREA_DATA *Areas_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count, i32 *count_out);

    extern "C" {

        typedef struct nuprimvertex_s { // size 0x18
            f32 x;                      // 0x00
            f32 y;                      // 0x04
            f32 z;                      // 0x08
            u32 colour;                 // 0x0c
            f32 u;                      // 0x10
            f32 v;                      // 0x14
        } NUPRIMVERTEX;
    }

    extern "C" {

        static void NuPrim2DSetColor(i32 colour) {
            u32 converted;
            if (NuPrimNeedsOverbrightening != 0) {
                converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
            } else {
                converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
            }
            ((NUPRIMVERTEX *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
        }

        static void NuPrim2DSetUV(f32 u, f32 v) {
            NUPRIMVERTEX *vertex = (NUPRIMVERTEX *)NuPrimStreamBufferPointer->void_ptr;
            vertex->u = u;
            vertex->v = v;
        }

        static void LoadPermBuildLegalTextureName(char *name) {
            i32 pal = NuRegionGet() != 2;
            NuStrCpy(name, "legal");
            if (!pal && (Text_Language == NULANGUAGE_ENGLISH || Text_Language == NULANGUAGE_US_ENGLISH)) {
                NuStrCat(name, "_us");
            } else if (!pal && Text_Language == NULANGUAGE_FRENCH) {
                NuStrCat(name, "_usfrench");
            } else if (!pal && Text_Language == NULANGUAGE_SPANISH) {
                NuStrCat(name, "_usspanish");
            } else {
                NuStrCat(name, "_");
                NuStrCat(name, Text_GetLanguagePath(Text_Language));
            }
            if (Text_Language == NULANGUAGE_JAPANESE) {
                TextScaleX = PermLegalScale;
                TextScaleY = PermLegalScale;
            }
        }

        i32 legal_tid = 0;
        i32 loadlegal_done = 0;

        void LoadPermLoadLegalTexture(void) {
            VARIPTR texture_base;
            VARIPTR texture_end;
            char path[128];
            char name[64];
            if (legal_tid != 0 || loadlegal_done != 0) {
                return;
            }
            texture_end = superbuffer_end;
            texture_base.addr = texture_end.addr - 0x400000;
            Text_Language = NuLanguageGet();
            if (StartupGameFlag1 != 0) {
                NuStrCpy(path, "stuff\\legal\\");
                LoadPermBuildLegalTextureName(name);
                NuStrCat(path, name);
            } else {
                NuStrCpy(path, "stuff\\legal\\");
                NuStrCat(path, "pc\\pc_");
                NuStrCat(path, "legal");
                if (PAL != 0 || (Text_Language != NULANGUAGE_ENGLISH && Text_Language != NULANGUAGE_US_ENGLISH)) {
                    NuStrCat(path, "_");
                    NuStrCat(path, Text_GetLanguagePath(Text_Language));
                }
            }
            NuStrCat(path, ".dds");
            legal_tid = NuTexRead(path, &texture_base, texture_end);
            loadlegal_done = 1;
        }

        static i32 LoadPermCreateLegalMtl(NUMTL **output, i32 texture_id) {
            NUMTL *material;
            if (*output != NULL) {
                return 1;
            }
            material = NuMtlCreate(1);
            if (material == NULL) {
                return 0;
            }
            material->diffuse_r = 1.0f;
            material->diffuse_g = 1.0f;
            material->diffuse_b = 1.0f;
            material->opacity = 1.0f;
            material->shader.flags = 0x1000;
            material->attribs.word = (material->attribs.word & 0xffffefe2u) | 0xe022;
            material->texture_id = (i16)texture_id;
            NuMtlUpdate(material);
            *output = material;
            return 1;
        }

        NUMTL *legal_mtl = 0;

        void LoadPermDestroyLegalMtl(void) {
            if (legal_mtl != NULL) {
                NuMtlDestroy(legal_mtl);
                legal_mtl = NULL;
            }
        }

        void fun_004c9da0() {
        }

        void LoadPermLoadFonts(void) {
            if (StartupGameFlag0 != 0) {
                Text_LoadFont("stuff\\text\\pc_font", &permbuffer_ptr, &permbuffer_end);
            } else if (StartupGameFlag1 != 0) {
                if (Text_Language == NULANGUAGE_JAPANESE) {
                    Text_LoadFont("stuff\\text\\starwars_font_j", &permbuffer_ptr, &permbuffer_end);
                } else {
                    Text_LoadFont("stuff\\text\\starwars_font", &permbuffer_ptr, &permbuffer_end);
                }
            } else {
                Text_LoadFont("stuff\\text\\batman_font", &permbuffer_ptr, &permbuffer_end);
            }
        }

        /* LSW draws the save icon from pc_bits' "mem_card" special. */
        void LoadPermLoadSaveIconScene(void) {
            if (StartupGameFlag1 != 0) {
                ObjTab[0].name = lsw_memcard_objname;
                save_icon_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, "stuff\\pc_bits.gsc");
            } else {
                save_icon_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, "stuff\\save_icon.gsc");
            }
        }

        void LoadPermProgressDraw(f32, f32, f32 fade) {
            NuRndrRect2di(0, 0, 0x2800, 0xe20, ((i32)(fade * 100.0) << 24) | 0xffffff, FadeMtl);
        }

        f32 LoadPermFadeAlpha(f32 t, f32 duration) {
            f32 end;
            f32 total;

            if (t > 0.0f && t < duration + 0.3f + 0.3f + 0.2f) {
                if (t < 0.3f) {
                    return t / 0.3f;
                }
                end = duration + 0.3f;
                if (t < end) {
                    return 1.0f;
                }
                total = end + 0.3f;
                if (t < total) {
                    return 1.0f - (t - end) / (total - end);
                }
            }
            return 0.0f;
        }

        void memorymanager_s::Init(VARIPTR *buffer, VARIPTR *end, u32 bytes) {
            buffer->addr = (buffer->addr + 15) & ~15;
            buffer_start = buffer->addr;
            buffer->addr += bytes;
            buffer_end = buffer->addr;
            start_anchor = &buffer_start;
            end_anchor = &buffer_end;
            allocated_bytes = 0;
            size = *end_anchor - *start_anchor;
            last_end = 0;
            memset(free_lists, 0, sizeof(free_lists));
        }

        i16 ID_DEFAULTCHARACTER_LSW_A = -1;
        i16 ID_DEFAULTCHARACTER_LSW_B = -1;
        i16 dat_007f1824 = -1;
        i16 dat_007f1844 = -1;
        i16 dat_007f1854 = -1;
        i16 dat_007f1858 = -1;
        i16 dat_007f1864 = -1;
        i16 dat_007f1870 = -1;
        i16 dat_007f18a8 = -1;
        i16 dat_007f1a10 = -1;
        i16 dat_007f1a14 = -1;
        i16 dat_007f1ac0 = -1;
        i16 dat_007f1b10 = -1;
        i16 dat_007f1b20 = -1;
        i16 dat_007f1b24 = -1;
        i16 dat_007f1b28 = -1;
        i16 dat_007f1b48 = -1;
        CHARFIXUP CharacterFixupTable[219] = {
            {const_cast<char *>("weirdo1"), &PermanentWeirdo1Id},
            {const_cast<char *>("weirdo2"), &PermanentWeirdo2Id},
            {const_cast<char *>("batman"), &ID_DEFAULTCHARACTER_A},
            {const_cast<char *>("robin"), &ID_DEFAULTCHARACTER_B},
            {"henchman", &dat_007f1810},
            {const_cast<char *>("buggy"), reinterpret_cast<i16 *>(&GameObjectRotationBuggyId)},
            {const_cast<char *>("glide_pack"), &dat_007f17bc},
            {const_cast<char *>("radiocar"), reinterpret_cast<i16 *>(&GameObjectRotationRadioCarId)},
            {const_cast<char *>("policecar"), reinterpret_cast<i16 *>(&GameObjectRotationPoliceCarId)},
            {const_cast<char *>("twoface"), &dat_007f1820},
            {const_cast<char *>("harlequinn"), &dat_007f1824},
            {const_cast<char *>("catwoman"), &PunchRangeUnitId},
            {const_cast<char *>("whip"), &PunchRangeCompanionId},
            {const_cast<char *>("PenguinBomb"), &dat_007f1828},
            {const_cast<char *>("p_goon"), &id_PENGUIN_GOON},
            {const_cast<char *>("p_goon_gun"), &id_PENGUIN_GOON_GUN},
            {const_cast<char *>("j_goon"), &id_JOKER_GOON},
            {const_cast<char *>("j_goon_gun"), &id_JOKER_GOON_GUN},
            {const_cast<char *>("r_goon"), &id_RIDDLER_GOON},
            {const_cast<char *>("r_goon_gun"), &id_RIDDLER_GOON_GUN},
            {const_cast<char *>("jackinabox"), &dat_007f1844},
            {const_cast<char *>("mower"), reinterpret_cast<i16 *>(&GameObjectRotationMowerId)},
            {const_cast<char *>("dodgem"), &id_DODGEM},
            {const_cast<char *>("gyrocopter"), &id_GYROCOPTER},
            {const_cast<char *>("clayface"), &dat_007f1854},
            {const_cast<char *>("penguin"), &dat_007f17c8},
            {const_cast<char *>("umbrella"), &dat_007f17cc},
            {const_cast<char *>("poisonivy"), &dat_007f1858},
            {const_cast<char *>("quigonjinn"), &ID_DEFAULTCHARACTER_LSW_A},
            {"obiwankenobi", &ID_DEFAULTCHARACTER_LSW_B},
            {const_cast<char *>("princessleia"), &dat_007f1864},
            {const_cast<char *>("princessleia_slave"), &DefaultAnimationLeiaSlaveId},
            {const_cast<char *>("princessleia_Boushh"), &id_PRINCESSLEIABOUSHH},
            {const_cast<char *>("captainantilles"), &dat_007f1870},
            {const_cast<char *>("captaintarpals"), &id_CAPTAINTARPALS},
            {const_cast<char *>("imperialguard"), &id_IMPERIALGUARD},
            {const_cast<char *>("bodyguard"), &id_BODYGUARD},
            {const_cast<char *>("darthmaul"), &id_DARTHMAUL},
            {const_cast<char *>("gamorreanguard"), &id_GAMORREANGUARD},
            {"battledroid", &id_BATTLEDROID},
            {const_cast<char *>("battledroid_commander"), &id_BATTLEDROIDCOMMANDER},
            {const_cast<char *>("battledroid_geonosian"), &id_BATTLEDROIDGEONOSIAN},
            {const_cast<char *>("battledroid_security"), &id_BATTLEDROIDSECURITY},
            {const_cast<char *>("Clone_Ep3"), &id_CLONEEP3},
            {const_cast<char *>("Clone_Ep3_Sand"), &id_CLONEEP3SAND},
            {const_cast<char *>("naffdroid1"), &id_NAFFDROID1},
            {const_cast<char *>("naffdroid2"), &id_NAFFDROID2},
            {const_cast<char *>("naffdroid3"), &dat_007f18a8},
            {const_cast<char *>("naffdroid4"), &id_NAFFDROID4},
            {const_cast<char *>("mousedroid"), &id_MOUSEDROID},
            {const_cast<char *>("probedroid"), &dat_007f18b4},
            {const_cast<char *>("pkdroid"), &id_PKDROID},
            {const_cast<char *>("snake"), &id_SNAKE},
            {const_cast<char *>("bat"), &id_BAT},
            {const_cast<char *>("womprat"), &dat_007f18c4},
            {const_cast<char *>("yoda"), &id_YODA},
            {const_cast<char *>("yoda_ghost"), &id_YODAGHOST},
            {const_cast<char *>("moseisleycitizen"), &id_MOSEISLEYCITIZEN},
            {const_cast<char *>("cantinaaliens"), &id_CANTINAALIEN},
            {const_cast<char *>("cloudcitycitizen"), &id_CLOUDCITYCITIZEN},
            {const_cast<char *>("geonosian"), &id_GEONOSIAN},
            {const_cast<char *>("watto"), &id_WATTO},
            {const_cast<char *>("weirdo1"), &PermanentWeirdo1Id},
            {const_cast<char *>("weirdo2"), &PermanentWeirdo2Id},
            {const_cast<char *>("chewbacca"), &id_CHEWBACCA},
            {const_cast<char *>("wookie"), &id_WOOKIEE},
            {"atst", reinterpret_cast<i16 *>(&GameObjectRotationAtstId)},
            {"atst_lowres", &id_ATST_LOWRES},
            {const_cast<char *>("barman"), &id_BARMAN},
            {const_cast<char *>("destroyer"), &id_DROIDEKA},
            {const_cast<char *>("superbattledroid"), &id_SUPERBATTLEDROID},
            {const_cast<char *>("bobafett"), &id_BOBAFETT},
            {const_cast<char *>("4lom"), &dat_007f190c},
            {const_cast<char *>("trainingremote"), &dat_007f1910},
            {const_cast<char *>("atat"), &id_ATAT},
            {const_cast<char *>("service_car"), &id_SERVICECAR},
            {const_cast<char *>("dragbomb"), reinterpret_cast<i16 *>(&id_DRAGBOMB)},
            {"clonewalker", &id_CLONEWALKER},
            {const_cast<char *>("ewok"), &dat_007f1924},
            {const_cast<char *>("wicket"), &dat_007f1928},
            {const_cast<char *>("catapult"), &id_CATAPULT},
            {const_cast<char *>("basketcannon"), &id_BASKETCANNON},
            {const_cast<char *>("bantha"), &id_BANTHA},
            {const_cast<char *>("bomarrmonk"), &id_BOMARRMONK},
            {const_cast<char *>("dewback"), &id_DEWBACK},
            {const_cast<char *>("speeder_land"), &id_LANDSPEEDER},
            {const_cast<char *>("tauntaun"), &id_TAUNTAUN},
            {const_cast<char *>("speederbike"), &id_SPEEDERBIKE},
            {const_cast<char *>("heavyrepeatingcannon"), &id_HEAVYREPEATINGCANNON},
            {const_cast<char *>("biggun"), &id_BIGGUN},
            {const_cast<char *>("troopercannon"), &id_TROOPERCANNON},
            {const_cast<char *>("jabba"), &DefaultAnimationJabbaId},
            {const_cast<char *>("slave1"), &dat_007f195c},
            {const_cast<char *>("ywing"), &id_YWING},
            {const_cast<char *>("tiebomber"), &id_TIEBOMBER},
            {const_cast<char *>("grievous"), &id_GRIEVOUS},
            {const_cast<char *>("tiefighterdarth"), &dat_007f196c},
            {const_cast<char *>("tiefighter"), &id_TIEFIGHTER},
            {const_cast<char *>("grabberControl"), &id_GRABCONTROL},
            {const_cast<char *>("grabberr2control"), &id_GRABR2CONTROL},
            {"grabber", &GrabMachineId},
            {const_cast<char *>("magnet"), &GrabMagnetId},
            {const_cast<char *>("robot_base"), &RobotBaseId},
            {const_cast<char *>("jawa"), &id_JAWA},
            {const_cast<char *>("lukeskywalker_dagobah"), &id_LUKESKYWALKERDAGOBAH},
            {const_cast<char *>("SnowMob"), &id_SNOWMOB},
            {const_cast<char *>("snowtrooper"), &id_SNOWTROOPER},
            {const_cast<char *>("deathstartrooper"), &id_DEATHSTARTROOPER},
            {const_cast<char *>("darthvader"), &dat_007f199c},
            {const_cast<char *>("theemperor"), &id_THEEMPEROR},
            {const_cast<char *>("millenniumfalcon"), &dat_007f19a4},
            {const_cast<char *>("speederbikesnow"), &dat_007f19a8},
            {const_cast<char *>("Kaadu"), &dat_007f19ac},
            {const_cast<char *>(AreaFixUpName_Gungan), &dat_007f19b0},
            {const_cast<char *>("Falumpaset"), &dat_007f19b4},
            {const_cast<char *>("Stap"), &id_STAP},
            {const_cast<char *>("Stap2"), &id_STAP2},
            {const_cast<char *>("jumbohomingdroid"), &id_JUMBOHOMINGDROID},
            {const_cast<char *>("JarJarBinks"), &dat_007f19c4},
            {const_cast<char *>("PadmeClawed"), &dat_007f19c8},
            {const_cast<char *>("Anakin_padawan"), &id_ANAKINPADAWAN},
            {const_cast<char *>("ObiWanKenobi_JediMaster"), &dat_007f19d0},
            {const_cast<char *>("shaakti"), &dat_007f19d4},
            {const_cast<char *>("luminara"), &dat_007f19d8},
            {const_cast<char *>("bob"), &id_BOB},
            {const_cast<char *>("JangoFett"), &id_JANGOFETT},
            {const_cast<char *>("JediStarfighter_Yellow_Ep3"), &id_JEDISTARFIGHTERYELLOWEP3},
            {const_cast<char *>("JediStarfighter_Red_Ep3"), &dat_007f19e8},
            {const_cast<char *>("Rancor"), &id_RANCOR},
            {const_cast<char *>("obiwankenobi_ep3"), &id_OBIWANKENOBIEP3},
            {const_cast<char *>("anakin_jedi"), &id_ANAKINJEDI},
            {"republicgunship", &id_REPUBLICGUNSHIP},
            {const_cast<char *>("republicgunship_green"), &id_REPUBLICGUNSHIP_GREEN},
            {const_cast<char *>("newrepublicgunship"), &id_NEW_REPUBLIC_GUNSHIP},
            {const_cast<char *>("newrepublicgunship_green"), &id_NEW_REPUBLIC_GUNSHIP_GREEN},
            {const_cast<char *>("countdooku"), &id_DOOKU},
            {const_cast<char *>("palpatine"), &dat_007f1a0c},
            {const_cast<char *>("kitfisto"), &dat_007f1a10},
            {const_cast<char *>("r2d2"), &dat_007f1a14},
            {const_cast<char *>("r2q5"), &id_R2Q5},
            {const_cast<char *>("xwing"), &id_XWING},
            {const_cast<char *>("royalguard"), &id_ROYALGUARD},
            {const_cast<char *>("snowspeeder"), &dat_007f1a24},
            {const_cast<char *>("tieinterceptor"), &dat_007f1a28},
            {const_cast<char *>("imperialshuttle"), &dat_007f1a2c},
            {const_cast<char *>("ugnaught"), &id_UGNAUGHT},
            {const_cast<char *>("sentrydroid"), &id_SENTRYDROID},
            {const_cast<char *>("kaminoandroid"), &dat_007f1a38},
            {"stormtrooper", &dat_007f1a3c},
            {const_cast<char *>("beachtrooper"), &dat_007f1a40},
            {const_cast<char *>("ImperialShuttlePilot"), &dat_007f1a44},
            {const_cast<char *>("imperialofficer"), &dat_007f1a48},
            {const_cast<char *>("grandmofftarkin"), &dat_007f1a4c},
            {const_cast<char *>("gonkdroid"), reinterpret_cast<i16 *>(&id_GONKDROID)},
            {const_cast<char *>("c3po"), &id_C3PO},
            {const_cast<char *>("tc14"), &id_TC14},
            {const_cast<char *>("mooncar"), &id_MOONCAR},
            {const_cast<char *>("mapcar"), &id_MAPCAR},
            {const_cast<char *>("wookieflyer"), &id_WOOKIEFLYER},
            {const_cast<char *>("imperialspy"), &id_IMPERIALSPY},
            {const_cast<char *>("greedo"), &id_GREEDO},
            {const_cast<char *>("bossk"), &id_BOSSK},
            {const_cast<char *>("cantinaband"), &id_CANTINABAND},
            {const_cast<char *>("pitdroid"), &dat_007f1a78},
            {const_cast<char *>("TownCar"), &dat_007f1a7c},
            {const_cast<char *>("Tractor"), &id_TRACTOR},
            {const_cast<char *>("fireTruck"), &id_FIRETRUCK},
            {const_cast<char *>("lifeBoat"), &dat_007f1a88},
            {const_cast<char *>("lamasu"), &DefaultAnimationLamaSuId},
            {const_cast<char *>("taunwe"), &DefaultAnimationTaunWeId},
            {const_cast<char *>("buzzdroid"), &id_BUZZDROID},
            {const_cast<char *>("clonearc"), &dat_007f1a98},
            {"naboostarfighter", &dat_007f1a9c},
            {const_cast<char *>("naboostarfighter_lime"), &dat_007f1aa0},
            {const_cast<char *>("hansolo_indy"), &dat_007f17d4},
            {const_cast<char *>("flashspeeder"), &id_FLASHSPEEDER},
            {const_cast<char *>("skeleton"), &DefaultMotionSkeletonId},
            {const_cast<char *>("indianajones"), &dat_007f17d0},
            {const_cast<char *>("satipo"), &dat_007f1aac},
            {const_cast<char *>("chuck"), &dat_007f1ab0},
            {const_cast<char *>("belloq"), &dat_007f1ab4},
            {const_cast<char *>("hovitostribesman"), &VehicleSpecialShooterId},
            {const_cast<char *>("barranca"), &dat_007f1abc},
            {const_cast<char *>("crocodile"), &dat_007f1ac0},
            {const_cast<char *>("indyspider"), &ZipUpLineCharacterId},
            {const_cast<char *>("indysnake"), &DefaultMotionIndySnakeId},
            {const_cast<char *>("mini_x_wing"), &id_MINIXWING},
            {const_cast<char *>("mini_y_wing"), &id_MINIYWING},
            {const_cast<char *>("mini_tie_interceptor"), &dat_007f1ad8},
            {const_cast<char *>("mini_tie_bomber"), &id_MINITIEBOMBER},
            {const_cast<char *>("mini_atat"), &id_MINIATAT},
            {const_cast<char *>("mini_star_destroyer"), &id_MINISTARDESTROYER},
            {const_cast<char *>("mini_royal_starship"), &id_MINIROYALSTARSHIP},
            {const_cast<char *>("mini_imperial_shuttle"), &dat_007f1aec},
            {const_cast<char *>("Mini_Millennium_Falcon"), &dat_007f1af0},
            {"mini_atst", &id_MINIATST},
            {const_cast<char *>("mini_atte"), &id_MINIATTE},
            {const_cast<char *>("mini_slave1"), &id_MINISLAVE1},
            {const_cast<char *>("mini_droideka"), &id_MINIDROIDEKA},
            {const_cast<char *>("mini_tie_fighter"), &dat_007f1b00},
            {const_cast<char *>("mini_tie_advanced"), &dat_007f1b04},
            {const_cast<char *>("mini_sith_infiltrator"), &dat_007f1b08},
            {const_cast<char *>("mini_solar_sailor"), &dat_007f1b0c},
            {const_cast<char *>("mini_sand_crawler"), &dat_007f1b10},
            {const_cast<char *>("anakinspod"), &id_ANAKINSPOD},
            {const_cast<char *>("anakinspod_green"), &id_ANAKINSPODGREEN},
            {const_cast<char *>("sebulbaspod"), &id_SEBULBASPOD},
            {const_cast<char *>("gasganospod"), &dat_007f1b20},
            {const_cast<char *>("anothermiscpod"), &dat_007f1b24},
            {const_cast<char *>("anothermiscpod2"), &dat_007f1b28},
            {const_cast<char *>("newanakinspod"), &id_ANAKINSNEWPOD},
            {const_cast<char *>("newanakinspod_green"), &id_ANAKINSNEWPODGREEN},
            {const_cast<char *>("anakinsspeeder"), &dat_007f1b34},
            {const_cast<char *>("anakinsspeeder_green"), &dat_007f1b38},
            {const_cast<char *>("ZamsSpeeder"), &id_ZAMSSPEEDER},
            {const_cast<char *>("vulturedroid"), &dat_007f1b40},
            {const_cast<char *>("droidtrifighter"), &dat_007f1b44},
            {const_cast<char *>("droidstarfighter"), &dat_007f1b48},
            {NULL, NULL},
        };
        LEVELSPLINE LevelSplinesTable[26] = {
            {NULL, "start", 2, 0, -1, -1},
            {NULL, "start_cam", 2, 0, -1, -1},
            {NULL, "point_cam", 2, 2, -1, -1},
            {NULL, "finish_line", 2, 2, -1, -1},
            {NULL, "cam_start", 2, 2, -1, -1},
            {NULL, "split", 2, 2, -1, -1},
            {NULL, "map_cam", 2, 0, -1, -1},
            {NULL, "map_look", 2, 0, -1, -1},
            {NULL, "e1_entrance", 2, 2, -1, -1},
            {NULL, "e2_entrance", 2, 2, -1, -1},
            {NULL, "e3_entrance", 2, 2, -1, -1},
            {NULL, "e4_entrance", 2, 2, -1, -1},
            {NULL, "e5_entrance", 2, 2, -1, -1},
            {NULL, "e6_entrance", 2, 2, -1, -1},
            {NULL, "jabba_entrance1", 2, 2, -1, -1},
            {NULL, "char_cam", 2, 2, -1, -1},
            {NULL, "Turn_Around_1", 2, 1024, -1, -1},
            {NULL, "Turn_Around_2", 2, 1024, -1, -1},
            {NULL, "custard", 4, 4, -1, -1},
            {NULL, "bonus_zone_1", 3, 0, -1, -1},
            {NULL, "bonus_zone_2", 3, 0, -1, -1},
            {NULL, "bonus_zone_3", 3, 0, -1, -1},
            {NULL, "bonus_zone_4", 3, 0, -1, -1},
            {NULL, "bonus_zone_5", 3, 0, -1, -1},
            {NULL, "mission_cam", 2, 2, -1, -1},
            {NULL, NULL, 0, 0, 0, 0},
        };
        LEVEL_OBJECT_CONFIG ObjTab[750] = {
            {4, 0, 0, const_cast<char *>("save_icon")},
            {1, 0, 0, const_cast<char *>("parallax")},
            {1, 0, 0, const_cast<char *>("legal")},
            {1, 0, 0, const_cast<char *>("status_01")},
            {1, 0, 0, const_cast<char *>("status_02")},
            {1, 0, 0, const_cast<char *>("status_03")},
            {1, 0, 0, const_cast<char *>("status_04")},
            {1, 0, 0, const_cast<char *>("status_05")},
            {1, 0, 0, const_cast<char *>("status_06")},
            {1, 0, 0, const_cast<char *>("rod")},
            {1, 0, 0, const_cast<char *>("phone")},
            {1, 0, 0, const_cast<char *>("slave1_level")},
            {0, 0, 0, const_cast<char *>("machinegun")},
            {0, 0, 0, "blaster"},
            {0, 0, 0, const_cast<char *>("walkie_talkie")},
            {0, 0, 0, const_cast<char *>("pistolgrey")},
            {0, 0, 0, const_cast<char *>("pistolchrome")},
            {0, 0, 0, const_cast<char *>("lightsabrehandle")},
            {0, 0, 0, const_cast<char *>("doublelightsabrehandle")},
            {0, 0, 0, const_cast<char *>("bentlightsabre")},
            {0, 0, 0, "gun"},
            {0, 0, 0, const_cast<char *>("bowcaster")},
            {0, 0, 0, const_cast<char *>("blaster_blue")},
            {0, 0, 0, const_cast<char *>("submachinegun")},
            {0, 0, 0, const_cast<char *>("poop")},
            {0, 0, 0, const_cast<char *>("peri")},
            {0, 0, 0, const_cast<char *>("spear")},
            {0, 0, 0, const_cast<char *>("spearblack")},
            {0, 0, 0, const_cast<char *>("storm_target")},
            {0, 0, 0, const_cast<char *>("bounty_target")},
            {0, 0, 0, const_cast<char *>("lever")},
            {0, 0, 0, const_cast<char *>("lever_base")},
            {0, 0, 0, const_cast<char *>("lever_nob1")},
            {0, 0, 0, const_cast<char *>("lever_nob2")},
            {0, 0, 0, const_cast<char *>("lever_nob3")},
            {0, 0, 0, const_cast<char *>("lever_nob4")},
            {0, 0, 0, const_cast<char *>("lever_nob5")},
            {0, 0, 0, const_cast<char *>("lever_nob6")},
            {0, 0, 0, const_cast<char *>("lever_nob7")},
            {0, 0, 0, const_cast<char *>("lever_nob8")},
            {0, 0, 0, const_cast<char *>("lever_nob9")},
            {0, 0, 0, const_cast<char *>("lever_glow")},
            {0, 0, 0, const_cast<char *>("lever_on")},
            {0, 0, 0, const_cast<char *>("lever_off")},
            {0, 0, 0, const_cast<char *>("hack_control")},
            {0, 0, 0, const_cast<char *>("hack_door1")},
            {0, 0, 0, const_cast<char *>("hack_door2")},
            {0, 0, 0, const_cast<char *>("winch_base")},
            {0, 0, 0, const_cast<char *>("winch_bracket")},
            {0, 0, 0, const_cast<char *>("winch")},
            {0, 0, 0, const_cast<char *>("batarang")},
            {0, 0, 0, const_cast<char *>("robinarang")},
            {0, 0, 0, const_cast<char *>("bullet")},
            {0, 1, 0, const_cast<char *>("pistolflash")},
            {0, 0, 0, const_cast<char *>("signal_base")},
            {0, 0, 0, const_cast<char *>("bat_signal_off")},
            {0, 0, 0, const_cast<char *>("bat_signal_on")},
            {0, 0, 0, const_cast<char *>("bat_signal_light")},
            {0, 0, 0, const_cast<char *>("robin_signal_off")},
            {0, 0, 0, const_cast<char *>("robin_signal_on")},
            {0, 0, 0, const_cast<char *>("robin_signal_light")},
            {0, 0, 0, const_cast<char *>("grapple_point")},
            {0, 0, 0, const_cast<char *>("grapple_point1")},
            {0, 0, 0, const_cast<char *>("tightrope_base1")},
            {0, 0, 0, const_cast<char *>("tightrope_base2")},
            {0, 0, 0, const_cast<char *>("metal_bit1")},
            {0, 0, 0, const_cast<char *>("metal_bit2")},
            {0, 0, 0, const_cast<char *>("metal_bit3")},
            {0, 0, 0, const_cast<char *>("metal_bit4")},
            {0, 0, 0, const_cast<char *>("metal_bit5")},
            {0, 0, 0, const_cast<char *>("metal_bit6")},
            {0, 0, 0, const_cast<char *>("metal_bit7")},
            {0, 0, 0, const_cast<char *>("attract_o_matic")},
            {0, 0, 0, const_cast<char *>("transform1a")},
            {0, 0, 0, const_cast<char *>("transform1b")},
            {0, 0, 0, const_cast<char *>("ledge_1")},
            {0, 0, 0, const_cast<char *>("ledge_2")},
            {0, 0, 0, const_cast<char *>("ledge_3")},
            {0, 0, 0, const_cast<char *>("ledge_4")},
            {0, 0, 0, const_cast<char *>("ledge_5")},
            {0, 0, 0, const_cast<char *>("ledge_6")},
            {0, 0, 0, const_cast<char *>("door_1_l")},
            {0, 0, 0, const_cast<char *>("door_1_r")},
            {0, 0, 0, const_cast<char *>("zipup_hook")},
            {0, 0, 0, const_cast<char *>("zipup_hook1")},
            {0, 0, 0, const_cast<char *>("zipup_target")},
            {0, 0, 0, const_cast<char *>("target")},
            {0, 0, 0, const_cast<char *>("Spinner_default_Base")},
            {0, 0, 0, const_cast<char *>("Spinner_default_Arm")},
            {0, 0, 0, const_cast<char *>("carrot")},
            {1, 0, 0, const_cast<char *>("basketball")},
            {0, 0, 1, const_cast<char *>("blasterbolt")},
            {0, 0, 1, const_cast<char *>("blasterboltglow")},
            {0, 0, 1, const_cast<char *>("blasterbolt_red")},
            {0, 0, 1, const_cast<char *>("blasterboltglow_red")},
            {0, 0, 1, const_cast<char *>("blasterbolt_green")},
            {0, 0, 1, const_cast<char *>("blasterboltglow_green")},
            {0, 0, 1, const_cast<char *>("blasterbolt_blue")},
            {0, 0, 1, const_cast<char *>("blasterboltglow_blue")},
            {0, 0, 1, const_cast<char *>("double_blasterBolt")},
            {0, 0, 1, const_cast<char *>("double_blasterBoltglow")},
            {0, 0, 1, const_cast<char *>("lightsabrered")},
            {0, 0, 1, const_cast<char *>("lightsabreglowred")},
            {0, 0, 1, const_cast<char *>("lightsabregreen")},
            {0, 0, 1, const_cast<char *>("lightsabreglowgreen")},
            {0, 0, 1, const_cast<char *>("lightsabreblue")},
            {0, 0, 1, const_cast<char *>("lightsabreglow")},
            {0, 0, 1, const_cast<char *>("lightsabrepurple")},
            {0, 0, 1, const_cast<char *>("lightsabreglowpurple")},
            {0, 0, 1, const_cast<char *>("doublelightsabrered")},
            {0, 0, 1, const_cast<char *>("doublelightsabreglow")},
            {3, 0, 1, const_cast<char *>("destroyershield")},
            {3, 0, 1, const_cast<char *>("destroyershield_red")},
            {5, 0, 1, const_cast<char *>("xwingbolt")},
            {5, 0, 1, const_cast<char *>("xwingboltglow")},
            {5, 0, 1, const_cast<char *>("laser_green")},
            {5, 0, 1, const_cast<char *>("laser_green_glow")},
            {0, 0, 1, const_cast<char *>("green_blob")},
            {0, 0, 1, const_cast<char *>("green_blob_glow")},
            {5, 0, 1, const_cast<char *>("xwing_thruster")},
            {5, 0, 1, const_cast<char *>("xwing_thruster_glow")},
            {5, 0, 1, const_cast<char *>("photontorpedo")},
            {5, 0, 1, const_cast<char *>("photontorpedoglow")},
            {5, 0, 1, const_cast<char *>("photontorpedoglow1")},
            {5, 0, 1, const_cast<char *>("jedi_starfighter_blasterBolt")},
            {5, 0, 1, const_cast<char *>("jedi_starfighter_blasterBoltglow")},
            {0, 0, 2, const_cast<char *>("blasterbolt_ref")},
            {0, 0, 2, const_cast<char *>("blasterboltglow_ref")},
            {0, 0, 2, const_cast<char *>("blasterbolt_red_ref")},
            {0, 0, 2, const_cast<char *>("blasterboltglow_red_ref")},
            {0, 0, 2, const_cast<char *>("blasterbolt_green_ref")},
            {0, 0, 2, const_cast<char *>("blasterboltglow_green_ref")},
            {0, 0, 2, const_cast<char *>("blasterbolt_blue_ref")},
            {0, 0, 2, const_cast<char *>("blasterboltglow_blue_ref")},
            {0, 0, 2, const_cast<char *>("double_blasterbolt_ref")},
            {0, 0, 2, const_cast<char *>("double_blasterboltGlow_ref")},
            {0, 0, 2, const_cast<char *>("lightsabrered_ref")},
            {0, 0, 2, const_cast<char *>("lightsabreglowred_ref")},
            {0, 0, 2, const_cast<char *>("lightsabregreen_ref")},
            {0, 0, 2, const_cast<char *>("lightsabreglowgreen_ref")},
            {0, 0, 2, const_cast<char *>("lightsabreblue_ref")},
            {0, 0, 2, const_cast<char *>("lightsabreglow_ref")},
            {0, 0, 2, const_cast<char *>("lightsabrepurple_ref")},
            {0, 0, 2, const_cast<char *>("lightsabreglowpurple_ref")},
            {0, 0, 2, const_cast<char *>("doublelightsabrered_ref")},
            {0, 0, 2, const_cast<char *>("doublelightsabreglow_ref")},
            {3, 0, 2, const_cast<char *>("destroyershield_ref")},
            {3, 0, 2, const_cast<char *>("destroyershield_red_ref")},
            {0, 0, 2, const_cast<char *>("xwingbolt_ref")},
            {0, 0, 2, const_cast<char *>("xwingboltglow_ref")},
            {0, 0, 2, const_cast<char *>("laser_green_ref")},
            {0, 0, 2, const_cast<char *>("laserglow_green_ref")},
            {0, 0, 2, const_cast<char *>("green_blob_ref")},
            {0, 0, 2, const_cast<char *>("green_blob_glow_ref")},
            {0, 0, 2, const_cast<char *>("xwing_thruster_ref")},
            {0, 0, 2, const_cast<char *>("xwing_thruster_glow_ref")},
            {0, 0, 2, const_cast<char *>("photontorpedo_ref")},
            {0, 0, 2, const_cast<char *>("photontorpedo_glow_ref")},
            {0, 0, 2, const_cast<char *>("photontorpedo_glow1_ref")},
            {5, 0, 2, const_cast<char *>("jedi_starfighter_blasterBolt_ref")},
            {5, 0, 2, const_cast<char *>("jedi_starfighter_blasterBoltglow_ref")},
            {3, 0, 0, const_cast<char *>("question_icon")},
            {3, 0, 0, const_cast<char *>("mouse_glow")},
            {3, 0, 0, const_cast<char *>("silhouette_icon")},
            {3, 0, 0, const_cast<char *>("silhouette_icon1")},
            {0, 0, 0, const_cast<char *>("icon_back_green")},
            {0, 0, 0, const_cast<char *>("icon_back_blue")},
            {0, 0, 0, const_cast<char *>("icon_back_neutral")},
            {0, 0, 0, const_cast<char *>("icon_back_gold")},
            {0, 0, 0, const_cast<char *>("coin_side1")},
            {0, 0, 0, const_cast<char *>("coin_side2")},
            {0, 0, 0, const_cast<char *>("coin_side3")},
            {0, 0, 0, const_cast<char *>("coin_side4")},
            {0, 0, 0, const_cast<char *>("coin_side5")},
            {0, 0, 0, const_cast<char *>("coin_side6")},
            {0, 0, 0, const_cast<char *>("coin_side7")},
            {0, 0, 0, const_cast<char *>("coin_side8")},
            {0, 0, 0, const_cast<char *>("coin_side9")},
            {0, 0, 0, const_cast<char *>("coin_side10")},
            {0, 0, 0, const_cast<char *>("silver_pan_coin1")},
            {0, 0, 0, const_cast<char *>("silver_pan_coin2")},
            {0, 0, 0, const_cast<char *>("silver_pan_coin3")},
            {0, 0, 0, const_cast<char *>("silver_pan_coin4")},
            {0, 0, 0, const_cast<char *>("silver_coin1")},
            {0, 0, 0, const_cast<char *>("silver_coin2")},
            {0, 0, 0, const_cast<char *>("silver_coin3")},
            {0, 0, 0, const_cast<char *>("silver_coin4")},
            {0, 0, 0, const_cast<char *>("gold_pan_coin1")},
            {0, 0, 0, const_cast<char *>("gold_pan_coin2")},
            {0, 0, 0, const_cast<char *>("gold_pan_coin3")},
            {0, 0, 0, const_cast<char *>("gold_pan_coin4")},
            {0, 0, 0, const_cast<char *>("gold_coin1")},
            {0, 0, 0, const_cast<char *>("gold_coin2")},
            {0, 0, 0, const_cast<char *>("gold_coin3")},
            {0, 0, 0, const_cast<char *>("gold_coin4")},
            {0, 0, 0, const_cast<char *>("blue_pan_coin1")},
            {0, 0, 0, const_cast<char *>("blue_pan_coin2")},
            {0, 0, 0, const_cast<char *>("blue_pan_coin3")},
            {0, 0, 0, const_cast<char *>("blue_pan_coin4")},
            {0, 0, 0, const_cast<char *>("blue_coin1")},
            {0, 0, 0, const_cast<char *>("blue_coin2")},
            {0, 0, 0, const_cast<char *>("blue_coin3")},
            {0, 0, 0, const_cast<char *>("blue_coin4")},
            {0, 0, 0, const_cast<char *>("heart")},
            {0, 0, 0, const_cast<char *>("flat_heart")},
            {0, 0, 0, const_cast<char *>("silver_heart")},
            {0, 0, 0, const_cast<char *>("mini_kit_pickup")},
            {0, 0, 0, const_cast<char *>("char_pickup")},
            {0, 0, 0, const_cast<char *>("plop")},
            {0, 0, 0, const_cast<char *>("power_up_glow")},
            {0, 0, 0, const_cast<char *>("red_brick")},
            {0, 0, 0, const_cast<char *>("gold_brick")},
            {0, 0, 0, const_cast<char *>("info")},
            {0, 0, 0, const_cast<char *>("purple_coin1")},
            {0, 0, 0, const_cast<char *>("purple_coin2")},
            {0, 0, 0, const_cast<char *>("purple_coin3")},
            {0, 0, 0, const_cast<char *>("purple_coin4")},
            {0, 0, 0, const_cast<char *>("purple_pan_coin1")},
            {0, 0, 0, const_cast<char *>("purple_pan_coin2")},
            {0, 0, 0, const_cast<char *>("purple_pan_coin3")},
            {0, 0, 0, const_cast<char *>("purple_pan_coin4")},
            {0, 0, 0, const_cast<char *>("ripple_green")},
            {0, 0, 0, const_cast<char *>("ripple_green1")},
            {0, 0, 0, const_cast<char *>("ripple_red")},
            {0, 0, 0, const_cast<char *>("ripple_red1")},
            {0, 0, 0, const_cast<char *>("ripple_blue")},
            {0, 0, 0, const_cast<char *>("ripple_blue1")},
            {0, 0, 0, const_cast<char *>("ripple_purple")},
            {0, 0, 0, const_cast<char *>("ripple_purple1")},
            {0, 0, 0, const_cast<char *>("zip_ring")},
            {0, 0, 0, const_cast<char *>("crate")},
            {0, 0, 0, const_cast<char *>("moustache")},
            {0, 0, 0, const_cast<char *>("jango_rocket")},
            {0, 0, 0, const_cast<char *>("thermal")},
            {0, 0, 0, const_cast<char *>("thermal_red_off")},
            {0, 0, 0, const_cast<char *>("thermal_red_on")},
            {0, 0, 0, const_cast<char *>("bomb_main")},
            {0, 0, 0, const_cast<char *>("bomb_off")},
            {0, 0, 0, const_cast<char *>("bomb_red_off")},
            {0, 0, 0, const_cast<char *>("bomb_red_on")},
            {3, 0, 0, const_cast<char *>("part_boulder")},
            {3, 0, 0, const_cast<char *>("pebble_ewok")},
            {3, 0, 0, const_cast<char *>("photontorpedo_ewok")},
            {3, 0, 0, const_cast<char *>("photontorpedoglow_ewok")},
            {3, 0, 0, const_cast<char *>("pebble_wicket")},
            {3, 0, 0, const_cast<char *>("photontorpedo_wicket")},
            {3, 0, 0, const_cast<char *>("photontorpedoglow_wicket")},
            {3, 0, 0, const_cast<char *>("umbrella1")},
            {3, 0, 0, const_cast<char *>("umbrella2")},
            {3, 0, 0, const_cast<char *>("twoface_coin")},
            {0, 0, 0, const_cast<char *>("hat_1")},
            {0, 0, 0, const_cast<char *>("hat_3")},
            {0, 0, 0, const_cast<char *>("hat_4")},
            {0, 0, 0, const_cast<char *>("hat_5")},
            {0, 0, 0, const_cast<char *>("stormTrooperHelmet")},
            {0, 0, 0, const_cast<char *>("bountyHelmet")},
            {0, 0, 0, const_cast<char *>("C3PO_base")},
            {0, 0, 0, const_cast<char *>("C3PO_pic")},
            {0, 0, 0, const_cast<char *>("TC14_pic")},
            {0, 0, 0, const_cast<char *>("C3PO_static")},
            {0, 0, 0, const_cast<char *>("C3PO_light_on")},
            {0, 0, 0, const_cast<char *>("C3PO_light_off")},
            {0, 0, 0, const_cast<char *>("R2_base")},
            {0, 0, 0, const_cast<char *>("R2_base1")},
            {0, 0, 0, const_cast<char *>("R2_pic")},
            {0, 0, 0, const_cast<char *>("R4_pic")},
            {0, 0, 0, const_cast<char *>("R2_static")},
            {0, 0, 0, const_cast<char *>("R2_light_on")},
            {0, 0, 0, const_cast<char *>("R2_light_off")},
            {0, 0, 0, const_cast<char *>("Bounty_base")},
            {0, 0, 0, const_cast<char *>("Bounty_pic")},
            {0, 0, 0, const_cast<char *>("Bounty_static")},
            {0, 0, 0, const_cast<char *>("Bounty_light_on")},
            {0, 0, 0, const_cast<char *>("Bounty_light_off")},
            {0, 0, 0, const_cast<char *>("Bounty_cam")},
            {0, 0, 0, const_cast<char *>("Bounty_Target")},
            {0, 0, 0, const_cast<char *>("Storm_base")},
            {0, 0, 0, const_cast<char *>("Storm_pic")},
            {0, 0, 0, const_cast<char *>("Storm_static")},
            {0, 0, 0, const_cast<char *>("Storm_light_on")},
            {0, 0, 0, const_cast<char *>("Storm_light_off")},
            {0, 0, 0, const_cast<char *>("Storm_cam")},
            {0, 0, 0, const_cast<char *>("Storm_Target")},
            {0, 0, 0, const_cast<char *>("Hat_machine_base")},
            {0, 0, 0, const_cast<char *>("Hat_machine_down")},
            {0, 0, 0, const_cast<char *>("Hat_machine_out")},
            {0, 0, 0, const_cast<char *>("Hat_machine_on")},
            {0, 0, 0, const_cast<char *>("Hat_machine_off")},
            {0, 0, 0, const_cast<char *>("lever_helmet")},
            {0, 0, 0, const_cast<char *>("Hat_machine_down1")},
            {0, 0, 0, const_cast<char *>("photon2")},
            {0, 0, 0, const_cast<char *>("photon1")},
            {0, 0, 0, const_cast<char *>("zip_flower1")},
            {0, 0, 0, const_cast<char *>("zip_flower2")},
            {0, 0, 0, const_cast<char *>("zip_flower3")},
            {5, 0, 0, const_cast<char *>("FalconGlow")},
            {5, 0, 0, const_cast<char *>("ATAT_PART_1")},
            {1, 0, 0, const_cast<char *>("vd")},
            {1, 0, 0, const_cast<char *>("bvd")},
            {1, 0, 0, const_cast<char *>("dtf")},
            {1, 0, 0, const_cast<char *>("ca")},
            {1, 0, 0, const_cast<char *>("blasterbolt")},
            {1, 0, 0, const_cast<char *>("blasterglow")},
            {1, 0, 0, const_cast<char *>("blasterbolt_red")},
            {1, 0, 0, const_cast<char *>("blasterglow_red")},
            {1, 0, 0, const_cast<char *>("blasterbolt_blue")},
            {1, 0, 0, const_cast<char *>("blasterglow_blue")},
            {1, 0, 0, const_cast<char *>("missile")},
            {5, 0, 0, const_cast<char *>("arrow")},
            {5, 0, 0, const_cast<char *>("death_star")},
            {5, 0, 0, const_cast<char *>("lap1")},
            {5, 0, 0, const_cast<char *>("lap2")},
            {5, 0, 0, const_cast<char *>("lap3")},
            {0, 0, 0, const_cast<char *>("Network_Icon")},
            {0, 0, 0, const_cast<char *>("Network_Icon_Glow")},
            {255, 0, 0, NULL},
        };
        /* Special the save icon is drawn from (LoadPermLoadSaveIconScene). */
        char *lsw_memcard_objname = "mem_card";
        i32 PermDataLoaded = 1;
        volatile i32 LoadPerm_LanguageSelect = 0;
        i32 LoadPerm_StringsLoaded = 0;

        void LoadPermData(void *process) {
            PC_CONTROL_CONFIG control_config;
            char blocked_keys[4];

            LoadPermLoadFonts();
            while (LoadPerm_LanguageSelect != 3) {
            }

            Text_InitStringTable(0x70d, &permbuffer_ptr, &permbuffer_end);

            if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0) {
                Text_InitTable(Text_TableMenu, 0, 0x31f);
                Text_InitTable(Text_TableGame, 0x320, 0x70c);
            } else {
                Text_InitTable(Text_TableMain, 0, 0x70c);
            }

            Text_LoadStrings(&permbuffer_ptr, &permbuffer_end);
            Text_InitDefaultStrings();
            LoadPerm_StringsLoaded = 1;

            if (process != NULL) {
                LoadPermLoadLegalTexture();
            }

            NuStringFilterLoad("stuff\\text\\badwords.txt", &permbuffer_ptr, permbuffer_end);
            MusicInfo = ConfigureMusic("audio\\music.txt", &permbuffer_ptr, &permbuffer_end);
            SfxSetFileTable((SFX_FILE_INFO *)MusicInfo);
            InitSfx(&permbuffer_ptr, permbuffer_end, "Audio\\Audio.cfg");
            InitStillRender(&permbuffer_ptr, permbuffer_end);

            OldLevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, 0x80);
            LevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, 0x80);
            LevelHackDataSize = 0;

            LevelProgress_ReserveBufferSpace(&permbuffer_ptr, permbuffer_end);
            FadeSys.Init();
            FadeSys.AddFade((FadeBase *)&fade);
            FadeSys.AddFade((FadeBase *)&fadeWipe);
            FadeSys.AddFade((FadeBase *)&fadeStillWipe);
            FadeSys.AddFade((FadeBase *)&fadeStill);
            pFadeInfo = &FadeSys;
            LevelObjects_InitForGame(ObjTab, &permbuffer_ptr, &permbuffer_end, 0x2ee, 0x1f40);
            LevelSplines_InitForGame(LevelSplinesTable);
            LoadPermLoadSaveIconScene();

            theMemoryManager.Init(&permbuffer_ptr, &permbuffer_end, 0x200);

            CreateThingManager();

            if (StartupGameFlag0 != 0) {
                RegisterDefaultGizmoTypes(&permbuffer_ptr, &permbuffer_end);
                GameAnimSys_AllocateLevelProgressData(&permbuffer_ptr, &permbuffer_end, 0x280, 0xc);
                Alt_registerStatusScreen();
                Alt_registerStatusScreenExtra();
            } else if (StartupGameFlag1 != 0) {
                RegisterLswGizmoTypes(&permbuffer_ptr, &permbuffer_end);
                GameAnimSys_AllocateLevelProgressData(&permbuffer_ptr, &permbuffer_end, 0x280, 0xc);
                LSW_registerStatusScreen();
                initGameHintSys_LSW();
                CutScene_StartFn = CutScene_StartFn_LSW;
                CutScene_PreUpdateFn = CutScene_PreUpdateFn_LSW;
                CutScene_PostUpdateFn = CutScene_PostUpdateFn_LSW;
                CutScene_StoppedFn = CutScene_StoppedFn_LSW;
                CutScene_ReplaceCharacterModelFn = CutScene_ReplaceCharacterModelFn_LSW;
                InitBolt_AddMomentumType = InitBolt_AddMomentumType_LSW;
                Bolt_HitPlatFn = Bolt_HitPlatFn_LSW;
                Bolt_HitCustomFn = Bolt_HitCustomFn_LSW;
                GameBlowUpBlownUpFn = GameBlowUpBlownUpFn_LSW;
                GizObstacle_SetDefaultSFXFn = GizObstacle_SetDefaultSFXFn_LSW;
            } else {
                RegisterAlternateGizmoTypes(&permbuffer_ptr, &permbuffer_end);
                GameAnimSys_AllocateLevelProgressData(&permbuffer_ptr, &permbuffer_end, 0x280, 0xc);
                Alt_registerStatusScreen();
                Alt_registerStatusScreenExtra();
            }

            Movies_ConfigureList("movies\\movies.txt", &permbuffer_ptr, &permbuffer_end);
            SetProceduralAnimationFn(NuAnimBuffProceduralAnimation);
            CDataList = ConfigureCharacterList("chars\\chars.txt", &permbuffer_ptr, &permbuffer_end, 0x154, &CHARCOUNT, 0x120, &GCDataList);
            CharScenes_Init(&permbuffer_ptr, &permbuffer_end);
            IconScenes_Init("stuff\\icons\\", &permbuffer_ptr, &permbuffer_end);
            FixUpCharacters(CharacterFixupTable);
            MiniKits_Init(&permbuffer_ptr, &permbuffer_end);

            if (StartupGameFlag1 != 0) {
                id_DEFAULTCHARACTER[0] = ID_DEFAULTCHARACTER_LSW_A;
                id_DEFAULTCHARACTER[1] = ID_DEFAULTCHARACTER_LSW_B;
                CharCategories_Init(StartupCharCategoriesLsw);
                Cheats_Init(ExtrasCheats);
            } else {
                id_DEFAULTCHARACTER[0] = ID_DEFAULTCHARACTER_A;
                id_DEFAULTCHARACTER[1] = ID_DEFAULTCHARACTER_B;
                CharCategories_Init(StartupCharCategoriesDefault);
                Cheats_Init(ExtrasCheats);
            }
            PlayerID[0] = id_DEFAULTCHARACTER[0];
            PlayerID[1] = id_DEFAULTCHARACTER[1];
            CharVariants_Init(StartupCharVariantNames, 0x17);

            LDataList = Levels_ConfigureList("levels\\levels.txt", &permbuffer_ptr, &permbuffer_end, 0x16d, &LEVELCOUNT, Level_SetDefaults);
            FixUpLevels(LevelFixupStartup);

            ADataList = Areas_ConfigureList("levels\\areas.txt", &permbuffer_ptr, &permbuffer_end, 0x48, &AREACOUNT);
            FixUpAreas();

            EDataList = Episodes_ConfigureList("levels\\episodes.txt", &permbuffer_ptr, &permbuffer_end, 6, &EPISODECOUNT);

            NewGame();
            InitGameAfterConfig();

            if (StartupGameFlag1 == 0) {
                CutScenes_ConfigureList((char *)"cut\\cutscenes.txt", &permbuffer_ptr, permbuffer_end);
            }

            APICharacterSysInit(&permbuffer_ptr, permbuffer_end, CHARCOUNT, 0x30, 0xe9, 0x400, (APICHARACTERDATA *)CDataList,
                                SetCreatureLights);
            SetActionInfo((API_ACTION_INFO *)PlayerBlockActionInfo, PlayerExtraActionInfo);
            gizaimessagesys = CreateGizAIMessageSys(&permbuffer_ptr, &permbuffer_end, 0x40);

            permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xfU) & ~0xfU;

            if (StartupGameFlag1 != 0) {
                BackDrop_Init("stuff\\starfield.gsc", &permbuffer_ptr, &permbuffer_end);
            }

            LoadPerm1();
            LoadPerm2();
            theGameThings->AddOnceOnlyThings();
            RegisterHelpers();

            blocked_keys[0] = 0x3b;
            blocked_keys[1] = 0x3c;
            blocked_keys[2] = 1;
            blocked_keys[3] = 0;
            control_config.font = SmartTextFont != NULL ? SmartTextFont : QFont2D;
            control_config.draw_background = LoadPermProgressDraw;
            control_config.rows = NULL;
            control_config.play_sound = PcControlPlaySound;
            control_config.blocked_keys = blocked_keys;
            PcControlRegister(&control_config, &permbuffer_ptr, &permbuffer_end);

            PcInput_SetStringUCGfxRemapFn(TextInputGlyphName);
            PcInput_SetIsUsedCallBack(PcInputCommandAllowed);

            permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xfU) & ~0xfU;
            PermDataLoaded = 1;
        }

        void LoadPerm(void) {
            WORLDINFO *saved_world;
            f32 language_timer;
            f32 legal_timer;
            f32 ready_timer;
            f32 intro_timer;
            f32 tail_timer;
            f32 menu_alpha;
            f32 menu_timer;
            f32 legal_alpha;
            f32 intro_alpha;
            f32 aspect;
            f32 half;
            f32 frame_time;
            NUMTX scale;

            SetBackgroundMusic(1);
            PermDataLoaded = 0;
            saved_world = WORLD;
            WORLD = NULL;

            if (BGLOAD == 0 || LOADEROFF != 0) {
                LoadPerm_LanguageSelect = 3;
                LoadPermData(NULL);
                LoadPermDestroyLegalMtl();
                WORLD = saved_world;
                return;
            }

            FRAMETIME = DEFAULTFRAMETIME;
            tail_timer = 0.2f;
            pNuCam->mtx = NuMtxSetIdentityTemplate;
            NuCameraSet(pNuCam);
            InitPanel(Game.options_save.widescreen);
            NuQFntSetCoordinateSystem(NUQFNT_CS_640X224);
            nuhspecial_s special = {};
            NUVEC scale_vec = {0.125f, 0.125f, 0.125f};
            NuMtxSetScale(&scale, &scale_vec);
            scale.m32 = 1.0f;
            language_timer = 3.3f;
            if (PAL == 0) {
                LoadPerm_LanguageSelect = 3;
            }
            legal_timer = 0.0f;
            ready_timer = 0.0f;
            intro_timer = 0.0f;
            menu_alpha = 0.0f;
            menu_timer = 0.0f;
            bgPostRequest((bgprocdofn *)LoadPermData, NULL, NULL, 0);

            while (PermDataLoaded == 0 || tail_timer > 0.0f) {
                NuFrameBegin();
                NuCameraSet(&global_camera);
                readpads_always = 1;
                ReadPads();
                UpdateGameMenu(&GamePadStorage[0], 0);
                UpdateTimer(&GlobalTimer);
                menu_flash = NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.2f) < 0.1f;
                if (NOSOUND == 0) {
                    NuSound3Update();
                }

                if (language_timer < 3.3f && save_icon_scene != NULL) {
                    if (language_timer == 0.0f && NuSpecialExistsFn(&special) == 0) {
                        if (NuSpecialFind(save_icon_scene, &special, ObjTab[0].name, 1) == 0) {
                            language_timer = 3.3f;
                        }
                    } else {
                        language_timer += FRAMETIME;
                        if (language_timer > 3.3f) {
                            language_timer = 3.3f;
                        }
                    }
                }

                if (menu_timer < 0.5f && language_timer >= 3.3f && Text_IsFontLoaded() != NULL) {
                    if (LoadPerm_LanguageSelect == 0) {
                        NewMenu(MENU_SELECT_LANGUAGE, 0, -1);
                        FadeSys.fade = -1.0f;
                        LoadPerm_LanguageSelect = 1;
                    }
                    if (LoadPerm_LanguageSelect == 1 && GameMenuLevel == 0) {
                        LoadPerm_LanguageSelect = 2;
                    }
                    if (LoadPerm_LanguageSelect == 2) {
                        menu_timer += FRAMETIME;
                        if (menu_timer >= 0.5f) {
                            LoadPerm_LanguageSelect = 3;
                        }
                    }
                }

                if (ready_timer < 1.0f && language_timer >= 3.3f && LoadPerm_LanguageSelect == 3 && LoadPerm_StringsLoaded != 0 &&
                    saveload_status == 1 && GameMenuLevel == 0) {
                    ready_timer += FRAMETIME;
                    if (ready_timer > 1.0f) {
                        ready_timer = 1.0f;
                    }
                }

                if (legal_timer < 5.8f && language_timer >= 3.3f && LoadPerm_LanguageSelect == 3 && ready_timer >= 1.0f &&
                    loadlegal_done != 0) {
                    if (legal_timer == 0.0f && legal_mtl == NULL) {
                        if (legal_tid == 0) {
                            legal_timer = 5.8f;
                        } else {
                            if (LoadPermCreateLegalMtl(&legal_mtl, legal_tid) == 0) {
                                legal_timer = 5.8f;
                            }
                        }
                    } else {
                        legal_timer += FRAMETIME;
                        if (legal_timer > 5.8f) {
                            legal_timer = 5.8f;
                        }
                    }
                }

                if (intro_timer < 3.8f && language_timer >= 3.3f && LoadPerm_LanguageSelect == 3 && ready_timer >= 1.0f &&
                    legal_timer >= 5.8f && Text_IsFontLoaded() != 0 && LoadPerm_StringsLoaded != 0) {
                    if (StartupGameFlag1 == 0 || INTROTEXT_ID != -1) {
                        intro_timer += FRAMETIME;
                        if (intro_timer > 3.8f) {
                            intro_timer = 3.8f;
                        }
                    }
                }

                if (PermDataLoaded != 0 && LoadPerm_LanguageSelect == 3 && language_timer >= 3.3f && ready_timer >= 1.0f &&
                    legal_timer >= 5.8f && intro_timer >= 3.8f) {
                    tail_timer -= FRAMETIME;
                    if (tail_timer < 0.0f) {
                        tail_timer = 0.0f;
                    }
                }

                NuRndrBeginScene(1);
                NuRndrGradClear(0xf00, (i32)0x80000000, (i32)0x80000000, 1.0f);

                if (tail_timer == 0.2f) {
                    menu_alpha = LoadPermFadeAlpha(language_timer, 2.5f);
                    if (menu_alpha > 0.0f) {
                        NuSpecialDrawAtAlpha(&special, &scale, menu_alpha);
                    }
                    DrawMenu(0);

                    legal_alpha = LoadPermFadeAlpha(legal_timer, 5.0f);
                    if (legal_alpha > 0.0f) {
                        switch (PcWindowOptions.aspect_mode) {
                            case PC_ASPECT_4_3:
                                aspect = 1.3333334f;
                                break;
                            case PC_ASPECT_16_9:
                                aspect = 1.7777778f;
                                break;
                            case PC_ASPECT_16_10:
                                aspect = 1.6f;
                                break;
                            case PC_ASPECT_AUTO:
                                aspect = PCDeviceBackend.active_aspect;
                                break;
                            default:
                                aspect = 1.3333334f;
                                break;
                        }
                        half = (1.0f - 1.3333334f / aspect) * 0.5f;
                        NuRndrClear(0xb00, 0, 1.0f);
                        ++NuPrimCSPos;
                        NuPrimSetCoordinateSystem(2);
                        NuPrim2DBegin(0x10, 7, legal_mtl);
                        NuPrim2DSetColor((i32)(((u32)(i32)(legal_alpha * 255.0) << 24) | 0x808080));
                        NuPrim2DSetUV(0.0f, 0.0f);
                        NuPrim2DAddXYZ(half + 0.0f, 0.0f, 0.0f);
                        NuPrim2DSetColor((i32)(((u32)(i32)(legal_alpha * 255.0) << 24) | 0x808080));
                        NuPrim2DSetUV(1.0f, 1.0f);
                        NuPrim2DAddXYZ(1.0f - half, 1.0f, 0.0f);
                        NuPrim2DEnd();
                        --NuPrimCSPos;
                        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
                    }

                    intro_alpha = LoadPermFadeAlpha(intro_timer, 3.0f);
                    if (intro_alpha > 0.0f && StartupGameFlag1 != 0) {
                        IntroText_Draw(intro_alpha);
                    }
                }

                NuRndrEndScene();
                edGraEnableTerrainSwap();
                frame_time = NuFrameEnd();
                FRAMETIME = frame_time;
                edGraDisableTerrainSwap();
                if (FRAMETIME > MAXFRAMETIME && CUTSTOPGAME == 0) {
                    FRAMETIME = MAXFRAMETIME;
                }
            }

            LoadPermDestroyLegalMtl();
            WORLD = saved_world;
        }
    }
}
