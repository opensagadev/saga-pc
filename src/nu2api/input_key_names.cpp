// Key names and input labels of every language; input_runtime.cpp selects them by language.
#include "nu2api/PCInput.h"
#include "gamelib/util/CRC16.h"
#include "legoapi/items/collect/torpedo.h"
#include "nu2api/huffdecode.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/gizmos/object/lever.h"

extern "C" {
    char dat_00753370[2] = "]";
    char dat_00753374[2] = "[";
    char dat_0076fde8[7] = "Delete";
    char dat_007719bc[12] = "MEDIASELECT";
    char dat_007719c8[5] = "MAIL";
    char dat_007719d0[11] = "MYCOMPUTER";
    char dat_007719dc[8] = "WEBBACK";
    char dat_007719e4[11] = "WEBFORWARD";
    char dat_007719f0[8] = "WEBSTOP";
    char dat_007719f8[11] = "WEBREFRESH";
    char dat_00771a04[13] = "WEBFAVORITES";
    char dat_00771a14[10] = "WEBSEARCH";
    char dat_00771a20[5] = "WAKE";
    char dat_00771a28[6] = "SLEEP";
    char dat_00771a30[6] = "POWER";
    char dat_00771a38[5] = "APPS";
    char dat_00771a40[7] = "R. Win";
    char dat_00771a48[7] = "L. Win";
    char dat_00771a50[7] = "Insert";
    char dat_00771a58[9] = "Pg. Down";
    char dat_00771a64[4] = "End";
    char dat_00771a68[6] = "Right";
    char dat_00771a70[5] = "Left";
    char dat_00771a78[7] = "Pg. Up";
    char dat_00771a80[3] = "Up";
    char dat_00771a84[5] = "Home";
    char dat_00771a8c[6] = "Pause";
    char dat_00771a94[7] = "R. Alt";
    char dat_00771a9c[6] = "SysRq";
    char dat_00771aa4[7] = "Num. /";
    char dat_00771ab8[8] = "WEBHOME";
    char dat_00771ac0[6] = "VOLUP";
    char dat_00771ac8[8] = "VOLDOWN";
    char dat_00771ad0[10] = "MEDIASTOP";
    char dat_00771adc[5] = "PLAY";
    char dat_00771ae4[5] = "CALC";
    char dat_00771aec[5] = "MUTE";
    char dat_00771af4[8] = "R. Ctrl";
    char dat_00771afc[11] = "Num. Enter";
    char dat_00771b08[10] = "NEXTTRACK";
    char dat_00771b14[10] = "UNLABELED";
    char dat_00771b20[3] = "AX";
    char dat_00771b24[5] = "STOP";
    char dat_00771b2c[6] = "KANJI";
    char dat_00771b34[10] = "UNDERLINE";
    char dat_00771b40[6] = "COLON";
    char dat_00771b48[3] = "AT";
    char dat_00771b4c[10] = "PREVTRACK";
    char dat_00771b58[7] = "Num. =";
    char dat_00771b60[8] = "ABNT_C2";
    char dat_00771b68[4] = "YEN";
    char dat_00771b6c[10] = "NOCONVERT";
    char dat_00771b78[8] = "CONVERT";
    char dat_00771b80[8] = "ABNT_C1";
    char dat_00771b88[5] = "KANA";
    char dat_00771b90[4] = "F15";
    char dat_00771b94[4] = "F14";
    char dat_00771b98[4] = "F13";
    char dat_00771b9c[4] = "F12";
    char dat_00771ba0[4] = "F11";
    char dat_00771bb0[7] = "Num. 0";
    char dat_00771bb8[7] = "Num. 3";
    char dat_00771bc0[7] = "Num. 2";
    char dat_00771bc8[7] = "Num. 1";
    char dat_00771bd0[7] = "Num. +";
    char dat_00771bd8[7] = "Num. 6";
    char dat_00771be0[7] = "Num. 5";
    char dat_00771be8[7] = "Num. 4";
    char dat_00771bf0[7] = "Num. -";
    char dat_00771bf8[7] = "Num. 9";
    char dat_00771c00[7] = "Num. 8";
    char dat_00771c08[7] = "Num. 7";
    char dat_00771c10[12] = "Scroll Lock";
    char dat_00771c1c[9] = "Num Lock";
    char dat_00771c28[4] = "F10";
    char dat_00771c2c[3] = "F9";
    char dat_00771c30[3] = "F8";
    char dat_00771c34[3] = "F7";
    char dat_00771c38[3] = "F6";
    char dat_00771c3c[3] = "F5";
    char dat_00771c40[3] = "F4";
    char dat_00771c44[3] = "F3";
    char dat_00771c48[3] = "F2";
    char dat_00771c4c[3] = "F1";
    char dat_00771c50[10] = "Caps Lock";
    char dat_00771c5c[6] = "Space";
    char dat_00771c64[7] = "L. Alt";
    char dat_00771c6c[7] = "Num. *";
    char dat_00771c74[9] = "R. Shift";
    char dat_00771c80[11] = "For. Slash";
    char dat_00771c8c[5] = "Stop";
    char dat_00771c94[6] = "Comma";
    char dat_00771c9c[2] = "M";
    char dat_00771ca0[2] = "N";
    char dat_00771ca4[2] = "B";
    char dat_00771ca8[2] = "V";
    char dat_00771cac[2] = "C";
    char dat_00771cb0[2] = "#";
    char dat_00771cb4[9] = "L. Shift";
    char dat_00771cc0[6] = "Grave";
    char dat_00771cc8[11] = "Apostrophe";
    char dat_00771cd4[10] = "Semicolon";
    char dat_00771ce0[2] = "L";
    char dat_00771ce4[2] = "K";
    char dat_00771ce8[2] = "J";
    char dat_00771cec[2] = "H";
    char dat_00771cf0[2] = "G";
    char dat_00771cf4[2] = "F";
    char dat_00771cf8[2] = "D";
    char dat_00771cfc[2] = "S";
    char dat_00771d00[2] = "A";
    char dat_00771d04[8] = "L. Ctrl";
    char dat_00771d0c[6] = "Enter";
    char dat_00771d14[2] = "P";
    char dat_00771d18[2] = "O";
    char dat_00771d1c[2] = "I";
    char dat_00771d20[2] = "U";
    char dat_00771d24[2] = "T";
    char dat_00771d28[2] = "R";
    char dat_00771d2c[2] = "E";
    char dat_00771d30[2] = "W";
    char dat_00771d34[2] = "Q";
    char dat_00771d38[4] = "Tab";
    char dat_00771d3c[10] = "Backspace";
    char dat_00771d48[7] = "Equals";
    char dat_00771d50[6] = "Minus";
    char dat_00771d58[2] = "0";
    char dat_00771d5c[2] = "9";
    char dat_00771d60[2] = "8";
    char dat_00771d64[2] = "7";
    char dat_00771d68[2] = "6";
    char dat_00771d6c[2] = "5";
    char dat_00771d70[2] = "4";
    char dat_00771d74[2] = "3";
    char dat_00771d78[2] = "2";
    char dat_00771d7c[4] = "Esc";
    char dat_00771d80[8] = "(empty)";
    char dat_00774180[2] = "Z";
    char dat_00774184[2] = "Y";
    static char dat_00775418[9] = "No Input";
    static char dat_00775424[16] = "Mouse Button %d";
    static char dat_00775434[20] = "Mouse Middle Button";
    static char dat_00775448[19] = "Mouse Right Button";
    static char dat_0077545c[18] = "Mouse Left Button";
    static char dat_00775470[15] = "Mouse Wheel Up";
    static char dat_00775480[11] = "Mouse Down";
    static char dat_0077548c[12] = "Mouse Right";
    static char dat_00775498[17] = "Mouse Wheel Down";
    static char dat_007754ac[9] = "Mouse Up";
    static char dat_007754b8[11] = "Mouse Left";
    static char dat_007754c4[8] = "Mouse Z";
    static char dat_007754cc[8] = "Mouse Y";
    static char dat_007754d4[8] = "Mouse X";
    static char dat_007754dc[6] = "Start";
    static char dat_007754e4[18] = "Right Thumb Stick";
    static char dat_007754f8[17] = "Left Thumb Stick";
    static char dat_0077550c[14] = "Right Trigger";
    static char dat_0077551c[13] = "Left Trigger";
    static char dat_0077552c[15] = "Right Shoulder";
    static char dat_0077553c[14] = "Left Shoulder";
    static char dat_0077554c[9] = "Button X";
    static char dat_00775558[9] = "Button A";
    static char dat_00775564[9] = "Button B";
    static char dat_00775570[9] = "Button Y";
    static char dat_0077557c[17] = "Right Stick Down";
    static char dat_00775590[15] = "Right Stick Up";
    static char dat_007755a0[18] = "Right Stick Right";
    static char dat_007755b4[17] = "Right Stick Left";
    static char dat_007755c8[16] = "Left Stick Down";
    static char dat_007755d8[14] = "Left Stick Up";
    static char dat_007755e8[17] = "Left Stick Right";
    static char dat_007755fc[16] = "Left Stick Left";
    static char dat_0077560c[11] = "Z Rotation";
    static char dat_00775618[11] = "Y Rotation";
    static char dat_00775624[11] = "X Rotation";
    static char dat_00775630[7] = "Z Axis";
    static char dat_00775638[7] = "Y Axis";
    static char dat_00775640[7] = "X Axis";
    static char dat_00775648[17] = "Analogue %d (%s)";
    static char dat_0077565c[5] = "High";
    static char dat_00775664[4] = "Low";
    static char dat_00775668[7] = "Button";
    static char dat_00775670[9] = "Hat Left";
    static char dat_0077567c[9] = "Hat Down";
    static char dat_00775688[10] = "Hat Right";
    static char dat_00775694[7] = "Hat Up";
}
TEXT_INPUT_LABELS dat_00820480 = {
    {{dat_00775694, dat_00775688,        dat_0077567c, dat_00775670, dat_00775668, dat_00775664, dat_0077565c, dat_00775648,
      dat_00775640, dat_00775638,        dat_00775630, dat_00775624, dat_00775618, dat_0077560c, dat_007755fc, dat_007755e8,
      dat_007755d8, dat_007755c8,        dat_007755b4, dat_007755a0, dat_00775590, dat_0077557c, dat_00775570, dat_00775564,
      dat_00775558, dat_0077554c,        dat_0077553c, dat_0077552c, dat_0077551c, dat_0077550c, dat_007754f8, dat_007754e4,
      dat_007754dc, UiBackPromptLiteral, dat_007754d4, dat_007754cc, dat_007754c4}},
    {dat_007754b8, dat_007754ac, dat_00775498},
    {dat_0077548c, dat_00775480, dat_00775470},
    {dat_0077545c, dat_00775448, dat_00775434},
    dat_00775424,
    dat_00775418
};
extern "C" {
    static char dat_0076fe70[6] = "START";
    static char dat_00775064[13] = "\345\205\245\345\212\233\343\201\252\343\201\227";
    static char dat_00775074[23] = "\343\203\236\343\202\246\343\202\271 \343\203\234\343\202\277\343\203\263 %d";
    static char dat_0077508c[26] = "\343\203\236\343\202\246\343\202\271 \344\270\255\345\244\256\343\203\234\343\202\277\343\203\263";
    static char dat_007750a8[23] = "\343\203\236\343\202\246\343\202\271 \345\217\263\343\203\234\343\202\277\343\203\263";
    static char dat_007750c0[23] = "\343\203\236\343\202\246\343\202\271 \345\267\246\343\203\234\343\202\277\343\203\263";
    static char dat_007750d8[26] = "\343\203\236\343\202\246\343\202\271 \343\203\233\343\202\244\343\203\274\343\203\253\344\270\212";
    static char dat_007750f4[14] = "\343\203\236\343\202\246\343\202\271 \344\270\213";
    static char dat_00775104[14] = "\343\203\236\343\202\246\343\202\271 \345\217\263";
    static char dat_00775114[26] = "\343\203\236\343\202\246\343\202\271 \343\203\233\343\202\244\343\203\274\343\203\253\344\270\213";
    static char dat_00775130[14] = "\343\203\236\343\202\246\343\202\271 \344\270\212";
    static char dat_00775140[14] = "\343\203\236\343\202\246\343\202\271 \345\267\246";
    static char dat_00775150[12] = "\343\203\236\343\202\246\343\202\271 Z";
    static char dat_0077515c[12] = "\343\203\236\343\202\246\343\202\271 Y";
    static char dat_00775168[12] = "\343\203\236\343\202\246\343\202\271 X";
    static char dat_00775174[5] = "BACK";
    static char dat_0077517c[26] = "\345\217\263 \350\246\252\346\214\207\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257";
    static char dat_00775198[26] = "\345\267\246 \350\246\252\346\214\207\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257";
    static char dat_007751b4[17] = "\345\217\263 \343\203\210\343\203\252\343\202\254\343\203\274";
    static char dat_007751c8[17] = "\345\267\246 \343\203\210\343\203\252\343\202\254\343\203\274";
    static char dat_007751dc[3] = "RB";
    static char dat_007751e0[3] = "LB";
    static char dat_007751e4[12] = "X \343\203\234\343\202\277\343\203\263";
    static char dat_007751f0[12] = "A \343\203\234\343\202\277\343\203\263";
    static char dat_007751fc[12] = "B \343\203\234\343\202\277\343\203\263";
    static char dat_00775208[12] = "Y \343\203\234\343\202\277\343\203\263";
    static char dat_00775214[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\213";
    static char dat_0077522c[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\212";
    static char dat_00775244[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\217\263";
    static char dat_0077525c[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\267\246";
    static char dat_00775274[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\213";
    static char dat_0077528c[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\212";
    static char dat_007752a4[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\217\263";
    static char dat_007752bc[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\267\246";
    static char dat_007752d4[9] = "Z \345\233\236\350\273\242";
    static char dat_007752e0[9] = "Y \345\233\236\350\273\242";
    static char dat_007752ec[9] = "X \345\233\236\350\273\242";
    static char dat_007752f8[6] = "Z \350\273\270";
    static char dat_00775300[6] = "Y \350\273\270";
    static char dat_00775308[6] = "X \350\273\270";
    static char dat_00775310[21] = "\343\202\242\343\203\212\343\203\255\343\202\260 %d (%s)";
    static char dat_00775328[4] = "\351\253\230";
    static char dat_0077532c[4] = "\344\275\216";
    static char dat_00775330[10] = "\343\203\234\343\202\277\343\203\263";
    static char dat_0077533c[14] = "\343\203\217\343\203\203\343\203\210 \345\267\246";
    static char dat_0077534c[14] = "\343\203\217\343\203\203\343\203\210 \344\270\213";
    static char dat_0077535c[14] = "\343\203\217\343\203\203\343\203\210 \345\217\263";
    static char dat_0077536c[14] = "\343\203\217\343\203\203\343\203\210 \344\270\212";
}
TEXT_INPUT_LABELS dat_00820560 = {
    {{dat_0077536c, dat_0077535c, dat_0077534c, dat_0077533c, dat_00775330, dat_0077532c, dat_00775328, dat_00775310,
      dat_00775308, dat_00775300, dat_007752f8, dat_007752ec, dat_007752e0, dat_007752d4, dat_007752bc, dat_007752a4,
      dat_0077528c, dat_00775274, dat_0077525c, dat_00775244, dat_0077522c, dat_00775214, dat_00775208, dat_007751fc,
      dat_007751f0, dat_007751e4, dat_007751e0, dat_007751dc, dat_007751c8, dat_007751b4, dat_00775198, dat_0077517c,
      dat_0076fe70, dat_00775174, dat_00775168, dat_0077515c, dat_00775150}},
    {dat_00775140, dat_00775130, dat_00775114},
    {dat_00775104, dat_007750f4, dat_007750d8},
    {dat_007750c0, dat_007750a8, dat_0077508c},
    dat_00775074,
    dat_00775064
};
extern "C" {
    static char dat_00774cd8[12] = "Geen invoer";
    static char dat_00774ce4[11] = "Muisknop N";
    static char dat_00774cf0[19] = "Middelste muisknop";
    static char dat_00774d04[16] = "Rechtermuisknop";
    static char dat_00774d14[15] = "Linkermuisknop";
    static char dat_00774d24[16] = "Muiswiel omhoog";
    static char dat_00774d34[12] = "Muis omlaag";
    static char dat_00774d40[12] = "Muis rechts";
    static char dat_00774d4c[16] = "Muiswiel omlaag";
    static char dat_00774d5c[12] = "Muis omhoog";
    static char dat_00774d68[10] = "Muis Left";
    static char dat_00774d74[7] = "Muis Z";
    static char dat_00774d7c[7] = "Muis Y";
    static char dat_00774d84[7] = "Muis X";
    static char dat_00774d8c[6] = "Terug";
    static char dat_00774d94[17] = "Rechter joystick";
    static char dat_00774da8[16] = "Linker joystick";
    static char dat_00774db8[16] = "Rechter trekker";
    static char dat_00774dc8[15] = "Linker Trekker";
    static char dat_00774dd8[17] = "Rechter Schouder";
    static char dat_00774dec[16] = "Linker Schouder";
    static char dat_00774dfc[7] = "Knop X";
    static char dat_00774e04[7] = "Knop A";
    static char dat_00774e0c[7] = "Knop B";
    static char dat_00774e14[7] = "Knop Y";
    static char dat_00774e1c[21] = "Rechter Stick Omlaag";
    static char dat_00774e34[21] = "Rechter Stick Omhoog";
    static char dat_00774e4c[21] = "Rechter Stick Rechts";
    static char dat_00774e64[20] = "Rechter Stick Links";
    static char dat_00774e78[20] = "Linker Stick Omlaag";
    static char dat_00774e8c[20] = "Linker Stick Omhoog";
    static char dat_00774ea0[20] = "Linker Stick Rechts";
    static char dat_00774eb4[19] = "Linker Stick Links";
    static char dat_00774ec8[10] = "Z Rotatie";
    static char dat_00774ed4[10] = "Y Rotatie";
    static char dat_00774ee0[10] = "X Rotatie";
    static char dat_00774eec[5] = "Z As";
    static char dat_00774ef4[5] = "Y As";
    static char dat_00774efc[5] = "X As";
    static char dat_00774f04[16] = "Analoog %d (%s)";
    static char dat_00774f14[5] = "Hoog";
    static char dat_00774f1c[5] = "Laag";
    static char dat_00774f24[5] = "Knop";
    static char dat_00774f2c[19] = "Navigatiepad Links";
    static char dat_00774f40[20] = "Navigatiepad Omlaag";
    static char dat_00774f54[20] = "Navigatiepad Rechts";
    static char dat_00774f68[20] = "Navigatiepad Omhoog";
}
TEXT_INPUT_LABELS dat_00820640 = {
    {{dat_00774f68, dat_00774f54, dat_00774f40, dat_00774f2c, dat_00774f24, dat_00774f1c, dat_00774f14, dat_00774f04,
      dat_00774efc, dat_00774ef4, dat_00774eec, dat_00774ee0, dat_00774ed4, dat_00774ec8, dat_00774eb4, dat_00774ea0,
      dat_00774e8c, dat_00774e78, dat_00774e64, dat_00774e4c, dat_00774e34, dat_00774e1c, dat_00774e14, dat_00774e0c,
      dat_00774e04, dat_00774dfc, dat_00774dec, dat_00774dd8, dat_00774dc8, dat_00774db8, dat_00774da8, dat_00774d94,
      dat_007754dc, dat_00774d8c, dat_00774d84, dat_00774d7c, dat_00774d74}},
    {dat_00774d68, dat_00774d5c, dat_00774d4c},
    {dat_00774d40, dat_00774d34, dat_00774d24},
    {dat_00774d14, dat_00774d04, dat_00774cf0},
    dat_00774ce4,
    dat_00774cd8
};
extern "C" {
    static char dat_00774948[12] = "Ingen input";
    static char dat_00774954[12] = "Museknapp N";
    static char dat_00774960[17] = "Midtre museknapp";
    static char dat_00774974[17] = "H\303\270yre museknapp";
    static char dat_00774988[18] = "Venstre museknapp";
    static char dat_0077499c[13] = "Musehjul opp";
    static char dat_007749ac[8] = "Mus ned";
    static char dat_007749b4[11] = "Mus h\303\270yre";
    static char dat_007749c0[13] = "Musehjul ned";
    static char dat_007749d0[8] = "Mus opp";
    static char dat_007749d8[12] = "Mus venstre";
    static char dat_007749e4[6] = "Mus Z";
    static char dat_007749ec[6] = "Mus Y";
    static char dat_007749f4[6] = "Mus X";
    static char dat_007749fc[8] = "Tilbake";
    static char dat_00774a04[12] = "H\303\270yre spak";
    static char dat_00774a10[13] = "Venstre spak";
    static char dat_00774a20[17] = "H\303\270yre avtrekker";
    static char dat_00774a34[18] = "Venstre avtrekker";
    static char dat_00774a48[20] = "H\303\270yre skulderknapp";
    static char dat_00774a5c[21] = "Venstre skulderknapp";
    static char dat_00774a74[8] = "X-knapp";
    static char dat_00774a7c[8] = "A-knapp";
    static char dat_00774a84[8] = "B-knapp";
    static char dat_00774a8c[8] = "Y-knapp";
    static char dat_00774a94[21] = "H\303\270yre styrespak ned";
    static char dat_00774aac[21] = "H\303\270yre styrespak opp";
    static char dat_00774ac4[24] = "H\303\270yre styrespak h\303\270yre";
    static char dat_00774adc[25] = "H\303\270yre styrespak venstre";
    static char dat_00774af8[22] = "Venstre styrespak ned";
    static char dat_00774b10[22] = "Venstre styrespak opp";
    static char dat_00774b28[25] = "Venstre styrespak h\303\270yre";
    static char dat_00774b44[26] = "Venstre styrespak venstre";
    static char dat_00774b60[11] = "Z-rotasjon";
    static char dat_00774b6c[11] = "Y-rotasjon";
    static char dat_00774b78[11] = "X-rotasjon";
    static char dat_00774b84[7] = "Z-akse";
    static char dat_00774b8c[7] = "Y-akse";
    static char dat_00774b94[7] = "X-akse";
    static char dat_00774b9c[15] = "Analog %d (%s)";
    static char dat_00774bac[5] = "H\303\270y";
    static char dat_00774bb4[4] = "Lav";
    static char dat_00774bb8[6] = "Knapp";
    static char dat_00774bc0[19] = "Hatteknapp venstre";
    static char dat_00774bd4[15] = "Hatteknapp ned";
    static char dat_00774be4[18] = "Hatteknapp h\303\270yre";
    static char dat_00774bf8[15] = "Hatteknapp opp";
}
TEXT_INPUT_LABELS dat_00820720 = {
    {{dat_00774bf8, dat_00774be4, dat_00774bd4, dat_00774bc0, dat_00774bb8, dat_00774bb4, dat_00774bac, dat_00774b9c,
      dat_00774b94, dat_00774b8c, dat_00774b84, dat_00774b78, dat_00774b6c, dat_00774b60, dat_00774b44, dat_00774b28,
      dat_00774b10, dat_00774af8, dat_00774adc, dat_00774ac4, dat_00774aac, dat_00774a94, dat_00774a8c, dat_00774a84,
      dat_00774a7c, dat_00774a74, dat_00774a5c, dat_00774a48, dat_00774a34, dat_00774a20, dat_00774a10, dat_00774a04,
      dat_007754dc, dat_007749fc, dat_007749f4, dat_007749ec, dat_007749e4}},
    {dat_007749d8, dat_007749d0, dat_007749c0},
    {dat_007749b4, dat_007749ac, dat_0077499c},
    {dat_00774988, dat_00774974, dat_00774960},
    dat_00774954,
    dat_00774948
};
extern "C" {
    static char dat_00774664[16] = "Ingen inmatning";
    static char dat_00774674[11] = "Musknapp N";
    static char dat_00774680[21] = "Mittersta musknappen";
    static char dat_00774698[16] = "H\303\266ger musknapp";
    static char dat_007746a8[18] = "V\303\244nster musknapp";
    static char dat_007746bc[12] = "Mushjul upp";
    static char dat_007746c8[11] = "Mus h\303\266ger";
    static char dat_007746d4[12] = "Mushjul ned";
    static char dat_007746e0[8] = "Mus upp";
    static char dat_007746e8[13] = "Mus v\303\244nster";
    static char dat_007746f8[9] = "Tillbaka";
    static char dat_00774704[16] = "H\303\266ger styrspak";
    static char dat_00774714[18] = "V\303\244nster styrspak";
    static char dat_00774728[18] = "H\303\266ger avtryckare";
    static char dat_0077473c[3] = "LT";
    static char dat_00774740[8] = "R-knapp";
    static char dat_00774748[8] = "L-knapp";
    static char dat_00774750[8] = "Knapp X";
    static char dat_00774758[8] = "Knapp A";
    static char dat_00774760[8] = "Knapp B";
    static char dat_00774768[8] = "Knapp Y";
    static char dat_00774770[16] = "H\303\266ger Spak Ner";
    static char dat_00774780[16] = "H\303\266ger Spak Upp";
    static char dat_00774790[19] = "H\303\266ger Spak H\303\266ger";
    static char dat_007747a4[21] = "H\303\266ger Spak V\303\244nster";
    static char dat_007747bc[18] = "V\303\244nster Spak Ner";
    static char dat_007747d0[18] = "V\303\244nster Spak Upp";
    static char dat_007747e4[21] = "V\303\244nster Spak H\303\266ger";
    static char dat_007747fc[23] = "V\303\244nster Spak V\303\244nster";
    static char dat_00774814[11] = "Z-rotation";
    static char dat_00774820[11] = "Y-rotation";
    static char dat_0077482c[11] = "X-rotation";
    static char dat_00774838[7] = "Z-axel";
    static char dat_00774840[7] = "Y-axel";
    static char dat_00774848[7] = "X-axel";
    static char dat_00774850[5] = "H\303\266g";
    static char dat_00774858[5] = "L\303\245g";
    static char dat_00774860[24] = "Riktningsknapp V\303\244nster";
    static char dat_00774878[19] = "Riktningsknapp Ner";
    static char dat_0077488c[22] = "Riktningsknapp H\303\266ger";
    static char dat_007748a4[19] = "Riktningsknapp Upp";
}
TEXT_INPUT_LABELS dat_00820800 = {
    {{dat_007748a4, dat_0077488c, dat_00774878, dat_00774860, dat_00774bb8, dat_00774858, dat_00774850, dat_00774b9c,
      dat_00774848, dat_00774840, dat_00774838, dat_0077482c, dat_00774820, dat_00774814, dat_007747fc, dat_007747e4,
      dat_007747d0, dat_007747bc, dat_007747a4, dat_00774790, dat_00774780, dat_00774770, dat_00774768, dat_00774760,
      dat_00774758, dat_00774750, dat_00774748, dat_00774740, dat_0077473c, dat_00774728, dat_00774714, dat_00774704,
      dat_007754dc, dat_007746f8, dat_007749f4, dat_007749ec, dat_007749e4}},
    {dat_007746e8, dat_007746e0, dat_007746d4},
    {dat_007746c8, dat_007749ac, dat_007746bc},
    {dat_007746a8, dat_00774698, dat_00774680},
    dat_00774674,
    dat_00774664
};
extern "C" {
    static char dat_007742a8[14] = "Ei sy\303\266tett\303\244";
    static char dat_007742b8[17] = "Hiiren painike N";
    static char dat_007742cc[20] = "Hiiren rullapainike";
    static char dat_007742e0[21] = "Hiiren oikea painike";
    static char dat_007742f8[21] = "Hiiren vasen painike";
    static char dat_00774310[19] = "Hiiren rulla yl\303\266s";
    static char dat_00774324[11] = "Hiiri alas";
    static char dat_00774330[15] = "Hiiri oikealle";
    static char dat_00774340[18] = "Hiiren rulla alas";
    static char dat_00774354[12] = "Hiiri yl\303\266s";
    static char dat_00774360[17] = "Hiiri vasemmalle";
    static char dat_00774374[16] = "Hiiren Z-akseli";
    static char dat_00774384[16] = "Hiiren Y-akseli";
    static char dat_00774394[16] = "Hiiren X-akseli";
    static char dat_007743a4[9] = "Takaisin";
    static char dat_007743b0[7] = "Aloita";
    static char dat_007743b8[19] = "Oikea peukalosauva";
    static char dat_007743cc[19] = "Vasen peukalosauva";
    static char dat_007743e0[12] = "Oikea sauva";
    static char dat_007743ec[16] = "Vasen liipaisin";
    static char dat_007743fc[20] = "Oikea olkan\303\244pp\303\244in";
    static char dat_00774410[20] = "Vasen olkan\303\244pp\303\244in";
    static char dat_00774424[12] = "N\303\244pp\303\244in X";
    static char dat_00774430[12] = "N\303\244pp\303\244in A";
    static char dat_0077443c[12] = "N\303\244pp\303\244in B";
    static char dat_00774448[12] = "N\303\244pp\303\244in Y";
    static char dat_00774454[17] = "Oikea sauva alas";
    static char dat_00774468[18] = "Oikea sauva yl\303\266s";
    static char dat_0077447c[18] = "Oikea sauva oikea";
    static char dat_00774490[18] = "Oikea sauva vasen";
    static char dat_007744a4[17] = "Vasen sauva alas";
    static char dat_007744b8[18] = "Vasen sauva yl\303\266s";
    static char dat_007744cc[18] = "Vasen sauva oikea";
    static char dat_007744e0[18] = "Vasen sauva vasen";
    static char dat_007744f4[11] = "Z-rotaatio";
    static char dat_00774500[11] = "Y-rotaatio";
    static char dat_0077450c[11] = "X-rotaatio";
    static char dat_00774518[9] = "Z-akseli";
    static char dat_00774524[9] = "Y-akseli";
    static char dat_00774530[9] = "X-akseli";
    static char dat_0077453c[19] = "Analoginen %d (%s)";
    static char dat_00774550[7] = "Korkea";
    static char dat_00774558[7] = "Matala";
    static char dat_00774560[10] = "N\303\244pp\303\244in";
    static char dat_0077456c[12] = "Hattu vasen";
    static char dat_00774578[11] = "Hattu alas";
    static char dat_00774584[12] = "Hattu oikea";
    static char dat_00774590[12] = "Hattu yl\303\266s";
}
TEXT_INPUT_LABELS dat_008208e0 = {
    {{dat_00774590, dat_00774584, dat_00774578, dat_0077456c, dat_00774560, dat_00774558, dat_00774550, dat_0077453c,
      dat_00774530, dat_00774524, dat_00774518, dat_0077450c, dat_00774500, dat_007744f4, dat_007744e0, dat_007744cc,
      dat_007744b8, dat_007744a4, dat_00774490, dat_0077447c, dat_00774468, dat_00774454, dat_00774448, dat_0077443c,
      dat_00774430, dat_00774424, dat_00774410, dat_007743fc, dat_007743ec, dat_007743e0, dat_007743cc, dat_007743b8,
      dat_007743b0, dat_007743a4, dat_00774394, dat_00774384, dat_00774374}},
    {dat_00774360, dat_00774354, dat_00774340},
    {dat_00774330, dat_00774324, dat_00774310},
    {dat_007742f8, dat_007742e0, dat_007742cc},
    dat_007742b8,
    dat_007742a8
};
extern "C" {
    static char dat_00773f08[12] = "Sem entrada";
    static char dat_00773f14[15] = "Mouse bot\303\243o N";
    static char dat_00773f24[21] = "Mouse bot\303\243o do meio";
    static char dat_00773f3c[21] = "Mouse bot\303\243o direito";
    static char dat_00773f54[22] = "Mouse bot\303\243o esquerdo";
    static char dat_00773f6c[21] = "Mouse roda para cima";
    static char dat_00773f84[17] = "Mouse para baixo";
    static char dat_00773f98[21] = "Mouse para a direita";
    static char dat_00773fb0[22] = "Mouse roda para baixo";
    static char dat_00773fc8[16] = "Mouse para cima";
    static char dat_00773fd8[22] = "Mouse para a esquerda";
    static char dat_00773ff0[8] = "Z Mouse";
    static char dat_00773ff8[8] = "Y Mouse";
    static char dat_00774000[8] = "X Mouse";
    static char dat_00774008[7] = "Voltar";
    static char dat_00774010[8] = "Iniciar";
    static char dat_00774018[19] = "Direcional direito";
    static char dat_0077402c[20] = "Direcional esquerdo";
    static char dat_00774040[16] = "Gatilho direito";
    static char dat_00774050[18] = "Gatilho Inf. Dir.";
    static char dat_00774064[18] = "Gatilho Inf. Esq.";
    static char dat_00774078[9] = "Bot\303\243o X";
    static char dat_00774084[9] = "Bot\303\243o A";
    static char dat_00774090[9] = "Bot\303\243o B";
    static char dat_0077409c[9] = "Bot\303\243o Y";
    static char dat_007740a8[21] = "Direc. dir. p/ baixo";
    static char dat_007740c0[20] = "Direc. dir. p/ cima";
    static char dat_007740d4[20] = "Direc. dir. p/ dir.";
    static char dat_007740e8[19] = "Direc. dir. p/ esq";
    static char dat_007740fc[24] = "Deixe o direc. p/ baixo";
    static char dat_00774114[23] = "Deixe o direc. p/ cima";
    static char dat_0077412c[23] = "Deixe o direc. p/ dir.";
    static char dat_00774144[23] = "Deixe o direc. p/ esq.";
    static char dat_0077415c[12] = "Rota\303\247\303\243o Z";
    static char dat_00774168[12] = "Rota\303\247\303\243o Y";
    static char dat_00774174[12] = "Rota\303\247\303\243o X";
    static char dat_00774188[19] = "Anal\303\263gico %d (%s)";
    static char dat_0077419c[5] = "Alto";
    static char dat_007741a4[6] = "Baixa";
    static char dat_007741ac[7] = "Bot\303\243o";
    static char dat_007741b4[15] = "Direc. p/ esq.";
    static char dat_007741c4[16] = "Direc. p/ baixo";
    static char dat_007741d4[18] = "Direc. p/ direita";
    static char dat_007741e8[15] = "Direc. p/ cima";
}
TEXT_INPUT_LABELS dat_008209c0 = {
    {{dat_007741e8, dat_007741d4, dat_007741c4, dat_007741b4, dat_007741ac, dat_007741a4, dat_0077419c, dat_00774188,
      dat_00759770, dat_00774184, dat_00774180, dat_00774174, dat_00774168, dat_0077415c, dat_00774144, dat_0077412c,
      dat_00774114, dat_007740fc, dat_007740e8, dat_007740d4, dat_007740c0, dat_007740a8, dat_0077409c, dat_00774090,
      dat_00774084, dat_00774078, dat_00774064, dat_00774050, dat_00774064, dat_00774040, dat_0077402c, dat_00774018,
      dat_00774010, dat_00774008, dat_00774000, dat_00773ff8, dat_00773ff0}},
    {dat_00773fd8, dat_00773fc8, dat_00773fb0},
    {dat_00773f98, dat_00773f84, dat_00773f6c},
    {dat_00773f54, dat_00773f3c, dat_00773f24},
    dat_00773f14,
    dat_00773f08
};
extern "C" {
    static char dat_00773a78[21] = "Aucune touche saisie";
    static char dat_00773a90[16] = "Souris bouton N";
    static char dat_00773aa0[21] = "Bouton souris milieu";
    static char dat_00773ab8[20] = "Bouton souris droit";
    static char dat_00773acc[21] = "Bouton souris gauche";
    static char dat_00773ae4[28] = "Molette souris vers le haut";
    static char dat_00773b00[19] = "Souris vers le bas";
    static char dat_00773b14[22] = "Souris vers la droite";
    static char dat_00773b2c[27] = "Molette souris vers le bas";
    static char dat_00773b48[20] = "Souris vers le haut";
    static char dat_00773b5c[22] = "Souris vers la gauche";
    static char dat_00773b74[9] = "Souris Z";
    static char dat_00773b80[9] = "Souris Y";
    static char dat_00773b8c[9] = "Souris X";
    static char dat_00773b98[7] = "Retour";
    static char dat_00773ba0[13] = "Touche START";
    static char dat_00773bb0[23] = "Stick analogique droit";
    static char dat_00773bc8[24] = "Stick analogique gauche";
    static char dat_00773be0[17] = "G\303\242chette droite";
    static char dat_00773bf4[17] = "G\303\242chette gauche";
    static char dat_00773c08[23] = "G\303\242chette haute droite";
    static char dat_00773c20[23] = "G\303\242chette haute gauche";
    static char dat_00773c38[9] = "Touche X";
    static char dat_00773c44[9] = "Touche A";
    static char dat_00773c50[9] = "Touche B";
    static char dat_00773c5c[9] = "Touche Y";
    static char dat_00773c68[29] = "Stick analogique droit - Bas";
    static char dat_00773c88[30] = "Stick analogique droit - Haut";
    static char dat_00773ca8[32] = "Stick analogique droit - Droite";
    static char dat_00773cc8[32] = "Stick analogique droit - Gauche";
    static char dat_00773ce8[30] = "Stick analogique gauche - Bas";
    static char dat_00773d08[31] = "Stick analogique gauche - Haut";
    static char dat_00773d28[33] = "Stick analogique gauche - Droite";
    static char dat_00773d4c[33] = "Stick analogique gauche - Gauche";
    static char dat_00773d70[11] = "Rotation Z";
    static char dat_00773d7c[11] = "Rotation Y";
    static char dat_00773d88[11] = "Rotation X";
    static char dat_00773d94[6] = "Axe Z";
    static char dat_00773d9c[6] = "Axe Y";
    static char dat_00773da4[6] = "Axe X";
    static char dat_00773dac[19] = "%d (%s) analogique";
    static char dat_00773dc0[6] = "Haute";
    static char dat_00773dc8[6] = "Basse";
    static char dat_00773dd0[7] = "Touche";
    static char dat_00773dd8[11] = "BMD Gauche";
    static char dat_00773de4[8] = "BMD Bas";
    static char dat_00773dec[11] = "BMD Droite";
    static char dat_00773df8[9] = "BMD Haut";
}
TEXT_INPUT_LABELS dat_00820aa0 = {
    {{dat_00773df8, dat_00773dec, dat_00773de4, dat_00773dd8, dat_00773dd0, dat_00773dc8, dat_00773dc0, dat_00773dac,
      dat_00773da4, dat_00773d9c, dat_00773d94, dat_00773d88, dat_00773d7c, dat_00773d70, dat_00773d4c, dat_00773d28,
      dat_00773d08, dat_00773ce8, dat_00773cc8, dat_00773ca8, dat_00773c88, dat_00773c68, dat_00773c5c, dat_00773c50,
      dat_00773c44, dat_00773c38, dat_00773c20, dat_00773c08, dat_00773bf4, dat_00773be0, dat_00773bc8, dat_00773bb0,
      dat_00773ba0, dat_00773b98, dat_00773b8c, dat_00773b80, dat_00773b74}},
    {dat_00773b5c, dat_00773b48, dat_00773b2c},
    {dat_00773b14, dat_00773b00, dat_00773ae4},
    {dat_00773acc, dat_00773ab8, dat_00773aa0},
    dat_00773a90,
    dat_00773a78
};
extern "C" {
    static char dat_0077367c[13] = "Nessun Input";
    static char dat_0077368c[18] = "Tasto N del mouse";
    static char dat_007736a0[25] = "Tasto centrale del mouse";
    static char dat_007736bc[23] = "Tasto destro del mouse";
    static char dat_007736d4[25] = "Tasto sinistro del mouse";
    static char dat_007736f0[21] = "Rotella del mouse su";
    static char dat_00773708[11] = "Mouse gi\303\271";
    static char dat_00773714[15] = "Mouse a destra";
    static char dat_00773724[23] = "Rotella del mouse gi\303\271";
    static char dat_0077373c[9] = "Mouse su";
    static char dat_00773748[17] = "Mouse a sinistra";
    static char dat_0077375c[9] = "Indietro";
    static char dat_00773768[24] = "Pulsante levetta destro";
    static char dat_00773780[26] = "Pulsante levetta sinistro";
    static char dat_0077379c[17] = "Grilletto destro";
    static char dat_007737b0[19] = "Grilletto sinistro";
    static char dat_007737c4[24] = "Pulsante dorsale destro";
    static char dat_007737dc[26] = "Pulsante dorsale sinistro";
    static char dat_007737f8[11] = "Pulsante X";
    static char dat_00773804[11] = "Pulsante A";
    static char dat_00773810[11] = "Pulsante B";
    static char dat_0077381c[11] = "Pulsante Y";
    static char dat_00773828[23] = "Levetta destra in gi\303\271";
    static char dat_00773840[21] = "Levetta destra in su";
    static char dat_00773858[24] = "Levetta destra a destra";
    static char dat_00773870[26] = "Levetta destra a sinistra";
    static char dat_0077388c[25] = "Levetta sinistra in gi\303\271";
    static char dat_007738a8[23] = "Levetta sinistra in su";
    static char dat_007738c0[26] = "Levetta sinistra a destra";
    static char dat_007738dc[28] = "Levetta sinistra a sinistra";
    static char dat_007738f8[12] = "Rotazione Z";
    static char dat_00773904[12] = "Rotazione Y";
    static char dat_00773910[12] = "Rotazione X";
    static char dat_0077391c[7] = "Asse Z";
    static char dat_00773924[7] = "Asse Y";
    static char dat_0077392c[7] = "Asse X";
    static char dat_00773934[5] = "Alta";
    static char dat_0077393c[6] = "Bassa";
    static char dat_00773944[9] = "Pulsante";
    static char dat_00773950[27] = "Tasto direzionale sinistra";
    static char dat_0077396c[23] = "Tasto direzionale gi\303\271";
    static char dat_00773984[25] = "Tasto direzionale destra";
    static char dat_007739a0[21] = "Tasto direzionale su";
}
TEXT_INPUT_LABELS dat_00820b80 = {
    {{dat_007739a0, dat_00773984, dat_0077396c, dat_00773950, dat_00773944, dat_0077393c, dat_00773934, dat_00775648,
      dat_0077392c, dat_00773924, dat_0077391c, dat_00773910, dat_00773904, dat_007738f8, dat_007738dc, dat_007738c0,
      dat_007738a8, dat_0077388c, dat_00773870, dat_00773858, dat_00773840, dat_00773828, dat_0077381c, dat_00773810,
      dat_00773804, dat_007737f8, dat_007737dc, dat_007737c4, dat_007737b0, dat_0077379c, dat_00773780, dat_00773768,
      dat_007754dc, dat_0077375c, dat_007754d4, dat_007754cc, dat_007754c4}},
    {dat_00773748, dat_0077373c, dat_00773724},
    {dat_00773714, dat_00773708, dat_007736f0},
    {dat_007736d4, dat_007736bc, dat_007736a0},
    dat_0077368c,
    dat_0077367c
};
extern "C" {
    static char dat_00773334[14] = "Keine Eingabe";
    static char dat_00773344[12] = "Maustaste N";
    static char dat_00773350[19] = "Mittlere Maustaste";
    static char dat_00773364[17] = "Rechte Maustaste";
    static char dat_00773378[16] = "Linke Maustaste";
    static char dat_00773388[13] = "Mausrad Oben";
    static char dat_00773398[11] = "Maus Unten";
    static char dat_007733a4[12] = "Maus Rechts";
    static char dat_007733b0[14] = "Mausrad Unten";
    static char dat_007733c0[10] = "Maus Oben";
    static char dat_007733cc[11] = "Maus Links";
    static char dat_007733d8[7] = "Maus Z";
    static char dat_007733e0[7] = "Maus Y";
    static char dat_007733e8[7] = "Maus X";
    static char dat_007733f0[8] = "Zur\303\274ck";
    static char dat_007733f8[14] = "Rechter Stick";
    static char dat_00773408[13] = "Linker Stick";
    static char dat_00773418[17] = "Rechter Schalter";
    static char dat_0077342c[16] = "Linker Schalter";
    static char dat_0077343c[21] = "Rechte Schultertaste";
    static char dat_00773454[20] = "Linke Schultertaste";
    static char dat_00773468[8] = "X-Taste";
    static char dat_00773470[8] = "A-Taste";
    static char dat_00773478[8] = "B-Taste";
    static char dat_00773480[8] = "Y-Taste";
    static char dat_00773488[20] = "Rechter Stick unten";
    static char dat_0077349c[19] = "Rechter Stick oben";
    static char dat_007734b0[21] = "Rechter Stick rechts";
    static char dat_007734c8[20] = "Rechter Stick links";
    static char dat_007734dc[19] = "Linker Stick unten";
    static char dat_007734f0[18] = "Linker Stick oben";
    static char dat_00773504[20] = "Linker Stick rechts";
    static char dat_00773518[19] = "Linker Stick links";
    static char dat_0077352c[11] = "Z-Rotation";
    static char dat_00773538[11] = "Y-Rotation";
    static char dat_00773544[11] = "X-Rotation";
    static char dat_00773550[8] = "Z-Achse";
    static char dat_00773558[8] = "Y-Achse";
    static char dat_00773560[8] = "X-Achse";
    static char dat_00773568[5] = "Hoch";
    static char dat_00773570[8] = "Niedrig";
    static char dat_00773578[6] = "Taste";
    static char dat_00773580[18] = "Steuerkreuz links";
    static char dat_00773594[18] = "Steuerkreuz unten";
    static char dat_007735a8[19] = "Steuerkreuz rechts";
    static char dat_007735bc[17] = "Steuerkreuz oben";
}
TEXT_INPUT_LABELS dat_00820c60 = {
    {{dat_007735bc, dat_007735a8, dat_00773594, dat_00773580, dat_00773578, dat_00773570, dat_00773568, dat_00774b9c,
      dat_00773560, dat_00773558, dat_00773550, dat_00773544, dat_00773538, dat_0077352c, dat_00773518, dat_00773504,
      dat_007734f0, dat_007734dc, dat_007734c8, dat_007734b0, dat_0077349c, dat_00773488, dat_00773480, dat_00773478,
      dat_00773470, dat_00773468, dat_00773454, dat_0077343c, dat_0077342c, dat_00773418, dat_00773408, dat_007733f8,
      dat_007754dc, dat_007733f0, dat_007733e8, dat_007733e0, dat_007733d8}},
    {dat_007733cc, dat_007733c0, dat_007733b0},
    {dat_007733a4, dat_00773398, dat_00773388},
    {dat_00773378, dat_00773364, dat_00773350},
    dat_00773344,
    dat_00773334
};
extern "C" {
    static char dat_00772eec[7] = "Vac\303\255o";
    static char dat_00772ef4[16] = "Bot\303\263n rat\303\263n N";
    static char dat_00772f04[26] = "Bot\303\263n central del rat\303\263n";
    static char dat_00772f20[26] = "Bot\303\263n derecho del rat\303\263n";
    static char dat_00772f3c[28] = "Bot\303\263n izquierdo del rat\303\263n";
    static char dat_00772f58[30] = "Rueda del rat\303\263n hacia arriba";
    static char dat_00772f78[19] = "Rat\303\263n hacia abajo";
    static char dat_00772f8c[20] = "Rat\303\263n a la derecha";
    static char dat_00772fa0[29] = "Rueda del rat\303\263n hacia abajo";
    static char dat_00772fc0[20] = "Rat\303\263n hacia arriba";
    static char dat_00772fd4[22] = "Rat\303\263n a la izquierda";
    static char dat_00772fec[9] = "Rat\303\263n Z";
    static char dat_00772ff8[9] = "Rat\303\263n Y";
    static char dat_00773004[9] = "Rat\303\263n X";
    static char dat_00773010[7] = "Volver";
    static char dat_00773018[13] = "Bot\303\263n START";
    static char dat_00773028[14] = "Stick derecho";
    static char dat_00773038[16] = "Stick izquierdo";
    static char dat_00773048[25] = "Gatillo inferior derecho";
    static char dat_00773064[27] = "Gatillo inferior izquierdo";
    static char dat_00773080[25] = "Gatillo superior derecho";
    static char dat_0077309c[27] = "Gatillo superior izquierdo";
    static char dat_007730b8[9] = "Bot\303\263n X";
    static char dat_007730c4[9] = "Bot\303\263n A";
    static char dat_007730d0[9] = "Bot\303\263n B";
    static char dat_007730dc[9] = "Bot\303\263n Y";
    static char dat_007730e8[20] = "Stick derecho abajo";
    static char dat_007730fc[21] = "Stick derecho arriba";
    static char dat_00773114[27] = "Stick derecho a la derecha";
    static char dat_00773130[29] = "Stick derecho a la izquierda";
    static char dat_00773150[22] = "Stick izquierdo abajo";
    static char dat_00773168[23] = "Stick izquierdo arriba";
    static char dat_00773180[29] = "Stick izquierdo a la derecha";
    static char dat_007731a0[31] = "Stick izquierdo a la izquierda";
    static char dat_007731c0[12] = "Rotaci\303\263n Z";
    static char dat_007731cc[12] = "Rotaci\303\263n Y";
    static char dat_007731d8[12] = "Rotaci\303\263n X";
    static char dat_007731e4[6] = "Eje Z";
    static char dat_007731ec[6] = "Eje Y";
    static char dat_007731f4[6] = "Eje X";
    static char dat_007731fc[19] = "%d (%s) anal\303\263gico";
    static char dat_00773210[5] = "Baja";
    static char dat_00773218[7] = "Bot\303\263n";
    static char dat_00773220[30] = "Mando de direcci\303\263n izquierda";
    static char dat_00773240[26] = "Mando de direcci\303\263n abajo";
    static char dat_0077325c[28] = "Mando de direcci\303\263n derecha";
    static char dat_00773278[27] = "Mando de direcci\303\263n arriba";
}
TEXT_INPUT_LABELS dat_00820d40 = {
    {{dat_00773278, dat_0077325c, dat_00773240, dat_00773220, dat_00773218, dat_00773210, dat_0077419c, dat_007731fc,
      dat_007731f4, dat_007731ec, dat_007731e4, dat_007731d8, dat_007731cc, dat_007731c0, dat_007731a0, dat_00773180,
      dat_00773168, dat_00773150, dat_00773130, dat_00773114, dat_007730fc, dat_007730e8, dat_007730dc, dat_007730d0,
      dat_007730c4, dat_007730b8, dat_0077309c, dat_00773080, dat_00773064, dat_00773048, dat_00773038, dat_00773028,
      dat_00773018, dat_00773010, dat_00773004, dat_00772ff8, dat_00772fec}},
    {dat_00772fd4, dat_00772fc0, dat_00772fa0},
    {dat_00772f8c, dat_00772f78, dat_00772f58},
    {dat_00772f3c, dat_00772f20, dat_00772f04},
    dat_00772ef4,
    dat_00772eec
};
extern "C" {
    static char dat_00772bd8[11] = "Museknap N";
    static char dat_00772be4[19] = "Midterste museknap";
    static char dat_00772bf8[16] = "H\303\270jre museknap";
    static char dat_00772c08[17] = "Venstre museknap";
    static char dat_00772c1c[11] = "Mushjul op";
    static char dat_00772c28[11] = "Mus h\303\270jre";
    static char dat_00772c34[7] = "Mus op";
    static char dat_00772c3c[7] = "\011Mus X";
    static char dat_00772c44[8] = "Tilbage";
    static char dat_00772c4c[17] = "H\303\270jre styrepind";
    static char dat_00772c60[18] = "Venstre styrepind";
    static char dat_00772c74[17] = "Venstre udl\303\270ser";
    static char dat_00772c88[19] = "H\303\270jre skulderknap";
    static char dat_00772c9c[20] = "Venstre skulderknap";
    static char dat_00772cb0[7] = "Knap X";
    static char dat_00772cb8[7] = "Knap A";
    static char dat_00772cc0[7] = "Knap B";
    static char dat_00772cc8[7] = "Knap Y";
    static char dat_00772cd0[21] = "H\303\270jre Styrepind Ned";
    static char dat_00772ce8[20] = "H\303\270jre Styrepind Op";
    static char dat_00772cfc[24] = "H\303\270jre Styrepind H\303\270jre";
    static char dat_00772d14[25] = "H\303\270jre Styrepind Venstre";
    static char dat_00772d30[22] = "Venstre Styrepind Ned";
    static char dat_00772d48[21] = "Venstre Styrepind Op";
    static char dat_00772d60[25] = "Venstre Styrepind H\303\270jre";
    static char dat_00772d7c[26] = "Venstre Styrepind Venstre";
    static char dat_00772d98[9] = "Rot\303\251r Z";
    static char dat_00772da4[9] = "Rot\303\251r Y";
    static char dat_00772db0[10] = "Rot\303\251r X ";
    static char dat_00772dbc[8] = "Z-aksen";
    static char dat_00772dc4[8] = "Y-aksen";
    static char dat_00772dcc[8] = "X-aksen";
    static char dat_00772dd4[5] = "H\303\270j";
    static char dat_00772ddc[5] = "knap";
    static char dat_00772de4[19] = "Hat-switch venstre";
    static char dat_00772df8[15] = "Hat-switch ned";
    static char dat_00772e08[18] = "Hat-switch h\303\270jre";
    static char dat_00772e1c[14] = "Hat-switch Op";
}
TEXT_INPUT_LABELS dat_00820e20 = {
    {{dat_00772e1c, dat_00772e08, dat_00772df8, dat_00772de4, dat_00772ddc, dat_00774bb4, dat_00772dd4, dat_00774b9c,
      dat_00772dcc, dat_00772dc4, dat_00772dbc, dat_00772db0, dat_00772da4, dat_00772d98, dat_00772d7c, dat_00772d60,
      dat_00772d48, dat_00772d30, dat_00772d14, dat_00772cfc, dat_00772ce8, dat_00772cd0, dat_00772cc8, dat_00772cc0,
      dat_00772cb8, dat_00772cb0, dat_00772c9c, dat_00772c88, dat_00772c74, dat_00772c88, dat_00772c60, dat_00772c4c,
      dat_007754dc, dat_00772c44, dat_00772c3c, dat_007749ec, dat_007749e4}},
    {dat_007749d8, dat_00772c34, dat_007746d4},
    {dat_00772c28, dat_007749ac, dat_00772c1c},
    {dat_00772c08, dat_00772bf8, dat_00772be4},
    dat_00772bd8,
    dat_00774948
};
extern "C" {
    static char dat_00772830[14] = "Brak wej\305\233cia";
    static char dat_00772840[20] = "Myszka - przycisk N";
    static char dat_00772854[26] = "\305\232rodkowy przycisk myszki";
    static char dat_00772870[22] = "Prawy przycisk myszki";
    static char dat_00772888[21] = "Lewy przycisk myszki";
    static char dat_007728a0[24] = "K\303\263\305\202ko myszki w g\303\263r\304\231";
    static char dat_007728b8[15] = "Myszka w d\303\263\305\202";
    static char dat_007728c8[23] = "K\303\263\305\202ko myszki w d\303\263\305\202";
    static char dat_007728e0[16] = "Myszka w g\303\263r\304\231";
    static char dat_007728f0[14] = "Myszka w lewo";
    static char dat_00772900[9] = "Myszka Z";
    static char dat_0077290c[9] = "Myszka Y";
    static char dat_00772918[9] = "Myszka X";
    static char dat_00772924[7] = "Wr\303\263\304\207";
    static char dat_0077292c[15] = "Prawy dr\304\205\305\274ek";
    static char dat_0077293c[14] = "Lewy dr\304\205\305\274ek";
    static char dat_0077294c[12] = "Prawy spust";
    static char dat_00772958[11] = "Lewy spust";
    static char dat_00772964[23] = "Prawy przycisk przedni";
    static char dat_0077297c[22] = "Lewy przycisk przedni";
    static char dat_00772994[11] = "Przycisk X";
    static char dat_007729a0[11] = "Przycisk A";
    static char dat_007729ac[11] = "Przycisk B";
    static char dat_007729b8[11] = "Przycisk Y";
    static char dat_007729c4[23] = "Prawy dr\304\205\305\274ek w d\303\263\305\202";
    static char dat_007729dc[24] = "Prawy dr\304\205\305\274ek w g\303\263r\304\231";
    static char dat_007729f4[23] = "Prawy dr\304\205\305\274ek w prawo";
    static char dat_00772a0c[22] = "Prawy dr\304\205\305\274ek w lewo";
    static char dat_00772a24[22] = "Lewy dr\304\205\305\274ek w d\303\263\305\202";
    static char dat_00772a3c[23] = "Lewy dr\304\205\305\274ek w g\303\263r\304\231";
    static char dat_00772a54[22] = "Lewy dr\304\205\305\274ek w prawo";
    static char dat_00772a6c[21] = "Lewy dr\304\205\305\274ek w lewo";
    static char dat_00772a84[15] = "Obr\303\263t w osi Z";
    static char dat_00772a94[15] = "Obr\303\263t w osi Y";
    static char dat_00772aa4[15] = "Obr\303\263t w osi X";
    static char dat_00772ab4[6] = "O\305\233 Z";
    static char dat_00772abc[6] = "O\305\233 Y";
    static char dat_00772ac4[6] = "O\305\233 X";
    static char dat_00772acc[18] = "Analogowy (%s) %d";
    static char dat_00772ae0[7] = "Wysoka";
    static char dat_00772ae8[6] = "Niska";
    static char dat_00772af0[9] = "Przycisk";
    static char dat_00772afc[16] = "Kapturek w lewo";
    static char dat_00772b0c[17] = "Kapturek w d\303\263\305\202";
    static char dat_00772b20[17] = "Kapturek w prawo";
    static char dat_00772b34[18] = "Kapturek w g\303\263r\304\231";
}
TEXT_INPUT_LABELS dat_00820f00 = {
    {{dat_00772b34, dat_00772b20, dat_00772b0c, dat_00772afc, dat_00772af0, dat_00772ae8, dat_00772ae0, dat_00772acc,
      dat_00772ac4, dat_00772abc, dat_00772ab4, dat_00772aa4, dat_00772a94, dat_00772a84, dat_00772a6c, dat_00772a54,
      dat_00772a3c, dat_00772a24, dat_00772a0c, dat_007729f4, dat_007729dc, dat_007729c4, dat_007729b8, dat_007729ac,
      dat_007729a0, dat_00772994, dat_0077297c, dat_00772964, dat_00772958, dat_0077294c, dat_0077293c, dat_0077292c,
      dat_007754dc, dat_00772924, dat_00772918, dat_0077290c, dat_00772900}},
    {dat_007728f0, dat_007728e0, dat_007728c8},
    {dat_007728e0, dat_007728b8, dat_007728a0},
    {dat_00772888, dat_00772870, dat_00772854},
    dat_00772840,
    dat_00772830
};
extern "C" {
    static char dat_0077229c[39] =
        "\320\235\320\265\321\202 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 "
        "\320\262\320\262\320\276\320\264\320\260";
    static char dat_007722c4[24] = "\320\232\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270 N";
    static char dat_007722dc[37] = "\320\241\321\200\320\265\320\264\320\275\321\217\321\217 "
                                   "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
    static char dat_00772304[35] = "\320\237\321\200\320\260\320\262\320\260\321\217 "
                                   "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
    static char dat_00772328[33] = "\320\233\320\265\320\262\320\260\321\217 "
                                   "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
    static char dat_0077234c[33] = "\320\232\320\276\320\273\320\265\321\201\320\276 \320\274\321\213\321\210\320\270 "
                                   "\320\262\320\262\320\265\321\200\321\205";
    static char dat_00772370[18] = "\320\234\321\213\321\210\321\214 \320\262\320\275\320\270\320\267";
    static char dat_00772384[22] = "\320\234\321\213\321\210\321\214 \320\262\320\277\321\200\320\260\320\262\320\276";
    static char dat_0077239c[31] = "\320\232\320\276\320\273\320\265\321\201\320\276 \320\274\321\213\321\210\320\270 "
                                   "\320\262\320\275\320\270\320\267";
    static char dat_007723bc[20] = "\320\234\321\213\321\210\321\214 \320\262\320\262\320\265\321\200\321\205";
    static char dat_007723d0[20] = "\320\234\321\213\321\210\321\214 \320\262\320\273\320\265\320\262\320\276";
    static char dat_007723e4[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 Z)";
    static char dat_007723f8[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 Y)";
    static char dat_0077240c[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 X)";
    static char dat_00772420[11] = "\320\235\320\260\320\267\320\260\320\264";
    static char dat_0077242c[11] = "\320\241\321\202\320\260\321\200\321\202";
    static char dat_00772438[30] =
        "\320\237\321\200\320\260\320\262\321\213\320\271 \320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272";
    static char dat_00772458[28] =
        "\320\233\320\265\320\262\321\213\320\271 \320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272";
    static char dat_00772474[24] = "\320\237\321\200\320\260\320\262\321\213\320\271 \320\272\321\203\321\200\320\276\320\272";
    static char dat_0077248c[22] = "\320\233\320\265\320\262\321\213\320\271 \320\272\321\203\321\200\320\276\320\272";
    static char dat_007724a4[28] =
        "\320\237\321\200\320\260\320\262\321\213\320\271 \321\202\321\200\320\270\320\263\320\263\320\265\321\200";
    static char dat_007724c0[26] = "\320\233\320\265\320\262\321\213\320\271 \321\202\321\200\320\270\320\263\320\263\320\265\321\200";
    static char dat_007724dc[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 X";
    static char dat_007724ec[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 A";
    static char dat_007724fc[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 B";
    static char dat_0077250c[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 Y";
    static char dat_0077251c[41] = "\320\222\320\275\320\270\320\267 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_00772548[43] = "\320\222\320\262\320\265\321\200\321\205 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_00772574[45] = "\320\222\320\277\321\200\320\260\320\262\320\276 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_007725a4[43] = "\320\222\320\273\320\265\320\262\320\276 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_007725d0[39] = "\320\222\320\275\320\270\320\267 (\320\273\320\265\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_007725f8[41] = "\320\222\320\262\320\265\321\200\321\205 (\320\273\320\265\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_00772624[43] = "\320\222\320\277\321\200\320\260\320\262\320\276 (\320\273\320\265\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_00772650[41] = "\320\222\320\273\320\265\320\262\320\276 (\320\273\320\265\320\262\321\213\320\271 "
                                   "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
    static char dat_0077267c[24] = "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 Z";
    static char dat_00772694[24] = "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 Y";
    static char dat_007726ac[24] = "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 X";
    static char dat_007726c4[9] = "\320\236\321\201\321\214 Z";
    static char dat_007726d0[9] = "\320\236\321\201\321\214 Y";
    static char dat_007726dc[9] = "\320\236\321\201\321\214 X";
    static char dat_007726e8[29] = "\320\220\320\275\320\260\320\273\320\276\320\263\320\276\320\262\321\213\320\271 %d (%s)";
    static char dat_00772708[15] = "\320\222\321\213\321\201\320\276\320\272\320\276\320\265";
    static char dat_00772718[13] = "\320\235\320\270\320\267\320\272\320\276\320\265";
    static char dat_00772728[13] = "\320\232\320\275\320\276\320\277\320\272\320\260";
    static char dat_00772738[11] = "\320\222\320\273\320\265\320\262\320\276";
    static char dat_00772744[9] = "\320\222\320\275\320\270\320\267";
    static char dat_00772750[13] = "\320\222\320\277\321\200\320\260\320\262\320\276";
    static char dat_00772760[11] = "\320\222\320\262\320\265\321\200\321\205";
}
TEXT_INPUT_LABELS dat_00820fe0 = {
    {{dat_00772760, dat_00772750, dat_00772744, dat_00772738, dat_00772728, dat_00772718, dat_00772708, dat_007726e8,
      dat_007726dc, dat_007726d0, dat_007726c4, dat_007726ac, dat_00772694, dat_0077267c, dat_00772650, dat_00772624,
      dat_007725f8, dat_007725d0, dat_007725a4, dat_00772574, dat_00772548, dat_0077251c, dat_0077250c, dat_007724fc,
      dat_007724ec, dat_007724dc, dat_007724c0, dat_007724a4, dat_0077248c, dat_00772474, dat_00772458, dat_00772438,
      dat_0077242c, dat_00772420, dat_0077240c, dat_007723f8, dat_007723e4}},
    {dat_007723d0, dat_007723bc, dat_0077239c},
    {dat_00772384, dat_00772370, dat_0077234c},
    {dat_00772328, dat_00772304, dat_007722dc},
    dat_007722c4,
    dat_0077229c
};
extern "C" {
    static char dat_00771e38[15] = "\305\275\303\241dn\303\275 vstup";
    static char dat_00771e48[19] = "Tla\304\215\303\255tko my\305\241i A";
    static char dat_00771e5c[27] = "St\305\231edn\303\255 tla\304\215\303\255tko my\305\241i";
    static char dat_00771e78[24] = "Prav\303\251 tla\304\215\303\255tko my\305\241i";
    static char dat_00771e90[23] = "Lev\303\251 tla\304\215\303\255tko my\305\241i";
    static char dat_00771ea8[22] = "Kole\304\215ko my\305\241i nahoru";
    static char dat_00771ec0[19] = "Pohyb my\305\241\303\255 dol\305\257";
    static char dat_00771ed4[20] = "Pohyb my\305\241\303\255 vpravo";
    static char dat_00771ee8[21] = "Kole\304\215ko my\305\241i dol\305\257";
    static char dat_00771f00[20] = "Pohyb my\305\241\303\255 nahoru";
    static char dat_00771f14[19] = "Pohyb my\305\241\303\255 vlevo";
    static char dat_00771f28[6] = "Ose X";
    static char dat_00771f30[6] = "Zp\304\233t";
    static char dat_00771f38[15] = "Prav\303\241 p\303\241\304\215ka";
    static char dat_00771f48[14] = "Lev\303\241 p\303\241\304\215ka";
    static char dat_00771f58[16] = "Prav\303\241 spou\305\241\305\245";
    static char dat_00771f68[15] = "Lev\303\241 spou\305\241\305\245";
    static char dat_00771f78[26] = "Prav\303\251 bo\304\215n\303\255 tla\304\215\303\255tko";
    static char dat_00771f94[25] = "Lev\303\251 bo\304\215n\303\255 tla\304\215\303\255tko";
    static char dat_00771fb0[13] = "Tla\304\215\303\255tko X";
    static char dat_00771fc0[13] = "Tla\304\215\303\255tko A";
    static char dat_00771fd0[13] = "Tla\304\215\303\255tko B";
    static char dat_00771fe0[13] = "Tla\304\215\303\255tko Y";
    static char dat_00771ff0[21] = "Prav\303\241 p\303\241\304\215ka dol\305\257";
    static char dat_00772008[22] = "Prav\303\241 p\303\241\304\215ka nahoru";
    static char dat_00772020[22] = "Prav\303\241 p\303\241\304\215ka vpravo";
    static char dat_00772038[21] = "Prav\303\241 p\303\241\304\215ka vlevo";
    static char dat_00772050[20] = "Lev\303\241 p\303\241\304\215ka dol\305\257";
    static char dat_00772064[21] = "Lev\303\241 p\303\241\304\215ka nahoru";
    static char dat_0077207c[21] = "Lev\303\241 p\303\241\304\215ka vpravo";
    static char dat_00772094[20] = "Lev\303\241 p\303\241\304\215ka vlevo";
    static char dat_007720a8[9] = "Rotace Z";
    static char dat_007720b4[9] = "Rotace Y";
    static char dat_007720c0[9] = "Rotace X";
    static char dat_007720cc[6] = "Osa Z";
    static char dat_007720d4[6] = "Osa Y";
    static char dat_007720dc[6] = "Osa X";
    static char dat_007720e4[9] = "Vy\305\241\305\241\303\255";
    static char dat_007720f0[9] = "Ni\305\276\305\241\303\255";
    static char dat_007720fc[11] = "Tla\304\215\303\255tko";
    static char dat_00772108[21] = "Sm\304\233r. k\305\231\303\255\305\276 vlevo";
    static char dat_00772120[21] = "Sm\304\233r. k\305\231\303\255\305\276 dol\305\257";
    static char dat_00772138[22] = "Sm\304\233r. k\305\231\303\255\305\276 vpravo";
    static char dat_00772150[22] = "Sm\304\233r. k\305\231\303\255\305\276 nahoru";
}
TEXT_INPUT_LABELS dat_008210c0 = {
    {{dat_00772150, dat_00772138, dat_00772120, dat_00772108, dat_007720fc, dat_007720f0, dat_007720e4, dat_00774b9c,
      dat_007720dc, dat_007720d4, dat_007720cc, dat_007720c0, dat_007720b4, dat_007720a8, dat_00772094, dat_0077207c,
      dat_00772064, dat_00772050, dat_00772038, dat_00772020, dat_00772008, dat_00771ff0, dat_00771fe0, dat_00771fd0,
      dat_00771fc0, dat_00771fb0, dat_00771f94, dat_00771f78, dat_00771f68, dat_00771f58, dat_00771f48, dat_00771f38,
      dat_007754dc, dat_00771f30, dat_00771f28, dat_007720d4, dat_007720cc}},
    {dat_00771f14, dat_00771f00, dat_00771ee8},
    {dat_00771ed4, dat_00771ec0, dat_00771ea8},
    {dat_00771e90, dat_00771e78, dat_00771e5c},
    dat_00771e48,
    dat_00771e38
};
extern "C" {
    char dat_00771aac[11] = "Num. COMMA";
    char dat_00771ba4[11] = "Num. Point";
    char *TextInputKeyNameTable[238] = {
        dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
        dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771d50, dat_00771d48, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
        dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
        dat_00753370, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
        dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771cd4, dat_00771cc8, dat_00771cc0, dat_00771cb4, dat_00771cb0, dat_00774180,
        dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771c94, dat_00771c8c, dat_00771c80,
        dat_00771c74, dat_00771c6c, dat_00771c64, dat_00771c5c, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
        dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
        dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
        dat_00771bb8, dat_00771bb0, dat_00771ba4, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
        dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
        dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
        dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00771af4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
        dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00771aac,
        dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00771a94, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
        dat_007516c0, dat_00771a84, dat_00771a80, dat_00771a78, dat_007516c0, dat_00771a70, dat_007516c0, dat_00771a68, dat_007516c0,
        dat_00771a64, dat_0075ac34, dat_00771a58, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a48, dat_00771a40, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
        dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
    static char dat_0077199c[11] = "Num. Comma";
    static char dat_007719a8[8] = "Oem_102";
    static char dat_007719b0[10] = "Num. Stop";
}
/* Entries past the end of a 222-entry table read the next table. */
char *dat_00821558[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771d50, dat_00771d48, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
    dat_00753370, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771cd4, dat_00771cc8, dat_00771cc0, dat_00771cb4, "\\",         dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771c94, dat_00771c8c, dat_00771c80,
    dat_00771c74, dat_00771c6c, dat_00771c64, dat_00771c5c, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007719b0, dat_007516c0, dat_007516c0, dat_007719a8, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00771af4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00771a94, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00771a80, dat_00771a78, dat_007516c0, dat_00771a70, dat_007516c0, dat_00771a68, dat_007516c0,
    dat_00771a64, dat_0075ac34, dat_00771a58, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a48, dat_00771a40, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_007716dc[25] = "\343\202\242\343\203\227\343\203\252\343\202\261\343\203\274\343\202\267\343\203\247\343\203\263";
    static char dat_007716f8[23] = "\345\217\263 \343\202\246\343\202\243\343\203\263\343\203\211\343\202\246\343\202\272";
    static char dat_00771710[23] = "\345\267\246 \343\202\246\343\202\243\343\203\263\343\203\211\343\202\246\343\202\272";
    static char dat_00771728[10] = "Page Down";
    static char dat_00771734[10] = "\344\270\213\347\237\242\345\215\260";
    static char dat_00771740[10] = "\345\217\263\347\237\242\345\215\260";
    static char dat_0077174c[10] = "\345\267\246\347\237\242\345\215\260";
    static char dat_00771758[8] = "Page Up";
    static char dat_00771760[10] = "\344\270\212\347\237\242\345\215\260";
    static char dat_0077176c[8] = "\345\217\263 Alt";
    static char dat_00771774[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 /";
    static char dat_00771788[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 ,";
    static char dat_0077179c[9] = "\345\217\263 Ctrl";
    static char dat_007717a8[22] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 Enter";
    static char dat_007717c0[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 .";
    static char dat_007717d4[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 0";
    static char dat_007717e8[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 3";
    static char dat_007717fc[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 2";
    static char dat_00771810[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 1";
    static char dat_00771824[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 +";
    static char dat_00771838[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 6";
    static char dat_0077184c[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 5";
    static char dat_00771860[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 4";
    static char dat_00771874[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 -";
    static char dat_00771888[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 9";
    static char dat_0077189c[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 8";
    static char dat_007718b0[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 7";
    static char dat_007718c4[13] = "\343\202\271\343\203\232\343\203\274\343\202\271";
    static char dat_007718d4[8] = "\345\267\246 Alt";
    static char dat_007718dc[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 *";
    static char dat_007718f0[10] = "\345\217\263 Shift";
    static char dat_007718fc[34] = "\343\203\225\343\202\251\343\203\257\343\203\274\343\203\211\343\202\271\343\203"
                                   "\251\343\203\203\343\202\267\343\203\245(/)";
    static char dat_00771920[16] = "\343\203\224\343\203\252\343\202\252\343\203\211(.)";
    static char dat_00771930[13] = "\343\202\253\343\203\263\343\203\236(,)";
    static char dat_00771940[10] = "\345\267\246 Shift";
    static char dat_0077194c[16] = "\343\202\260\343\203\254\343\202\244\343\203\264(`)";
    static char dat_0077195c[25] = "\343\202\242\343\203\235\343\202\271\343\203\210\343\203\255\343\203\225\343\202\243(')";
    static char dat_00771978[21] = "\343\202\273\343\203\237\343\202\263\343\203\255\343\203\263(\357\274\233)";
    static char dat_00771990[9] = "\345\267\246 Ctrl";
}
char *dat_00821910[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771d50, dat_00771d48, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
    dat_00753370, dat_00771d0c, dat_00771990, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771978, dat_0077195c, dat_0077194c, dat_00771940, "\\",         dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771930, dat_00771920, dat_007718fc,
    dat_007718f0, dat_007718dc, dat_007718d4, dat_007718c4, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_007718b0,
    dat_0077189c, dat_00771888, dat_00771874, dat_00771860, dat_0077184c, dat_00771838, dat_00771824, dat_00771810, dat_007717fc,
    dat_007717e8, dat_007717d4, dat_007717c0, dat_007516c0, dat_007516c0, dat_007719a8, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007717a8, dat_0077179c, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00771788,
    dat_007516c0, dat_00771774, dat_007516c0, dat_00771a9c, dat_0077176c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00771760, dat_00771758, dat_007516c0, dat_0077174c, dat_007516c0, dat_00771740, dat_007516c0,
    dat_00771a64, dat_00771734, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771710, dat_007716f8, dat_007716dc, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_00771554[6] = "P.Win";
    static char dat_0077155c[6] = "L.Win";
    static char dat_00771564[8] = "Pg Down";
    static char dat_0077156c[6] = "Dol\305\257";
    static char dat_00771574[7] = "Vpravo";
    static char dat_0077157c[6] = "Vlevo";
    static char dat_00771584[6] = "Pg Up";
    static char dat_0077158c[7] = "Nahoru";
    static char dat_00771594[6] = "P.Alt";
    static char dat_0077159c[6] = "Num./";
    static char dat_007715a4[7] = "P.Ctrl";
    static char dat_007715ac[10] = "Num.Enter";
    static char dat_007715b8[14] = "Num.Rovn\303\241 Se";
    static char dat_007715c8[6] = "Num .";
    static char dat_007715d0[6] = "Num.0";
    static char dat_007715d8[6] = "Num.3";
    static char dat_007715e0[6] = "Num.2";
    static char dat_007715e8[6] = "Num.1";
    static char dat_007715f0[6] = "Num.+";
    static char dat_007715f8[6] = "Num.6";
    static char dat_00771600[6] = "Num.5";
    static char dat_00771608[6] = "Num.4";
    static char dat_00771610[6] = "Num.-";
    static char dat_00771618[6] = "Num.9";
    static char dat_00771620[6] = "Num.8";
    static char dat_00771628[6] = "Num.7";
    static char dat_00771630[10] = "Mezern\303\255k";
    static char dat_0077163c[6] = "L.Alt";
    static char dat_00771644[6] = "Kr\303\241t";
    static char dat_0077164c[8] = "P.Shift";
    static char dat_00771654[9] = "Lom\303\255tko";
    static char dat_00771660[7] = "Te\304\215ka";
    static char dat_00771668[8] = "\304\214\303\241rka";
    static char dat_00771670[9] = "Uvozovky";
    static char dat_0077167c[8] = "L.Shift";
    static char dat_00771684[8] = "Vlnovka";
    static char dat_0077168c[9] = "Apostrof";
    static char dat_00771698[3] = "\305\256";
    static char dat_0077169c[7] = "L.Ctrl";
    static char dat_007716a4[11] = "P.Z\303\241vorka";
    static char dat_007716b0[3] = "\303\232";
    static char dat_007716b4[11] = "Tabul\303\241tor";
    static char dat_007716c0[10] = "Rovn\303\241 Se";
    static char dat_007716cc[7] = "M\303\255nus";
    static char dat_007716d4[8] = "(EMPTY)";
}
char *dat_00821cc8[238] = {
    dat_007716d4, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007716cc, dat_007716c0, dat_00771d3c, dat_007716b4, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774180, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_007716b0,
    dat_007716a4, dat_00771d0c, dat_0077169c, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771698, dat_0077168c, dat_00771684, dat_0077167c, dat_00771670, dat_00774184,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771668, dat_00771660, dat_00771654,
    dat_0077164c, dat_00771644, dat_0077163c, dat_00771630, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771628,
    dat_00771620, dat_00771618, dat_00771610, dat_00771608, dat_00771600, dat_007715f8, dat_007715f0, dat_007715e8, dat_007715e0,
    dat_007715d8, dat_007715d0, dat_007715c8, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007715b8, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007715ac, dat_007715a4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_0077159c, dat_007516c0, dat_00771a9c, dat_00771594, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_0077158c, dat_00771584, dat_007516c0, dat_0077157c, dat_007516c0, dat_00771574, dat_007516c0,
    dat_00771a64, dat_0077156c, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_0077155c, dat_00771554, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_00755d70[2] = ")";
    static char dat_0077130c[10] = "Windows D";
    static char dat_00771318[10] = "Windows G";
    static char dat_00771324[6] = "Suppr";
    static char dat_0077132c[6] = "Inser";
    static char dat_00771334[11] = "Page Suiv.";
    static char dat_00771340[4] = "Bas";
    static char dat_00771344[4] = "Fin";
    static char dat_00771348[7] = "Droite";
    static char dat_00771350[7] = "Gauche";
    static char dat_00771358[12] = "Page Pr\303\250c.";
    static char dat_00771364[5] = "Haut";
    static char dat_0077136c[8] = "Origine";
    static char dat_00771374[6] = "Alt D";
    static char dat_0077137c[17] = "Impr \303\211cran Syst";
    static char dat_00771390[12] = "Pav. Num. /";
    static char dat_0077139c[18] = "Pav. Num. Virgule";
    static char dat_007713b0[4] = "www";
    static char dat_007713b4[7] = "Ctrl D";
    static char dat_007713bc[18] = "Pav. Num. Entr\303\251e";
    static char dat_007713d0[2] = "\260";
    static char dat_007713d4[2] = "%";
    static char dat_007713d8[12] = "Pav. Num. =";
    static char dat_007713e4[2] = "{";
    static char dat_007713e8[16] = "Pav. Num. Point";
    static char dat_007713f8[12] = "Pav. Num. 0";
    static char dat_00771404[12] = "Pav. Num. 3";
    static char dat_00771410[12] = "Pav. Num. 2";
    static char dat_0077141c[12] = "Pav. Num. 1";
    static char dat_00771428[12] = "Pav. Num. +";
    static char dat_00771434[12] = "Pav. Num. 6";
    static char dat_00771440[12] = "Pav. Num. 5";
    static char dat_0077144c[12] = "Pav. Num. 4";
    static char dat_00771458[12] = "Pav. Num. -";
    static char dat_00771464[12] = "Pav. Num. 9";
    static char dat_00771470[12] = "Pav. Num. 8";
    static char dat_0077147c[12] = "Pav. Num. 7";
    static char dat_00771488[14] = "Arr\303\252t D\303\251fil";
    static char dat_00771498[9] = "Verr Num";
    static char dat_007714a4[9] = "Verr Maj";
    static char dat_007714b0[7] = "Espace";
    static char dat_007714b8[6] = "Alt G";
    static char dat_007714c0[12] = "Pav. Num. *";
    static char dat_007714cc[6] = "Maj D";
    static char dat_007714d4[20] = "Point D'Exclamation";
    static char dat_007714e8[12] = "Deux Points";
    static char dat_007714f4[14] = "Point Virgule";
    static char dat_00771504[8] = "Virgule";
    static char dat_0077150c[2] = "*";
    static char dat_00771510[6] = "Maj G";
    static char dat_00771518[3] = "\302\262";
    static char dat_0077151c[3] = "\303\231";
    static char dat_00771520[7] = "Ctrl G";
    static char dat_00771528[8] = "Entr\303\251e";
    static char dat_00771530[2] = "|";
    static char dat_00771534[2] = "^";
    static char dat_00771538[11] = "Tabulation";
    static char dat_00771544[5] = "Egal";
    static char dat_0077154c[6] = "Echap";
}
char *dat_00822080[222] = {
    dat_00771d80, dat_0077154c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00755d70, dat_00771544, dat_00773b98, dat_00771538, dat_00771d00, dat_00774180,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00771534,
    dat_00771530, dat_00771528, dat_00771520, dat_00771d34, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771c9c, dat_0077151c, dat_00771518, dat_00771510, dat_0077150c, dat_00771d30,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771504, dat_007714f4, dat_007714e8, dat_007714d4,
    dat_007714cc, dat_007714c0, dat_007714b8, dat_007714b0, dat_007714a4, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771498, dat_00771488, dat_0077147c,
    dat_00771470, dat_00771464, dat_00771458, dat_0077144c, dat_00771440, dat_00771434, dat_00771428, dat_0077141c, dat_00771410,
    dat_00771404, dat_007713f8, dat_007713e8, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007713d8, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_007713d4, dat_00771c9c, dat_007713d0, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007713bc, dat_007713b4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_007713b0, dat_0077139c,
    dat_007516c0, dat_00771390, dat_007516c0, dat_0077137c, dat_00771374, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_0077136c, dat_00771364, dat_00771358, dat_007516c0, dat_00771350, dat_007516c0, dat_00771348, dat_007516c0,
    dat_00771344, dat_00771340, dat_00771334, dat_0077132c, dat_00771324, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771318, dat_0077130c, dat_00771a38};
extern "C" {
    static char dat_0075c044[2] = ":";
    static char dat_007710d0[9] = "Win Des.";
    static char dat_007710dc[9] = "Win Sin.";
    static char dat_007710e8[6] = "Canc.";
    static char dat_007710f0[10] = "Inserisci";
    static char dat_007710fc[10] = "Pag. Gi\303\231";
    static char dat_00771108[13] = "Freccia Gi\303\271";
    static char dat_00771118[5] = "Fine";
    static char dat_00771120[13] = "Freccia Des.";
    static char dat_00771130[13] = "Freccia Sin.";
    static char dat_00771140[8] = "Pag. Su";
    static char dat_00771148[12] = "Freccia S\303\271";
    static char dat_00771154[6] = "Pausa";
    static char dat_0077115c[9] = "Alt Des.";
    static char dat_00771168[6] = "RSIST";
    static char dat_00771170[10] = "Tastnum /";
    static char dat_0077117c[10] = "Tastnum ,";
    static char dat_00771188[7] = "Vol Su";
    static char dat_00771190[9] = "Vol Gi\303\271";
    static char dat_0077119c[10] = "Ctrl Des.";
    static char dat_007711a8[14] = "Tastnum Invio";
    static char dat_007711b8[10] = "Tastnum =";
    static char dat_007711c4[10] = "Tastnum .";
    static char dat_007711d0[10] = "Tastnum 0";
    static char dat_007711dc[10] = "Tastnum 3";
    static char dat_007711e8[10] = "Tastnum 2";
    static char dat_007711f4[10] = "Tastnum 1";
    static char dat_00771200[10] = "Tastnum +";
    static char dat_0077120c[10] = "Tastnum 6";
    static char dat_00771218[10] = "Tastnum 5";
    static char dat_00771224[10] = "Tastnum 4";
    static char dat_00771230[10] = "Tastnum -";
    static char dat_0077123c[10] = "Tastnum 9";
    static char dat_00771248[10] = "Tastnum 8";
    static char dat_00771254[10] = "Tastnum 7";
    static char dat_00771260[13] = "Blocca Scorr";
    static char dat_00771270[9] = "Bloc Num";
    static char dat_0077127c[12] = "BLOC MAIUSC";
    static char dat_00771288[7] = "Spazio";
    static char dat_00771290[9] = "Alt Sin.";
    static char dat_0077129c[10] = "Tastnum *";
    static char dat_007712a8[11] = "Shift Des.";
    static char dat_007712b4[2] = "-";
    static char dat_007712b8[6] = "Punto";
    static char dat_007712c0[8] = "Virgola";
    static char dat_007712c8[3] = "\303\271";
    static char dat_007712cc[11] = "Shift Sin.";
    static char dat_007712d8[3] = "\303\240";
    static char dat_007712dc[3] = "\303\262";
    static char dat_007712e0[10] = "Ctrl Sin.";
    static char dat_007712ec[6] = "Invio";
    static char dat_007712f4[2] = "+";
    static char dat_007712f8[3] = "\303\250";
    static char dat_007712fc[3] = "\303\254";
    static char dat_00771300[10] = "APOSTROFO";
}
char *dat_008223f8[222] = {
    dat_007716d4, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771300, dat_007712fc, dat_0077375c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_007712f8,
    dat_007712f4, dat_007712ec, dat_007712e0, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_007712dc, dat_007712d8, "\\",         dat_007712cc, dat_007712c8, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_007712c0, dat_007712b8, dat_007712b4,
    dat_007712a8, dat_0077129c, dat_00771290, dat_00771288, dat_0077127c, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771270, dat_00771260, dat_00771254,
    dat_00771248, dat_0077123c, dat_00771230, dat_00771224, dat_00771218, dat_0077120c, dat_00771200, dat_007711f4, dat_007711e8,
    dat_007711dc, dat_007711d0, dat_007711c4, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007711b8, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_0075c044, "_",          dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007711a8, dat_0077119c, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771190, dat_007516c0, dat_00771188, dat_007516c0, dat_00771ab8, dat_0077117c,
    dat_007516c0, dat_00771170, dat_007516c0, dat_00771168, dat_0077115c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00771a84, dat_00771148, dat_00771140, dat_007516c0, dat_00771130, dat_007516c0, dat_00771120, dat_007516c0,
    dat_00771118, dat_00771108, dat_007710fc, dat_007710f0, dat_007710e8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007710dc, dat_007710d0, dat_00771a38};
extern "C" {
    static char dat_0075eba8[2] = ".";
    static char dat_00770f78[13] = "Win-Taste R.";
    static char dat_00770f88[13] = "Win-Taste L.";
    static char dat_00770f98[5] = "Entf";
    static char dat_00770fa0[6] = "Einfg";
    static char dat_00770fa8[8] = "Bild Ab";
    static char dat_00770fb0[12] = "Pfeil unten";
    static char dat_00770fbc[5] = "Ende";
    static char dat_00770fc4[13] = "Pfeil rechts";
    static char dat_00770fd4[12] = "Pfeil links";
    static char dat_00770fe0[9] = "Bild Auf";
    static char dat_00770fec[11] = "Pfeil oben";
    static char dat_00770ff8[5] = "Pos1";
    static char dat_00771000[6] = "lt R.";
    static char dat_00771008[6] = "S-Abf";
    static char dat_00771010[7] = "Num. ,";
    static char dat_00771018[8] = "Strg R.";
    static char dat_00771020[13] = "Num. Eingabe";
    static char dat_00771030[10] = "UNBENANNT";
    static char dat_0077103c[2] = "\366";
    static char dat_00771040[2] = "\344";
    static char dat_00771044[7] = "Rollen";
    static char dat_0077104c[10] = "Num-Taste";
    static char dat_00771058[15] = "Feststelltaste";
    static char dat_00771068[10] = "Leertaste";
    static char dat_00771074[7] = "Alt L.";
    static char dat_0077107c[10] = "Umsch. R.";
    static char dat_00771088[2] = ",";
    static char dat_0077108c[10] = "Umsch. L.";
    static char dat_00771098[3] = "\303\204";
    static char dat_0077109c[3] = "\303\226";
    static char dat_007710a0[8] = "Strg L.";
    static char dat_007710a8[8] = "Eingabe";
    static char dat_007710b0[3] = "\303\234";
    static char dat_007710b4[11] = "R\303\274cktaste";
    static char dat_007710c0[3] = "\302\264";
    static char dat_007710c4[3] = "\303\237";
    static char dat_007710c8[7] = "(LEER)";
}
char *dat_00822770[222] = {
    dat_007710c8, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007710c4, dat_007710c0, dat_007710b4, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774180, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_007710b0,
    dat_007712f4, dat_007710a8, dat_007710a0, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_0077109c, dat_00771098, dat_00771534, dat_0077108c, dat_00771cb0, dat_00774184,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771088, dat_0075eba8, dat_007712b4,
    dat_0077107c, dat_00771c6c, dat_00771074, dat_00771068, dat_00771058, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_0077104c, dat_00771044, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007715c8, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771040, dat_0077103c, "?",          dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771030, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771020, dat_00771018, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00771010,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771008, dat_00771000, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00770ff8, dat_00770fec, dat_00770fe0, dat_007516c0, dat_00770fd4, dat_007516c0, dat_00770fc4, dat_007516c0,
    dat_00770fbc, dat_00770fb0, dat_00770fa8, dat_00770fa0, dat_00770f98, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770f88, dat_00770f78, dat_00771a38};
extern "C" {
    static char dat_00770d30[9] = "Win Der.";
    static char dat_00770d3c[9] = "Win Izq.";
    static char dat_00770d48[5] = "Supr";
    static char dat_00770d50[8] = "Av P\303\241g";
    static char dat_00770d58[13] = "Flecha Abajo";
    static char dat_00770d68[12] = "Flecha Der.";
    static char dat_00770d74[12] = "Flecha Izq.";
    static char dat_00770d80[8] = "Re P\303\241g";
    static char dat_00770d88[11] = "Flecha Arr";
    static char dat_00770d94[7] = "Inicio";
    static char dat_00770d9c[9] = "Alt Der.";
    static char dat_00770da8[7] = "PETSIS";
    static char dat_00770db0[7] = "N\303\232M /";
    static char dat_00770db8[10] = "N\303\232M Coma";
    static char dat_00770dc4[10] = "INICIOWEB";
    static char dat_00770dd0[9] = "VOL ARRI";
    static char dat_00770ddc[9] = "VOL ABAJ";
    static char dat_00770de8[7] = "REPROD";
    static char dat_00770df0[9] = "SILENCIO";
    static char dat_00770dfc[10] = "Ctrl Der.";
    static char dat_00770e08[11] = "N\303\232M Intro";
    static char dat_00770e14[10] = "PISTA SIG";
    static char dat_00770e20[8] = "DETENER";
    static char dat_00770e28[9] = "SUBRAYAR";
    static char dat_00770e34[11] = "DOS PUNTOS";
    static char dat_00770e40[7] = "ARROBA";
    static char dat_00770e48[10] = "PISTA ANT";
    static char dat_00770e54[11] = "N\303\232M Igual";
    static char dat_00770e60[11] = "N\303\232M Punto";
    static char dat_00770e6c[7] = "N\303\232M 0";
    static char dat_00770e74[7] = "N\303\232M 3";
    static char dat_00770e7c[7] = "N\303\232M 2";
    static char dat_00770e84[7] = "N\303\232M 1";
    static char dat_00770e8c[7] = "N\303\232M +";
    static char dat_00770e94[7] = "N\303\232M 6";
    static char dat_00770e9c[7] = "N\303\232M 5";
    static char dat_00770ea4[7] = "N\303\232M 4";
    static char dat_00770eac[7] = "N\303\232M -";
    static char dat_00770eb4[7] = "N\303\232M 9";
    static char dat_00770ebc[7] = "N\303\232M 8";
    static char dat_00770ec4[7] = "N\303\232M 7";
    static char dat_00770ecc[11] = "Bloq Despl";
    static char dat_00770ed8[10] = "Bloq N\303\232M";
    static char dat_00770ee4[12] = "Bloq May\303\272s";
    static char dat_00770ef0[8] = "Espacio";
    static char dat_00770ef8[9] = "Alt Izq.";
    static char dat_00770f04[7] = "N\303\232M *";
    static char dat_00770f0c[11] = "Mayus Der.";
    static char dat_00770f18[6] = "Menos";
    static char dat_00770f20[5] = "Coma";
    static char dat_00770f28[3] = "\303\207";
    static char dat_00770f2c[12] = "May\303\272s Izq.";
    static char dat_00770f38[3] = "\302\272";
    static char dat_00770f3c[3] = "\303\221";
    static char dat_00770f40[10] = "Ctrl Izq.";
    static char dat_00770f4c[6] = "Intro";
    static char dat_00770f54[2] = "`";
    static char dat_00770f58[10] = "Retroceso";
    static char dat_00770f64[3] = "\302\241";
    static char dat_00770f68[2] = "'";
    static char dat_00770f6c[9] = "(VAC\303\215O)";
}
char *dat_00822ae8[222] = {
    dat_00770f6c, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00770f68, dat_00770f64, dat_00770f58, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770f54,
    dat_007712f4, dat_00770f4c, dat_00770f40, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770f3c, dat_007710c0, dat_00770f38, dat_00770f2c, dat_00770f28, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770f20, dat_007712b8, dat_00770f18,
    dat_00770f0c, dat_00770f04, dat_00770ef8, dat_00770ef0, dat_00770ee4, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00770ed8, dat_00770ecc, dat_00770ec4,
    dat_00770ebc, dat_00770eb4, dat_00770eac, dat_00770ea4, dat_00770e9c, dat_00770e94, dat_00770e8c, dat_00770e84, dat_00770e7c,
    dat_00770e74, dat_00770e6c, dat_00770e60, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00770e54, dat_007516c0, dat_007516c0,
    dat_00770e48, dat_00770e40, dat_00770e34, dat_00770e28, dat_00771b2c, dat_00770e20, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00770e14, dat_007516c0, dat_007516c0, dat_00770e08, dat_00770dfc, dat_007516c0, dat_007516c0, dat_00770df0, dat_00771ae4,
    dat_00770de8, dat_007516c0, dat_00770e20, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770ddc, dat_007516c0, dat_00770dd0, dat_007516c0, dat_00770dc4, dat_00770db8,
    dat_007516c0, dat_00770db0, dat_007516c0, dat_00770da8, dat_00770d9c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00770d94, dat_00770d88, dat_00770d80, dat_007516c0, dat_00770d74, dat_007516c0, dat_00770d68, dat_007516c0,
    dat_00771344, dat_00770d58, dat_00770d50, dat_00771a50, dat_00770d48, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770d3c, dat_00770d30, dat_00771a38};
extern "C" {
    static char dat_00770bcc[15] = "H. Windowsknap";
    static char dat_00770bdc[15] = "V. Windowsknap";
    static char dat_00770bec[8] = "Pil Ned";
    static char dat_00770bf4[11] = "Pil H\303\270jre";
    static char dat_00770c00[12] = "Pil Venstre";
    static char dat_00770c0c[7] = "Pil Op";
    static char dat_00770c14[7] = "H. Alt";
    static char dat_00770c1c[13] = "Print Screen";
    static char dat_00770c2c[11] = "Num. Komma";
    static char dat_00770c38[6] = "VOLOP";
    static char dat_00770c40[7] = "VOLNED";
    static char dat_00770c48[5] = "REGN";
    static char dat_00770c50[8] = "H. Ctrl";
    static char dat_00770c58[11] = "N\303\206STESPOR";
    static char dat_00770c64[15] = "UDEN ANGIVELSE";
    static char dat_00770c74[17] = "SP\303\230RGSM\303\205LSTEGN";
    static char dat_00770c88[9] = "STORT \303\206";
    static char dat_00770c94[15] = "ANF\303\230RSELSTEGN";
    static char dat_00770ca4[13] = "Num. Lig Med";
    static char dat_00770cb4[13] = "Num. Punktum";
    static char dat_00770cc4[15] = "Mellemrumstast";
    static char dat_00770cd4[7] = "V. Alt";
    static char dat_00770cdc[9] = "H. Shift";
    static char dat_00770ce8[8] = "Punktum";
    static char dat_00770cf0[6] = "Komma";
    static char dat_00770cf8[9] = "V. Shift";
    static char dat_00770d04[3] = "\302\275";
    static char dat_00770d08[3] = "\303\230";
    static char dat_00770d0c[3] = "\303\206";
    static char dat_00770d10[8] = "V. Ctrl";
    static char dat_00770d18[3] = "\302\250";
    static char dat_00770d1c[3] = "\303\205";
    static char dat_00770d20[7] = "Accent";
    static char dat_00770d28[6] = "(TOM)";
}
char *dat_00822e60[222] = {
    dat_00770d28, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, dat_00770d20, dat_00772c44, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770d0c, dat_00770d08, dat_00770d04, dat_00770cf8, dat_0077168c, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770cf0, dat_00770ce8, dat_00771d50,
    dat_00770cdc, dat_00771c6c, dat_00770cd4, dat_00770cc4, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770cb4, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00770ca4, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00770c94, dat_00770c88, dat_00770c74, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00770c64, dat_007516c0,
    dat_00770c58, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770c50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00770c48,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770c40, dat_007516c0, dat_00770c38, dat_007516c0, dat_00771ab8, dat_00770c2c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00770c1c, dat_00770c14, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770c0c, dat_00771758, dat_007516c0, dat_00770c00, dat_007516c0, dat_00770bf4, dat_007516c0,
    dat_00771a64, dat_00770bec, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770bdc, dat_00770bcc, dat_00771a38};
extern "C" {
    static char dat_00770b80[6] = "R.Win";
    static char dat_00770b88[7] = "Omlaag";
    static char dat_00770b90[7] = "Rechts";
    static char dat_00770b98[6] = "Links";
    static char dat_00770ba0[7] = "Omhoog";
    static char dat_00770ba8[10] = "Num. Punt";
    static char dat_00770bb4[7] = "Spatie";
    static char dat_00770bbc[5] = "Punt";
    static char dat_00770bc4[2] = "@";
    static char dat_00770bc8[3] = "\302\260";
}
char *dat_008231d8[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, "/",          dat_00770bc8, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d18,
    dat_0077150c, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_007712f4, dat_0077168c, dat_00770bc4, dat_00771cb4, dat_00771cb0, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770cf0, dat_00770bbc, dat_00771d50,
    dat_00771c74, dat_00771c6c, dat_00771c64, dat_00770bb4, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770ba8, dat_007516c0, dat_007516c0, dat_00753374, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00771af4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00770c2c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00771a94, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770ba0, dat_00771584, dat_007516c0, dat_00770b98, dat_007516c0, dat_00770b90, dat_007516c0,
    dat_00771a64, dat_00770b88, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_0077155c, dat_00770b80, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_00770b20[7] = "P. Win";
    static char dat_00770b28[6] = "D\303\263\305\200";
    static char dat_00770b30[6] = "Prawo";
    static char dat_00770b38[5] = "Lewo";
    static char dat_00770b40[6] = "G\303\263ra";
    static char dat_00770b48[7] = "P. Alt";
    static char dat_00770b50[8] = "P. Ctrl";
    static char dat_00770b58[7] = "Num. .";
    static char dat_00770b60[7] = "Spacja";
    static char dat_00770b68[9] = "P. Shift";
    static char dat_00770b74[6] = "GRAVE";
    static char dat_00770b7c[2] = ";";
}
char *dat_00823590[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712b4, "=",          dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
    dat_00753370, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770b7c, dat_00770f68, dat_00770b74, dat_00771cb4, dat_00771cb0, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771088, dat_0075eba8, "/",
    dat_00770b68, dat_00771c6c, dat_00771c64, dat_00770b60, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770b58, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770b50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00770b48, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770b40, dat_00771758, dat_007516c0, dat_00770b38, dat_007516c0, dat_00770b30, dat_007516c0,
    dat_00771a64, dat_00770b28, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a48, dat_00770b20, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_007708ac[13] = "\320\237\321\200\320\260\320\262.Win";
    static char dat_007708bc[11] = "\320\233\320\265\320\262.Win";
    static char dat_007708c8[13] = "\320\237\321\200\320\260\320\262.Alt";
    static char dat_007708d8[17] = "/ (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_007708ec[17] = ", (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770900[14] = "\320\237\321\200\320\260\320\262.Ctrl";
    static char dat_00770910[21] = "Enter (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770928[17] = "= (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_0077093c[17] = ". (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770950[17] = "0 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770964[17] = "3 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770978[17] = "2 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_0077098c[17] = "1 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_007709a0[17] = "+ (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_007709b4[17] = "6 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_007709c8[17] = "5 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_007709dc[17] = "4 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_007709f0[17] = "- (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770a04[17] = "9 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770a18[17] = "8 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770a2c[17] = "7 (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770a40[13] = "\320\237\321\200\320\276\320\261\320\265\320\273";
    static char dat_00770a50[11] = "\320\233\320\265\320\262.Alt";
    static char dat_00770a5c[17] = "* (\321\206\320\270\321\204.\320\272\320\273.)";
    static char dat_00770a70[15] = "\320\237\321\200\320\260\320\262.shift";
    static char dat_00770a80[3] = "\320\256";
    static char dat_00770a84[3] = "\320\221";
    static char dat_00770a88[3] = "\320\254";
    static char dat_00770a8c[3] = "\320\242";
    static char dat_00770a90[3] = "\320\230";
    static char dat_00770a94[3] = "\320\234";
    static char dat_00770a98[3] = "\320\241";
    static char dat_00770a9c[3] = "\320\247";
    static char dat_00770aa0[3] = "\320\257";
    static char dat_00770aa4[13] = "\320\233\320\265\320\262.Shift";
    static char dat_00770ab4[3] = "\320\255";
    static char dat_00770ab8[3] = "\320\226";
    static char dat_00770abc[3] = "\320\224";
    static char dat_00770ac0[3] = "\320\233";
    static char dat_00770ac4[3] = "\320\236";
    static char dat_00770ac8[3] = "\320\240";
    static char dat_00770acc[3] = "\320\237";
    static char dat_00770ad0[3] = "\320\220";
    static char dat_00770ad4[3] = "\320\222";
    static char dat_00770ad8[3] = "\320\253";
    static char dat_00770adc[3] = "\320\244";
    static char dat_00770ae0[12] = "\320\233\320\265\320\262.Ctrl";
    static char dat_00770aec[3] = "\320\252";
    static char dat_00770af0[3] = "\320\245";
    static char dat_00770af4[3] = "\320\227";
    static char dat_00770af8[3] = "\320\251";
    static char dat_00770afc[3] = "\320\250";
    static char dat_00770b00[3] = "\320\223";
    static char dat_00770b04[3] = "\320\235";
    static char dat_00770b08[3] = "\320\225";
    static char dat_00770b0c[3] = "\320\232";
    static char dat_00770b10[3] = "\320\243";
    static char dat_00770b14[3] = "\320\246";
    static char dat_00770b18[3] = "\320\231";
    static char dat_00770b1c[4] = "ESC";
}
char *dat_00823948[238] = {
    dat_00771d80, dat_00770b1c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712b4, "=",          dat_00771d3c, dat_00771d38, dat_00770b18, dat_00770b14,
    dat_00770b10, dat_00770b0c, dat_00770b08, dat_00770b04, dat_00770b00, dat_00770afc, dat_00770af8, dat_00770af4, dat_00770af0,
    dat_00770aec, dat_00771d0c, dat_00770ae0, dat_00770adc, dat_00770ad8, dat_00770ad4, dat_00770ad0, dat_00770acc, dat_00770ac8,
    dat_00770ac4, dat_00770ac0, dat_00770abc, dat_00770ab8, dat_00770ab4, dat_00770f54, dat_00770aa4, "\\",         dat_00770aa0,
    dat_00770a9c, dat_00770a98, dat_00770a94, dat_00770a90, dat_00770a8c, dat_00770a88, dat_00770a84, dat_00770a80, dat_0075eba8,
    dat_00770a70, dat_00770a5c, dat_00770a50, dat_00770a40, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00770a2c,
    dat_00770a18, dat_00770a04, dat_007709f0, dat_007709dc, dat_007709c8, dat_007709b4, dat_007709a0, dat_0077098c, dat_00770978,
    dat_00770964, dat_00770950, dat_0077093c, dat_007516c0, dat_007516c0, dat_00771530, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00770928, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00770910, dat_00770900, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_007708ec,
    dat_007516c0, dat_007708d8, dat_007516c0, dat_00771a9c, dat_007708c8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00772760, dat_00771758, dat_007516c0, dat_00772738, dat_007516c0, dat_00772750, dat_007516c0,
    dat_00771a64, dat_00772744, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007708bc, dat_007708ac, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_0077075c[9] = "Win Dto.";
    static char dat_00770768[9] = "Win Esq.";
    static char dat_00770774[16] = "Seta Para Baixo";
    static char dat_00770784[18] = "Seta Para Direita";
    static char dat_00770798[19] = "Seta Para Esquerda";
    static char dat_007707ac[15] = "Seta Para Cima";
    static char dat_007707bc[9] = "Alt Dto.";
    static char dat_007707c8[9] = "Ctr Dto.";
    static char dat_007707d4[11] = "Num. Ponto";
    static char dat_007707e0[18] = "Barra De Espa\303\247os";
    static char dat_007707f4[9] = "Alt Esq.";
    static char dat_00770800[11] = "Shift Dto.";
    static char dat_0077080c[21] = "Barra Para A Direita";
    static char dat_00770824[6] = "Ponto";
    static char dat_0077082c[9] = "V\303\255rgula";
    static char dat_00770838[17] = "Par\303\252nteses Dto.";
    static char dat_0077084c[11] = "Shift Esq.";
    static char dat_00770858[13] = "Acento Grave";
    static char dat_00770868[2] = "~";
    static char dat_0077086c[10] = "Ctrl Esq.";
    static char dat_00770878[17] = "Par\303\252nteses Esq.";
    static char dat_0077088c[11] = "Ap\303\263strofo";
    static char dat_00770898[11] = "Retroceder";
    static char dat_007708a4[8] = "Igual A";
}
char *dat_00823d00[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00770f18, dat_007708a4, dat_00770898, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_0077088c,
    dat_00770878, dat_00771d0c, dat_0077086c, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770f28, dat_00770868, dat_00770858, dat_0077084c, dat_00770838, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_0077082c, dat_00770824, dat_0077080c,
    dat_00770800, dat_00771c6c, dat_007707f4, dat_007707e0, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007707d4, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_007707c8, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_007707bc, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00771a84, dat_007707ac, dat_00771584, dat_007516c0, dat_00770798, dat_007516c0, dat_00770784, dat_007516c0,
    dat_00771a64, dat_00770774, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770768, dat_0077075c, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_00770748[8] = "NUM DIV";
    static char dat_00770750[10] = "NUM COMMA";
}
char *dat_008240b8[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00770f18, dat_007708a4, dat_00770898, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_0077088c,
    dat_00770878, dat_00771d0c, dat_0077086c, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770f28, dat_00770868, dat_00770858, dat_0077084c, dat_00770838, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_0077082c, dat_00770824, dat_0077080c,
    dat_00770800, dat_00771c6c, dat_007707f4, dat_007707e0, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007707d4, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_007707c8, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00770750,
    dat_007516c0, dat_00770748, dat_007516c0, dat_00771a9c, dat_007707bc, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00771a84, dat_007707ac, dat_00771584, dat_007516c0, dat_00770798, dat_007516c0, dat_00770784, dat_007516c0,
    dat_00771a64, dat_00770774, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770768, dat_0077075c, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_00770720[7] = "H. Win";
    static char dat_00770728[7] = "V. Win";
    static char dat_00770730[4] = "Ned";
    static char dat_00770734[7] = "H\303\270yre";
    static char dat_0077073c[8] = "Venstre";
    static char dat_00770744[3] = "UP";
}
char *dat_00824470[238] = {
    dat_00771d80, dat_00770b1c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, "\\",         dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770d08, dat_00770d0c, dat_00771530, dat_00770cf8, dat_00770f68, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770cf0, dat_00770ce8, dat_007712b4,
    dat_00770cdc, dat_00771c6c, dat_00770cd4, dat_00771c5c, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770cb4, dat_007516c0, dat_007516c0, "<",          dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770c50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00770c14, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770744, dat_00771584, dat_007516c0, dat_0077073c, dat_007516c0, dat_00770734, dat_007516c0,
    dat_00771a64, dat_00770730, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770728, dat_00770720, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_007517a8[3] = "\302\247";
    static char dat_007706bc[7] = "H\303\266ger";
    static char dat_007706c4[9] = "V\303\244nster";
    static char dat_007706d0[4] = "Upp";
    static char dat_007706d4[11] = "Print Scrn";
    static char dat_007706e0[17] = "Num. Kommatecken";
    static char dat_007706f4[11] = "Num. Punkt";
    static char dat_00770700[10] = "Blanksteg";
    static char dat_0077070c[6] = "Punkt";
    static char dat_00770714[12] = "Kommatecken";
}
char *dat_00824828[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, dat_007710c0, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_0077109c, dat_00771098, dat_007517a8, dat_00770cf8, dat_00770f68, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770714, dat_0077070c, dat_00771d50,
    dat_00770cdc, dat_00771c6c, dat_00770cd4, dat_00770700, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007706f4, dat_007516c0, dat_007516c0, "<",          dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770c50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_007706e0,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_007706d4, dat_00770c14, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_007706d0, dat_00771584, dat_007516c0, dat_007706c4, dat_007516c0, dat_007706bc, dat_007516c0,
    dat_00771a64, dat_00770730, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770728, dat_00770720, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
extern "C" {
    static char dat_00770634[7] = "O. Win";
    static char dat_0077063c[5] = "Alas";
    static char dat_00770644[6] = "Oikea";
    static char dat_0077064c[6] = "Vasen";
    static char dat_00770654[6] = "Yl\303\266s";
    static char dat_0077065c[7] = "O. Alt";
    static char dat_00770664[8] = "O. Ctrl";
    static char dat_0077066c[11] = "Num. Piste";
    static char dat_00770678[6] = "V\303\244li";
    static char dat_00770680[9] = "O.Vaihto";
    static char dat_0077068c[7] = "Miinus";
    static char dat_00770694[6] = "Piste";
    static char dat_0077069c[7] = "Pilkku";
    static char dat_007706a4[8] = "Sarkain";
    static char dat_007706ac[14] = "Askelpalautin";
}
char *dat_00824be0[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, dat_007710c0, dat_007706ac, dat_007706a4, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_0077109c, dat_00771098, dat_007517a8, dat_00770cf8, dat_00770f68, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_0077069c, dat_00770694, dat_0077068c,
    dat_00770680, dat_00771c6c, dat_00770cd4, dat_00770678, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_0077066c, dat_007516c0, dat_007516c0, "<",          dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770664, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_007706e0,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_007706d4, dat_0077065c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770654, dat_00771584, dat_007516c0, dat_0077064c, dat_007516c0, dat_00770644, dat_007516c0,
    dat_00771a64, dat_0077063c, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770728, dat_00770634, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
