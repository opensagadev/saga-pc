#include "saga.h"
extern "C" {
    i32 PCGraphicsFrameState = 1;
    i32 PCGraphicsBoundShader = 0;
    i32 PCGraphicsFrameScratchCount = 0;
    void(__cdecl *PCGraphicsDeviceLostCallback)(GRAPHICS_PRESENT_PARAMETERS_PC *) = NULL;
}

extern "C" {
    PC_WINDOW_OPTIONS_PC PcWindowOptions = {-1, {0}, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, {0}, 0,
                                            0,  0,   {0}, 0, 0, 0, {0}, 0, 0, 0, 1, 0, 0, 0, 1, {0}, 0};
    i32 PCOptionsFileVersion = 2;
    extern const char PCGraphicsGpuName00777bac[9] = "RIVA TNT";
    extern const char PCGraphicsGpuName00777b98[19] = "RIVA TNT2/TNT2 Pro";
    extern const char PCGraphicsGpuName00777b88[16] = "RIVA TNT2 Ultra";
    extern const char PCGraphicsGpuName00777b78[15] = "Vanta/Vanta LT";
    extern const char PCGraphicsGpuName00777b58[32] = "RIVA TNT2 Model 64/Model 64 Pro";
    extern const char PCGraphicsGpuName00777b48[13] = "Aladdin TNT2";
    extern const char PCGraphicsGpuName00777b3c[12] = "GeForce 256";
    extern const char PCGraphicsGpuName00777b30[12] = "GeForce DDR";
    extern const char PCGraphicsGpuName00777b1c[19] = "GeForce2 MX/MX 400";
    extern const char PCGraphicsGpuName00777b08[20] = "GeForce2 MX 100/200";
    extern const char PCGraphicsGpuName00777af0[24] = "GeForce2 Integrated GPU";
    extern const char PCGraphicsGpuName00777ad4[26] = "GeForce2 GTS/GeForce2 Pro";
    extern const char PCGraphicsGpuName00777ac8[12] = "GeForce2 Ti";
    extern const char PCGraphicsGpuName00777ab8[15] = "GeForce2 Ultra";
    extern const char PCGraphicsGpuName00777ab0[7] = "Quadro";
    extern const char PCGraphicsGpuName00777aa4[12] = "Quadro2 Pro";
    extern const char PCGraphicsGpuName00777a90[18] = "Quadro2 MXR/EX/Go";
    extern const char PCGraphicsGpuName00777a84[12] = "GeForce2 Go";
    extern const char PCGraphicsGpuName00777a74[16] = "GeForce4 MX 460";
    extern const char PCGraphicsGpuName00777a64[16] = "GeForce4 MX 440";
    extern const char PCGraphicsGpuName00777a54[16] = "GeForce4 MX 420";
    extern const char PCGraphicsGpuName00777a40[19] = "GeForce4 MX 440-SE";
    extern const char PCGraphicsGpuName00777a24[27] = "GeForce4 MX 440 with AGP8X";
    extern const char PCGraphicsGpuName00777a04[29] = "GeForce4 MX 440SE with AGP8X";
    extern const char PCGraphicsGpuName007779e8[27] = "GeForce4 MX 420 with AGP8X";
    extern const char PCGraphicsGpuName007779cc[27] = "GeForce4 MX Integrated GPU";
    extern const char PCGraphicsGpuName007779bc[16] = "Quadro4 380 XGL";
    extern const char PCGraphicsGpuName007779ac[16] = "Quadro4 550 XGL";
    extern const char PCGraphicsGpuName0077799c[16] = "Quadro4 580 XGL";
    extern const char PCGraphicsGpuName00777990[11] = "Quadro NVS";
    extern const char PCGraphicsGpuName00777978[22] = "Quadro NVS with AGP8X";
    extern const char PCGraphicsGpuName00777964[18] = "Quadro NVS 50 PCI";
    extern const char PCGraphicsGpuName0077794c[22] = "NVIDIA Quadro NVS 285";
    extern const char PCGraphicsGpuName00777940[9] = "GeForce3";
    extern const char PCGraphicsGpuName00777930[16] = "GeForce3 Ti 200";
    extern const char PCGraphicsGpuName00777920[16] = "GeForce3 Ti 500";
    extern const char PCGraphicsGpuName00777914[11] = "Quadro DCC";
    extern const char PCGraphicsGpuName0077790c[5] = "NV25";
    extern const char PCGraphicsGpuName007778f8[17] = "GeForce4 Ti 4200";
    extern const char PCGraphicsGpuName007778e4[17] = "GeForce4 Ti 4800";
    extern const char PCGraphicsGpuName007778c8[28] = "GeForce4 Ti 4200 with AGP8X";
    extern const char PCGraphicsGpuName007778b4[20] = "GeForce4 Ti 4800 SE";
    extern const char PCGraphicsGpuName007778a0[17] = "GeForce4 Ti 4600";
    extern const char PCGraphicsGpuName0077788c[17] = "GeForce4 Ti 4400";
    extern const char PCGraphicsGpuName0077787c[16] = "Quadro4 900 XGL";
    extern const char PCGraphicsGpuName0077786c[16] = "Quadro4 750 XGL";
    extern const char PCGraphicsGpuName0077785c[16] = "Quadro4 700 XGL";
    extern const char PCGraphicsGpuName0077784c[16] = "Quadro4 980 XGL";
    extern const char PCGraphicsGpuName0077783c[16] = "Quadro4 780 XGL";
    extern const char PCGraphicsGpuName00777824[22] = "GeForce FX 5800 Ultra";
    extern const char PCGraphicsGpuName00777814[16] = "GeForce FX 5800";
    extern const char PCGraphicsGpuName00777804[15] = "Quadro FX 2000";
    extern const char PCGraphicsGpuName007777f4[15] = "Quadro FX 1000";
    extern const char PCGraphicsGpuName007777e8[12] = "ICE FX 2000";
    extern const char PCGraphicsGpuName007777d0[22] = "GeForce FX 5600 Ultra";
    extern const char PCGraphicsGpuName007777c0[16] = "GeForce FX 5600";
    extern const char PCGraphicsGpuName007777b8[5] = "NV31";
    extern const char PCGraphicsGpuName007777a4[18] = "GeForce FX 5600SE";
    extern const char PCGraphicsGpuName0077778c[22] = "GeForce FX 5200 Ultra";
    extern const char PCGraphicsGpuName0077777c[16] = "GeForce FX 5200";
    extern const char PCGraphicsGpuName00777768[18] = "GeForce FX 5200LE";
    extern const char PCGraphicsGpuName00777750[22] = "GeForce FX 5700 Ultra";
    extern const char PCGraphicsGpuName00777740[16] = "GeForce FX 5700";
    extern const char PCGraphicsGpuName0077772c[18] = "GeForce FX 5700LE";
    extern const char PCGraphicsGpuName00777718[18] = "GeForce FX 5700VE";
    extern const char PCGraphicsGpuName00777710[5] = "NV36";
    extern const char PCGraphicsGpuName007776fc[18] = "GeForce FX Go5700";
    extern const char PCGraphicsGpuName007776f0[10] = "NV36M Pro";
    extern const char PCGraphicsGpuName007776e8[8] = "NV36MAP";
    extern const char PCGraphicsGpuName007776e0[7] = "NV36GL";
    extern const char PCGraphicsGpuName007776d8[7] = "NV34GL";
    extern const char PCGraphicsGpuName007776c0[22] = "GeForce FX 5900 Ultra";
    extern const char PCGraphicsGpuName007776b0[16] = "GeForce FX 5900";
    extern const char PCGraphicsGpuName0077769c[18] = "GeForce FX 5900XT";
    extern const char PCGraphicsGpuName00777684[22] = "GeForce FX 5950 Ultra";
    extern const char PCGraphicsGpuName00777670[18] = "GeForce FX 5900ZT";
    extern const char PCGraphicsGpuName00777660[14] = "Quadro FX 500";
    extern const char PCGraphicsGpuName00777650[14] = "Quadro FX 700";
    extern const char PCGraphicsGpuName00777648[8] = "NV34MAP";
    extern const char PCGraphicsGpuName00777638[15] = "Quadro FX 3000";
    extern const char PCGraphicsGpuName00777628[16] = "GeForce4 440 Go";
    extern const char PCGraphicsGpuName00777618[16] = "GeForce4 420 Go";
    extern const char PCGraphicsGpuName00777604[20] = "GeForce4 420 Go 32M";
    extern const char PCGraphicsGpuName007775f4[16] = "GeForce4 460 Go";
    extern const char PCGraphicsGpuName007775e0[20] = "GeForce4 440 Go 64M";
    extern const char PCGraphicsGpuName007775cc[20] = "GeForce4 410 Go 16M";
    extern const char PCGraphicsGpuName007775b8[17] = "GeForce4 MX 4000";
    extern const char PCGraphicsGpuName007775a8[16] = "GeForce4 448 Go";
    extern const char PCGraphicsGpuName00777598[16] = "GeForce4 488 Go";
    extern const char PCGraphicsGpuName00777584[17] = "GeForce4 4200 Go";
    extern const char PCGraphicsGpuName00777570[17] = "Quadro4 500 GoGL";
    extern const char PCGraphicsGpuName0077755c[17] = "Quadro4 700 GoGL";
    extern const char PCGraphicsGpuName00777548[18] = "GeForce FX Go5600";
    extern const char PCGraphicsGpuName00777534[18] = "GeForce FX Go5650";
    extern const char PCGraphicsGpuName00777520[18] = "GeForce FX Go5200";
    extern const char PCGraphicsGpuName0077750c[18] = "GeForce FX Go5250";
    extern const char PCGraphicsGpuName007774fc[16] = "GeForce FX 5500";
    extern const char PCGraphicsGpuName007774ec[16] = "GeForce FX 5100";
    extern const char PCGraphicsGpuName007774d0[26] = "GeForce FX Go5200 32M/64M";
    extern const char PCGraphicsGpuName007774bc[18] = "GeForce FX Go53xx";
    extern const char PCGraphicsGpuName007774a8[18] = "GeForce FX Go5100";
    extern const char PCGraphicsGpuName00777498[16] = "Quadro FX Go700";
    extern const char PCGraphicsGpuName00777484[17] = "Quadro FX Go1000";
    extern const char PCGraphicsGpuName00777474[15] = "Quadro FX 1100";
    extern const char PCGraphicsGpuName0077746c[6] = "NV31M";
    extern const char PCGraphicsGpuName00777460[10] = "NV31M Pro";
    extern const char PCGraphicsGpuName00777458[8] = "NV31GLM";
    extern const char PCGraphicsGpuName0077744c[12] = "NV31GLM Pro";
    extern const char PCGraphicsGpuName00777434[21] = "NVIDIA Quadro FX 540";
    extern const char PCGraphicsGpuName0077741c[22] = "NVIDIA Quadro FX 4000";
    extern const char PCGraphicsGpuName00777404[24] = "NVIDIA Quadro FX Go1400";
    extern const char PCGraphicsGpuName007773ec[22] = "NVIDIA Quadro FX 1400";
    extern const char PCGraphicsGpuName007773cc[31] = "NVIDIA Quadro FX 3450/4000 SDI";
    extern const char PCGraphicsGpuName007773b0[27] = "NVIDIA Quadro FX 3400/4400";
    extern const char PCGraphicsGpuName00777398[24] = "NVIDIA GeForce PCX 5750";
    extern const char PCGraphicsGpuName00777380[24] = "NVIDIA GeForce PCX 5900";
    extern const char PCGraphicsGpuName00777368[24] = "NVIDIA GeForce PCX 5300";
    extern const char PCGraphicsGpuName0077734c[27] = "NVIDIA Quadro PCI-E Series";
    extern const char PCGraphicsGpuName00777334[22] = "NVIDIA Quadro FX 1300";
    extern const char PCGraphicsGpuName0077731c[24] = "NVIDIA GeForce PCX 4300";
    extern const char PCGraphicsGpuName00777300[26] = "NVIDIA GeForce 6800 Ultra";
    extern const char PCGraphicsGpuName007772ec[20] = "NVIDIA GeForce 6800";
    extern const char PCGraphicsGpuName007772d4[23] = "NVIDIA GeForce 6800 LE";
    extern const char PCGraphicsGpuName007772bc[23] = "NVIDIA GeForce 6800 GT";
    extern const char PCGraphicsGpuName007772b0[12] = "NVIDIA NV40";
    extern const char PCGraphicsGpuName007772a0[14] = "NVIDIA NV40GL";
    extern const char PCGraphicsGpuName00777288[21] = "NVIDIA GeForce 6800 ";
    extern const char PCGraphicsGpuName00777270[23] = "NVIDIA GeForce 6600 GT";
    extern const char PCGraphicsGpuName0077725c[20] = "NVIDIA GeForce 6600";
    extern const char PCGraphicsGpuName00777244[23] = "NVIDIA GeForce 6600 LE";
    extern const char PCGraphicsGpuName00777238[12] = "NVIDIA NV43";
    extern const char PCGraphicsGpuName00777220[23] = "NVIDIA GeForce 6610 XL";
    extern const char PCGraphicsGpuName00777210[14] = "NVIDIA NV43GL";
    extern const char PCGraphicsGpuName007771fc[20] = "NVIDIA GeForce 6200";
    extern const char PCGraphicsGpuName007771e4[24] = "NVIDIA GeForce 6600 GT ";
    extern const char PCGraphicsGpuName007771cc[21] = "NVIDIA GeForce 6600 ";
    extern const char PCGraphicsGpuName007771b4[21] = "NVIDIA GeForce 6200 ";
    extern const char PCGraphicsGpuName00777194[31] = "NVIDIA GeForce 6800 Series GPU";
    extern const char PCGraphicsGpuName00777174[29] = "NVIDIA GeForce Go 6800 Ultra";
    extern const char PCGraphicsGpuName0077715c[23] = "NVIDIA GeForce Go 6800";
    extern const char PCGraphicsGpuName00777144[23] = "NVIDIA GeForce Go 6600";
    extern const char PCGraphicsGpuName00777128[26] = "NVIDIA GeForce Go 6600 TE";
    extern const char PCGraphicsGpuName0077710c[26] = "NVIDIA GeForce Go 6200 TE";
    extern const char PCGraphicsGpuName007770f4[23] = "NVIDIA GeForce Go 6200";
    extern const char PCGraphicsGpuName007770d0[35] = "NVIDIA GeForce 6200 TurboCache(TM)";
    extern const char PCGraphicsGpuName007770c8[5] = "NV44";
    extern const char PCGraphicsGpuName007770b0[23] = "NVIDIA GeForce Go 6290";
    extern const char PCGraphicsGpuName00777098[23] = "NVIDIA GeForce Go 6400";
    extern const char PCGraphicsGpuName00777088[13] = "GeForce 6200";
    extern const char PCGraphicsGpuName00777080[6] = "NV44M";
    extern const char PCGraphicsGpuName00777078[8] = "NV44GLM";
    extern const char PCGraphicsGpuName00777070[7] = "NV44GL";
    extern const char PCGraphicsGpuName00777060[13] = "NVIDIA Other";
    extern const char PCGraphicsGpuName00777054[11] = "NVIDIA NEW";
    PC_GRAPHICS_GPU_RECORD_PC PCGraphicsGpuRecords[268] = {
        {32u, 0u, PCGraphicsGpuName00777bac, PCGraphicsGpuName00777bac},
        {40u, 0u, PCGraphicsGpuName00777b98, PCGraphicsGpuName00777b98},
        {41u, 0u, PCGraphicsGpuName00777b88, PCGraphicsGpuName00777b88},
        {44u, 0u, PCGraphicsGpuName00777b78, PCGraphicsGpuName00777b78},
        {45u, 0u, PCGraphicsGpuName00777b58, PCGraphicsGpuName00777b58},
        {160u, 0u, PCGraphicsGpuName00777b48, PCGraphicsGpuName00777b48},
        {256u, 0u, PCGraphicsGpuName00777b3c, PCGraphicsGpuName00777b3c},
        {257u, 0u, PCGraphicsGpuName00777b30, PCGraphicsGpuName00777b30},
        {272u, 0u, PCGraphicsGpuName00777b1c, PCGraphicsGpuName00777b1c},
        {273u, 0u, PCGraphicsGpuName00777b08, PCGraphicsGpuName00777b08},
        {416u, 0u, PCGraphicsGpuName00777af0, PCGraphicsGpuName00777af0},
        {336u, 0u, PCGraphicsGpuName00777ad4, PCGraphicsGpuName00777ad4},
        {337u, 0u, PCGraphicsGpuName00777ac8, PCGraphicsGpuName00777ac8},
        {338u, 0u, PCGraphicsGpuName00777ab8, PCGraphicsGpuName00777ab8},
        {259u, 0u, PCGraphicsGpuName00777ab0, PCGraphicsGpuName00777ab0},
        {339u, 0u, PCGraphicsGpuName00777aa4, PCGraphicsGpuName00777aa4},
        {275u, 0u, PCGraphicsGpuName00777a90, PCGraphicsGpuName00777a90},
        {274u, 0u, PCGraphicsGpuName00777a84, PCGraphicsGpuName00777a84},
        {368u, 0u, PCGraphicsGpuName00777a74, PCGraphicsGpuName00777a74},
        {369u, 0u, PCGraphicsGpuName00777a64, PCGraphicsGpuName00777a64},
        {370u, 0u, PCGraphicsGpuName00777a54, PCGraphicsGpuName00777a54},
        {371u, 0u, PCGraphicsGpuName00777a40, PCGraphicsGpuName00777a40},
        {385u, 0u, PCGraphicsGpuName00777a24, PCGraphicsGpuName00777a24},
        {386u, 0u, PCGraphicsGpuName00777a04, PCGraphicsGpuName00777a04},
        {387u, 0u, PCGraphicsGpuName007779e8, PCGraphicsGpuName007779e8},
        {496u, 0u, PCGraphicsGpuName007779cc, PCGraphicsGpuName007779cc},
        {395u, 0u, PCGraphicsGpuName007779bc, PCGraphicsGpuName007779bc},
        {376u, 0u, PCGraphicsGpuName007779ac, PCGraphicsGpuName007779ac},
        {392u, 0u, PCGraphicsGpuName0077799c, PCGraphicsGpuName0077799c},
        {378u, 0u, PCGraphicsGpuName00777990, PCGraphicsGpuName00777990},
        {394u, 0u, PCGraphicsGpuName00777978, PCGraphicsGpuName00777978},
        {396u, 0u, PCGraphicsGpuName00777964, PCGraphicsGpuName00777964},
        {357u, 0u, PCGraphicsGpuName0077794c, PCGraphicsGpuName0077794c},
        {512u, 0u, PCGraphicsGpuName00777940, PCGraphicsGpuName00777940},
        {513u, 0u, PCGraphicsGpuName00777930, PCGraphicsGpuName00777930},
        {514u, 0u, PCGraphicsGpuName00777920, PCGraphicsGpuName00777920},
        {515u, 0u, PCGraphicsGpuName00777914, PCGraphicsGpuName00777914},
        {594u, 0u, PCGraphicsGpuName0077790c, PCGraphicsGpuName0077790c},
        {595u, 0u, PCGraphicsGpuName007778f8, PCGraphicsGpuName007778f8},
        {640u, 0u, PCGraphicsGpuName007778e4, PCGraphicsGpuName007778e4},
        {641u, 0u, PCGraphicsGpuName007778c8, PCGraphicsGpuName007778c8},
        {642u, 0u, PCGraphicsGpuName007778b4, PCGraphicsGpuName007778b4},
        {592u, 0u, PCGraphicsGpuName007778a0, PCGraphicsGpuName007778a0},
        {593u, 0u, PCGraphicsGpuName0077788c, PCGraphicsGpuName0077788c},
        {600u, 0u, PCGraphicsGpuName0077787c, PCGraphicsGpuName0077787c},
        {601u, 0u, PCGraphicsGpuName0077786c, PCGraphicsGpuName0077786c},
        {603u, 0u, PCGraphicsGpuName0077785c, PCGraphicsGpuName0077785c},
        {648u, 0u, PCGraphicsGpuName0077784c, PCGraphicsGpuName0077784c},
        {649u, 0u, PCGraphicsGpuName0077783c, PCGraphicsGpuName0077783c},
        {769u, 3u, PCGraphicsGpuName00777824, PCGraphicsGpuName00777824},
        {770u, 3u, PCGraphicsGpuName00777814, PCGraphicsGpuName00777814},
        {776u, 3u, PCGraphicsGpuName00777804, PCGraphicsGpuName00777804},
        {777u, 3u, PCGraphicsGpuName007777f4, PCGraphicsGpuName007777f4},
        {778u, 3u, PCGraphicsGpuName007777e8, PCGraphicsGpuName007777e8},
        {785u, 3u, PCGraphicsGpuName007777d0, PCGraphicsGpuName007777d0},
        {786u, 1u, PCGraphicsGpuName007777c0, PCGraphicsGpuName007777c0},
        {787u, 1u, PCGraphicsGpuName007777b8, PCGraphicsGpuName007777b8},
        {788u, 1u, PCGraphicsGpuName007777a4, PCGraphicsGpuName007777a4},
        {801u, 1u, PCGraphicsGpuName0077778c, PCGraphicsGpuName0077778c},
        {802u, 1u, PCGraphicsGpuName0077777c, PCGraphicsGpuName0077777c},
        {803u, 1u, PCGraphicsGpuName00777768, PCGraphicsGpuName00777768},
        {833u, 3u, PCGraphicsGpuName00777750, PCGraphicsGpuName00777750},
        {834u, 3u, PCGraphicsGpuName00777740, PCGraphicsGpuName00777740},
        {835u, 1u, PCGraphicsGpuName0077772c, PCGraphicsGpuName0077772c},
        {836u, 1u, PCGraphicsGpuName00777718, PCGraphicsGpuName00777718},
        {837u, 3u, PCGraphicsGpuName00777710, PCGraphicsGpuName00777710},
        {839u, 3u, PCGraphicsGpuName007776fc, PCGraphicsGpuName007776fc},
        {840u, 3u, PCGraphicsGpuName007776fc, PCGraphicsGpuName007776fc},
        {841u, 3u, PCGraphicsGpuName007776f0, PCGraphicsGpuName007776f0},
        {843u, 3u, PCGraphicsGpuName007776e8, PCGraphicsGpuName007776e8},
        {847u, 3u, PCGraphicsGpuName007776e0, PCGraphicsGpuName007776e0},
        {810u, 3u, PCGraphicsGpuName007776d8, PCGraphicsGpuName007776d8},
        {815u, 3u, PCGraphicsGpuName007776d8, PCGraphicsGpuName007776d8},
        {816u, 3u, PCGraphicsGpuName007776c0, PCGraphicsGpuName007776c0},
        {817u, 3u, PCGraphicsGpuName007776b0, PCGraphicsGpuName007776b0},
        {818u, 3u, PCGraphicsGpuName0077769c, PCGraphicsGpuName0077769c},
        {819u, 3u, PCGraphicsGpuName00777684, PCGraphicsGpuName00777684},
        {818u, 3u, PCGraphicsGpuName00777670, PCGraphicsGpuName00777670},
        {811u, 3u, PCGraphicsGpuName00777660, PCGraphicsGpuName00777660},
        {815u, 3u, PCGraphicsGpuName00777650, PCGraphicsGpuName00777650},
        {809u, 3u, PCGraphicsGpuName00777648, PCGraphicsGpuName00777648},
        {824u, 3u, PCGraphicsGpuName00777638, PCGraphicsGpuName00777638},
        {372u, 0u, PCGraphicsGpuName00777628, PCGraphicsGpuName00777628},
        {373u, 0u, PCGraphicsGpuName00777618, PCGraphicsGpuName00777618},
        {374u, 0u, PCGraphicsGpuName00777604, PCGraphicsGpuName00777604},
        {375u, 0u, PCGraphicsGpuName007775f4, PCGraphicsGpuName007775f4},
        {377u, 0u, PCGraphicsGpuName007775e0, PCGraphicsGpuName007775e0},
        {381u, 0u, PCGraphicsGpuName007775cc, PCGraphicsGpuName007775cc},
        {389u, 0u, PCGraphicsGpuName007775b8, PCGraphicsGpuName007775b8},
        {390u, 0u, PCGraphicsGpuName007775a8, PCGraphicsGpuName007775a8},
        {391u, 0u, PCGraphicsGpuName00777598, PCGraphicsGpuName00777598},
        {397u, 0u, PCGraphicsGpuName007775a8, PCGraphicsGpuName007775a8},
        {646u, 0u, PCGraphicsGpuName00777584, PCGraphicsGpuName00777584},
        {380u, 0u, PCGraphicsGpuName00777570, PCGraphicsGpuName00777570},
        {652u, 0u, PCGraphicsGpuName0077755c, PCGraphicsGpuName0077755c},
        {794u, 1u, PCGraphicsGpuName00777548, PCGraphicsGpuName00777548},
        {795u, 1u, PCGraphicsGpuName00777534, PCGraphicsGpuName00777534},
        {804u, 1u, PCGraphicsGpuName00777520, PCGraphicsGpuName00777520},
        {805u, 1u, PCGraphicsGpuName0077750c, PCGraphicsGpuName0077750c},
        {806u, 1u, PCGraphicsGpuName007774fc, PCGraphicsGpuName007774fc},
        {807u, 1u, PCGraphicsGpuName007774ec, PCGraphicsGpuName007774ec},
        {808u, 1u, PCGraphicsGpuName007774d0, PCGraphicsGpuName007774d0},
        {812u, 1u, PCGraphicsGpuName007774bc, PCGraphicsGpuName007774bc},
        {813u, 1u, PCGraphicsGpuName007774a8, PCGraphicsGpuName007774a8},
        {796u, 1u, PCGraphicsGpuName00777498, PCGraphicsGpuName00777498},
        {844u, 1u, PCGraphicsGpuName00777484, PCGraphicsGpuName00777484},
        {846u, 1u, PCGraphicsGpuName00777474, PCGraphicsGpuName00777474},
        {790u, 3u, PCGraphicsGpuName0077746c, PCGraphicsGpuName0077746c},
        {791u, 3u, PCGraphicsGpuName00777460, PCGraphicsGpuName00777460},
        {797u, 3u, PCGraphicsGpuName00777458, PCGraphicsGpuName00777458},
        {798u, 3u, PCGraphicsGpuName0077744c, PCGraphicsGpuName0077744c},
        {799u, 3u, PCGraphicsGpuName0077744c, PCGraphicsGpuName0077744c},
        {334u, 1u, PCGraphicsGpuName00777434, PCGraphicsGpuName00777434},
        {78u, 3u, PCGraphicsGpuName0077741c, PCGraphicsGpuName0077741c},
        {204u, 3u, PCGraphicsGpuName00777404, PCGraphicsGpuName00777404},
        {205u, 3u, PCGraphicsGpuName007773ec, PCGraphicsGpuName007773ec},
        {206u, 3u, PCGraphicsGpuName007773cc, PCGraphicsGpuName007773cc},
        {248u, 3u, PCGraphicsGpuName007773b0, PCGraphicsGpuName007773b0},
        {250u, 3u, PCGraphicsGpuName00777398, PCGraphicsGpuName00777398},
        {251u, 3u, PCGraphicsGpuName00777380, PCGraphicsGpuName00777380},
        {252u, 1u, PCGraphicsGpuName00777368, PCGraphicsGpuName00777368},
        {253u, 3u, PCGraphicsGpuName0077734c, PCGraphicsGpuName0077734c},
        {254u, 1u, PCGraphicsGpuName00777334, PCGraphicsGpuName00777334},
        {255u, 3u, PCGraphicsGpuName0077731c, PCGraphicsGpuName0077731c},
        {240u, 67u, PCGraphicsGpuName00777300, PCGraphicsGpuName00777300},
        {64u, 67u, PCGraphicsGpuName00777300, PCGraphicsGpuName00777300},
        {528u, 67u, PCGraphicsGpuName007772ec, PCGraphicsGpuName007772ec},
        {529u, 67u, PCGraphicsGpuName007772ec, PCGraphicsGpuName007772ec},
        {530u, 67u, PCGraphicsGpuName007772d4, PCGraphicsGpuName007772d4},
        {533u, 67u, PCGraphicsGpuName007772bc, PCGraphicsGpuName007772bc},
        {65u, 67u, PCGraphicsGpuName007772ec, PCGraphicsGpuName007772ec},
        {66u, 67u, PCGraphicsGpuName007772d4, PCGraphicsGpuName007772d4},
        {67u, 67u, PCGraphicsGpuName007772b0, PCGraphicsGpuName007772d4},
        {69u, 67u, PCGraphicsGpuName007772bc, PCGraphicsGpuName007772bc},
        {73u, 67u, PCGraphicsGpuName007772a0, PCGraphicsGpuName007772d4},
        {192u, 67u, PCGraphicsGpuName00777288, PCGraphicsGpuName00777288},
        {193u, 67u, PCGraphicsGpuName00777288, PCGraphicsGpuName00777288},
        {194u, 67u, PCGraphicsGpuName007772d4, PCGraphicsGpuName007772d4},
        {320u, 67u, PCGraphicsGpuName00777270, PCGraphicsGpuName00777270},
        {321u, 67u, PCGraphicsGpuName0077725c, PCGraphicsGpuName0077725c},
        {322u, 67u, PCGraphicsGpuName00777244, PCGraphicsGpuName00777244},
        {323u, 67u, PCGraphicsGpuName00777238, PCGraphicsGpuName00777238},
        {325u, 67u, PCGraphicsGpuName00777220, PCGraphicsGpuName00777220},
        {331u, 67u, PCGraphicsGpuName00777238, PCGraphicsGpuName00777238},
        {332u, 67u, PCGraphicsGpuName00777210, PCGraphicsGpuName00777210},
        {333u, 67u, PCGraphicsGpuName00777210, PCGraphicsGpuName00777210},
        {335u, 67u, PCGraphicsGpuName007771fc, PCGraphicsGpuName007771fc},
        {241u, 67u, PCGraphicsGpuName007771e4, PCGraphicsGpuName007771e4},
        {242u, 67u, PCGraphicsGpuName007771cc, PCGraphicsGpuName007771cc},
        {242u, 67u, PCGraphicsGpuName007771b4, PCGraphicsGpuName007771cc},
        {249u, 67u, PCGraphicsGpuName00777194, PCGraphicsGpuName00777194},
        {201u, 67u, PCGraphicsGpuName00777174, PCGraphicsGpuName00777174},
        {200u, 67u, PCGraphicsGpuName0077715c, PCGraphicsGpuName0077715c},
        {324u, 67u, PCGraphicsGpuName00777144, PCGraphicsGpuName00777144},
        {326u, 67u, PCGraphicsGpuName00777128, PCGraphicsGpuName00777128},
        {327u, 67u, PCGraphicsGpuName0077710c, PCGraphicsGpuName0077710c},
        {328u, 67u, PCGraphicsGpuName00777144, PCGraphicsGpuName00777144},
        {329u, 67u, PCGraphicsGpuName0077710c, PCGraphicsGpuName0077710c},
        {352u, 67u, PCGraphicsGpuName007770f4, PCGraphicsGpuName007770f4},
        {353u, 67u, PCGraphicsGpuName007770d0, PCGraphicsGpuName007770d0},
        {354u, 67u, PCGraphicsGpuName007770d0, PCGraphicsGpuName007770d0},
        {355u, 67u, PCGraphicsGpuName007770c8, PCGraphicsGpuName007770c8},
        {356u, 67u, PCGraphicsGpuName007770b0, PCGraphicsGpuName007770f4},
        {358u, 67u, PCGraphicsGpuName00777098, PCGraphicsGpuName00777098},
        {359u, 67u, PCGraphicsGpuName007770f4, PCGraphicsGpuName007770f4},
        {360u, 67u, PCGraphicsGpuName00777098, PCGraphicsGpuName00777098},
        {544u, 67u, PCGraphicsGpuName007770c8, PCGraphicsGpuName007770c8},
        {545u, 67u, PCGraphicsGpuName00777088, PCGraphicsGpuName00777088},
        {546u, 67u, PCGraphicsGpuName007770c8, PCGraphicsGpuName007770c8},
        {552u, 67u, PCGraphicsGpuName00777080, PCGraphicsGpuName00777080},
        {361u, 67u, PCGraphicsGpuName00777080, PCGraphicsGpuName00777080},
        {362u, 67u, PCGraphicsGpuName00777078, PCGraphicsGpuName00777078},
        {363u, 67u, PCGraphicsGpuName00777078, PCGraphicsGpuName00777078},
        {364u, 67u, PCGraphicsGpuName00777078, PCGraphicsGpuName00777078},
        {365u, 67u, PCGraphicsGpuName00777078, PCGraphicsGpuName00777078},
        {366u, 67u, PCGraphicsGpuName00777070, PCGraphicsGpuName00777070},
        {367u, 67u, PCGraphicsGpuName00777078, PCGraphicsGpuName00777078},
        {248u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {248u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {249u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {249u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {288u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {289u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {290u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {291u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {292u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {293u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {294u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {295u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {296u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {297u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {298u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {299u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {300u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {301u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {302u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {303u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {144u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {145u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {146u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {147u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {148u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {149u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {150u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {151u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {152u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {153u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {156u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {157u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {158u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {159u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {400u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {401u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {402u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {403u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {404u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {413u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {414u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {415u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {656u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {657u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {658u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {659u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {660u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {669u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {670u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {671u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {912u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {913u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {914u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {915u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {916u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {925u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {926u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {927u, 67u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {464u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {465u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {466u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {467u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {468u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {469u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {470u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {471u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {472u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {473u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {474u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {475u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {476u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {477u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {478u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {479u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {576u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {577u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {578u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {579u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {580u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {581u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {582u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {583u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {584u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {585u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {586u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {587u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {588u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {589u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {590u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {591u, 3u, PCGraphicsGpuName00777060, PCGraphicsGpuName00777060},
        {4294967295u, 67u, PCGraphicsGpuName00777054, PCGraphicsGpuName00777054},
    };
    PC_GRAPHICS_GPU_RECORD_PC PCGraphicsGpuFallback = {4294967295u, 67u, PCGraphicsGpuName00777054,
                                                       PCGraphicsGpuName00777054};
    PC_GRAPHICS_QUERIED_GPU_PC PCGraphicsQueriedGpu = {0xffffffffu, 0xffffffffu};
    i32 PCGraphicsGpuSelectionEnabled = 0;
    PC_GRAPHICS_GPU_RECORD_PC *PCGraphicsSelectedGpuRecord = NULL;
}

extern const GUID PcWbemClassId = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern const GUID PcWbemInterfaceId = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern const GUID PcSystemKeyboardId = {0x6f1d2b61, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcSystemMouseId = {0x6f1d2b60, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784dfc = {
    0xa36d02f2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e0c = {
    0x55728220, 0xd33c, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e2c = {
    0xa36d02e4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e3c = {
    0xa36d02e3, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e4c = {
    0xa36d02f5, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e5c = {
    0xa36d02f4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e6c = {
    0xa36d02e2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e7c = {
    0xa36d02e1, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcInputObjectGuid784e8c = {
    0xa36d02e0, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern const GUID PcDirectInput8AId = {0xbf798030, 0x483a, 0x4da2, {0xaa, 0x99, 0x5d, 0x64, 0xed, 0x36, 0x97, 0x00}};
static const PC_DI_OBJECT_FORMAT_PC PcKeyboardDataFormatObjects[256] = {
    {&PcInputObjectGuid784e0c, 0x0, 0x8000000c, 0x0},  {&PcInputObjectGuid784e0c, 0x1, 0x8000010c, 0x0},
    {&PcInputObjectGuid784e0c, 0x2, 0x8000020c, 0x0},  {&PcInputObjectGuid784e0c, 0x3, 0x8000030c, 0x0},
    {&PcInputObjectGuid784e0c, 0x4, 0x8000040c, 0x0},  {&PcInputObjectGuid784e0c, 0x5, 0x8000050c, 0x0},
    {&PcInputObjectGuid784e0c, 0x6, 0x8000060c, 0x0},  {&PcInputObjectGuid784e0c, 0x7, 0x8000070c, 0x0},
    {&PcInputObjectGuid784e0c, 0x8, 0x8000080c, 0x0},  {&PcInputObjectGuid784e0c, 0x9, 0x8000090c, 0x0},
    {&PcInputObjectGuid784e0c, 0xa, 0x80000a0c, 0x0},  {&PcInputObjectGuid784e0c, 0xb, 0x80000b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xc, 0x80000c0c, 0x0},  {&PcInputObjectGuid784e0c, 0xd, 0x80000d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xe, 0x80000e0c, 0x0},  {&PcInputObjectGuid784e0c, 0xf, 0x80000f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x10, 0x8000100c, 0x0}, {&PcInputObjectGuid784e0c, 0x11, 0x8000110c, 0x0},
    {&PcInputObjectGuid784e0c, 0x12, 0x8000120c, 0x0}, {&PcInputObjectGuid784e0c, 0x13, 0x8000130c, 0x0},
    {&PcInputObjectGuid784e0c, 0x14, 0x8000140c, 0x0}, {&PcInputObjectGuid784e0c, 0x15, 0x8000150c, 0x0},
    {&PcInputObjectGuid784e0c, 0x16, 0x8000160c, 0x0}, {&PcInputObjectGuid784e0c, 0x17, 0x8000170c, 0x0},
    {&PcInputObjectGuid784e0c, 0x18, 0x8000180c, 0x0}, {&PcInputObjectGuid784e0c, 0x19, 0x8000190c, 0x0},
    {&PcInputObjectGuid784e0c, 0x1a, 0x80001a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x1b, 0x80001b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x1c, 0x80001c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x1d, 0x80001d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x1e, 0x80001e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x1f, 0x80001f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x20, 0x8000200c, 0x0}, {&PcInputObjectGuid784e0c, 0x21, 0x8000210c, 0x0},
    {&PcInputObjectGuid784e0c, 0x22, 0x8000220c, 0x0}, {&PcInputObjectGuid784e0c, 0x23, 0x8000230c, 0x0},
    {&PcInputObjectGuid784e0c, 0x24, 0x8000240c, 0x0}, {&PcInputObjectGuid784e0c, 0x25, 0x8000250c, 0x0},
    {&PcInputObjectGuid784e0c, 0x26, 0x8000260c, 0x0}, {&PcInputObjectGuid784e0c, 0x27, 0x8000270c, 0x0},
    {&PcInputObjectGuid784e0c, 0x28, 0x8000280c, 0x0}, {&PcInputObjectGuid784e0c, 0x29, 0x8000290c, 0x0},
    {&PcInputObjectGuid784e0c, 0x2a, 0x80002a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x2b, 0x80002b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x2c, 0x80002c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x2d, 0x80002d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x2e, 0x80002e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x2f, 0x80002f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x30, 0x8000300c, 0x0}, {&PcInputObjectGuid784e0c, 0x31, 0x8000310c, 0x0},
    {&PcInputObjectGuid784e0c, 0x32, 0x8000320c, 0x0}, {&PcInputObjectGuid784e0c, 0x33, 0x8000330c, 0x0},
    {&PcInputObjectGuid784e0c, 0x34, 0x8000340c, 0x0}, {&PcInputObjectGuid784e0c, 0x35, 0x8000350c, 0x0},
    {&PcInputObjectGuid784e0c, 0x36, 0x8000360c, 0x0}, {&PcInputObjectGuid784e0c, 0x37, 0x8000370c, 0x0},
    {&PcInputObjectGuid784e0c, 0x38, 0x8000380c, 0x0}, {&PcInputObjectGuid784e0c, 0x39, 0x8000390c, 0x0},
    {&PcInputObjectGuid784e0c, 0x3a, 0x80003a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x3b, 0x80003b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x3c, 0x80003c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x3d, 0x80003d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x3e, 0x80003e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x3f, 0x80003f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x40, 0x8000400c, 0x0}, {&PcInputObjectGuid784e0c, 0x41, 0x8000410c, 0x0},
    {&PcInputObjectGuid784e0c, 0x42, 0x8000420c, 0x0}, {&PcInputObjectGuid784e0c, 0x43, 0x8000430c, 0x0},
    {&PcInputObjectGuid784e0c, 0x44, 0x8000440c, 0x0}, {&PcInputObjectGuid784e0c, 0x45, 0x8000450c, 0x0},
    {&PcInputObjectGuid784e0c, 0x46, 0x8000460c, 0x0}, {&PcInputObjectGuid784e0c, 0x47, 0x8000470c, 0x0},
    {&PcInputObjectGuid784e0c, 0x48, 0x8000480c, 0x0}, {&PcInputObjectGuid784e0c, 0x49, 0x8000490c, 0x0},
    {&PcInputObjectGuid784e0c, 0x4a, 0x80004a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x4b, 0x80004b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x4c, 0x80004c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x4d, 0x80004d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x4e, 0x80004e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x4f, 0x80004f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x50, 0x8000500c, 0x0}, {&PcInputObjectGuid784e0c, 0x51, 0x8000510c, 0x0},
    {&PcInputObjectGuid784e0c, 0x52, 0x8000520c, 0x0}, {&PcInputObjectGuid784e0c, 0x53, 0x8000530c, 0x0},
    {&PcInputObjectGuid784e0c, 0x54, 0x8000540c, 0x0}, {&PcInputObjectGuid784e0c, 0x55, 0x8000550c, 0x0},
    {&PcInputObjectGuid784e0c, 0x56, 0x8000560c, 0x0}, {&PcInputObjectGuid784e0c, 0x57, 0x8000570c, 0x0},
    {&PcInputObjectGuid784e0c, 0x58, 0x8000580c, 0x0}, {&PcInputObjectGuid784e0c, 0x59, 0x8000590c, 0x0},
    {&PcInputObjectGuid784e0c, 0x5a, 0x80005a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x5b, 0x80005b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x5c, 0x80005c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x5d, 0x80005d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x5e, 0x80005e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x5f, 0x80005f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x60, 0x8000600c, 0x0}, {&PcInputObjectGuid784e0c, 0x61, 0x8000610c, 0x0},
    {&PcInputObjectGuid784e0c, 0x62, 0x8000620c, 0x0}, {&PcInputObjectGuid784e0c, 0x63, 0x8000630c, 0x0},
    {&PcInputObjectGuid784e0c, 0x64, 0x8000640c, 0x0}, {&PcInputObjectGuid784e0c, 0x65, 0x8000650c, 0x0},
    {&PcInputObjectGuid784e0c, 0x66, 0x8000660c, 0x0}, {&PcInputObjectGuid784e0c, 0x67, 0x8000670c, 0x0},
    {&PcInputObjectGuid784e0c, 0x68, 0x8000680c, 0x0}, {&PcInputObjectGuid784e0c, 0x69, 0x8000690c, 0x0},
    {&PcInputObjectGuid784e0c, 0x6a, 0x80006a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x6b, 0x80006b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x6c, 0x80006c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x6d, 0x80006d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x6e, 0x80006e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x6f, 0x80006f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x70, 0x8000700c, 0x0}, {&PcInputObjectGuid784e0c, 0x71, 0x8000710c, 0x0},
    {&PcInputObjectGuid784e0c, 0x72, 0x8000720c, 0x0}, {&PcInputObjectGuid784e0c, 0x73, 0x8000730c, 0x0},
    {&PcInputObjectGuid784e0c, 0x74, 0x8000740c, 0x0}, {&PcInputObjectGuid784e0c, 0x75, 0x8000750c, 0x0},
    {&PcInputObjectGuid784e0c, 0x76, 0x8000760c, 0x0}, {&PcInputObjectGuid784e0c, 0x77, 0x8000770c, 0x0},
    {&PcInputObjectGuid784e0c, 0x78, 0x8000780c, 0x0}, {&PcInputObjectGuid784e0c, 0x79, 0x8000790c, 0x0},
    {&PcInputObjectGuid784e0c, 0x7a, 0x80007a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x7b, 0x80007b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x7c, 0x80007c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x7d, 0x80007d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x7e, 0x80007e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x7f, 0x80007f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x80, 0x8000800c, 0x0}, {&PcInputObjectGuid784e0c, 0x81, 0x8000810c, 0x0},
    {&PcInputObjectGuid784e0c, 0x82, 0x8000820c, 0x0}, {&PcInputObjectGuid784e0c, 0x83, 0x8000830c, 0x0},
    {&PcInputObjectGuid784e0c, 0x84, 0x8000840c, 0x0}, {&PcInputObjectGuid784e0c, 0x85, 0x8000850c, 0x0},
    {&PcInputObjectGuid784e0c, 0x86, 0x8000860c, 0x0}, {&PcInputObjectGuid784e0c, 0x87, 0x8000870c, 0x0},
    {&PcInputObjectGuid784e0c, 0x88, 0x8000880c, 0x0}, {&PcInputObjectGuid784e0c, 0x89, 0x8000890c, 0x0},
    {&PcInputObjectGuid784e0c, 0x8a, 0x80008a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x8b, 0x80008b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x8c, 0x80008c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x8d, 0x80008d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x8e, 0x80008e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x8f, 0x80008f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x90, 0x8000900c, 0x0}, {&PcInputObjectGuid784e0c, 0x91, 0x8000910c, 0x0},
    {&PcInputObjectGuid784e0c, 0x92, 0x8000920c, 0x0}, {&PcInputObjectGuid784e0c, 0x93, 0x8000930c, 0x0},
    {&PcInputObjectGuid784e0c, 0x94, 0x8000940c, 0x0}, {&PcInputObjectGuid784e0c, 0x95, 0x8000950c, 0x0},
    {&PcInputObjectGuid784e0c, 0x96, 0x8000960c, 0x0}, {&PcInputObjectGuid784e0c, 0x97, 0x8000970c, 0x0},
    {&PcInputObjectGuid784e0c, 0x98, 0x8000980c, 0x0}, {&PcInputObjectGuid784e0c, 0x99, 0x8000990c, 0x0},
    {&PcInputObjectGuid784e0c, 0x9a, 0x80009a0c, 0x0}, {&PcInputObjectGuid784e0c, 0x9b, 0x80009b0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x9c, 0x80009c0c, 0x0}, {&PcInputObjectGuid784e0c, 0x9d, 0x80009d0c, 0x0},
    {&PcInputObjectGuid784e0c, 0x9e, 0x80009e0c, 0x0}, {&PcInputObjectGuid784e0c, 0x9f, 0x80009f0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xa0, 0x8000a00c, 0x0}, {&PcInputObjectGuid784e0c, 0xa1, 0x8000a10c, 0x0},
    {&PcInputObjectGuid784e0c, 0xa2, 0x8000a20c, 0x0}, {&PcInputObjectGuid784e0c, 0xa3, 0x8000a30c, 0x0},
    {&PcInputObjectGuid784e0c, 0xa4, 0x8000a40c, 0x0}, {&PcInputObjectGuid784e0c, 0xa5, 0x8000a50c, 0x0},
    {&PcInputObjectGuid784e0c, 0xa6, 0x8000a60c, 0x0}, {&PcInputObjectGuid784e0c, 0xa7, 0x8000a70c, 0x0},
    {&PcInputObjectGuid784e0c, 0xa8, 0x8000a80c, 0x0}, {&PcInputObjectGuid784e0c, 0xa9, 0x8000a90c, 0x0},
    {&PcInputObjectGuid784e0c, 0xaa, 0x8000aa0c, 0x0}, {&PcInputObjectGuid784e0c, 0xab, 0x8000ab0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xac, 0x8000ac0c, 0x0}, {&PcInputObjectGuid784e0c, 0xad, 0x8000ad0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xae, 0x8000ae0c, 0x0}, {&PcInputObjectGuid784e0c, 0xaf, 0x8000af0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xb0, 0x8000b00c, 0x0}, {&PcInputObjectGuid784e0c, 0xb1, 0x8000b10c, 0x0},
    {&PcInputObjectGuid784e0c, 0xb2, 0x8000b20c, 0x0}, {&PcInputObjectGuid784e0c, 0xb3, 0x8000b30c, 0x0},
    {&PcInputObjectGuid784e0c, 0xb4, 0x8000b40c, 0x0}, {&PcInputObjectGuid784e0c, 0xb5, 0x8000b50c, 0x0},
    {&PcInputObjectGuid784e0c, 0xb6, 0x8000b60c, 0x0}, {&PcInputObjectGuid784e0c, 0xb7, 0x8000b70c, 0x0},
    {&PcInputObjectGuid784e0c, 0xb8, 0x8000b80c, 0x0}, {&PcInputObjectGuid784e0c, 0xb9, 0x8000b90c, 0x0},
    {&PcInputObjectGuid784e0c, 0xba, 0x8000ba0c, 0x0}, {&PcInputObjectGuid784e0c, 0xbb, 0x8000bb0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xbc, 0x8000bc0c, 0x0}, {&PcInputObjectGuid784e0c, 0xbd, 0x8000bd0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xbe, 0x8000be0c, 0x0}, {&PcInputObjectGuid784e0c, 0xbf, 0x8000bf0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xc0, 0x8000c00c, 0x0}, {&PcInputObjectGuid784e0c, 0xc1, 0x8000c10c, 0x0},
    {&PcInputObjectGuid784e0c, 0xc2, 0x8000c20c, 0x0}, {&PcInputObjectGuid784e0c, 0xc3, 0x8000c30c, 0x0},
    {&PcInputObjectGuid784e0c, 0xc4, 0x8000c40c, 0x0}, {&PcInputObjectGuid784e0c, 0xc5, 0x8000c50c, 0x0},
    {&PcInputObjectGuid784e0c, 0xc6, 0x8000c60c, 0x0}, {&PcInputObjectGuid784e0c, 0xc7, 0x8000c70c, 0x0},
    {&PcInputObjectGuid784e0c, 0xc8, 0x8000c80c, 0x0}, {&PcInputObjectGuid784e0c, 0xc9, 0x8000c90c, 0x0},
    {&PcInputObjectGuid784e0c, 0xca, 0x8000ca0c, 0x0}, {&PcInputObjectGuid784e0c, 0xcb, 0x8000cb0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xcc, 0x8000cc0c, 0x0}, {&PcInputObjectGuid784e0c, 0xcd, 0x8000cd0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xce, 0x8000ce0c, 0x0}, {&PcInputObjectGuid784e0c, 0xcf, 0x8000cf0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xd0, 0x8000d00c, 0x0}, {&PcInputObjectGuid784e0c, 0xd1, 0x8000d10c, 0x0},
    {&PcInputObjectGuid784e0c, 0xd2, 0x8000d20c, 0x0}, {&PcInputObjectGuid784e0c, 0xd3, 0x8000d30c, 0x0},
    {&PcInputObjectGuid784e0c, 0xd4, 0x8000d40c, 0x0}, {&PcInputObjectGuid784e0c, 0xd5, 0x8000d50c, 0x0},
    {&PcInputObjectGuid784e0c, 0xd6, 0x8000d60c, 0x0}, {&PcInputObjectGuid784e0c, 0xd7, 0x8000d70c, 0x0},
    {&PcInputObjectGuid784e0c, 0xd8, 0x8000d80c, 0x0}, {&PcInputObjectGuid784e0c, 0xd9, 0x8000d90c, 0x0},
    {&PcInputObjectGuid784e0c, 0xda, 0x8000da0c, 0x0}, {&PcInputObjectGuid784e0c, 0xdb, 0x8000db0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xdc, 0x8000dc0c, 0x0}, {&PcInputObjectGuid784e0c, 0xdd, 0x8000dd0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xde, 0x8000de0c, 0x0}, {&PcInputObjectGuid784e0c, 0xdf, 0x8000df0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xe0, 0x8000e00c, 0x0}, {&PcInputObjectGuid784e0c, 0xe1, 0x8000e10c, 0x0},
    {&PcInputObjectGuid784e0c, 0xe2, 0x8000e20c, 0x0}, {&PcInputObjectGuid784e0c, 0xe3, 0x8000e30c, 0x0},
    {&PcInputObjectGuid784e0c, 0xe4, 0x8000e40c, 0x0}, {&PcInputObjectGuid784e0c, 0xe5, 0x8000e50c, 0x0},
    {&PcInputObjectGuid784e0c, 0xe6, 0x8000e60c, 0x0}, {&PcInputObjectGuid784e0c, 0xe7, 0x8000e70c, 0x0},
    {&PcInputObjectGuid784e0c, 0xe8, 0x8000e80c, 0x0}, {&PcInputObjectGuid784e0c, 0xe9, 0x8000e90c, 0x0},
    {&PcInputObjectGuid784e0c, 0xea, 0x8000ea0c, 0x0}, {&PcInputObjectGuid784e0c, 0xeb, 0x8000eb0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xec, 0x8000ec0c, 0x0}, {&PcInputObjectGuid784e0c, 0xed, 0x8000ed0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xee, 0x8000ee0c, 0x0}, {&PcInputObjectGuid784e0c, 0xef, 0x8000ef0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xf0, 0x8000f00c, 0x0}, {&PcInputObjectGuid784e0c, 0xf1, 0x8000f10c, 0x0},
    {&PcInputObjectGuid784e0c, 0xf2, 0x8000f20c, 0x0}, {&PcInputObjectGuid784e0c, 0xf3, 0x8000f30c, 0x0},
    {&PcInputObjectGuid784e0c, 0xf4, 0x8000f40c, 0x0}, {&PcInputObjectGuid784e0c, 0xf5, 0x8000f50c, 0x0},
    {&PcInputObjectGuid784e0c, 0xf6, 0x8000f60c, 0x0}, {&PcInputObjectGuid784e0c, 0xf7, 0x8000f70c, 0x0},
    {&PcInputObjectGuid784e0c, 0xf8, 0x8000f80c, 0x0}, {&PcInputObjectGuid784e0c, 0xf9, 0x8000f90c, 0x0},
    {&PcInputObjectGuid784e0c, 0xfa, 0x8000fa0c, 0x0}, {&PcInputObjectGuid784e0c, 0xfb, 0x8000fb0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xfc, 0x8000fc0c, 0x0}, {&PcInputObjectGuid784e0c, 0xfd, 0x8000fd0c, 0x0},
    {&PcInputObjectGuid784e0c, 0xfe, 0x8000fe0c, 0x0}, {&PcInputObjectGuid784e0c, 0xff, 0x8000ff0c, 0x0},
};
extern const PC_DI_DATA_FORMAT_PC PcKeyboardDataFormat = {24, 16, 2, 256, 256, PcKeyboardDataFormatObjects};
static const PC_DI_OBJECT_FORMAT_PC PcJoystickDataFormatObjects[164] = {
    {&PcInputObjectGuid784e8c, 0x0, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784e7c, 0x4, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784e6c, 0x8, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784e5c, 0xc, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784e4c, 0x10, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784e3c, 0x14, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784e2c, 0x18, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784e2c, 0x1c, 0x80ffff03, 0x100},
    {&PcInputObjectGuid784dfc, 0x20, 0x80ffff10, 0x0},
    {&PcInputObjectGuid784dfc, 0x24, 0x80ffff10, 0x0},
    {&PcInputObjectGuid784dfc, 0x28, 0x80ffff10, 0x0},
    {&PcInputObjectGuid784dfc, 0x2c, 0x80ffff10, 0x0},
    {NULL, 0x30, 0x80ffff0c, 0x0},
    {NULL, 0x31, 0x80ffff0c, 0x0},
    {NULL, 0x32, 0x80ffff0c, 0x0},
    {NULL, 0x33, 0x80ffff0c, 0x0},
    {NULL, 0x34, 0x80ffff0c, 0x0},
    {NULL, 0x35, 0x80ffff0c, 0x0},
    {NULL, 0x36, 0x80ffff0c, 0x0},
    {NULL, 0x37, 0x80ffff0c, 0x0},
    {NULL, 0x38, 0x80ffff0c, 0x0},
    {NULL, 0x39, 0x80ffff0c, 0x0},
    {NULL, 0x3a, 0x80ffff0c, 0x0},
    {NULL, 0x3b, 0x80ffff0c, 0x0},
    {NULL, 0x3c, 0x80ffff0c, 0x0},
    {NULL, 0x3d, 0x80ffff0c, 0x0},
    {NULL, 0x3e, 0x80ffff0c, 0x0},
    {NULL, 0x3f, 0x80ffff0c, 0x0},
    {NULL, 0x40, 0x80ffff0c, 0x0},
    {NULL, 0x41, 0x80ffff0c, 0x0},
    {NULL, 0x42, 0x80ffff0c, 0x0},
    {NULL, 0x43, 0x80ffff0c, 0x0},
    {NULL, 0x44, 0x80ffff0c, 0x0},
    {NULL, 0x45, 0x80ffff0c, 0x0},
    {NULL, 0x46, 0x80ffff0c, 0x0},
    {NULL, 0x47, 0x80ffff0c, 0x0},
    {NULL, 0x48, 0x80ffff0c, 0x0},
    {NULL, 0x49, 0x80ffff0c, 0x0},
    {NULL, 0x4a, 0x80ffff0c, 0x0},
    {NULL, 0x4b, 0x80ffff0c, 0x0},
    {NULL, 0x4c, 0x80ffff0c, 0x0},
    {NULL, 0x4d, 0x80ffff0c, 0x0},
    {NULL, 0x4e, 0x80ffff0c, 0x0},
    {NULL, 0x4f, 0x80ffff0c, 0x0},
    {NULL, 0x50, 0x80ffff0c, 0x0},
    {NULL, 0x51, 0x80ffff0c, 0x0},
    {NULL, 0x52, 0x80ffff0c, 0x0},
    {NULL, 0x53, 0x80ffff0c, 0x0},
    {NULL, 0x54, 0x80ffff0c, 0x0},
    {NULL, 0x55, 0x80ffff0c, 0x0},
    {NULL, 0x56, 0x80ffff0c, 0x0},
    {NULL, 0x57, 0x80ffff0c, 0x0},
    {NULL, 0x58, 0x80ffff0c, 0x0},
    {NULL, 0x59, 0x80ffff0c, 0x0},
    {NULL, 0x5a, 0x80ffff0c, 0x0},
    {NULL, 0x5b, 0x80ffff0c, 0x0},
    {NULL, 0x5c, 0x80ffff0c, 0x0},
    {NULL, 0x5d, 0x80ffff0c, 0x0},
    {NULL, 0x5e, 0x80ffff0c, 0x0},
    {NULL, 0x5f, 0x80ffff0c, 0x0},
    {NULL, 0x60, 0x80ffff0c, 0x0},
    {NULL, 0x61, 0x80ffff0c, 0x0},
    {NULL, 0x62, 0x80ffff0c, 0x0},
    {NULL, 0x63, 0x80ffff0c, 0x0},
    {NULL, 0x64, 0x80ffff0c, 0x0},
    {NULL, 0x65, 0x80ffff0c, 0x0},
    {NULL, 0x66, 0x80ffff0c, 0x0},
    {NULL, 0x67, 0x80ffff0c, 0x0},
    {NULL, 0x68, 0x80ffff0c, 0x0},
    {NULL, 0x69, 0x80ffff0c, 0x0},
    {NULL, 0x6a, 0x80ffff0c, 0x0},
    {NULL, 0x6b, 0x80ffff0c, 0x0},
    {NULL, 0x6c, 0x80ffff0c, 0x0},
    {NULL, 0x6d, 0x80ffff0c, 0x0},
    {NULL, 0x6e, 0x80ffff0c, 0x0},
    {NULL, 0x6f, 0x80ffff0c, 0x0},
    {NULL, 0x70, 0x80ffff0c, 0x0},
    {NULL, 0x71, 0x80ffff0c, 0x0},
    {NULL, 0x72, 0x80ffff0c, 0x0},
    {NULL, 0x73, 0x80ffff0c, 0x0},
    {NULL, 0x74, 0x80ffff0c, 0x0},
    {NULL, 0x75, 0x80ffff0c, 0x0},
    {NULL, 0x76, 0x80ffff0c, 0x0},
    {NULL, 0x77, 0x80ffff0c, 0x0},
    {NULL, 0x78, 0x80ffff0c, 0x0},
    {NULL, 0x79, 0x80ffff0c, 0x0},
    {NULL, 0x7a, 0x80ffff0c, 0x0},
    {NULL, 0x7b, 0x80ffff0c, 0x0},
    {NULL, 0x7c, 0x80ffff0c, 0x0},
    {NULL, 0x7d, 0x80ffff0c, 0x0},
    {NULL, 0x7e, 0x80ffff0c, 0x0},
    {NULL, 0x7f, 0x80ffff0c, 0x0},
    {NULL, 0x80, 0x80ffff0c, 0x0},
    {NULL, 0x81, 0x80ffff0c, 0x0},
    {NULL, 0x82, 0x80ffff0c, 0x0},
    {NULL, 0x83, 0x80ffff0c, 0x0},
    {NULL, 0x84, 0x80ffff0c, 0x0},
    {NULL, 0x85, 0x80ffff0c, 0x0},
    {NULL, 0x86, 0x80ffff0c, 0x0},
    {NULL, 0x87, 0x80ffff0c, 0x0},
    {NULL, 0x88, 0x80ffff0c, 0x0},
    {NULL, 0x89, 0x80ffff0c, 0x0},
    {NULL, 0x8a, 0x80ffff0c, 0x0},
    {NULL, 0x8b, 0x80ffff0c, 0x0},
    {NULL, 0x8c, 0x80ffff0c, 0x0},
    {NULL, 0x8d, 0x80ffff0c, 0x0},
    {NULL, 0x8e, 0x80ffff0c, 0x0},
    {NULL, 0x8f, 0x80ffff0c, 0x0},
    {NULL, 0x90, 0x80ffff0c, 0x0},
    {NULL, 0x91, 0x80ffff0c, 0x0},
    {NULL, 0x92, 0x80ffff0c, 0x0},
    {NULL, 0x93, 0x80ffff0c, 0x0},
    {NULL, 0x94, 0x80ffff0c, 0x0},
    {NULL, 0x95, 0x80ffff0c, 0x0},
    {NULL, 0x96, 0x80ffff0c, 0x0},
    {NULL, 0x97, 0x80ffff0c, 0x0},
    {NULL, 0x98, 0x80ffff0c, 0x0},
    {NULL, 0x99, 0x80ffff0c, 0x0},
    {NULL, 0x9a, 0x80ffff0c, 0x0},
    {NULL, 0x9b, 0x80ffff0c, 0x0},
    {NULL, 0x9c, 0x80ffff0c, 0x0},
    {NULL, 0x9d, 0x80ffff0c, 0x0},
    {NULL, 0x9e, 0x80ffff0c, 0x0},
    {NULL, 0x9f, 0x80ffff0c, 0x0},
    {NULL, 0xa0, 0x80ffff0c, 0x0},
    {NULL, 0xa1, 0x80ffff0c, 0x0},
    {NULL, 0xa2, 0x80ffff0c, 0x0},
    {NULL, 0xa3, 0x80ffff0c, 0x0},
    {NULL, 0xa4, 0x80ffff0c, 0x0},
    {NULL, 0xa5, 0x80ffff0c, 0x0},
    {NULL, 0xa6, 0x80ffff0c, 0x0},
    {NULL, 0xa7, 0x80ffff0c, 0x0},
    {NULL, 0xa8, 0x80ffff0c, 0x0},
    {NULL, 0xa9, 0x80ffff0c, 0x0},
    {NULL, 0xaa, 0x80ffff0c, 0x0},
    {NULL, 0xab, 0x80ffff0c, 0x0},
    {NULL, 0xac, 0x80ffff0c, 0x0},
    {NULL, 0xad, 0x80ffff0c, 0x0},
    {NULL, 0xae, 0x80ffff0c, 0x0},
    {NULL, 0xaf, 0x80ffff0c, 0x0},
    {&PcInputObjectGuid784e8c, 0xb0, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e7c, 0xb4, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e6c, 0xb8, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e5c, 0xbc, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e4c, 0xc0, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e3c, 0xc4, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e2c, 0x18, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e2c, 0x1c, 0x80ffff03, 0x200},
    {&PcInputObjectGuid784e8c, 0xd0, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e7c, 0xd4, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e6c, 0xd8, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e5c, 0xdc, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e4c, 0xe0, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e3c, 0xe4, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e2c, 0x18, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e2c, 0x1c, 0x80ffff03, 0x300},
    {&PcInputObjectGuid784e8c, 0xf0, 0x80ffff03, 0x400},
    {&PcInputObjectGuid784e7c, 0xf4, 0x80ffff03, 0x400},
    {&PcInputObjectGuid784e6c, 0xf8, 0x80ffff03, 0x400},
    {&PcInputObjectGuid784e5c, 0xfc, 0x80ffff03, 0x400},
    {&PcInputObjectGuid784e4c, 0x100, 0x80ffff03, 0x400},
    {&PcInputObjectGuid784e3c, 0x104, 0x80ffff03, 0x400},
    {&PcInputObjectGuid784e2c, 0x18, 0x80ffff03, 0x400},
    {&PcInputObjectGuid784e2c, 0x1c, 0x80ffff03, 0x400},
};
extern const PC_DI_DATA_FORMAT_PC PcJoystickDataFormat = {24, 16, 1, 272, 164, PcJoystickDataFormatObjects};
static const PC_DI_OBJECT_FORMAT_PC PcMouseDataFormatObjects[11] = {
    {&PcInputObjectGuid784e8c, 0x0, 0xffff03, 0x0},
    {&PcInputObjectGuid784e7c, 0x4, 0xffff03, 0x0},
    {&PcInputObjectGuid784e6c, 0x8, 0x80ffff03, 0x0},
    {NULL, 0xc, 0xffff0c, 0x0},
    {NULL, 0xd, 0xffff0c, 0x0},
    {NULL, 0xe, 0x80ffff0c, 0x0},
    {NULL, 0xf, 0x80ffff0c, 0x0},
    {NULL, 0x10, 0x80ffff0c, 0x0},
    {NULL, 0x11, 0x80ffff0c, 0x0},
    {NULL, 0x12, 0x80ffff0c, 0x0},
    {NULL, 0x13, 0x80ffff0c, 0x0},
};
extern const PC_DI_DATA_FORMAT_PC PcMouseDataFormat = {24, 16, 2, 20, 11, PcMouseDataFormatObjects};
PC_INPUT_CONFIG_PC PcStaticInputConfigs[15] = {
    {"USB Joystick     ",
     0x30e8f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"XBOX 360 For Windows (Controller)",
     0x28e045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
     {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Controller (Xbox 360 Wireless Receiver for Windows)",
     0x719045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
     {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"PS Vibration Feedback Converter ",
     0x50c12,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Rockfire PSX\\USB Bridge",
     0x20500583,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logic3 12 button vibration",
     0x31207b5,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Interact Gaming Device",
     0x262a05fd,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x01},
      {0x01, 0x04},
      {0x01, 0x03},
      {0x01, 0x00},
      {0x01, 0x0a},
      {0x01, 0x02},
      {0x01, 0x05},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Keyboard",
     0x1,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x2,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x07, 0x10},
      {0x07, 0x12},
      {0x07, 0x02},
      {0x07, 0x04},
      {0x07, 0x48},
      {0x07, 0x4d},
      {0x07, 0x50},
      {0x07, 0x4b},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0x14},
      {0x07, 0x23},
      {0x07, 0x22},
      {0x07, 0x21}}},
    {"Keyboard",
     0x4,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"PLAYSTATION(R)3 Controller",
     0x268054c,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x02, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x00, 0x05, 0x05}, {0x00, 0x02, 0x02}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07}}},
    {"Saitek P2900 Wireless Pad",
     0x40c06a3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0e, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"4 axis 16 button joystick",
     0x6676666,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x10, 0x00, 0x00, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f}}},
    {"MP-8888 USB Joypad",
     0x88880925,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
     {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
};
i32 PcInputSelectedPort = -1;
i32 PcXInputDeviceCount = 0;
PC_DI_CAPS_PC PcKeyboardCapabilities = {};

extern "C" {
    i32 KNOBS = -1;
    NUGSCN *area_scene = NULL;
    NUGSCN *vehicle_scene = NULL;
    NUGSCN *big_icon_scene = NULL;
}

#include "saga.h"

extern "C" char TextInputKeyNameTextPc7516c0[];
extern "C" char TextInputKeyNameTextPc753370[];
extern "C" char TextInputKeyNameTextPc753374[];
extern "C" const char Text_UnknownText[];
extern "C" char TextInputKeyNameTextPc759770[];
extern "C" const char WorldPathSeparator[];
extern "C" char TextInputKeyNameTextPc75ac34[];
extern "C" const char PermLegalSeparator[];
extern "C" char UiBackPromptLiteralPc[];
extern "C" char TextInputKeyNameTextPc76fde8[];
extern "C" char TextInputKeyNameTextPc7719bc[];
extern "C" char TextInputKeyNameTextPc7719c8[];
extern "C" char TextInputKeyNameTextPc7719d0[];
extern "C" char TextInputKeyNameTextPc7719dc[];
extern "C" char TextInputKeyNameTextPc7719e4[];
extern "C" char TextInputKeyNameTextPc7719f0[];
extern "C" char TextInputKeyNameTextPc7719f8[];
extern "C" char TextInputKeyNameTextPc771a04[];
extern "C" char TextInputKeyNameTextPc771a14[];
extern "C" char TextInputKeyNameTextPc771a20[];
extern "C" char TextInputKeyNameTextPc771a28[];
extern "C" char TextInputKeyNameTextPc771a30[];
extern "C" char TextInputKeyNameTextPc771a38[];
extern "C" char TextInputKeyNameTextPc771a40[];
extern "C" char TextInputKeyNameTextPc771a48[];
extern "C" char TextInputKeyNameTextPc771a50[];
extern "C" char TextInputKeyNameTextPc771a58[];
extern "C" char TextInputKeyNameTextPc771a64[];
extern "C" char TextInputKeyNameTextPc771a68[];
extern "C" char TextInputKeyNameTextPc771a70[];
extern "C" char TextInputKeyNameTextPc771a78[];
extern "C" char TextInputKeyNameTextPc771a80[];
extern "C" char TextInputKeyNameTextPc771a84[];
extern "C" char TextInputKeyNameTextPc771a8c[];
extern "C" char TextInputKeyNameTextPc771a94[];
extern "C" char TextInputKeyNameTextPc771a9c[];
extern "C" char TextInputKeyNameTextPc771aa4[];
extern "C" char TextInputKeyNameTextPc771ab8[];
extern "C" char TextInputKeyNameTextPc771ac0[];
extern "C" char TextInputKeyNameTextPc771ac8[];
extern "C" char TextInputKeyNameTextPc771ad0[];
extern "C" char TextInputKeyNameTextPc771adc[];
extern "C" char TextInputKeyNameTextPc771ae4[];
extern "C" char TextInputKeyNameTextPc771aec[];
extern "C" char TextInputKeyNameTextPc771af4[];
extern "C" char TextInputKeyNameTextPc771afc[];
extern "C" char TextInputKeyNameTextPc771b08[];
extern "C" char TextInputKeyNameTextPc771b14[];
extern "C" char TextInputKeyNameTextPc771b20[];
extern "C" char TextInputKeyNameTextPc771b24[];
extern "C" char TextInputKeyNameTextPc771b2c[];
extern "C" char TextInputKeyNameTextPc771b34[];
extern "C" char TextInputKeyNameTextPc771b40[];
extern "C" char TextInputKeyNameTextPc771b48[];
extern "C" char TextInputKeyNameTextPc771b4c[];
extern "C" char TextInputKeyNameTextPc771b58[];
extern "C" char TextInputKeyNameTextPc771b60[];
extern "C" char TextInputKeyNameTextPc771b68[];
extern "C" char TextInputKeyNameTextPc771b6c[];
extern "C" char TextInputKeyNameTextPc771b78[];
extern "C" char TextInputKeyNameTextPc771b80[];
extern "C" char TextInputKeyNameTextPc771b88[];
extern "C" char TextInputKeyNameTextPc771b90[];
extern "C" char TextInputKeyNameTextPc771b94[];
extern "C" char TextInputKeyNameTextPc771b98[];
extern "C" char TextInputKeyNameTextPc771b9c[];
extern "C" char TextInputKeyNameTextPc771ba0[];
extern "C" char TextInputKeyNameTextPc771bb0[];
extern "C" char TextInputKeyNameTextPc771bb8[];
extern "C" char TextInputKeyNameTextPc771bc0[];
extern "C" char TextInputKeyNameTextPc771bc8[];
extern "C" char TextInputKeyNameTextPc771bd0[];
extern "C" char TextInputKeyNameTextPc771bd8[];
extern "C" char TextInputKeyNameTextPc771be0[];
extern "C" char TextInputKeyNameTextPc771be8[];
extern "C" char TextInputKeyNameTextPc771bf0[];
extern "C" char TextInputKeyNameTextPc771bf8[];
extern "C" char TextInputKeyNameTextPc771c00[];
extern "C" char TextInputKeyNameTextPc771c08[];
extern "C" char TextInputKeyNameTextPc771c10[];
extern "C" char TextInputKeyNameTextPc771c1c[];
extern "C" char TextInputKeyNameTextPc771c28[];
extern "C" char TextInputKeyNameTextPc771c2c[];
extern "C" char TextInputKeyNameTextPc771c30[];
extern "C" char TextInputKeyNameTextPc771c34[];
extern "C" char TextInputKeyNameTextPc771c38[];
extern "C" char TextInputKeyNameTextPc771c3c[];
extern "C" char TextInputKeyNameTextPc771c40[];
extern "C" char TextInputKeyNameTextPc771c44[];
extern "C" char TextInputKeyNameTextPc771c48[];
extern "C" char TextInputKeyNameTextPc771c4c[];
extern "C" char TextInputKeyNameTextPc771c50[];
extern "C" char TextInputKeyNameTextPc771c5c[];
extern "C" char TextInputKeyNameTextPc771c64[];
extern "C" char TextInputKeyNameTextPc771c6c[];
extern "C" char TextInputKeyNameTextPc771c74[];
extern "C" char TextInputKeyNameTextPc771c80[];
extern "C" char TextInputKeyNameTextPc771c8c[];
extern "C" char TextInputKeyNameTextPc771c94[];
extern "C" char TextInputKeyNameTextPc771c9c[];
extern "C" char TextInputKeyNameTextPc771ca0[];
extern "C" char TextInputKeyNameTextPc771ca4[];
extern "C" char TextInputKeyNameTextPc771ca8[];
extern "C" char TextInputKeyNameTextPc771cac[];
extern "C" char TextInputKeyNameTextPc771cb0[];
extern "C" char TextInputKeyNameTextPc771cb4[];
extern "C" char TextInputKeyNameTextPc771cc0[];
extern "C" char TextInputKeyNameTextPc771cc8[];
extern "C" char TextInputKeyNameTextPc771cd4[];
extern "C" char TextInputKeyNameTextPc771ce0[];
extern "C" char TextInputKeyNameTextPc771ce4[];
extern "C" char TextInputKeyNameTextPc771ce8[];
extern "C" char TextInputKeyNameTextPc771cec[];
extern "C" char TextInputKeyNameTextPc771cf0[];
extern "C" char TextInputKeyNameTextPc771cf4[];
extern "C" char TextInputKeyNameTextPc771cf8[];
extern "C" char TextInputKeyNameTextPc771cfc[];
extern "C" char TextInputKeyNameTextPc771d00[];
extern "C" char TextInputKeyNameTextPc771d04[];
extern "C" char TextInputKeyNameTextPc771d0c[];
extern "C" char TextInputKeyNameTextPc771d14[];
extern "C" char TextInputKeyNameTextPc771d18[];
extern "C" char TextInputKeyNameTextPc771d1c[];
extern "C" char TextInputKeyNameTextPc771d20[];
extern "C" char TextInputKeyNameTextPc771d24[];
extern "C" char TextInputKeyNameTextPc771d28[];
extern "C" char TextInputKeyNameTextPc771d2c[];
extern "C" char TextInputKeyNameTextPc771d30[];
extern "C" char TextInputKeyNameTextPc771d34[];
extern "C" char TextInputKeyNameTextPc771d38[];
extern "C" char TextInputKeyNameTextPc771d3c[];
extern "C" char TextInputKeyNameTextPc771d48[];
extern "C" char TextInputKeyNameTextPc771d50[];
extern "C" char TextInputKeyNameTextPc771d58[];
extern "C" char TextInputKeyNameTextPc771d5c[];
extern "C" char TextInputKeyNameTextPc771d60[];
extern "C" char TextInputKeyNameTextPc771d64[];
extern "C" char TextInputKeyNameTextPc771d68[];
extern "C" char TextInputKeyNameTextPc771d6c[];
extern "C" char TextInputKeyNameTextPc771d70[];
extern "C" char TextInputKeyNameTextPc771d74[];
extern "C" char TextInputKeyNameTextPc771d78[];
extern "C" char TextInputKeyNameTextPc771d7c[];
extern "C" char TextInputKeyNameTextPc771d80[];
extern "C" char TextInputKeyNameTextPc774180[];
extern "C" char TextInputKeyNameTextPc774184[];
static char PcInputText7517a8[3] = "\302\247";
static char PcInputText755d70[2] = ")";
static char PcInputText75c044[2] = ":";
static char PcInputText75eba8[2] = ".";
static char PcInputText76afa8[2] = "/";
static char PcInputText76fe70[6] = "START";
static char PcInputText770634[7] = "O. Win";
static char PcInputText77063c[5] = "Alas";
static char PcInputText770644[6] = "Oikea";
static char PcInputText77064c[6] = "Vasen";
static char PcInputText770654[6] = "Yl\303\266s";
static char PcInputText77065c[7] = "O. Alt";
static char PcInputText770664[8] = "O. Ctrl";
static char PcInputText77066c[11] = "Num. Piste";
static char PcInputText770678[6] = "V\303\244li";
static char PcInputText770680[9] = "O.Vaihto";
static char PcInputText77068c[7] = "Miinus";
static char PcInputText770694[6] = "Piste";
static char PcInputText77069c[7] = "Pilkku";
static char PcInputText7706a4[8] = "Sarkain";
static char PcInputText7706ac[14] = "Askelpalautin";
static char PcInputText7706bc[7] = "H\303\266ger";
static char PcInputText7706c4[9] = "V\303\244nster";
static char PcInputText7706d0[4] = "Upp";
static char PcInputText7706d4[11] = "Print Scrn";
static char PcInputText7706e0[17] = "Num. Kommatecken";
static char PcInputText7706f4[11] = "Num. Punkt";
static char PcInputText770700[10] = "Blanksteg";
static char PcInputText77070c[6] = "Punkt";
static char PcInputText770714[12] = "Kommatecken";
static char PcInputText770720[7] = "H. Win";
static char PcInputText770728[7] = "V. Win";
static char PcInputText770730[4] = "Ned";
static char PcInputText770734[7] = "H\303\270yre";
static char PcInputText77073c[8] = "Venstre";
static char PcInputText770744[3] = "UP";
static char PcInputText770748[8] = "NUM DIV";
static char PcInputText770750[10] = "NUM COMMA";
static char PcInputText77075c[9] = "Win Dto.";
static char PcInputText770768[9] = "Win Esq.";
static char PcInputText770774[16] = "Seta Para Baixo";
static char PcInputText770784[18] = "Seta Para Direita";
static char PcInputText770798[19] = "Seta Para Esquerda";
static char PcInputText7707ac[15] = "Seta Para Cima";
static char PcInputText7707bc[9] = "Alt Dto.";
static char PcInputText7707c8[9] = "Ctr Dto.";
static char PcInputText7707d4[11] = "Num. Ponto";
static char PcInputText7707e0[18] = "Barra De Espa\303\247os";
static char PcInputText7707f4[9] = "Alt Esq.";
static char PcInputText770800[11] = "Shift Dto.";
static char PcInputText77080c[21] = "Barra Para A Direita";
static char PcInputText770824[6] = "Ponto";
static char PcInputText77082c[9] = "V\303\255rgula";
static char PcInputText770838[17] = "Par\303\252nteses Dto.";
static char PcInputText77084c[11] = "Shift Esq.";
static char PcInputText770858[13] = "Acento Grave";
static char PcInputText770868[2] = "~";
static char PcInputText77086c[10] = "Ctrl Esq.";
static char PcInputText770878[17] = "Par\303\252nteses Esq.";
static char PcInputText77088c[11] = "Ap\303\263strofo";
static char PcInputText770898[11] = "Retroceder";
static char PcInputText7708a4[8] = "Igual A";
static char PcInputText7708ac[13] = "\320\237\321\200\320\260\320\262.Win";
static char PcInputText7708bc[11] = "\320\233\320\265\320\262.Win";
static char PcInputText7708c8[13] = "\320\237\321\200\320\260\320\262.Alt";
static char PcInputText7708d8[17] = "/ (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText7708ec[17] = ", (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770900[14] = "\320\237\321\200\320\260\320\262.Ctrl";
static char PcInputText770910[21] = "Enter (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770928[17] = "= (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText77093c[17] = ". (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770950[17] = "0 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770964[17] = "3 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770978[17] = "2 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText77098c[17] = "1 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText7709a0[17] = "+ (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText7709b4[17] = "6 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText7709c8[17] = "5 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText7709dc[17] = "4 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText7709f0[17] = "- (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770a04[17] = "9 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770a18[17] = "8 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770a2c[17] = "7 (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770a40[13] = "\320\237\321\200\320\276\320\261\320\265\320\273";
static char PcInputText770a50[11] = "\320\233\320\265\320\262.Alt";
static char PcInputText770a5c[17] = "* (\321\206\320\270\321\204.\320\272\320\273.)";
static char PcInputText770a70[15] = "\320\237\321\200\320\260\320\262.shift";
static char PcInputText770a80[3] = "\320\256";
static char PcInputText770a84[3] = "\320\221";
static char PcInputText770a88[3] = "\320\254";
static char PcInputText770a8c[3] = "\320\242";
static char PcInputText770a90[3] = "\320\230";
static char PcInputText770a94[3] = "\320\234";
static char PcInputText770a98[3] = "\320\241";
static char PcInputText770a9c[3] = "\320\247";
static char PcInputText770aa0[3] = "\320\257";
static char PcInputText770aa4[13] = "\320\233\320\265\320\262.Shift";
static char PcInputText770ab4[3] = "\320\255";
static char PcInputText770ab8[3] = "\320\226";
static char PcInputText770abc[3] = "\320\224";
static char PcInputText770ac0[3] = "\320\233";
static char PcInputText770ac4[3] = "\320\236";
static char PcInputText770ac8[3] = "\320\240";
static char PcInputText770acc[3] = "\320\237";
static char PcInputText770ad0[3] = "\320\220";
static char PcInputText770ad4[3] = "\320\222";
static char PcInputText770ad8[3] = "\320\253";
static char PcInputText770adc[3] = "\320\244";
static char PcInputText770ae0[12] = "\320\233\320\265\320\262.Ctrl";
static char PcInputText770aec[3] = "\320\252";
static char PcInputText770af0[3] = "\320\245";
static char PcInputText770af4[3] = "\320\227";
static char PcInputText770af8[3] = "\320\251";
static char PcInputText770afc[3] = "\320\250";
static char PcInputText770b00[3] = "\320\223";
static char PcInputText770b04[3] = "\320\235";
static char PcInputText770b08[3] = "\320\225";
static char PcInputText770b0c[3] = "\320\232";
static char PcInputText770b10[3] = "\320\243";
static char PcInputText770b14[3] = "\320\246";
static char PcInputText770b18[3] = "\320\231";
static char PcInputText770b1c[4] = "ESC";
static char PcInputText770b20[7] = "P. Win";
static char PcInputText770b28[6] = "D\303\263\305\200";
static char PcInputText770b30[6] = "Prawo";
static char PcInputText770b38[5] = "Lewo";
static char PcInputText770b40[6] = "G\303\263ra";
static char PcInputText770b48[7] = "P. Alt";
static char PcInputText770b50[8] = "P. Ctrl";
static char PcInputText770b58[7] = "Num. .";
static char PcInputText770b60[7] = "Spacja";
static char PcInputText770b68[9] = "P. Shift";
static char PcInputText770b74[6] = "GRAVE";
static char PcInputText770b7c[2] = ";";
static char PcInputText770b80[6] = "R.Win";
static char PcInputText770b88[7] = "Omlaag";
static char PcInputText770b90[7] = "Rechts";
static char PcInputText770b98[6] = "Links";
static char PcInputText770ba0[7] = "Omhoog";
static char PcInputText770ba8[10] = "Num. Punt";
static char PcInputText770bb4[7] = "Spatie";
static char PcInputText770bbc[5] = "Punt";
static char PcInputText770bc4[2] = "@";
static char PcInputText770bc8[3] = "\302\260";
static char PcInputText770bcc[15] = "H. Windowsknap";
static char PcInputText770bdc[15] = "V. Windowsknap";
static char PcInputText770bec[8] = "Pil Ned";
static char PcInputText770bf4[11] = "Pil H\303\270jre";
static char PcInputText770c00[12] = "Pil Venstre";
static char PcInputText770c0c[7] = "Pil Op";
static char PcInputText770c14[7] = "H. Alt";
static char PcInputText770c1c[13] = "Print Screen";
static char PcInputText770c2c[11] = "Num. Komma";
static char PcInputText770c38[6] = "VOLOP";
static char PcInputText770c40[7] = "VOLNED";
static char PcInputText770c48[5] = "REGN";
static char PcInputText770c50[8] = "H. Ctrl";
static char PcInputText770c58[11] = "N\303\206STESPOR";
static char PcInputText770c64[15] = "UDEN ANGIVELSE";
static char PcInputText770c74[17] = "SP\303\230RGSM\303\205LSTEGN";
static char PcInputText770c88[9] = "STORT \303\206";
static char PcInputText770c94[15] = "ANF\303\230RSELSTEGN";
static char PcInputText770ca4[13] = "Num. Lig Med";
static char PcInputText770cb4[13] = "Num. Punktum";
static char PcInputText770cc4[15] = "Mellemrumstast";
static char PcInputText770cd4[7] = "V. Alt";
static char PcInputText770cdc[9] = "H. Shift";
static char PcInputText770ce8[8] = "Punktum";
static char PcInputText770cf0[6] = "Komma";
static char PcInputText770cf8[9] = "V. Shift";
static char PcInputText770d04[3] = "\302\275";
static char PcInputText770d08[3] = "\303\230";
static char PcInputText770d0c[3] = "\303\206";
static char PcInputText770d10[8] = "V. Ctrl";
static char PcInputText770d18[3] = "\302\250";
static char PcInputText770d1c[3] = "\303\205";
static char PcInputText770d20[7] = "Accent";
static char PcInputText770d28[6] = "(TOM)";
static char PcInputText770d30[9] = "Win Der.";
static char PcInputText770d3c[9] = "Win Izq.";
static char PcInputText770d48[5] = "Supr";
static char PcInputText770d50[8] = "Av P\303\241g";
static char PcInputText770d58[13] = "Flecha Abajo";
static char PcInputText770d68[12] = "Flecha Der.";
static char PcInputText770d74[12] = "Flecha Izq.";
static char PcInputText770d80[8] = "Re P\303\241g";
static char PcInputText770d88[11] = "Flecha Arr";
static char PcInputText770d94[7] = "Inicio";
static char PcInputText770d9c[9] = "Alt Der.";
static char PcInputText770da8[7] = "PETSIS";
static char PcInputText770db0[7] = "N\303\232M /";
static char PcInputText770db8[10] = "N\303\232M Coma";
static char PcInputText770dc4[10] = "INICIOWEB";
static char PcInputText770dd0[9] = "VOL ARRI";
static char PcInputText770ddc[9] = "VOL ABAJ";
static char PcInputText770de8[7] = "REPROD";
static char PcInputText770df0[9] = "SILENCIO";
static char PcInputText770dfc[10] = "Ctrl Der.";
static char PcInputText770e08[11] = "N\303\232M Intro";
static char PcInputText770e14[10] = "PISTA SIG";
static char PcInputText770e20[8] = "DETENER";
static char PcInputText770e28[9] = "SUBRAYAR";
static char PcInputText770e34[11] = "DOS PUNTOS";
static char PcInputText770e40[7] = "ARROBA";
static char PcInputText770e48[10] = "PISTA ANT";
static char PcInputText770e54[11] = "N\303\232M Igual";
static char PcInputText770e60[11] = "N\303\232M Punto";
static char PcInputText770e6c[7] = "N\303\232M 0";
static char PcInputText770e74[7] = "N\303\232M 3";
static char PcInputText770e7c[7] = "N\303\232M 2";
static char PcInputText770e84[7] = "N\303\232M 1";
static char PcInputText770e8c[7] = "N\303\232M +";
static char PcInputText770e94[7] = "N\303\232M 6";
static char PcInputText770e9c[7] = "N\303\232M 5";
static char PcInputText770ea4[7] = "N\303\232M 4";
static char PcInputText770eac[7] = "N\303\232M -";
static char PcInputText770eb4[7] = "N\303\232M 9";
static char PcInputText770ebc[7] = "N\303\232M 8";
static char PcInputText770ec4[7] = "N\303\232M 7";
static char PcInputText770ecc[11] = "Bloq Despl";
static char PcInputText770ed8[10] = "Bloq N\303\232M";
static char PcInputText770ee4[12] = "Bloq May\303\272s";
static char PcInputText770ef0[8] = "Espacio";
static char PcInputText770ef8[9] = "Alt Izq.";
static char PcInputText770f04[7] = "N\303\232M *";
static char PcInputText770f0c[11] = "Mayus Der.";
static char PcInputText770f18[6] = "Menos";
static char PcInputText770f20[5] = "Coma";
static char PcInputText770f28[3] = "\303\207";
static char PcInputText770f2c[12] = "May\303\272s Izq.";
static char PcInputText770f38[3] = "\302\272";
static char PcInputText770f3c[3] = "\303\221";
static char PcInputText770f40[10] = "Ctrl Izq.";
static char PcInputText770f4c[6] = "Intro";
static char PcInputText770f54[2] = "`";
static char PcInputText770f58[10] = "Retroceso";
static char PcInputText770f64[3] = "\302\241";
static char PcInputText770f68[2] = "'";
static char PcInputText770f6c[9] = "(VAC\303\215O)";
static char PcInputText770f78[13] = "Win-Taste R.";
static char PcInputText770f88[13] = "Win-Taste L.";
static char PcInputText770f98[5] = "Entf";
static char PcInputText770fa0[6] = "Einfg";
static char PcInputText770fa8[8] = "Bild Ab";
static char PcInputText770fb0[12] = "Pfeil unten";
static char PcInputText770fbc[5] = "Ende";
static char PcInputText770fc4[13] = "Pfeil rechts";
static char PcInputText770fd4[12] = "Pfeil links";
static char PcInputText770fe0[9] = "Bild Auf";
static char PcInputText770fec[11] = "Pfeil oben";
static char PcInputText770ff8[5] = "Pos1";
static char PcInputText771000[6] = "lt R.";
static char PcInputText771008[6] = "S-Abf";
static char PcInputText771010[7] = "Num. ,";
static char PcInputText771018[8] = "Strg R.";
static char PcInputText771020[13] = "Num. Eingabe";
static char PcInputText771030[10] = "UNBENANNT";
static char PcInputText77103c[2] = "\366";
static char PcInputText771040[2] = "\344";
static char PcInputText771044[7] = "Rollen";
static char PcInputText77104c[10] = "Num-Taste";
static char PcInputText771058[15] = "Feststelltaste";
static char PcInputText771068[10] = "Leertaste";
static char PcInputText771074[7] = "Alt L.";
static char PcInputText77107c[10] = "Umsch. R.";
static char PcInputText771088[2] = ",";
static char PcInputText77108c[10] = "Umsch. L.";
static char PcInputText771098[3] = "\303\204";
static char PcInputText77109c[3] = "\303\226";
static char PcInputText7710a0[8] = "Strg L.";
static char PcInputText7710a8[8] = "Eingabe";
static char PcInputText7710b0[3] = "\303\234";
static char PcInputText7710b4[11] = "R\303\274cktaste";
static char PcInputText7710c0[3] = "\302\264";
static char PcInputText7710c4[3] = "\303\237";
static char PcInputText7710c8[7] = "(LEER)";
static char PcInputText7710d0[9] = "Win Des.";
static char PcInputText7710dc[9] = "Win Sin.";
static char PcInputText7710e8[6] = "Canc.";
static char PcInputText7710f0[10] = "Inserisci";
static char PcInputText7710fc[10] = "Pag. Gi\303\231";
static char PcInputText771108[13] = "Freccia Gi\303\271";
static char PcInputText771118[5] = "Fine";
static char PcInputText771120[13] = "Freccia Des.";
static char PcInputText771130[13] = "Freccia Sin.";
static char PcInputText771140[8] = "Pag. Su";
static char PcInputText771148[12] = "Freccia S\303\271";
static char PcInputText771154[6] = "Pausa";
static char PcInputText77115c[9] = "Alt Des.";
static char PcInputText771168[6] = "RSIST";
static char PcInputText771170[10] = "Tastnum /";
static char PcInputText77117c[10] = "Tastnum ,";
static char PcInputText771188[7] = "Vol Su";
static char PcInputText771190[9] = "Vol Gi\303\271";
static char PcInputText77119c[10] = "Ctrl Des.";
static char PcInputText7711a8[14] = "Tastnum Invio";
static char PcInputText7711b8[10] = "Tastnum =";
static char PcInputText7711c4[10] = "Tastnum .";
static char PcInputText7711d0[10] = "Tastnum 0";
static char PcInputText7711dc[10] = "Tastnum 3";
static char PcInputText7711e8[10] = "Tastnum 2";
static char PcInputText7711f4[10] = "Tastnum 1";
static char PcInputText771200[10] = "Tastnum +";
static char PcInputText77120c[10] = "Tastnum 6";
static char PcInputText771218[10] = "Tastnum 5";
static char PcInputText771224[10] = "Tastnum 4";
static char PcInputText771230[10] = "Tastnum -";
static char PcInputText77123c[10] = "Tastnum 9";
static char PcInputText771248[10] = "Tastnum 8";
static char PcInputText771254[10] = "Tastnum 7";
static char PcInputText771260[13] = "Blocca Scorr";
static char PcInputText771270[9] = "Bloc Num";
static char PcInputText77127c[12] = "BLOC MAIUSC";
static char PcInputText771288[7] = "Spazio";
static char PcInputText771290[9] = "Alt Sin.";
static char PcInputText77129c[10] = "Tastnum *";
static char PcInputText7712a8[11] = "Shift Des.";
static char PcInputText7712b4[2] = "-";
static char PcInputText7712b8[6] = "Punto";
static char PcInputText7712c0[8] = "Virgola";
static char PcInputText7712c8[3] = "\303\271";
static char PcInputText7712cc[11] = "Shift Sin.";
static char PcInputText7712d8[3] = "\303\240";
static char PcInputText7712dc[3] = "\303\262";
static char PcInputText7712e0[10] = "Ctrl Sin.";
static char PcInputText7712ec[6] = "Invio";
static char PcInputText7712f4[2] = "+";
static char PcInputText7712f8[3] = "\303\250";
static char PcInputText7712fc[3] = "\303\254";
static char PcInputText771300[10] = "APOSTROFO";
static char PcInputText77130c[10] = "Windows D";
static char PcInputText771318[10] = "Windows G";
static char PcInputText771324[6] = "Suppr";
static char PcInputText77132c[6] = "Inser";
static char PcInputText771334[11] = "Page Suiv.";
static char PcInputText771340[4] = "Bas";
static char PcInputText771344[4] = "Fin";
static char PcInputText771348[7] = "Droite";
static char PcInputText771350[7] = "Gauche";
static char PcInputText771358[12] = "Page Pr\303\250c.";
static char PcInputText771364[5] = "Haut";
static char PcInputText77136c[8] = "Origine";
static char PcInputText771374[6] = "Alt D";
static char PcInputText77137c[17] = "Impr \303\211cran Syst";
static char PcInputText771390[12] = "Pav. Num. /";
static char PcInputText77139c[18] = "Pav. Num. Virgule";
static char PcInputText7713b0[4] = "www";
static char PcInputText7713b4[7] = "Ctrl D";
static char PcInputText7713bc[18] = "Pav. Num. Entr\303\251e";
static char PcInputText7713d0[2] = "\260";
static char PcInputText7713d4[2] = "%";
static char PcInputText7713d8[12] = "Pav. Num. =";
static char PcInputText7713e4[2] = "{";
static char PcInputText7713e8[16] = "Pav. Num. Point";
static char PcInputText7713f8[12] = "Pav. Num. 0";
static char PcInputText771404[12] = "Pav. Num. 3";
static char PcInputText771410[12] = "Pav. Num. 2";
static char PcInputText77141c[12] = "Pav. Num. 1";
static char PcInputText771428[12] = "Pav. Num. +";
static char PcInputText771434[12] = "Pav. Num. 6";
static char PcInputText771440[12] = "Pav. Num. 5";
static char PcInputText77144c[12] = "Pav. Num. 4";
static char PcInputText771458[12] = "Pav. Num. -";
static char PcInputText771464[12] = "Pav. Num. 9";
static char PcInputText771470[12] = "Pav. Num. 8";
static char PcInputText77147c[12] = "Pav. Num. 7";
static char PcInputText771488[14] = "Arr\303\252t D\303\251fil";
static char PcInputText771498[9] = "Verr Num";
static char PcInputText7714a4[9] = "Verr Maj";
static char PcInputText7714b0[7] = "Espace";
static char PcInputText7714b8[6] = "Alt G";
static char PcInputText7714c0[12] = "Pav. Num. *";
static char PcInputText7714cc[6] = "Maj D";
static char PcInputText7714d4[20] = "Point D'Exclamation";
static char PcInputText7714e8[12] = "Deux Points";
static char PcInputText7714f4[14] = "Point Virgule";
static char PcInputText771504[8] = "Virgule";
static char PcInputText77150c[2] = "*";
static char PcInputText771510[6] = "Maj G";
static char PcInputText771518[3] = "\302\262";
static char PcInputText77151c[3] = "\303\231";
static char PcInputText771520[7] = "Ctrl G";
static char PcInputText771528[8] = "Entr\303\251e";
static char PcInputText771530[2] = "|";
static char PcInputText771534[2] = "^";
static char PcInputText771538[11] = "Tabulation";
static char PcInputText771544[5] = "Egal";
static char PcInputText77154c[6] = "Echap";
static char PcInputText771554[6] = "P.Win";
static char PcInputText77155c[6] = "L.Win";
static char PcInputText771564[8] = "Pg Down";
static char PcInputText77156c[6] = "Dol\305\257";
static char PcInputText771574[7] = "Vpravo";
static char PcInputText77157c[6] = "Vlevo";
static char PcInputText771584[6] = "Pg Up";
static char PcInputText77158c[7] = "Nahoru";
static char PcInputText771594[6] = "P.Alt";
static char PcInputText77159c[6] = "Num./";
static char PcInputText7715a4[7] = "P.Ctrl";
static char PcInputText7715ac[10] = "Num.Enter";
static char PcInputText7715b8[14] = "Num.Rovn\303\241 Se";
static char PcInputText7715c8[6] = "Num .";
static char PcInputText7715d0[6] = "Num.0";
static char PcInputText7715d8[6] = "Num.3";
static char PcInputText7715e0[6] = "Num.2";
static char PcInputText7715e8[6] = "Num.1";
static char PcInputText7715f0[6] = "Num.+";
static char PcInputText7715f8[6] = "Num.6";
static char PcInputText771600[6] = "Num.5";
static char PcInputText771608[6] = "Num.4";
static char PcInputText771610[6] = "Num.-";
static char PcInputText771618[6] = "Num.9";
static char PcInputText771620[6] = "Num.8";
static char PcInputText771628[6] = "Num.7";
static char PcInputText771630[10] = "Mezern\303\255k";
static char PcInputText77163c[6] = "L.Alt";
static char PcInputText771644[6] = "Kr\303\241t";
static char PcInputText77164c[8] = "P.Shift";
static char PcInputText771654[9] = "Lom\303\255tko";
static char PcInputText771660[7] = "Te\304\215ka";
static char PcInputText771668[8] = "\304\214\303\241rka";
static char PcInputText771670[9] = "Uvozovky";
static char PcInputText77167c[8] = "L.Shift";
static char PcInputText771684[8] = "Vlnovka";
static char PcInputText77168c[9] = "Apostrof";
static char PcInputText771698[3] = "\305\256";
static char PcInputText77169c[7] = "L.Ctrl";
static char PcInputText7716a4[11] = "P.Z\303\241vorka";
static char PcInputText7716b0[3] = "\303\232";
static char PcInputText7716b4[11] = "Tabul\303\241tor";
static char PcInputText7716c0[10] = "Rovn\303\241 Se";
static char PcInputText7716cc[7] = "M\303\255nus";
static char PcInputText7716d4[8] = "(EMPTY)";
static char PcInputText7716dc[25] =
    "\343\202\242\343\203\227\343\203\252\343\202\261\343\203\274\343\202\267\343\203\247\343\203\263";
static char PcInputText7716f8[23] =
    "\345\217\263 \343\202\246\343\202\243\343\203\263\343\203\211\343\202\246\343\202\272";
static char PcInputText771710[23] =
    "\345\267\246 \343\202\246\343\202\243\343\203\263\343\203\211\343\202\246\343\202\272";
static char PcInputText771728[10] = "Page Down";
static char PcInputText771734[10] = "\344\270\213\347\237\242\345\215\260";
static char PcInputText771740[10] = "\345\217\263\347\237\242\345\215\260";
static char PcInputText77174c[10] = "\345\267\246\347\237\242\345\215\260";
static char PcInputText771758[8] = "Page Up";
static char PcInputText771760[10] = "\344\270\212\347\237\242\345\215\260";
static char PcInputText77176c[8] = "\345\217\263 Alt";
static char PcInputText771774[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 /";
static char PcInputText771788[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 ,";
static char PcInputText77179c[9] = "\345\217\263 Ctrl";
static char PcInputText7717a8[22] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 Enter";
static char PcInputText7717c0[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 .";
static char PcInputText7717d4[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 0";
static char PcInputText7717e8[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 3";
static char PcInputText7717fc[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 2";
static char PcInputText771810[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 1";
static char PcInputText771824[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 +";
static char PcInputText771838[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 6";
static char PcInputText77184c[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 5";
static char PcInputText771860[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 4";
static char PcInputText771874[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 -";
static char PcInputText771888[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 9";
static char PcInputText77189c[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 8";
static char PcInputText7718b0[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 7";
static char PcInputText7718c4[13] = "\343\202\271\343\203\232\343\203\274\343\202\271";
static char PcInputText7718d4[8] = "\345\267\246 Alt";
static char PcInputText7718dc[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 *";
static char PcInputText7718f0[10] = "\345\217\263 Shift";
static char PcInputText7718fc[34] = "\343\203\225\343\202\251\343\203\257\343\203\274\343\203\211\343\202\271\343\203"
                                    "\251\343\203\203\343\202\267\343\203\245(/)";
static char PcInputText771920[16] = "\343\203\224\343\203\252\343\202\252\343\203\211(.)";
static char PcInputText771930[13] = "\343\202\253\343\203\263\343\203\236(,)";
static char PcInputText771940[10] = "\345\267\246 Shift";
static char PcInputText77194c[16] = "\343\202\260\343\203\254\343\202\244\343\203\264(`)";
static char PcInputText77195c[25] =
    "\343\202\242\343\203\235\343\202\271\343\203\210\343\203\255\343\203\225\343\202\243(')";
static char PcInputText771978[21] = "\343\202\273\343\203\237\343\202\263\343\203\255\343\203\263(\357\274\233)";
static char PcInputText771990[9] = "\345\267\246 Ctrl";
static char PcInputText77199c[11] = "Num. Comma";
static char PcInputText7719a8[8] = "Oem_102";
static char PcInputText7719b0[10] = "Num. Stop";
static char PcInputText771e38[15] = "\305\275\303\241dn\303\275 vstup";
static char PcInputText771e48[19] = "Tla\304\215\303\255tko my\305\241i A";
static char PcInputText771e5c[27] = "St\305\231edn\303\255 tla\304\215\303\255tko my\305\241i";
static char PcInputText771e78[24] = "Prav\303\251 tla\304\215\303\255tko my\305\241i";
static char PcInputText771e90[23] = "Lev\303\251 tla\304\215\303\255tko my\305\241i";
static char PcInputText771ea8[22] = "Kole\304\215ko my\305\241i nahoru";
static char PcInputText771ec0[19] = "Pohyb my\305\241\303\255 dol\305\257";
static char PcInputText771ed4[20] = "Pohyb my\305\241\303\255 vpravo";
static char PcInputText771ee8[21] = "Kole\304\215ko my\305\241i dol\305\257";
static char PcInputText771f00[20] = "Pohyb my\305\241\303\255 nahoru";
static char PcInputText771f14[19] = "Pohyb my\305\241\303\255 vlevo";
static char PcInputText771f28[6] = "Ose X";
static char PcInputText771f30[6] = "Zp\304\233t";
static char PcInputText771f38[15] = "Prav\303\241 p\303\241\304\215ka";
static char PcInputText771f48[14] = "Lev\303\241 p\303\241\304\215ka";
static char PcInputText771f58[16] = "Prav\303\241 spou\305\241\305\245";
static char PcInputText771f68[15] = "Lev\303\241 spou\305\241\305\245";
static char PcInputText771f78[26] = "Prav\303\251 bo\304\215n\303\255 tla\304\215\303\255tko";
static char PcInputText771f94[25] = "Lev\303\251 bo\304\215n\303\255 tla\304\215\303\255tko";
static char PcInputText771fb0[13] = "Tla\304\215\303\255tko X";
static char PcInputText771fc0[13] = "Tla\304\215\303\255tko A";
static char PcInputText771fd0[13] = "Tla\304\215\303\255tko B";
static char PcInputText771fe0[13] = "Tla\304\215\303\255tko Y";
static char PcInputText771ff0[21] = "Prav\303\241 p\303\241\304\215ka dol\305\257";
static char PcInputText772008[22] = "Prav\303\241 p\303\241\304\215ka nahoru";
static char PcInputText772020[22] = "Prav\303\241 p\303\241\304\215ka vpravo";
static char PcInputText772038[21] = "Prav\303\241 p\303\241\304\215ka vlevo";
static char PcInputText772050[20] = "Lev\303\241 p\303\241\304\215ka dol\305\257";
static char PcInputText772064[21] = "Lev\303\241 p\303\241\304\215ka nahoru";
static char PcInputText77207c[21] = "Lev\303\241 p\303\241\304\215ka vpravo";
static char PcInputText772094[20] = "Lev\303\241 p\303\241\304\215ka vlevo";
static char PcInputText7720a8[9] = "Rotace Z";
static char PcInputText7720b4[9] = "Rotace Y";
static char PcInputText7720c0[9] = "Rotace X";
static char PcInputText7720cc[6] = "Osa Z";
static char PcInputText7720d4[6] = "Osa Y";
static char PcInputText7720dc[6] = "Osa X";
static char PcInputText7720e4[9] = "Vy\305\241\305\241\303\255";
static char PcInputText7720f0[9] = "Ni\305\276\305\241\303\255";
static char PcInputText7720fc[11] = "Tla\304\215\303\255tko";
static char PcInputText772108[21] = "Sm\304\233r. k\305\231\303\255\305\276 vlevo";
static char PcInputText772120[21] = "Sm\304\233r. k\305\231\303\255\305\276 dol\305\257";
static char PcInputText772138[22] = "Sm\304\233r. k\305\231\303\255\305\276 vpravo";
static char PcInputText772150[22] = "Sm\304\233r. k\305\231\303\255\305\276 nahoru";
static char PcInputText77229c[39] =
    "\320\235\320\265\321\202 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 "
    "\320\262\320\262\320\276\320\264\320\260";
static char PcInputText7722c4[24] =
    "\320\232\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270 N";
static char PcInputText7722dc[37] = "\320\241\321\200\320\265\320\264\320\275\321\217\321\217 "
                                    "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
static char PcInputText772304[35] = "\320\237\321\200\320\260\320\262\320\260\321\217 "
                                    "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
static char PcInputText772328[33] = "\320\233\320\265\320\262\320\260\321\217 "
                                    "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
static char PcInputText77234c[33] = "\320\232\320\276\320\273\320\265\321\201\320\276 \320\274\321\213\321\210\320\270 "
                                    "\320\262\320\262\320\265\321\200\321\205";
static char PcInputText772370[18] = "\320\234\321\213\321\210\321\214 \320\262\320\275\320\270\320\267";
static char PcInputText772384[22] = "\320\234\321\213\321\210\321\214 \320\262\320\277\321\200\320\260\320\262\320\276";
static char PcInputText77239c[31] = "\320\232\320\276\320\273\320\265\321\201\320\276 \320\274\321\213\321\210\320\270 "
                                    "\320\262\320\275\320\270\320\267";
static char PcInputText7723bc[20] = "\320\234\321\213\321\210\321\214 \320\262\320\262\320\265\321\200\321\205";
static char PcInputText7723d0[20] = "\320\234\321\213\321\210\321\214 \320\262\320\273\320\265\320\262\320\276";
static char PcInputText7723e4[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 Z)";
static char PcInputText7723f8[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 Y)";
static char PcInputText77240c[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 X)";
static char PcInputText772420[11] = "\320\235\320\260\320\267\320\260\320\264";
static char PcInputText77242c[11] = "\320\241\321\202\320\260\321\200\321\202";
static char PcInputText772438[30] =
    "\320\237\321\200\320\260\320\262\321\213\320\271 \320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272";
static char PcInputText772458[28] =
    "\320\233\320\265\320\262\321\213\320\271 \320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272";
static char PcInputText772474[24] =
    "\320\237\321\200\320\260\320\262\321\213\320\271 \320\272\321\203\321\200\320\276\320\272";
static char PcInputText77248c[22] = "\320\233\320\265\320\262\321\213\320\271 \320\272\321\203\321\200\320\276\320\272";
static char PcInputText7724a4[28] =
    "\320\237\321\200\320\260\320\262\321\213\320\271 \321\202\321\200\320\270\320\263\320\263\320\265\321\200";
static char PcInputText7724c0[26] =
    "\320\233\320\265\320\262\321\213\320\271 \321\202\321\200\320\270\320\263\320\263\320\265\321\200";
static char PcInputText7724dc[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 X";
static char PcInputText7724ec[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 A";
static char PcInputText7724fc[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 B";
static char PcInputText77250c[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 Y";
static char PcInputText77251c[41] =
    "\320\222\320\275\320\270\320\267 (\320\277\321\200\320\260\320\262\321\213\320\271 "
    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText772548[43] =
    "\320\222\320\262\320\265\321\200\321\205 (\320\277\321\200\320\260\320\262\321\213\320\271 "
    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText772574[45] =
    "\320\222\320\277\321\200\320\260\320\262\320\276 (\320\277\321\200\320\260\320\262\321\213\320\271 "
    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText7725a4[43] =
    "\320\222\320\273\320\265\320\262\320\276 (\320\277\321\200\320\260\320\262\321\213\320\271 "
    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText7725d0[39] = "\320\222\320\275\320\270\320\267 (\320\273\320\265\320\262\321\213\320\271 "
                                    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText7725f8[41] =
    "\320\222\320\262\320\265\321\200\321\205 (\320\273\320\265\320\262\321\213\320\271 "
    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText772624[43] =
    "\320\222\320\277\321\200\320\260\320\262\320\276 (\320\273\320\265\320\262\321\213\320\271 "
    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText772650[41] =
    "\320\222\320\273\320\265\320\262\320\276 (\320\273\320\265\320\262\321\213\320\271 "
    "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char PcInputText77267c[24] =
    "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 Z";
static char PcInputText772694[24] =
    "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 Y";
static char PcInputText7726ac[24] =
    "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 X";
static char PcInputText7726c4[9] = "\320\236\321\201\321\214 Z";
static char PcInputText7726d0[9] = "\320\236\321\201\321\214 Y";
static char PcInputText7726dc[9] = "\320\236\321\201\321\214 X";
static char PcInputText7726e8[29] =
    "\320\220\320\275\320\260\320\273\320\276\320\263\320\276\320\262\321\213\320\271 %d (%s)";
static char PcInputText772708[15] = "\320\222\321\213\321\201\320\276\320\272\320\276\320\265";
static char PcInputText772718[13] = "\320\235\320\270\320\267\320\272\320\276\320\265";
static char PcInputText772728[13] = "\320\232\320\275\320\276\320\277\320\272\320\260";
static char PcInputText772738[11] = "\320\222\320\273\320\265\320\262\320\276";
static char PcInputText772744[9] = "\320\222\320\275\320\270\320\267";
static char PcInputText772750[13] = "\320\222\320\277\321\200\320\260\320\262\320\276";
static char PcInputText772760[11] = "\320\222\320\262\320\265\321\200\321\205";
static char PcInputText772830[14] = "Brak wej\305\233cia";
static char PcInputText772840[20] = "Myszka - przycisk N";
static char PcInputText772854[26] = "\305\232rodkowy przycisk myszki";
static char PcInputText772870[22] = "Prawy przycisk myszki";
static char PcInputText772888[21] = "Lewy przycisk myszki";
static char PcInputText7728a0[24] = "K\303\263\305\202ko myszki w g\303\263r\304\231";
static char PcInputText7728b8[15] = "Myszka w d\303\263\305\202";
static char PcInputText7728c8[23] = "K\303\263\305\202ko myszki w d\303\263\305\202";
static char PcInputText7728e0[16] = "Myszka w g\303\263r\304\231";
static char PcInputText7728f0[14] = "Myszka w lewo";
static char PcInputText772900[9] = "Myszka Z";
static char PcInputText77290c[9] = "Myszka Y";
static char PcInputText772918[9] = "Myszka X";
static char PcInputText772924[7] = "Wr\303\263\304\207";
static char PcInputText77292c[15] = "Prawy dr\304\205\305\274ek";
static char PcInputText77293c[14] = "Lewy dr\304\205\305\274ek";
static char PcInputText77294c[12] = "Prawy spust";
static char PcInputText772958[11] = "Lewy spust";
static char PcInputText772964[23] = "Prawy przycisk przedni";
static char PcInputText77297c[22] = "Lewy przycisk przedni";
static char PcInputText772994[11] = "Przycisk X";
static char PcInputText7729a0[11] = "Przycisk A";
static char PcInputText7729ac[11] = "Przycisk B";
static char PcInputText7729b8[11] = "Przycisk Y";
static char PcInputText7729c4[23] = "Prawy dr\304\205\305\274ek w d\303\263\305\202";
static char PcInputText7729dc[24] = "Prawy dr\304\205\305\274ek w g\303\263r\304\231";
static char PcInputText7729f4[23] = "Prawy dr\304\205\305\274ek w prawo";
static char PcInputText772a0c[22] = "Prawy dr\304\205\305\274ek w lewo";
static char PcInputText772a24[22] = "Lewy dr\304\205\305\274ek w d\303\263\305\202";
static char PcInputText772a3c[23] = "Lewy dr\304\205\305\274ek w g\303\263r\304\231";
static char PcInputText772a54[22] = "Lewy dr\304\205\305\274ek w prawo";
static char PcInputText772a6c[21] = "Lewy dr\304\205\305\274ek w lewo";
static char PcInputText772a84[15] = "Obr\303\263t w osi Z";
static char PcInputText772a94[15] = "Obr\303\263t w osi Y";
static char PcInputText772aa4[15] = "Obr\303\263t w osi X";
static char PcInputText772ab4[6] = "O\305\233 Z";
static char PcInputText772abc[6] = "O\305\233 Y";
static char PcInputText772ac4[6] = "O\305\233 X";
static char PcInputText772acc[18] = "Analogowy (%s) %d";
static char PcInputText772ae0[7] = "Wysoka";
static char PcInputText772ae8[6] = "Niska";
static char PcInputText772af0[9] = "Przycisk";
static char PcInputText772afc[16] = "Kapturek w lewo";
static char PcInputText772b0c[17] = "Kapturek w d\303\263\305\202";
static char PcInputText772b20[17] = "Kapturek w prawo";
static char PcInputText772b34[18] = "Kapturek w g\303\263r\304\231";
static char PcInputText772bd8[11] = "Museknap N";
static char PcInputText772be4[19] = "Midterste museknap";
static char PcInputText772bf8[16] = "H\303\270jre museknap";
static char PcInputText772c08[17] = "Venstre museknap";
static char PcInputText772c1c[11] = "Mushjul op";
static char PcInputText772c28[11] = "Mus h\303\270jre";
static char PcInputText772c34[7] = "Mus op";
static char PcInputText772c3c[7] = "\011Mus X";
static char PcInputText772c44[8] = "Tilbage";
static char PcInputText772c4c[17] = "H\303\270jre styrepind";
static char PcInputText772c60[18] = "Venstre styrepind";
static char PcInputText772c74[17] = "Venstre udl\303\270ser";
static char PcInputText772c88[19] = "H\303\270jre skulderknap";
static char PcInputText772c9c[20] = "Venstre skulderknap";
static char PcInputText772cb0[7] = "Knap X";
static char PcInputText772cb8[7] = "Knap A";
static char PcInputText772cc0[7] = "Knap B";
static char PcInputText772cc8[7] = "Knap Y";
static char PcInputText772cd0[21] = "H\303\270jre Styrepind Ned";
static char PcInputText772ce8[20] = "H\303\270jre Styrepind Op";
static char PcInputText772cfc[24] = "H\303\270jre Styrepind H\303\270jre";
static char PcInputText772d14[25] = "H\303\270jre Styrepind Venstre";
static char PcInputText772d30[22] = "Venstre Styrepind Ned";
static char PcInputText772d48[21] = "Venstre Styrepind Op";
static char PcInputText772d60[25] = "Venstre Styrepind H\303\270jre";
static char PcInputText772d7c[26] = "Venstre Styrepind Venstre";
static char PcInputText772d98[9] = "Rot\303\251r Z";
static char PcInputText772da4[9] = "Rot\303\251r Y";
static char PcInputText772db0[10] = "Rot\303\251r X ";
static char PcInputText772dbc[8] = "Z-aksen";
static char PcInputText772dc4[8] = "Y-aksen";
static char PcInputText772dcc[8] = "X-aksen";
static char PcInputText772dd4[5] = "H\303\270j";
static char PcInputText772ddc[5] = "knap";
static char PcInputText772de4[19] = "Hat-switch venstre";
static char PcInputText772df8[15] = "Hat-switch ned";
static char PcInputText772e08[18] = "Hat-switch h\303\270jre";
static char PcInputText772e1c[14] = "Hat-switch Op";
static char PcInputText772eec[7] = "Vac\303\255o";
static char PcInputText772ef4[16] = "Bot\303\263n rat\303\263n N";
static char PcInputText772f04[26] = "Bot\303\263n central del rat\303\263n";
static char PcInputText772f20[26] = "Bot\303\263n derecho del rat\303\263n";
static char PcInputText772f3c[28] = "Bot\303\263n izquierdo del rat\303\263n";
static char PcInputText772f58[30] = "Rueda del rat\303\263n hacia arriba";
static char PcInputText772f78[19] = "Rat\303\263n hacia abajo";
static char PcInputText772f8c[20] = "Rat\303\263n a la derecha";
static char PcInputText772fa0[29] = "Rueda del rat\303\263n hacia abajo";
static char PcInputText772fc0[20] = "Rat\303\263n hacia arriba";
static char PcInputText772fd4[22] = "Rat\303\263n a la izquierda";
static char PcInputText772fec[9] = "Rat\303\263n Z";
static char PcInputText772ff8[9] = "Rat\303\263n Y";
static char PcInputText773004[9] = "Rat\303\263n X";
static char PcInputText773010[7] = "Volver";
static char PcInputText773018[13] = "Bot\303\263n START";
static char PcInputText773028[14] = "Stick derecho";
static char PcInputText773038[16] = "Stick izquierdo";
static char PcInputText773048[25] = "Gatillo inferior derecho";
static char PcInputText773064[27] = "Gatillo inferior izquierdo";
static char PcInputText773080[25] = "Gatillo superior derecho";
static char PcInputText77309c[27] = "Gatillo superior izquierdo";
static char PcInputText7730b8[9] = "Bot\303\263n X";
static char PcInputText7730c4[9] = "Bot\303\263n A";
static char PcInputText7730d0[9] = "Bot\303\263n B";
static char PcInputText7730dc[9] = "Bot\303\263n Y";
static char PcInputText7730e8[20] = "Stick derecho abajo";
static char PcInputText7730fc[21] = "Stick derecho arriba";
static char PcInputText773114[27] = "Stick derecho a la derecha";
static char PcInputText773130[29] = "Stick derecho a la izquierda";
static char PcInputText773150[22] = "Stick izquierdo abajo";
static char PcInputText773168[23] = "Stick izquierdo arriba";
static char PcInputText773180[29] = "Stick izquierdo a la derecha";
static char PcInputText7731a0[31] = "Stick izquierdo a la izquierda";
static char PcInputText7731c0[12] = "Rotaci\303\263n Z";
static char PcInputText7731cc[12] = "Rotaci\303\263n Y";
static char PcInputText7731d8[12] = "Rotaci\303\263n X";
static char PcInputText7731e4[6] = "Eje Z";
static char PcInputText7731ec[6] = "Eje Y";
static char PcInputText7731f4[6] = "Eje X";
static char PcInputText7731fc[19] = "%d (%s) anal\303\263gico";
static char PcInputText773210[5] = "Baja";
static char PcInputText773218[7] = "Bot\303\263n";
static char PcInputText773220[30] = "Mando de direcci\303\263n izquierda";
static char PcInputText773240[26] = "Mando de direcci\303\263n abajo";
static char PcInputText77325c[28] = "Mando de direcci\303\263n derecha";
static char PcInputText773278[27] = "Mando de direcci\303\263n arriba";
static char PcInputText773334[14] = "Keine Eingabe";
static char PcInputText773344[12] = "Maustaste N";
static char PcInputText773350[19] = "Mittlere Maustaste";
static char PcInputText773364[17] = "Rechte Maustaste";
static char PcInputText773378[16] = "Linke Maustaste";
static char PcInputText773388[13] = "Mausrad Oben";
static char PcInputText773398[11] = "Maus Unten";
static char PcInputText7733a4[12] = "Maus Rechts";
static char PcInputText7733b0[14] = "Mausrad Unten";
static char PcInputText7733c0[10] = "Maus Oben";
static char PcInputText7733cc[11] = "Maus Links";
static char PcInputText7733d8[7] = "Maus Z";
static char PcInputText7733e0[7] = "Maus Y";
static char PcInputText7733e8[7] = "Maus X";
static char PcInputText7733f0[8] = "Zur\303\274ck";
static char PcInputText7733f8[14] = "Rechter Stick";
static char PcInputText773408[13] = "Linker Stick";
static char PcInputText773418[17] = "Rechter Schalter";
static char PcInputText77342c[16] = "Linker Schalter";
static char PcInputText77343c[21] = "Rechte Schultertaste";
static char PcInputText773454[20] = "Linke Schultertaste";
static char PcInputText773468[8] = "X-Taste";
static char PcInputText773470[8] = "A-Taste";
static char PcInputText773478[8] = "B-Taste";
static char PcInputText773480[8] = "Y-Taste";
static char PcInputText773488[20] = "Rechter Stick unten";
static char PcInputText77349c[19] = "Rechter Stick oben";
static char PcInputText7734b0[21] = "Rechter Stick rechts";
static char PcInputText7734c8[20] = "Rechter Stick links";
static char PcInputText7734dc[19] = "Linker Stick unten";
static char PcInputText7734f0[18] = "Linker Stick oben";
static char PcInputText773504[20] = "Linker Stick rechts";
static char PcInputText773518[19] = "Linker Stick links";
static char PcInputText77352c[11] = "Z-Rotation";
static char PcInputText773538[11] = "Y-Rotation";
static char PcInputText773544[11] = "X-Rotation";
static char PcInputText773550[8] = "Z-Achse";
static char PcInputText773558[8] = "Y-Achse";
static char PcInputText773560[8] = "X-Achse";
static char PcInputText773568[5] = "Hoch";
static char PcInputText773570[8] = "Niedrig";
static char PcInputText773578[6] = "Taste";
static char PcInputText773580[18] = "Steuerkreuz links";
static char PcInputText773594[18] = "Steuerkreuz unten";
static char PcInputText7735a8[19] = "Steuerkreuz rechts";
static char PcInputText7735bc[17] = "Steuerkreuz oben";
static char PcInputText77367c[13] = "Nessun Input";
static char PcInputText77368c[18] = "Tasto N del mouse";
static char PcInputText7736a0[25] = "Tasto centrale del mouse";
static char PcInputText7736bc[23] = "Tasto destro del mouse";
static char PcInputText7736d4[25] = "Tasto sinistro del mouse";
static char PcInputText7736f0[21] = "Rotella del mouse su";
static char PcInputText773708[11] = "Mouse gi\303\271";
static char PcInputText773714[15] = "Mouse a destra";
static char PcInputText773724[23] = "Rotella del mouse gi\303\271";
static char PcInputText77373c[9] = "Mouse su";
static char PcInputText773748[17] = "Mouse a sinistra";
static char PcInputText77375c[9] = "Indietro";
static char PcInputText773768[24] = "Pulsante levetta destro";
static char PcInputText773780[26] = "Pulsante levetta sinistro";
static char PcInputText77379c[17] = "Grilletto destro";
static char PcInputText7737b0[19] = "Grilletto sinistro";
static char PcInputText7737c4[24] = "Pulsante dorsale destro";
static char PcInputText7737dc[26] = "Pulsante dorsale sinistro";
static char PcInputText7737f8[11] = "Pulsante X";
static char PcInputText773804[11] = "Pulsante A";
static char PcInputText773810[11] = "Pulsante B";
static char PcInputText77381c[11] = "Pulsante Y";
static char PcInputText773828[23] = "Levetta destra in gi\303\271";
static char PcInputText773840[21] = "Levetta destra in su";
static char PcInputText773858[24] = "Levetta destra a destra";
static char PcInputText773870[26] = "Levetta destra a sinistra";
static char PcInputText77388c[25] = "Levetta sinistra in gi\303\271";
static char PcInputText7738a8[23] = "Levetta sinistra in su";
static char PcInputText7738c0[26] = "Levetta sinistra a destra";
static char PcInputText7738dc[28] = "Levetta sinistra a sinistra";
static char PcInputText7738f8[12] = "Rotazione Z";
static char PcInputText773904[12] = "Rotazione Y";
static char PcInputText773910[12] = "Rotazione X";
static char PcInputText77391c[7] = "Asse Z";
static char PcInputText773924[7] = "Asse Y";
static char PcInputText77392c[7] = "Asse X";
static char PcInputText773934[5] = "Alta";
static char PcInputText77393c[6] = "Bassa";
static char PcInputText773944[9] = "Pulsante";
static char PcInputText773950[27] = "Tasto direzionale sinistra";
static char PcInputText77396c[23] = "Tasto direzionale gi\303\271";
static char PcInputText773984[25] = "Tasto direzionale destra";
static char PcInputText7739a0[21] = "Tasto direzionale su";
static char PcInputText773a78[21] = "Aucune touche saisie";
static char PcInputText773a90[16] = "Souris bouton N";
static char PcInputText773aa0[21] = "Bouton souris milieu";
static char PcInputText773ab8[20] = "Bouton souris droit";
static char PcInputText773acc[21] = "Bouton souris gauche";
static char PcInputText773ae4[28] = "Molette souris vers le haut";
static char PcInputText773b00[19] = "Souris vers le bas";
static char PcInputText773b14[22] = "Souris vers la droite";
static char PcInputText773b2c[27] = "Molette souris vers le bas";
static char PcInputText773b48[20] = "Souris vers le haut";
static char PcInputText773b5c[22] = "Souris vers la gauche";
static char PcInputText773b74[9] = "Souris Z";
static char PcInputText773b80[9] = "Souris Y";
static char PcInputText773b8c[9] = "Souris X";
static char PcInputText773b98[7] = "Retour";
static char PcInputText773ba0[13] = "Touche START";
static char PcInputText773bb0[23] = "Stick analogique droit";
static char PcInputText773bc8[24] = "Stick analogique gauche";
static char PcInputText773be0[17] = "G\303\242chette droite";
static char PcInputText773bf4[17] = "G\303\242chette gauche";
static char PcInputText773c08[23] = "G\303\242chette haute droite";
static char PcInputText773c20[23] = "G\303\242chette haute gauche";
static char PcInputText773c38[9] = "Touche X";
static char PcInputText773c44[9] = "Touche A";
static char PcInputText773c50[9] = "Touche B";
static char PcInputText773c5c[9] = "Touche Y";
static char PcInputText773c68[29] = "Stick analogique droit - Bas";
static char PcInputText773c88[30] = "Stick analogique droit - Haut";
static char PcInputText773ca8[32] = "Stick analogique droit - Droite";
static char PcInputText773cc8[32] = "Stick analogique droit - Gauche";
static char PcInputText773ce8[30] = "Stick analogique gauche - Bas";
static char PcInputText773d08[31] = "Stick analogique gauche - Haut";
static char PcInputText773d28[33] = "Stick analogique gauche - Droite";
static char PcInputText773d4c[33] = "Stick analogique gauche - Gauche";
static char PcInputText773d70[11] = "Rotation Z";
static char PcInputText773d7c[11] = "Rotation Y";
static char PcInputText773d88[11] = "Rotation X";
static char PcInputText773d94[6] = "Axe Z";
static char PcInputText773d9c[6] = "Axe Y";
static char PcInputText773da4[6] = "Axe X";
static char PcInputText773dac[19] = "%d (%s) analogique";
static char PcInputText773dc0[6] = "Haute";
static char PcInputText773dc8[6] = "Basse";
static char PcInputText773dd0[7] = "Touche";
static char PcInputText773dd8[11] = "BMD Gauche";
static char PcInputText773de4[8] = "BMD Bas";
static char PcInputText773dec[11] = "BMD Droite";
static char PcInputText773df8[9] = "BMD Haut";
static char PcInputText773f08[12] = "Sem entrada";
static char PcInputText773f14[15] = "Mouse bot\303\243o N";
static char PcInputText773f24[21] = "Mouse bot\303\243o do meio";
static char PcInputText773f3c[21] = "Mouse bot\303\243o direito";
static char PcInputText773f54[22] = "Mouse bot\303\243o esquerdo";
static char PcInputText773f6c[21] = "Mouse roda para cima";
static char PcInputText773f84[17] = "Mouse para baixo";
static char PcInputText773f98[21] = "Mouse para a direita";
static char PcInputText773fb0[22] = "Mouse roda para baixo";
static char PcInputText773fc8[16] = "Mouse para cima";
static char PcInputText773fd8[22] = "Mouse para a esquerda";
static char PcInputText773ff0[8] = "Z Mouse";
static char PcInputText773ff8[8] = "Y Mouse";
static char PcInputText774000[8] = "X Mouse";
static char PcInputText774008[7] = "Voltar";
static char PcInputText774010[8] = "Iniciar";
static char PcInputText774018[19] = "Direcional direito";
static char PcInputText77402c[20] = "Direcional esquerdo";
static char PcInputText774040[16] = "Gatilho direito";
static char PcInputText774050[18] = "Gatilho Inf. Dir.";
static char PcInputText774064[18] = "Gatilho Inf. Esq.";
static char PcInputText774078[9] = "Bot\303\243o X";
static char PcInputText774084[9] = "Bot\303\243o A";
static char PcInputText774090[9] = "Bot\303\243o B";
static char PcInputText77409c[9] = "Bot\303\243o Y";
static char PcInputText7740a8[21] = "Direc. dir. p/ baixo";
static char PcInputText7740c0[20] = "Direc. dir. p/ cima";
static char PcInputText7740d4[20] = "Direc. dir. p/ dir.";
static char PcInputText7740e8[19] = "Direc. dir. p/ esq";
static char PcInputText7740fc[24] = "Deixe o direc. p/ baixo";
static char PcInputText774114[23] = "Deixe o direc. p/ cima";
static char PcInputText77412c[23] = "Deixe o direc. p/ dir.";
static char PcInputText774144[23] = "Deixe o direc. p/ esq.";
static char PcInputText77415c[12] = "Rota\303\247\303\243o Z";
static char PcInputText774168[12] = "Rota\303\247\303\243o Y";
static char PcInputText774174[12] = "Rota\303\247\303\243o X";
static char PcInputText774188[19] = "Anal\303\263gico %d (%s)";
static char PcInputText77419c[5] = "Alto";
static char PcInputText7741a4[6] = "Baixa";
static char PcInputText7741ac[7] = "Bot\303\243o";
static char PcInputText7741b4[15] = "Direc. p/ esq.";
static char PcInputText7741c4[16] = "Direc. p/ baixo";
static char PcInputText7741d4[18] = "Direc. p/ direita";
static char PcInputText7741e8[15] = "Direc. p/ cima";
static char PcInputText7742a8[14] = "Ei sy\303\266tett\303\244";
static char PcInputText7742b8[17] = "Hiiren painike N";
static char PcInputText7742cc[20] = "Hiiren rullapainike";
static char PcInputText7742e0[21] = "Hiiren oikea painike";
static char PcInputText7742f8[21] = "Hiiren vasen painike";
static char PcInputText774310[19] = "Hiiren rulla yl\303\266s";
static char PcInputText774324[11] = "Hiiri alas";
static char PcInputText774330[15] = "Hiiri oikealle";
static char PcInputText774340[18] = "Hiiren rulla alas";
static char PcInputText774354[12] = "Hiiri yl\303\266s";
static char PcInputText774360[17] = "Hiiri vasemmalle";
static char PcInputText774374[16] = "Hiiren Z-akseli";
static char PcInputText774384[16] = "Hiiren Y-akseli";
static char PcInputText774394[16] = "Hiiren X-akseli";
static char PcInputText7743a4[9] = "Takaisin";
static char PcInputText7743b0[7] = "Aloita";
static char PcInputText7743b8[19] = "Oikea peukalosauva";
static char PcInputText7743cc[19] = "Vasen peukalosauva";
static char PcInputText7743e0[12] = "Oikea sauva";
static char PcInputText7743ec[16] = "Vasen liipaisin";
static char PcInputText7743fc[20] = "Oikea olkan\303\244pp\303\244in";
static char PcInputText774410[20] = "Vasen olkan\303\244pp\303\244in";
static char PcInputText774424[12] = "N\303\244pp\303\244in X";
static char PcInputText774430[12] = "N\303\244pp\303\244in A";
static char PcInputText77443c[12] = "N\303\244pp\303\244in B";
static char PcInputText774448[12] = "N\303\244pp\303\244in Y";
static char PcInputText774454[17] = "Oikea sauva alas";
static char PcInputText774468[18] = "Oikea sauva yl\303\266s";
static char PcInputText77447c[18] = "Oikea sauva oikea";
static char PcInputText774490[18] = "Oikea sauva vasen";
static char PcInputText7744a4[17] = "Vasen sauva alas";
static char PcInputText7744b8[18] = "Vasen sauva yl\303\266s";
static char PcInputText7744cc[18] = "Vasen sauva oikea";
static char PcInputText7744e0[18] = "Vasen sauva vasen";
static char PcInputText7744f4[11] = "Z-rotaatio";
static char PcInputText774500[11] = "Y-rotaatio";
static char PcInputText77450c[11] = "X-rotaatio";
static char PcInputText774518[9] = "Z-akseli";
static char PcInputText774524[9] = "Y-akseli";
static char PcInputText774530[9] = "X-akseli";
static char PcInputText77453c[19] = "Analoginen %d (%s)";
static char PcInputText774550[7] = "Korkea";
static char PcInputText774558[7] = "Matala";
static char PcInputText774560[10] = "N\303\244pp\303\244in";
static char PcInputText77456c[12] = "Hattu vasen";
static char PcInputText774578[11] = "Hattu alas";
static char PcInputText774584[12] = "Hattu oikea";
static char PcInputText774590[12] = "Hattu yl\303\266s";
static char PcInputText774664[16] = "Ingen inmatning";
static char PcInputText774674[11] = "Musknapp N";
static char PcInputText774680[21] = "Mittersta musknappen";
static char PcInputText774698[16] = "H\303\266ger musknapp";
static char PcInputText7746a8[18] = "V\303\244nster musknapp";
static char PcInputText7746bc[12] = "Mushjul upp";
static char PcInputText7746c8[11] = "Mus h\303\266ger";
static char PcInputText7746d4[12] = "Mushjul ned";
static char PcInputText7746e0[8] = "Mus upp";
static char PcInputText7746e8[13] = "Mus v\303\244nster";
static char PcInputText7746f8[9] = "Tillbaka";
static char PcInputText774704[16] = "H\303\266ger styrspak";
static char PcInputText774714[18] = "V\303\244nster styrspak";
static char PcInputText774728[18] = "H\303\266ger avtryckare";
static char PcInputText77473c[3] = "LT";
static char PcInputText774740[8] = "R-knapp";
static char PcInputText774748[8] = "L-knapp";
static char PcInputText774750[8] = "Knapp X";
static char PcInputText774758[8] = "Knapp A";
static char PcInputText774760[8] = "Knapp B";
static char PcInputText774768[8] = "Knapp Y";
static char PcInputText774770[16] = "H\303\266ger Spak Ner";
static char PcInputText774780[16] = "H\303\266ger Spak Upp";
static char PcInputText774790[19] = "H\303\266ger Spak H\303\266ger";
static char PcInputText7747a4[21] = "H\303\266ger Spak V\303\244nster";
static char PcInputText7747bc[18] = "V\303\244nster Spak Ner";
static char PcInputText7747d0[18] = "V\303\244nster Spak Upp";
static char PcInputText7747e4[21] = "V\303\244nster Spak H\303\266ger";
static char PcInputText7747fc[23] = "V\303\244nster Spak V\303\244nster";
static char PcInputText774814[11] = "Z-rotation";
static char PcInputText774820[11] = "Y-rotation";
static char PcInputText77482c[11] = "X-rotation";
static char PcInputText774838[7] = "Z-axel";
static char PcInputText774840[7] = "Y-axel";
static char PcInputText774848[7] = "X-axel";
static char PcInputText774850[5] = "H\303\266g";
static char PcInputText774858[5] = "L\303\245g";
static char PcInputText774860[24] = "Riktningsknapp V\303\244nster";
static char PcInputText774878[19] = "Riktningsknapp Ner";
static char PcInputText77488c[22] = "Riktningsknapp H\303\266ger";
static char PcInputText7748a4[19] = "Riktningsknapp Upp";
static char PcInputText774948[12] = "Ingen input";
static char PcInputText774954[12] = "Museknapp N";
static char PcInputText774960[17] = "Midtre museknapp";
static char PcInputText774974[17] = "H\303\270yre museknapp";
static char PcInputText774988[18] = "Venstre museknapp";
static char PcInputText77499c[13] = "Musehjul opp";
static char PcInputText7749ac[8] = "Mus ned";
static char PcInputText7749b4[11] = "Mus h\303\270yre";
static char PcInputText7749c0[13] = "Musehjul ned";
static char PcInputText7749d0[8] = "Mus opp";
static char PcInputText7749d8[12] = "Mus venstre";
static char PcInputText7749e4[6] = "Mus Z";
static char PcInputText7749ec[6] = "Mus Y";
static char PcInputText7749f4[6] = "Mus X";
static char PcInputText7749fc[8] = "Tilbake";
static char PcInputText774a04[12] = "H\303\270yre spak";
static char PcInputText774a10[13] = "Venstre spak";
static char PcInputText774a20[17] = "H\303\270yre avtrekker";
static char PcInputText774a34[18] = "Venstre avtrekker";
static char PcInputText774a48[20] = "H\303\270yre skulderknapp";
static char PcInputText774a5c[21] = "Venstre skulderknapp";
static char PcInputText774a74[8] = "X-knapp";
static char PcInputText774a7c[8] = "A-knapp";
static char PcInputText774a84[8] = "B-knapp";
static char PcInputText774a8c[8] = "Y-knapp";
static char PcInputText774a94[21] = "H\303\270yre styrespak ned";
static char PcInputText774aac[21] = "H\303\270yre styrespak opp";
static char PcInputText774ac4[24] = "H\303\270yre styrespak h\303\270yre";
static char PcInputText774adc[25] = "H\303\270yre styrespak venstre";
static char PcInputText774af8[22] = "Venstre styrespak ned";
static char PcInputText774b10[22] = "Venstre styrespak opp";
static char PcInputText774b28[25] = "Venstre styrespak h\303\270yre";
static char PcInputText774b44[26] = "Venstre styrespak venstre";
static char PcInputText774b60[11] = "Z-rotasjon";
static char PcInputText774b6c[11] = "Y-rotasjon";
static char PcInputText774b78[11] = "X-rotasjon";
static char PcInputText774b84[7] = "Z-akse";
static char PcInputText774b8c[7] = "Y-akse";
static char PcInputText774b94[7] = "X-akse";
static char PcInputText774b9c[15] = "Analog %d (%s)";
static char PcInputText774bac[5] = "H\303\270y";
static char PcInputText774bb4[4] = "Lav";
static char PcInputText774bb8[6] = "Knapp";
static char PcInputText774bc0[19] = "Hatteknapp venstre";
static char PcInputText774bd4[15] = "Hatteknapp ned";
static char PcInputText774be4[18] = "Hatteknapp h\303\270yre";
static char PcInputText774bf8[15] = "Hatteknapp opp";
static char PcInputText774cd8[12] = "Geen invoer";
static char PcInputText774ce4[11] = "Muisknop N";
static char PcInputText774cf0[19] = "Middelste muisknop";
static char PcInputText774d04[16] = "Rechtermuisknop";
static char PcInputText774d14[15] = "Linkermuisknop";
static char PcInputText774d24[16] = "Muiswiel omhoog";
static char PcInputText774d34[12] = "Muis omlaag";
static char PcInputText774d40[12] = "Muis rechts";
static char PcInputText774d4c[16] = "Muiswiel omlaag";
static char PcInputText774d5c[12] = "Muis omhoog";
static char PcInputText774d68[10] = "Muis Left";
static char PcInputText774d74[7] = "Muis Z";
static char PcInputText774d7c[7] = "Muis Y";
static char PcInputText774d84[7] = "Muis X";
static char PcInputText774d8c[6] = "Terug";
static char PcInputText774d94[17] = "Rechter joystick";
static char PcInputText774da8[16] = "Linker joystick";
static char PcInputText774db8[16] = "Rechter trekker";
static char PcInputText774dc8[15] = "Linker Trekker";
static char PcInputText774dd8[17] = "Rechter Schouder";
static char PcInputText774dec[16] = "Linker Schouder";
static char PcInputText774dfc[7] = "Knop X";
static char PcInputText774e04[7] = "Knop A";
static char PcInputText774e0c[7] = "Knop B";
static char PcInputText774e14[7] = "Knop Y";
static char PcInputText774e1c[21] = "Rechter Stick Omlaag";
static char PcInputText774e34[21] = "Rechter Stick Omhoog";
static char PcInputText774e4c[21] = "Rechter Stick Rechts";
static char PcInputText774e64[20] = "Rechter Stick Links";
static char PcInputText774e78[20] = "Linker Stick Omlaag";
static char PcInputText774e8c[20] = "Linker Stick Omhoog";
static char PcInputText774ea0[20] = "Linker Stick Rechts";
static char PcInputText774eb4[19] = "Linker Stick Links";
static char PcInputText774ec8[10] = "Z Rotatie";
static char PcInputText774ed4[10] = "Y Rotatie";
static char PcInputText774ee0[10] = "X Rotatie";
static char PcInputText774eec[5] = "Z As";
static char PcInputText774ef4[5] = "Y As";
static char PcInputText774efc[5] = "X As";
static char PcInputText774f04[16] = "Analoog %d (%s)";
static char PcInputText774f14[5] = "Hoog";
static char PcInputText774f1c[5] = "Laag";
static char PcInputText774f24[5] = "Knop";
static char PcInputText774f2c[19] = "Navigatiepad Links";
static char PcInputText774f40[20] = "Navigatiepad Omlaag";
static char PcInputText774f54[20] = "Navigatiepad Rechts";
static char PcInputText774f68[20] = "Navigatiepad Omhoog";
static char PcInputText775064[13] = "\345\205\245\345\212\233\343\201\252\343\201\227";
static char PcInputText775074[23] = "\343\203\236\343\202\246\343\202\271 \343\203\234\343\202\277\343\203\263 %d";
static char PcInputText77508c[26] =
    "\343\203\236\343\202\246\343\202\271 \344\270\255\345\244\256\343\203\234\343\202\277\343\203\263";
static char PcInputText7750a8[23] =
    "\343\203\236\343\202\246\343\202\271 \345\217\263\343\203\234\343\202\277\343\203\263";
static char PcInputText7750c0[23] =
    "\343\203\236\343\202\246\343\202\271 \345\267\246\343\203\234\343\202\277\343\203\263";
static char PcInputText7750d8[26] =
    "\343\203\236\343\202\246\343\202\271 \343\203\233\343\202\244\343\203\274\343\203\253\344\270\212";
static char PcInputText7750f4[14] = "\343\203\236\343\202\246\343\202\271 \344\270\213";
static char PcInputText775104[14] = "\343\203\236\343\202\246\343\202\271 \345\217\263";
static char PcInputText775114[26] =
    "\343\203\236\343\202\246\343\202\271 \343\203\233\343\202\244\343\203\274\343\203\253\344\270\213";
static char PcInputText775130[14] = "\343\203\236\343\202\246\343\202\271 \344\270\212";
static char PcInputText775140[14] = "\343\203\236\343\202\246\343\202\271 \345\267\246";
static char PcInputText775150[12] = "\343\203\236\343\202\246\343\202\271 Z";
static char PcInputText77515c[12] = "\343\203\236\343\202\246\343\202\271 Y";
static char PcInputText775168[12] = "\343\203\236\343\202\246\343\202\271 X";
static char PcInputText775174[5] = "BACK";
static char PcInputText77517c[26] =
    "\345\217\263 \350\246\252\346\214\207\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257";
static char PcInputText775198[26] =
    "\345\267\246 \350\246\252\346\214\207\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257";
static char PcInputText7751b4[17] = "\345\217\263 \343\203\210\343\203\252\343\202\254\343\203\274";
static char PcInputText7751c8[17] = "\345\267\246 \343\203\210\343\203\252\343\202\254\343\203\274";
static char PcInputText7751dc[3] = "RB";
static char PcInputText7751e0[3] = "LB";
static char PcInputText7751e4[12] = "X \343\203\234\343\202\277\343\203\263";
static char PcInputText7751f0[12] = "A \343\203\234\343\202\277\343\203\263";
static char PcInputText7751fc[12] = "B \343\203\234\343\202\277\343\203\263";
static char PcInputText775208[12] = "Y \343\203\234\343\202\277\343\203\263";
static char PcInputText775214[23] =
    "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\213";
static char PcInputText77522c[23] =
    "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\212";
static char PcInputText775244[23] =
    "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\217\263";
static char PcInputText77525c[23] =
    "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\267\246";
static char PcInputText775274[23] =
    "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\213";
static char PcInputText77528c[23] =
    "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\212";
static char PcInputText7752a4[23] =
    "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\217\263";
static char PcInputText7752bc[23] =
    "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\267\246";
static char PcInputText7752d4[9] = "Z \345\233\236\350\273\242";
static char PcInputText7752e0[9] = "Y \345\233\236\350\273\242";
static char PcInputText7752ec[9] = "X \345\233\236\350\273\242";
static char PcInputText7752f8[6] = "Z \350\273\270";
static char PcInputText775300[6] = "Y \350\273\270";
static char PcInputText775308[6] = "X \350\273\270";
static char PcInputText775310[21] = "\343\202\242\343\203\212\343\203\255\343\202\260 %d (%s)";
static char PcInputText775328[4] = "\351\253\230";
static char PcInputText77532c[4] = "\344\275\216";
static char PcInputText775330[10] = "\343\203\234\343\202\277\343\203\263";
static char PcInputText77533c[14] = "\343\203\217\343\203\203\343\203\210 \345\267\246";
static char PcInputText77534c[14] = "\343\203\217\343\203\203\343\203\210 \344\270\213";
static char PcInputText77535c[14] = "\343\203\217\343\203\203\343\203\210 \345\217\263";
static char PcInputText77536c[14] = "\343\203\217\343\203\203\343\203\210 \344\270\212";
static char PcInputText775418[9] = "No Input";
static char PcInputText775424[16] = "Mouse Button %d";
static char PcInputText775434[20] = "Mouse Middle Button";
static char PcInputText775448[19] = "Mouse Right Button";
static char PcInputText77545c[18] = "Mouse Left Button";
static char PcInputText775470[15] = "Mouse Wheel Up";
static char PcInputText775480[11] = "Mouse Down";
static char PcInputText77548c[12] = "Mouse Right";
static char PcInputText775498[17] = "Mouse Wheel Down";
static char PcInputText7754ac[9] = "Mouse Up";
static char PcInputText7754b8[11] = "Mouse Left";
static char PcInputText7754c4[8] = "Mouse Z";
static char PcInputText7754cc[8] = "Mouse Y";
static char PcInputText7754d4[8] = "Mouse X";
static char PcInputText7754dc[6] = "Start";
static char PcInputText7754e4[18] = "Right Thumb Stick";
static char PcInputText7754f8[17] = "Left Thumb Stick";
static char PcInputText77550c[14] = "Right Trigger";
static char PcInputText77551c[13] = "Left Trigger";
static char PcInputText77552c[15] = "Right Shoulder";
static char PcInputText77553c[14] = "Left Shoulder";
static char PcInputText77554c[9] = "Button X";
static char PcInputText775558[9] = "Button A";
static char PcInputText775564[9] = "Button B";
static char PcInputText775570[9] = "Button Y";
static char PcInputText77557c[17] = "Right Stick Down";
static char PcInputText775590[15] = "Right Stick Up";
static char PcInputText7755a0[18] = "Right Stick Right";
static char PcInputText7755b4[17] = "Right Stick Left";
static char PcInputText7755c8[16] = "Left Stick Down";
static char PcInputText7755d8[14] = "Left Stick Up";
static char PcInputText7755e8[17] = "Left Stick Right";
static char PcInputText7755fc[16] = "Left Stick Left";
static char PcInputText77560c[11] = "Z Rotation";
static char PcInputText775618[11] = "Y Rotation";
static char PcInputText775624[11] = "X Rotation";
static char PcInputText775630[7] = "Z Axis";
static char PcInputText775638[7] = "Y Axis";
static char PcInputText775640[7] = "X Axis";
static char PcInputText775648[17] = "Analogue %d (%s)";
static char PcInputText77565c[5] = "High";
static char PcInputText775664[4] = "Low";
static char PcInputText775668[7] = "Button";
static char PcInputText775670[9] = "Hat Left";
static char PcInputText77567c[9] = "Hat Down";
static char PcInputText775688[10] = "Hat Right";
static char PcInputText775694[7] = "Hat Up";
static char PcInputText775eb4[10] = "GameTitle";
static char PcInputText775ec0[7] = "TTales";

const char *PcCompanyName = PcInputText775ec0;
const char *PcProductName = PcInputText775eb4;
i32 PcUsePersonalFolder = 1;
void(__cdecl *PcInputStateInitializedCallback)(PC_INPUT_RUNTIME_PC *) = NULL;
char PcUserFolder[260] = {};
char PcFileDirectory[128] = {};
char PcFilePath[128] = {};
TEXT_INPUT_LABELS_PC PcInputLabels820560 = {
    {{PcInputText77536c, PcInputText77535c, PcInputText77534c, PcInputText77533c, PcInputText775330, PcInputText77532c,
      PcInputText775328, PcInputText775310, PcInputText775308, PcInputText775300, PcInputText7752f8, PcInputText7752ec,
      PcInputText7752e0, PcInputText7752d4, PcInputText7752bc, PcInputText7752a4, PcInputText77528c, PcInputText775274,
      PcInputText77525c, PcInputText775244, PcInputText77522c, PcInputText775214, PcInputText775208, PcInputText7751fc,
      PcInputText7751f0, PcInputText7751e4, PcInputText7751e0, PcInputText7751dc, PcInputText7751c8, PcInputText7751b4,
      PcInputText775198, PcInputText77517c, PcInputText76fe70, PcInputText775174, PcInputText775168, PcInputText77515c,
      PcInputText775150}},
    {PcInputText775140, PcInputText775130, PcInputText775114},
    {PcInputText775104, PcInputText7750f4, PcInputText7750d8},
    {PcInputText7750c0, PcInputText7750a8, PcInputText77508c},
    PcInputText775074,
    PcInputText775064};
TEXT_INPUT_LABELS_PC PcInputLabels820480 = {
    {{PcInputText775694, PcInputText775688, PcInputText77567c, PcInputText775670,     PcInputText775668,
      PcInputText775664, PcInputText77565c, PcInputText775648, PcInputText775640,     PcInputText775638,
      PcInputText775630, PcInputText775624, PcInputText775618, PcInputText77560c,     PcInputText7755fc,
      PcInputText7755e8, PcInputText7755d8, PcInputText7755c8, PcInputText7755b4,     PcInputText7755a0,
      PcInputText775590, PcInputText77557c, PcInputText775570, PcInputText775564,     PcInputText775558,
      PcInputText77554c, PcInputText77553c, PcInputText77552c, PcInputText77551c,     PcInputText77550c,
      PcInputText7754f8, PcInputText7754e4, PcInputText7754dc, UiBackPromptLiteralPc, PcInputText7754d4,
      PcInputText7754cc, PcInputText7754c4}},
    {PcInputText7754b8, PcInputText7754ac, PcInputText775498},
    {PcInputText77548c, PcInputText775480, PcInputText775470},
    {PcInputText77545c, PcInputText775448, PcInputText775434},
    PcInputText775424,
    PcInputText775418};
TEXT_INPUT_LABELS_PC PcInputLabels820aa0 = {
    {{PcInputText773df8, PcInputText773dec, PcInputText773de4, PcInputText773dd8, PcInputText773dd0, PcInputText773dc8,
      PcInputText773dc0, PcInputText773dac, PcInputText773da4, PcInputText773d9c, PcInputText773d94, PcInputText773d88,
      PcInputText773d7c, PcInputText773d70, PcInputText773d4c, PcInputText773d28, PcInputText773d08, PcInputText773ce8,
      PcInputText773cc8, PcInputText773ca8, PcInputText773c88, PcInputText773c68, PcInputText773c5c, PcInputText773c50,
      PcInputText773c44, PcInputText773c38, PcInputText773c20, PcInputText773c08, PcInputText773bf4, PcInputText773be0,
      PcInputText773bc8, PcInputText773bb0, PcInputText773ba0, PcInputText773b98, PcInputText773b8c, PcInputText773b80,
      PcInputText773b74}},
    {PcInputText773b5c, PcInputText773b48, PcInputText773b2c},
    {PcInputText773b14, PcInputText773b00, PcInputText773ae4},
    {PcInputText773acc, PcInputText773ab8, PcInputText773aa0},
    PcInputText773a90,
    PcInputText773a78};
TEXT_INPUT_LABELS_PC PcInputLabels820d40 = {
    {{PcInputText773278, PcInputText77325c, PcInputText773240, PcInputText773220, PcInputText773218, PcInputText773210,
      PcInputText77419c, PcInputText7731fc, PcInputText7731f4, PcInputText7731ec, PcInputText7731e4, PcInputText7731d8,
      PcInputText7731cc, PcInputText7731c0, PcInputText7731a0, PcInputText773180, PcInputText773168, PcInputText773150,
      PcInputText773130, PcInputText773114, PcInputText7730fc, PcInputText7730e8, PcInputText7730dc, PcInputText7730d0,
      PcInputText7730c4, PcInputText7730b8, PcInputText77309c, PcInputText773080, PcInputText773064, PcInputText773048,
      PcInputText773038, PcInputText773028, PcInputText773018, PcInputText773010, PcInputText773004, PcInputText772ff8,
      PcInputText772fec}},
    {PcInputText772fd4, PcInputText772fc0, PcInputText772fa0},
    {PcInputText772f8c, PcInputText772f78, PcInputText772f58},
    {PcInputText772f3c, PcInputText772f20, PcInputText772f04},
    PcInputText772ef4,
    PcInputText772eec};
TEXT_INPUT_LABELS_PC PcInputLabels820c60 = {
    {{PcInputText7735bc, PcInputText7735a8, PcInputText773594, PcInputText773580, PcInputText773578, PcInputText773570,
      PcInputText773568, PcInputText774b9c, PcInputText773560, PcInputText773558, PcInputText773550, PcInputText773544,
      PcInputText773538, PcInputText77352c, PcInputText773518, PcInputText773504, PcInputText7734f0, PcInputText7734dc,
      PcInputText7734c8, PcInputText7734b0, PcInputText77349c, PcInputText773488, PcInputText773480, PcInputText773478,
      PcInputText773470, PcInputText773468, PcInputText773454, PcInputText77343c, PcInputText77342c, PcInputText773418,
      PcInputText773408, PcInputText7733f8, PcInputText7754dc, PcInputText7733f0, PcInputText7733e8, PcInputText7733e0,
      PcInputText7733d8}},
    {PcInputText7733cc, PcInputText7733c0, PcInputText7733b0},
    {PcInputText7733a4, PcInputText773398, PcInputText773388},
    {PcInputText773378, PcInputText773364, PcInputText773350},
    PcInputText773344,
    PcInputText773334};
TEXT_INPUT_LABELS_PC PcInputLabels820b80 = {
    {{PcInputText7739a0, PcInputText773984, PcInputText77396c, PcInputText773950, PcInputText773944, PcInputText77393c,
      PcInputText773934, PcInputText775648, PcInputText77392c, PcInputText773924, PcInputText77391c, PcInputText773910,
      PcInputText773904, PcInputText7738f8, PcInputText7738dc, PcInputText7738c0, PcInputText7738a8, PcInputText77388c,
      PcInputText773870, PcInputText773858, PcInputText773840, PcInputText773828, PcInputText77381c, PcInputText773810,
      PcInputText773804, PcInputText7737f8, PcInputText7737dc, PcInputText7737c4, PcInputText7737b0, PcInputText77379c,
      PcInputText773780, PcInputText773768, PcInputText7754dc, PcInputText77375c, PcInputText7754d4, PcInputText7754cc,
      PcInputText7754c4}},
    {PcInputText773748, PcInputText77373c, PcInputText773724},
    {PcInputText773714, PcInputText773708, PcInputText7736f0},
    {PcInputText7736d4, PcInputText7736bc, PcInputText7736a0},
    PcInputText77368c,
    PcInputText77367c};
TEXT_INPUT_LABELS_PC PcInputLabels820640 = {
    {{PcInputText774f68, PcInputText774f54, PcInputText774f40, PcInputText774f2c, PcInputText774f24, PcInputText774f1c,
      PcInputText774f14, PcInputText774f04, PcInputText774efc, PcInputText774ef4, PcInputText774eec, PcInputText774ee0,
      PcInputText774ed4, PcInputText774ec8, PcInputText774eb4, PcInputText774ea0, PcInputText774e8c, PcInputText774e78,
      PcInputText774e64, PcInputText774e4c, PcInputText774e34, PcInputText774e1c, PcInputText774e14, PcInputText774e0c,
      PcInputText774e04, PcInputText774dfc, PcInputText774dec, PcInputText774dd8, PcInputText774dc8, PcInputText774db8,
      PcInputText774da8, PcInputText774d94, PcInputText7754dc, PcInputText774d8c, PcInputText774d84, PcInputText774d7c,
      PcInputText774d74}},
    {PcInputText774d68, PcInputText774d5c, PcInputText774d4c},
    {PcInputText774d40, PcInputText774d34, PcInputText774d24},
    {PcInputText774d14, PcInputText774d04, PcInputText774cf0},
    PcInputText774ce4,
    PcInputText774cd8};
TEXT_INPUT_LABELS_PC PcInputLabels8209c0 = {{{PcInputText7741e8,
                                              PcInputText7741d4,
                                              PcInputText7741c4,
                                              PcInputText7741b4,
                                              PcInputText7741ac,
                                              PcInputText7741a4,
                                              PcInputText77419c,
                                              PcInputText774188,
                                              TextInputKeyNameTextPc759770,
                                              TextInputKeyNameTextPc774184,
                                              TextInputKeyNameTextPc774180,
                                              PcInputText774174,
                                              PcInputText774168,
                                              PcInputText77415c,
                                              PcInputText774144,
                                              PcInputText77412c,
                                              PcInputText774114,
                                              PcInputText7740fc,
                                              PcInputText7740e8,
                                              PcInputText7740d4,
                                              PcInputText7740c0,
                                              PcInputText7740a8,
                                              PcInputText77409c,
                                              PcInputText774090,
                                              PcInputText774084,
                                              PcInputText774078,
                                              PcInputText774064,
                                              PcInputText774050,
                                              PcInputText774064,
                                              PcInputText774040,
                                              PcInputText77402c,
                                              PcInputText774018,
                                              PcInputText774010,
                                              PcInputText774008,
                                              PcInputText774000,
                                              PcInputText773ff8,
                                              PcInputText773ff0}},
                                            {PcInputText773fd8, PcInputText773fc8, PcInputText773fb0},
                                            {PcInputText773f98, PcInputText773f84, PcInputText773f6c},
                                            {PcInputText773f54, PcInputText773f3c, PcInputText773f24},
                                            PcInputText773f14,
                                            PcInputText773f08};
TEXT_INPUT_LABELS_PC PcInputLabels820e20 = {
    {{PcInputText772e1c, PcInputText772e08, PcInputText772df8, PcInputText772de4, PcInputText772ddc, PcInputText774bb4,
      PcInputText772dd4, PcInputText774b9c, PcInputText772dcc, PcInputText772dc4, PcInputText772dbc, PcInputText772db0,
      PcInputText772da4, PcInputText772d98, PcInputText772d7c, PcInputText772d60, PcInputText772d48, PcInputText772d30,
      PcInputText772d14, PcInputText772cfc, PcInputText772ce8, PcInputText772cd0, PcInputText772cc8, PcInputText772cc0,
      PcInputText772cb8, PcInputText772cb0, PcInputText772c9c, PcInputText772c88, PcInputText772c74, PcInputText772c88,
      PcInputText772c60, PcInputText772c4c, PcInputText7754dc, PcInputText772c44, PcInputText772c3c, PcInputText7749ec,
      PcInputText7749e4}},
    {PcInputText7749d8, PcInputText772c34, PcInputText7746d4},
    {PcInputText772c28, PcInputText7749ac, PcInputText772c1c},
    {PcInputText772c08, PcInputText772bf8, PcInputText772be4},
    PcInputText772bd8,
    PcInputText774948};
TEXT_INPUT_LABELS_PC PcInputLabels8210c0 = {
    {{PcInputText772150, PcInputText772138, PcInputText772120, PcInputText772108, PcInputText7720fc, PcInputText7720f0,
      PcInputText7720e4, PcInputText774b9c, PcInputText7720dc, PcInputText7720d4, PcInputText7720cc, PcInputText7720c0,
      PcInputText7720b4, PcInputText7720a8, PcInputText772094, PcInputText77207c, PcInputText772064, PcInputText772050,
      PcInputText772038, PcInputText772020, PcInputText772008, PcInputText771ff0, PcInputText771fe0, PcInputText771fd0,
      PcInputText771fc0, PcInputText771fb0, PcInputText771f94, PcInputText771f78, PcInputText771f68, PcInputText771f58,
      PcInputText771f48, PcInputText771f38, PcInputText7754dc, PcInputText771f30, PcInputText771f28, PcInputText7720d4,
      PcInputText7720cc}},
    {PcInputText771f14, PcInputText771f00, PcInputText771ee8},
    {PcInputText771ed4, PcInputText771ec0, PcInputText771ea8},
    {PcInputText771e90, PcInputText771e78, PcInputText771e5c},
    PcInputText771e48,
    PcInputText771e38};
TEXT_INPUT_LABELS_PC PcInputLabels820f00 = {
    {{PcInputText772b34, PcInputText772b20, PcInputText772b0c, PcInputText772afc, PcInputText772af0, PcInputText772ae8,
      PcInputText772ae0, PcInputText772acc, PcInputText772ac4, PcInputText772abc, PcInputText772ab4, PcInputText772aa4,
      PcInputText772a94, PcInputText772a84, PcInputText772a6c, PcInputText772a54, PcInputText772a3c, PcInputText772a24,
      PcInputText772a0c, PcInputText7729f4, PcInputText7729dc, PcInputText7729c4, PcInputText7729b8, PcInputText7729ac,
      PcInputText7729a0, PcInputText772994, PcInputText77297c, PcInputText772964, PcInputText772958, PcInputText77294c,
      PcInputText77293c, PcInputText77292c, PcInputText7754dc, PcInputText772924, PcInputText772918, PcInputText77290c,
      PcInputText772900}},
    {PcInputText7728f0, PcInputText7728e0, PcInputText7728c8},
    {PcInputText7728e0, PcInputText7728b8, PcInputText7728a0},
    {PcInputText772888, PcInputText772870, PcInputText772854},
    PcInputText772840,
    PcInputText772830};
TEXT_INPUT_LABELS_PC PcInputLabels820fe0 = {
    {{PcInputText772760, PcInputText772750, PcInputText772744, PcInputText772738, PcInputText772728, PcInputText772718,
      PcInputText772708, PcInputText7726e8, PcInputText7726dc, PcInputText7726d0, PcInputText7726c4, PcInputText7726ac,
      PcInputText772694, PcInputText77267c, PcInputText772650, PcInputText772624, PcInputText7725f8, PcInputText7725d0,
      PcInputText7725a4, PcInputText772574, PcInputText772548, PcInputText77251c, PcInputText77250c, PcInputText7724fc,
      PcInputText7724ec, PcInputText7724dc, PcInputText7724c0, PcInputText7724a4, PcInputText77248c, PcInputText772474,
      PcInputText772458, PcInputText772438, PcInputText77242c, PcInputText772420, PcInputText77240c, PcInputText7723f8,
      PcInputText7723e4}},
    {PcInputText7723d0, PcInputText7723bc, PcInputText77239c},
    {PcInputText772384, PcInputText772370, PcInputText77234c},
    {PcInputText772328, PcInputText772304, PcInputText7722dc},
    PcInputText7722c4,
    PcInputText77229c};
TEXT_INPUT_LABELS_PC PcInputLabels820800 = {
    {{PcInputText7748a4, PcInputText77488c, PcInputText774878, PcInputText774860, PcInputText774bb8, PcInputText774858,
      PcInputText774850, PcInputText774b9c, PcInputText774848, PcInputText774840, PcInputText774838, PcInputText77482c,
      PcInputText774820, PcInputText774814, PcInputText7747fc, PcInputText7747e4, PcInputText7747d0, PcInputText7747bc,
      PcInputText7747a4, PcInputText774790, PcInputText774780, PcInputText774770, PcInputText774768, PcInputText774760,
      PcInputText774758, PcInputText774750, PcInputText774748, PcInputText774740, PcInputText77473c, PcInputText774728,
      PcInputText774714, PcInputText774704, PcInputText7754dc, PcInputText7746f8, PcInputText7749f4, PcInputText7749ec,
      PcInputText7749e4}},
    {PcInputText7746e8, PcInputText7746e0, PcInputText7746d4},
    {PcInputText7746c8, PcInputText7749ac, PcInputText7746bc},
    {PcInputText7746a8, PcInputText774698, PcInputText774680},
    PcInputText774674,
    PcInputText774664};
TEXT_INPUT_LABELS_PC PcInputLabels820720 = {
    {{PcInputText774bf8, PcInputText774be4, PcInputText774bd4, PcInputText774bc0, PcInputText774bb8, PcInputText774bb4,
      PcInputText774bac, PcInputText774b9c, PcInputText774b94, PcInputText774b8c, PcInputText774b84, PcInputText774b78,
      PcInputText774b6c, PcInputText774b60, PcInputText774b44, PcInputText774b28, PcInputText774b10, PcInputText774af8,
      PcInputText774adc, PcInputText774ac4, PcInputText774aac, PcInputText774a94, PcInputText774a8c, PcInputText774a84,
      PcInputText774a7c, PcInputText774a74, PcInputText774a5c, PcInputText774a48, PcInputText774a34, PcInputText774a20,
      PcInputText774a10, PcInputText774a04, PcInputText7754dc, PcInputText7749fc, PcInputText7749f4, PcInputText7749ec,
      PcInputText7749e4}},
    {PcInputText7749d8, PcInputText7749d0, PcInputText7749c0},
    {PcInputText7749b4, PcInputText7749ac, PcInputText77499c},
    {PcInputText774988, PcInputText774974, PcInputText774960},
    PcInputText774954,
    PcInputText774948};
TEXT_INPUT_LABELS_PC PcInputLabels8208e0 = {
    {{PcInputText774590, PcInputText774584, PcInputText774578, PcInputText77456c, PcInputText774560, PcInputText774558,
      PcInputText774550, PcInputText77453c, PcInputText774530, PcInputText774524, PcInputText774518, PcInputText77450c,
      PcInputText774500, PcInputText7744f4, PcInputText7744e0, PcInputText7744cc, PcInputText7744b8, PcInputText7744a4,
      PcInputText774490, PcInputText77447c, PcInputText774468, PcInputText774454, PcInputText774448, PcInputText77443c,
      PcInputText774430, PcInputText774424, PcInputText774410, PcInputText7743fc, PcInputText7743ec, PcInputText7743e0,
      PcInputText7743cc, PcInputText7743b8, PcInputText7743b0, PcInputText7743a4, PcInputText774394, PcInputText774384,
      PcInputText774374}},
    {PcInputText774360, PcInputText774354, PcInputText774340},
    {PcInputText774330, PcInputText774324, PcInputText774310},
    {PcInputText7742f8, PcInputText7742e0, PcInputText7742cc},
    PcInputText7742b8,
    PcInputText7742a8};
char *PcInputKeyNames821910[238] = {TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    TextInputKeyNameTextPc771d50,
                                    TextInputKeyNameTextPc771d48,
                                    TextInputKeyNameTextPc771d3c,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    TextInputKeyNameTextPc753374,
                                    TextInputKeyNameTextPc753370,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText771990,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText771978,
                                    PcInputText77195c,
                                    PcInputText77194c,
                                    PcInputText771940,
                                    (char *)WorldPathSeparator,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText771930,
                                    PcInputText771920,
                                    PcInputText7718fc,
                                    PcInputText7718f0,
                                    PcInputText7718dc,
                                    PcInputText7718d4,
                                    PcInputText7718c4,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    PcInputText7718b0,
                                    PcInputText77189c,
                                    PcInputText771888,
                                    PcInputText771874,
                                    PcInputText771860,
                                    PcInputText77184c,
                                    PcInputText771838,
                                    PcInputText771824,
                                    PcInputText771810,
                                    PcInputText7717fc,
                                    PcInputText7717e8,
                                    PcInputText7717d4,
                                    PcInputText7717c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7719a8,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7717a8,
                                    PcInputText77179c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText771788,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771774,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    PcInputText77176c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText771760,
                                    PcInputText771758,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77174c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771740,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText771734,
                                    PcInputText771728,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771710,
                                    PcInputText7716f8,
                                    PcInputText7716dc,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames822080[238] = {TextInputKeyNameTextPc771d80,
                                    PcInputText77154c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText755d70,
                                    PcInputText771544,
                                    PcInputText773b98,
                                    PcInputText771538,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText771534,
                                    PcInputText771530,
                                    PcInputText771528,
                                    PcInputText771520,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText77151c,
                                    PcInputText771518,
                                    PcInputText771510,
                                    PcInputText77150c,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    PcInputText771504,
                                    PcInputText7714f4,
                                    PcInputText7714e8,
                                    PcInputText7714d4,
                                    PcInputText7714cc,
                                    PcInputText7714c0,
                                    PcInputText7714b8,
                                    PcInputText7714b0,
                                    PcInputText7714a4,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    PcInputText771498,
                                    PcInputText771488,
                                    PcInputText77147c,
                                    PcInputText771470,
                                    PcInputText771464,
                                    PcInputText771458,
                                    PcInputText77144c,
                                    PcInputText771440,
                                    PcInputText771434,
                                    PcInputText771428,
                                    PcInputText77141c,
                                    PcInputText771410,
                                    PcInputText771404,
                                    PcInputText7713f8,
                                    PcInputText7713e8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713e4,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713d8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    PcInputText7713d4,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText7713d0,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713bc,
                                    PcInputText7713b4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713b0,
                                    PcInputText77139c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771390,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77137c,
                                    PcInputText771374,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77136c,
                                    PcInputText771364,
                                    PcInputText771358,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771350,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771348,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771344,
                                    PcInputText771340,
                                    PcInputText771334,
                                    PcInputText77132c,
                                    PcInputText771324,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771318,
                                    PcInputText77130c,
                                    TextInputKeyNameTextPc771a38,
                                    PcInputText7716d4,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText771300,
                                    PcInputText7712fc,
                                    PcInputText77375c,
                                    TextInputKeyNameTextPc771d38};
char *PcInputKeyNames822ae8[238] = {PcInputText770f6c,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText770f68,
                                    PcInputText770f64,
                                    PcInputText770f58,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText770f54,
                                    PcInputText7712f4,
                                    PcInputText770f4c,
                                    PcInputText770f40,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText770f3c,
                                    PcInputText7710c0,
                                    PcInputText770f38,
                                    PcInputText770f2c,
                                    PcInputText770f28,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText770f20,
                                    PcInputText7712b8,
                                    PcInputText770f18,
                                    PcInputText770f0c,
                                    PcInputText770f04,
                                    PcInputText770ef8,
                                    PcInputText770ef0,
                                    PcInputText770ee4,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    PcInputText770ed8,
                                    PcInputText770ecc,
                                    PcInputText770ec4,
                                    PcInputText770ebc,
                                    PcInputText770eb4,
                                    PcInputText770eac,
                                    PcInputText770ea4,
                                    PcInputText770e9c,
                                    PcInputText770e94,
                                    PcInputText770e8c,
                                    PcInputText770e84,
                                    PcInputText770e7c,
                                    PcInputText770e74,
                                    PcInputText770e6c,
                                    PcInputText770e60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713e4,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770e54,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770e48,
                                    PcInputText770e40,
                                    PcInputText770e34,
                                    PcInputText770e28,
                                    TextInputKeyNameTextPc771b2c,
                                    PcInputText770e20,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770e14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770e08,
                                    PcInputText770dfc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770df0,
                                    TextInputKeyNameTextPc771ae4,
                                    PcInputText770de8,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770e20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770ddc,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770dd0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770dc4,
                                    PcInputText770db8,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770db0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770da8,
                                    PcInputText770d9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771154,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770d94,
                                    PcInputText770d88,
                                    PcInputText770d80,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770d74,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770d68,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771344,
                                    PcInputText770d58,
                                    PcInputText770d50,
                                    TextInputKeyNameTextPc771a50,
                                    PcInputText770d48,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770d3c,
                                    PcInputText770d30,
                                    TextInputKeyNameTextPc771a38,
                                    PcInputText770d28,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7712f4,
                                    PcInputText770d20,
                                    PcInputText772c44,
                                    TextInputKeyNameTextPc771d38};
char *PcInputKeyNames822770[238] = {PcInputText7710c8,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7710c4,
                                    PcInputText7710c0,
                                    PcInputText7710b4,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText7710b0,
                                    PcInputText7712f4,
                                    PcInputText7710a8,
                                    PcInputText7710a0,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText77109c,
                                    PcInputText771098,
                                    PcInputText771534,
                                    PcInputText77108c,
                                    TextInputKeyNameTextPc771cb0,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText771088,
                                    PcInputText75eba8,
                                    PcInputText7712b4,
                                    PcInputText77107c,
                                    TextInputKeyNameTextPc771c6c,
                                    PcInputText771074,
                                    PcInputText771068,
                                    PcInputText771058,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    PcInputText77104c,
                                    PcInputText771044,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText7715c8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713e4,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    PcInputText771040,
                                    PcInputText77103c,
                                    (char *)Text_UnknownText,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    PcInputText771030,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771020,
                                    PcInputText771018,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText771010,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771008,
                                    PcInputText771000,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770ff8,
                                    PcInputText770fec,
                                    PcInputText770fe0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770fd4,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770fc4,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770fbc,
                                    PcInputText770fb0,
                                    PcInputText770fa8,
                                    PcInputText770fa0,
                                    PcInputText770f98,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770f88,
                                    PcInputText770f78,
                                    TextInputKeyNameTextPc771a38,
                                    PcInputText770f6c,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText770f68,
                                    PcInputText770f64,
                                    PcInputText770f58,
                                    TextInputKeyNameTextPc771d38};
char *PcInputKeyNames8223f8[238] = {PcInputText7716d4,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText771300,
                                    PcInputText7712fc,
                                    PcInputText77375c,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText7712f8,
                                    PcInputText7712f4,
                                    PcInputText7712ec,
                                    PcInputText7712e0,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText7712dc,
                                    PcInputText7712d8,
                                    (char *)WorldPathSeparator,
                                    PcInputText7712cc,
                                    PcInputText7712c8,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText7712c0,
                                    PcInputText7712b8,
                                    PcInputText7712b4,
                                    PcInputText7712a8,
                                    PcInputText77129c,
                                    PcInputText771290,
                                    PcInputText771288,
                                    PcInputText77127c,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    PcInputText771270,
                                    PcInputText771260,
                                    PcInputText771254,
                                    PcInputText771248,
                                    PcInputText77123c,
                                    PcInputText771230,
                                    PcInputText771224,
                                    PcInputText771218,
                                    PcInputText77120c,
                                    PcInputText771200,
                                    PcInputText7711f4,
                                    PcInputText7711e8,
                                    PcInputText7711dc,
                                    PcInputText7711d0,
                                    PcInputText7711c4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713e4,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7711b8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    PcInputText75c044,
                                    (char *)PermLegalSeparator,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7711a8,
                                    PcInputText77119c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771190,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771188,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText77117c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771170,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771168,
                                    PcInputText77115c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771154,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText771148,
                                    PcInputText771140,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771130,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771120,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771118,
                                    PcInputText771108,
                                    PcInputText7710fc,
                                    PcInputText7710f0,
                                    PcInputText7710e8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7710dc,
                                    PcInputText7710d0,
                                    TextInputKeyNameTextPc771a38,
                                    PcInputText7710c8,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7710c4,
                                    PcInputText7710c0,
                                    PcInputText7710b4,
                                    TextInputKeyNameTextPc771d38};
char *PcInputKeyNames8231d8[238] = {TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText76afa8,
                                    PcInputText770bc8,
                                    TextInputKeyNameTextPc771d3c,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText770d18,
                                    PcInputText77150c,
                                    TextInputKeyNameTextPc771d0c,
                                    TextInputKeyNameTextPc771d04,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText7712f4,
                                    PcInputText77168c,
                                    PcInputText770bc4,
                                    TextInputKeyNameTextPc771cb4,
                                    TextInputKeyNameTextPc771cb0,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText770cf0,
                                    PcInputText770bbc,
                                    TextInputKeyNameTextPc771d50,
                                    TextInputKeyNameTextPc771c74,
                                    TextInputKeyNameTextPc771c6c,
                                    TextInputKeyNameTextPc771c64,
                                    PcInputText770bb4,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText770ba8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc753374,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    TextInputKeyNameTextPc771af4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText770c2c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    TextInputKeyNameTextPc771a94,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText770ba0,
                                    PcInputText771584,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770b98,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText770b88,
                                    PcInputText771564,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77155c,
                                    PcInputText770b80,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames8240b8[238] = {TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText770f18,
                                    PcInputText7708a4,
                                    PcInputText770898,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText77088c,
                                    PcInputText770878,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText77086c,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText770f28,
                                    PcInputText770868,
                                    PcInputText770858,
                                    PcInputText77084c,
                                    PcInputText770838,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText77082c,
                                    PcInputText770824,
                                    PcInputText77080c,
                                    PcInputText770800,
                                    TextInputKeyNameTextPc771c6c,
                                    PcInputText7707f4,
                                    PcInputText7707e0,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText7707d4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    (char *)WorldPathSeparator,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    PcInputText7707c8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText770750,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770748,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    PcInputText7707bc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771154,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText7707ac,
                                    PcInputText771584,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770798,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770784,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText770774,
                                    PcInputText771564,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770768,
                                    PcInputText77075c,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames822e60[238] = {PcInputText770d28,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7712f4,
                                    PcInputText770d20,
                                    PcInputText772c44,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText770d1c,
                                    PcInputText770d18,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText770d10,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText770d0c,
                                    PcInputText770d08,
                                    PcInputText770d04,
                                    PcInputText770cf8,
                                    PcInputText77168c,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText770cf0,
                                    PcInputText770ce8,
                                    TextInputKeyNameTextPc771d50,
                                    PcInputText770cdc,
                                    TextInputKeyNameTextPc771c6c,
                                    PcInputText770cd4,
                                    PcInputText770cc4,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText770cb4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7713e4,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770ca4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    PcInputText770c94,
                                    PcInputText770c88,
                                    PcInputText770c74,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    PcInputText770c64,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770c58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    PcInputText770c50,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    PcInputText770c48,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770c40,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770c38,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText770c2c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770c1c,
                                    PcInputText770c14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText770c0c,
                                    PcInputText771758,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770c00,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770bf4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText770bec,
                                    PcInputText771728,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770bdc,
                                    PcInputText770bcc,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText76afa8,
                                    PcInputText770bc8,
                                    TextInputKeyNameTextPc771d3c,
                                    TextInputKeyNameTextPc771d38};
char *PcInputKeyNames821cc8[238] = {PcInputText7716d4,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7716cc,
                                    PcInputText7716c0,
                                    TextInputKeyNameTextPc771d3c,
                                    PcInputText7716b4,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText7716b0,
                                    PcInputText7716a4,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText77169c,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText771698,
                                    PcInputText77168c,
                                    PcInputText771684,
                                    PcInputText77167c,
                                    PcInputText771670,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText771668,
                                    PcInputText771660,
                                    PcInputText771654,
                                    PcInputText77164c,
                                    PcInputText771644,
                                    PcInputText77163c,
                                    PcInputText771630,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    PcInputText771628,
                                    PcInputText771620,
                                    PcInputText771618,
                                    PcInputText771610,
                                    PcInputText771608,
                                    PcInputText771600,
                                    PcInputText7715f8,
                                    PcInputText7715f0,
                                    PcInputText7715e8,
                                    PcInputText7715e0,
                                    PcInputText7715d8,
                                    PcInputText7715d0,
                                    PcInputText7715c8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    (char *)WorldPathSeparator,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7715b8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7715ac,
                                    PcInputText7715a4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText77199c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77159c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    PcInputText771594,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText77158c,
                                    PcInputText771584,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77157c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771574,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText77156c,
                                    PcInputText771564,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77155c,
                                    PcInputText771554,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames823590[238] = {TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7712b4,
                                    "=",
                                    TextInputKeyNameTextPc771d3c,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    TextInputKeyNameTextPc753374,
                                    TextInputKeyNameTextPc753370,
                                    TextInputKeyNameTextPc771d0c,
                                    TextInputKeyNameTextPc771d04,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText770b7c,
                                    PcInputText770f68,
                                    PcInputText770b74,
                                    TextInputKeyNameTextPc771cb4,
                                    TextInputKeyNameTextPc771cb0,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText771088,
                                    PcInputText75eba8,
                                    PcInputText76afa8,
                                    PcInputText770b68,
                                    TextInputKeyNameTextPc771c6c,
                                    TextInputKeyNameTextPc771c64,
                                    PcInputText770b60,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText770b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    (char *)WorldPathSeparator,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    PcInputText770b50,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText77199c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    PcInputText770b48,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText770b40,
                                    PcInputText771758,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770b38,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770b30,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText770b28,
                                    PcInputText771728,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a48,
                                    PcInputText770b20,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames823948[238] = {TextInputKeyNameTextPc771d80,
                                    PcInputText770b1c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7712b4,
                                    "=",
                                    TextInputKeyNameTextPc771d3c,
                                    TextInputKeyNameTextPc771d38,
                                    PcInputText770b18,
                                    PcInputText770b14,
                                    PcInputText770b10,
                                    PcInputText770b0c,
                                    PcInputText770b08,
                                    PcInputText770b04,
                                    PcInputText770b00,
                                    PcInputText770afc,
                                    PcInputText770af8,
                                    PcInputText770af4,
                                    PcInputText770af0,
                                    PcInputText770aec,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText770ae0,
                                    PcInputText770adc,
                                    PcInputText770ad8,
                                    PcInputText770ad4,
                                    PcInputText770ad0,
                                    PcInputText770acc,
                                    PcInputText770ac8,
                                    PcInputText770ac4,
                                    PcInputText770ac0,
                                    PcInputText770abc,
                                    PcInputText770ab8,
                                    PcInputText770ab4,
                                    PcInputText770f54,
                                    PcInputText770aa4,
                                    (char *)WorldPathSeparator,
                                    PcInputText770aa0,
                                    PcInputText770a9c,
                                    PcInputText770a98,
                                    PcInputText770a94,
                                    PcInputText770a90,
                                    PcInputText770a8c,
                                    PcInputText770a88,
                                    PcInputText770a84,
                                    PcInputText770a80,
                                    PcInputText75eba8,
                                    PcInputText770a70,
                                    PcInputText770a5c,
                                    PcInputText770a50,
                                    PcInputText770a40,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    PcInputText770a2c,
                                    PcInputText770a18,
                                    PcInputText770a04,
                                    PcInputText7709f0,
                                    PcInputText7709dc,
                                    PcInputText7709c8,
                                    PcInputText7709b4,
                                    PcInputText7709a0,
                                    PcInputText77098c,
                                    PcInputText770978,
                                    PcInputText770964,
                                    PcInputText770950,
                                    PcInputText77093c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771530,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770928,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770910,
                                    PcInputText770900,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText7708ec,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7708d8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    PcInputText7708c8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText772760,
                                    PcInputText771758,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText772738,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText772750,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText772744,
                                    PcInputText771728,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7708bc,
                                    PcInputText7708ac,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames824828[238] = {TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7712f4,
                                    PcInputText7710c0,
                                    TextInputKeyNameTextPc771d3c,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText770d1c,
                                    PcInputText770d18,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText770d10,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText77109c,
                                    PcInputText771098,
                                    PcInputText7517a8,
                                    PcInputText770cf8,
                                    PcInputText770f68,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText770714,
                                    PcInputText77070c,
                                    TextInputKeyNameTextPc771d50,
                                    PcInputText770cdc,
                                    TextInputKeyNameTextPc771c6c,
                                    PcInputText770cd4,
                                    PcInputText770700,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText7706f4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    "<",
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    PcInputText770c50,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText7706e0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7706d4,
                                    PcInputText770c14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText7706d0,
                                    PcInputText771584,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7706c4,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7706bc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText770730,
                                    PcInputText771564,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770728,
                                    PcInputText770720,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames824470[238] = {TextInputKeyNameTextPc771d80,
                                    PcInputText770b1c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7712f4,
                                    (char *)WorldPathSeparator,
                                    TextInputKeyNameTextPc771d3c,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText770d1c,
                                    PcInputText770d18,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText770d10,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText770d08,
                                    PcInputText770d0c,
                                    PcInputText771530,
                                    PcInputText770cf8,
                                    PcInputText770f68,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText770cf0,
                                    PcInputText770ce8,
                                    PcInputText7712b4,
                                    PcInputText770cdc,
                                    TextInputKeyNameTextPc771c6c,
                                    PcInputText770cd4,
                                    TextInputKeyNameTextPc771c5c,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText770cb4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    "<",
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    PcInputText770c50,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText77199c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    PcInputText770c14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText770744,
                                    PcInputText771584,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77073c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770734,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText770730,
                                    PcInputText771564,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770728,
                                    PcInputText770720,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames823d00[238] = {TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText770f18,
                                    PcInputText7708a4,
                                    PcInputText770898,
                                    TextInputKeyNameTextPc771d38,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText77088c,
                                    PcInputText770878,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText77086c,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText770f28,
                                    PcInputText770868,
                                    PcInputText770858,
                                    PcInputText77084c,
                                    PcInputText770838,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText77082c,
                                    PcInputText770824,
                                    PcInputText77080c,
                                    PcInputText770800,
                                    TextInputKeyNameTextPc771c6c,
                                    PcInputText7707f4,
                                    PcInputText7707e0,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText7707d4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    (char *)WorldPathSeparator,
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    PcInputText7707c8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText77199c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a9c,
                                    PcInputText7707bc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText771154,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText7707ac,
                                    PcInputText771584,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770798,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770784,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText770774,
                                    PcInputText771564,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770768,
                                    PcInputText77075c,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames824be0[238] = {TextInputKeyNameTextPc771d80,
                                    TextInputKeyNameTextPc771d7c,
                                    "1",
                                    TextInputKeyNameTextPc771d78,
                                    TextInputKeyNameTextPc771d74,
                                    TextInputKeyNameTextPc771d70,
                                    TextInputKeyNameTextPc771d6c,
                                    TextInputKeyNameTextPc771d68,
                                    TextInputKeyNameTextPc771d64,
                                    TextInputKeyNameTextPc771d60,
                                    TextInputKeyNameTextPc771d5c,
                                    TextInputKeyNameTextPc771d58,
                                    PcInputText7712f4,
                                    PcInputText7710c0,
                                    PcInputText7706ac,
                                    PcInputText7706a4,
                                    TextInputKeyNameTextPc771d34,
                                    TextInputKeyNameTextPc771d30,
                                    TextInputKeyNameTextPc771d2c,
                                    TextInputKeyNameTextPc771d28,
                                    TextInputKeyNameTextPc771d24,
                                    TextInputKeyNameTextPc774184,
                                    TextInputKeyNameTextPc771d20,
                                    TextInputKeyNameTextPc771d1c,
                                    TextInputKeyNameTextPc771d18,
                                    TextInputKeyNameTextPc771d14,
                                    PcInputText770d1c,
                                    PcInputText770d18,
                                    TextInputKeyNameTextPc771d0c,
                                    PcInputText770d10,
                                    TextInputKeyNameTextPc771d00,
                                    TextInputKeyNameTextPc771cfc,
                                    TextInputKeyNameTextPc771cf8,
                                    TextInputKeyNameTextPc771cf4,
                                    TextInputKeyNameTextPc771cf0,
                                    TextInputKeyNameTextPc771cec,
                                    TextInputKeyNameTextPc771ce8,
                                    TextInputKeyNameTextPc771ce4,
                                    TextInputKeyNameTextPc771ce0,
                                    PcInputText77109c,
                                    PcInputText771098,
                                    PcInputText7517a8,
                                    PcInputText770cf8,
                                    PcInputText770f68,
                                    TextInputKeyNameTextPc774180,
                                    TextInputKeyNameTextPc759770,
                                    TextInputKeyNameTextPc771cac,
                                    TextInputKeyNameTextPc771ca8,
                                    TextInputKeyNameTextPc771ca4,
                                    TextInputKeyNameTextPc771ca0,
                                    TextInputKeyNameTextPc771c9c,
                                    PcInputText77069c,
                                    PcInputText770694,
                                    PcInputText77068c,
                                    PcInputText770680,
                                    TextInputKeyNameTextPc771c6c,
                                    PcInputText770cd4,
                                    PcInputText770678,
                                    TextInputKeyNameTextPc771c50,
                                    TextInputKeyNameTextPc771c4c,
                                    TextInputKeyNameTextPc771c48,
                                    TextInputKeyNameTextPc771c44,
                                    TextInputKeyNameTextPc771c40,
                                    TextInputKeyNameTextPc771c3c,
                                    TextInputKeyNameTextPc771c38,
                                    TextInputKeyNameTextPc771c34,
                                    TextInputKeyNameTextPc771c30,
                                    TextInputKeyNameTextPc771c2c,
                                    TextInputKeyNameTextPc771c28,
                                    TextInputKeyNameTextPc771c1c,
                                    TextInputKeyNameTextPc771c10,
                                    TextInputKeyNameTextPc771c08,
                                    TextInputKeyNameTextPc771c00,
                                    TextInputKeyNameTextPc771bf8,
                                    TextInputKeyNameTextPc771bf0,
                                    TextInputKeyNameTextPc771be8,
                                    TextInputKeyNameTextPc771be0,
                                    TextInputKeyNameTextPc771bd8,
                                    TextInputKeyNameTextPc771bd0,
                                    TextInputKeyNameTextPc771bc8,
                                    TextInputKeyNameTextPc771bc0,
                                    TextInputKeyNameTextPc771bb8,
                                    TextInputKeyNameTextPc771bb0,
                                    PcInputText77066c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    "<",
                                    TextInputKeyNameTextPc771ba0,
                                    TextInputKeyNameTextPc771b9c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b98,
                                    TextInputKeyNameTextPc771b94,
                                    TextInputKeyNameTextPc771b90,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b88,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b80,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b78,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b6c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b68,
                                    TextInputKeyNameTextPc771b60,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b58,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b4c,
                                    TextInputKeyNameTextPc771b48,
                                    TextInputKeyNameTextPc771b40,
                                    TextInputKeyNameTextPc771b34,
                                    TextInputKeyNameTextPc771b2c,
                                    TextInputKeyNameTextPc771b24,
                                    TextInputKeyNameTextPc771b20,
                                    TextInputKeyNameTextPc771b14,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771b08,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771afc,
                                    PcInputText770664,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aec,
                                    TextInputKeyNameTextPc771ae4,
                                    TextInputKeyNameTextPc771adc,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ad0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ac0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771ab8,
                                    PcInputText7706e0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771aa4,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText7706d4,
                                    PcInputText77065c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a8c,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a84,
                                    PcInputText770654,
                                    PcInputText771584,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText77064c,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770644,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a64,
                                    PcInputText77063c,
                                    PcInputText771564,
                                    TextInputKeyNameTextPc771a50,
                                    TextInputKeyNameTextPc76fde8,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    PcInputText770728,
                                    PcInputText770634,
                                    TextInputKeyNameTextPc771a38,
                                    TextInputKeyNameTextPc771a30,
                                    TextInputKeyNameTextPc771a28,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a20,
                                    TextInputKeyNameTextPc7516c0,
                                    TextInputKeyNameTextPc771a14,
                                    TextInputKeyNameTextPc771a04,
                                    TextInputKeyNameTextPc7719f8,
                                    TextInputKeyNameTextPc7719f0,
                                    TextInputKeyNameTextPc7719e4,
                                    TextInputKeyNameTextPc7719dc,
                                    TextInputKeyNameTextPc7719d0,
                                    TextInputKeyNameTextPc7719c8,
                                    TextInputKeyNameTextPc7719bc};
char *PcInputKeyNames821558[238] = {
    TextInputKeyNameTextPc771d80, TextInputKeyNameTextPc771d7c, "1",
    TextInputKeyNameTextPc771d78, TextInputKeyNameTextPc771d74, TextInputKeyNameTextPc771d70,
    TextInputKeyNameTextPc771d6c, TextInputKeyNameTextPc771d68, TextInputKeyNameTextPc771d64,
    TextInputKeyNameTextPc771d60, TextInputKeyNameTextPc771d5c, TextInputKeyNameTextPc771d58,
    TextInputKeyNameTextPc771d50, TextInputKeyNameTextPc771d48, TextInputKeyNameTextPc771d3c,
    TextInputKeyNameTextPc771d38, TextInputKeyNameTextPc771d34, TextInputKeyNameTextPc771d30,
    TextInputKeyNameTextPc771d2c, TextInputKeyNameTextPc771d28, TextInputKeyNameTextPc771d24,
    TextInputKeyNameTextPc774184, TextInputKeyNameTextPc771d20, TextInputKeyNameTextPc771d1c,
    TextInputKeyNameTextPc771d18, TextInputKeyNameTextPc771d14, TextInputKeyNameTextPc753374,
    TextInputKeyNameTextPc753370, TextInputKeyNameTextPc771d0c, TextInputKeyNameTextPc771d04,
    TextInputKeyNameTextPc771d00, TextInputKeyNameTextPc771cfc, TextInputKeyNameTextPc771cf8,
    TextInputKeyNameTextPc771cf4, TextInputKeyNameTextPc771cf0, TextInputKeyNameTextPc771cec,
    TextInputKeyNameTextPc771ce8, TextInputKeyNameTextPc771ce4, TextInputKeyNameTextPc771ce0,
    TextInputKeyNameTextPc771cd4, TextInputKeyNameTextPc771cc8, TextInputKeyNameTextPc771cc0,
    TextInputKeyNameTextPc771cb4, (char *)WorldPathSeparator,   TextInputKeyNameTextPc774180,
    TextInputKeyNameTextPc759770, TextInputKeyNameTextPc771cac, TextInputKeyNameTextPc771ca8,
    TextInputKeyNameTextPc771ca4, TextInputKeyNameTextPc771ca0, TextInputKeyNameTextPc771c9c,
    TextInputKeyNameTextPc771c94, TextInputKeyNameTextPc771c8c, TextInputKeyNameTextPc771c80,
    TextInputKeyNameTextPc771c74, TextInputKeyNameTextPc771c6c, TextInputKeyNameTextPc771c64,
    TextInputKeyNameTextPc771c5c, TextInputKeyNameTextPc771c50, TextInputKeyNameTextPc771c4c,
    TextInputKeyNameTextPc771c48, TextInputKeyNameTextPc771c44, TextInputKeyNameTextPc771c40,
    TextInputKeyNameTextPc771c3c, TextInputKeyNameTextPc771c38, TextInputKeyNameTextPc771c34,
    TextInputKeyNameTextPc771c30, TextInputKeyNameTextPc771c2c, TextInputKeyNameTextPc771c28,
    TextInputKeyNameTextPc771c1c, TextInputKeyNameTextPc771c10, TextInputKeyNameTextPc771c08,
    TextInputKeyNameTextPc771c00, TextInputKeyNameTextPc771bf8, TextInputKeyNameTextPc771bf0,
    TextInputKeyNameTextPc771be8, TextInputKeyNameTextPc771be0, TextInputKeyNameTextPc771bd8,
    TextInputKeyNameTextPc771bd0, TextInputKeyNameTextPc771bc8, TextInputKeyNameTextPc771bc0,
    TextInputKeyNameTextPc771bb8, TextInputKeyNameTextPc771bb0, PcInputText7719b0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, PcInputText7719a8,
    TextInputKeyNameTextPc771ba0, TextInputKeyNameTextPc771b9c, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b98, TextInputKeyNameTextPc771b94,
    TextInputKeyNameTextPc771b90, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b88, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b80, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b78, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771b6c, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b68,
    TextInputKeyNameTextPc771b60, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771b58, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771b4c, TextInputKeyNameTextPc771b48, TextInputKeyNameTextPc771b40,
    TextInputKeyNameTextPc771b34, TextInputKeyNameTextPc771b2c, TextInputKeyNameTextPc771b24,
    TextInputKeyNameTextPc771b20, TextInputKeyNameTextPc771b14, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771b08, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771afc, TextInputKeyNameTextPc771af4, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771aec, TextInputKeyNameTextPc771ae4,
    TextInputKeyNameTextPc771adc, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ad0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771ac8, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ac0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ab8, PcInputText77199c,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771aa4, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771a9c, TextInputKeyNameTextPc771a94, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a8c,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a84, TextInputKeyNameTextPc771a80,
    TextInputKeyNameTextPc771a78, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a70,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a68, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771a64, TextInputKeyNameTextPc75ac34, TextInputKeyNameTextPc771a58,
    TextInputKeyNameTextPc771a50, TextInputKeyNameTextPc76fde8, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc771a48, TextInputKeyNameTextPc771a40, TextInputKeyNameTextPc771a38,
    TextInputKeyNameTextPc771a30, TextInputKeyNameTextPc771a28, TextInputKeyNameTextPc7516c0,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a20,
    TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a14, TextInputKeyNameTextPc771a04,
    TextInputKeyNameTextPc7719f8, TextInputKeyNameTextPc7719f0, TextInputKeyNameTextPc7719e4,
    TextInputKeyNameTextPc7719dc, TextInputKeyNameTextPc7719d0, TextInputKeyNameTextPc7719c8,
    TextInputKeyNameTextPc7719bc};

#include "saga.h"
extern "C" {
#pragma data_seg(".data$PlayerStartSplineIndexPc")
    i32 PlayerStartSplineIndexPc = -1;
#pragma data_seg()

#pragma data_seg(".data$hub_startoutsidebonusdoor_area")
    i32 hub_startoutsidebonusdoor_area = -1;
#pragma data_seg()

#pragma bss_seg(".bss$shop_from_cutsceneplayer")
    i32 shop_from_cutsceneplayer;
#pragma bss_seg()

#pragma bss_seg(".bss$DoorExitCameraSplineNamePc")
    char DoorExitCameraSplineNamePc[32];
#pragma bss_seg()

#pragma bss_seg(".bss$DoorCutSplinePc")
    NUGSPLINE *DoorCutSplinePc;
#pragma bss_seg()

#pragma bss_seg(".bss$DoorCutCameraPosition0Pc")
    NUVEC DoorCutCameraPosition0Pc;
#pragma bss_seg()

#pragma bss_seg(".bss$DoorCutCameraPosition1Pc")
    NUVEC DoorCutCameraPosition1Pc;
#pragma bss_seg()

#pragma data_seg(".data$PlayerMissionStartDoorPc")
    char PlayerMissionStartDoorPc[13] = "mission_door";
#pragma data_seg()

#pragma data_seg(".data$PlayerArcadeStartDoorPc")
    char PlayerArcadeStartDoorPc[16] = "door_to_network";
#pragma data_seg()

#pragma bss_seg(".bss$ResetBits")
    u32 ResetBits;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicAction")
    i32 LevMusicAction;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicAmbient")
    i32 LevMusicAmbient;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicOtherAction")
    i32 LevMusicOtherAction;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicOtherAmbient")
    i32 LevMusicOtherAmbient;
#pragma bss_seg()

#pragma bss_seg(".bss$WorldInfo_InitMenuFn")
    void(__cdecl *WorldInfo_InitMenuFn)(WORLDINFO *, i32 *, i32 *);
#pragma bss_seg()

#pragma bss_seg(".bss$WorldInfo_InitLastFn")
    void(__cdecl *WorldInfo_InitLastFn)(WORLDINFO *);
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$LevPathNodes")
    i32 LevPathNodes[8];
#pragma bss_seg()

#pragma bss_seg(".bss$LevPathCnx")
    void *LevPathCnx[16];
#pragma bss_seg()

#pragma bss_seg(".bss$LevGamePart")
    PART_PC *LevGamePart[8];
#pragma bss_seg()

#pragma bss_seg(".bss$LevBlowUp")
    GIZMOBLOWUP_PC *LevBlowUp[2];
#pragma bss_seg()

#pragma bss_seg(".bss$LevSfxId")
    i32 LevSfxId[4];
#pragma bss_seg()

#pragma bss_seg(".bss$LevelCodeSpline")
    NUGSPLINE *LevelCodeSpline[8];
#pragma bss_seg()

#pragma bss_seg(".bss$LevPathCnxDir")
    i32 LevPathCnxDir;
#pragma bss_seg()

#pragma bss_seg(".bss$LevelLocator")
    void *LevelLocator;
#pragma bss_seg()

#pragma bss_seg(".bss$LevPlatID")
    i32 LevPlatID[2];
#pragma bss_seg()

#pragma bss_seg(".bss$LevLock")
    u8 LevLock[4];
#pragma bss_seg()

#pragma data_seg(".data$SuperStoryEpisode")
    i32 SuperStoryEpisode = -1;
#pragma data_seg()

#pragma data_seg(".data$hub_from_superstory")
    i32 hub_from_superstory = -1;
#pragma data_seg()

#pragma data_seg(".data$hub_from_mission")
    i32 hub_from_mission = -1;
#pragma data_seg()

#pragma data_seg(".data$PodRaceMaxLapTimeKeywordPc")
    char PodRaceMaxLapTimeKeywordPc[12] = "maxlaptime=";
#pragma data_seg()

#pragma data_seg(".data$PodRaceLapTimeKeywordPc")
    char PodRaceLapTimeKeywordPc[9] = "laptime=";
#pragma data_seg()

#pragma data_seg(".data$PodRaceAttemptsKeywordPc")
    char PodRaceAttemptsKeywordPc[25] = "nattempts_per_increment=";
#pragma data_seg()

#pragma data_seg(".data$PodRaceIncrementKeywordPc")
    char PodRaceIncrementKeywordPc[11] = "increment=";
#pragma data_seg()

#pragma bss_seg(".bss$LevDeaths")
    i32 LevDeaths;
#pragma bss_seg()

#pragma data_seg(".data$newlevelfrommenu_newmenuid")
    i32 newlevelfrommenu_newmenuid = -1;
#pragma data_seg()

#pragma bss_seg(".bss$newlevelfrommenu_newmenuy")
    i32 newlevelfrommenu_newmenuy;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$LevInstAnimResetPc")
    LEVEL_INST_ANIM_RESET_VIEW_PC LevInstAnimResetPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$ActionPairTab")
    PC_MUSIC_ACTION_PAIR_PC *ActionPairTab;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$GizObstacleTriggerCountPc")
    i32 GizObstacleTriggerCountPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$LastDoorPointerPc")
    void *LastDoorPointerPc;
#pragma bss_seg()

#pragma data_seg(".data$RtlUidCounterPc")
    u16 RtlUidCounterPc = 1;
#pragma data_seg()
}
extern "C" {
#pragma const_seg(".rdata$GizmoSpecialErrorTabPc")
    extern const char GizmoSpecialErrorTabPc[3] = "\t\t";
#pragma const_seg()

#pragma const_seg(".rdata$GizObstacleTypeNamePc")
    extern const char GizObstacleTypeNamePc[12] = "GizObstacle";
#pragma const_seg()

#pragma bss_seg(".bss$GizmoBlowupNameCountPc")
    i32 GizmoBlowupNameCountPc;
#pragma bss_seg()

#pragma bss_seg(".bss$GizmoBlowupNameTablePc")
    char GizmoBlowupNameTablePc[32][32];
#pragma bss_seg()
}
extern "C" {
#pragma const_seg(".rdata$PCSoundBuffer8IIDPc")
    extern const IID PCSoundBuffer8IIDPc = {
        0x6825a449, 0x7524, 0x4d82, {0x92, 0xf, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e}};
#pragma const_seg()

#pragma data_seg(".data$LEGOOBJ_DEFAULTLASTCOIN")
    i16 LEGOOBJ_DEFAULTLASTCOIN = -1;
#pragma data_seg()

#pragma data_seg(".data$TerrPlatScanDist")
    f32 TerrPlatScanDist = 10.0f;
#pragma data_seg()

#pragma bss_seg(".bss$BatarangPacketsPc")
    PLAYER_BATARANG_SLOT_PC BatarangPacketsPc[8];
#pragma bss_seg()

#pragma bss_seg(".bss$PlayerSuit")
    SUIT_LAYERS_PC *PlayerSuit[8];
#pragma bss_seg()

#pragma bss_seg(".bss$CoinPacket")
    PLAYER_COIN_PACKET_PC CoinPacket[2];
#pragma bss_seg()

#pragma bss_seg(".bss$GizForceLOSInfo")
    PLAYER_FORCE_LOS_SLOT_PC GizForceLOSInfo[2];
#pragma bss_seg()

#pragma bss_seg(".bss$PlayerTorpedoCount")
    u8 PlayerTorpedoCount[8];
#pragma bss_seg()

#pragma bss_seg(".bss$Game_CharacterSave")
    u8 *Game_CharacterSave;
#pragma bss_seg()

#pragma bss_seg(".bss$BackUpPlayers")
    GAMEOBJECT BackUpPlayers[2];
#pragma bss_seg()

#pragma bss_seg(".bss$OldPlrSPos")
    SOCKPOSITION_PC OldPlrSPos[8];
#pragma bss_seg()

#pragma bss_seg(".bss$Hub_UsePlayerList")
    i32 Hub_UsePlayerList;
#pragma bss_seg()

#pragma bss_seg(".bss$Area_PlayerIDList")
    i16 Area_PlayerIDList[8];
#pragma bss_seg()

#pragma bss_seg(".bss$Area_StoryModelListPc")
    i16 Area_StoryModelListPc[3];
#pragma bss_seg()

#pragma bss_seg(".bss$Game_AreaSave")
    COLLECTION_AREA_SAVE_PC *Game_AreaSave;
#pragma bss_seg()

#pragma bss_seg(".bss$cntrots")
    i32 cntrots;
#pragma bss_seg()

#pragma bss_seg(".bss$PlatCallback")
    TERRAIN_CALLBACK_LIFECYCLE_PC PlatCallback[8];
#pragma bss_seg()

#pragma bss_seg(".bss$PlatCodeCallback")
    i32 PlatCodeCallback;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundStorageLockPc")
    CRITICAL_SECTION PCSoundStorageLockPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundStorageLockReadyPc")
    i32 PCSoundStorageLockReadyPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundDrainStartPc")
    i32 PCSoundDrainStartPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundDrainRemainingPc")
    i32 PCSoundDrainRemainingPc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$PCDynamicDisplaySceneNameText")
    extern const char PCDynamicDisplaySceneNameText[31] = "Dynamic Material Display Scene";
#pragma const_seg()

#pragma const_seg(".rdata$NuTextureDdsExtensionPc")
    extern const char NuTextureDdsExtensionPc[4] = "dds";
#pragma const_seg()

#pragma data_seg(".data$PCDynamicDisplaySceneNamePointer")
    const char *PCDynamicDisplaySceneNamePointer = PCDynamicDisplaySceneNameText;
#pragma data_seg()

#pragma data_seg(".data$DebrisNullEffectPc")
    DEBRIS_INFO_PC DebrisNullEffectPc = {"null",
                                         0,
                                         0,
                                         0,
                                         0,
                                         100,
                                         60,
                                         0.0f,
                                         1.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         0,
                                         0,
                                         0,
                                         0,
                                         40000.0f,
                                         0.0f,
                                         25.0f,
                                         0.0f,
                                         0.0f,
                                         0.5f,
                                         1.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         {0.0f, 0.0f, 0.0f},
                                         {0.0f, 0.0f, 0.0f},
                                         {0},
                                         0.0f,
                                         1.0f,
                                         {0},
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         {{0.0f, 64, 64, 64, 0},
                                          {1.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0}},
                                         {{0.0f, 64.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         0.125f,
                                         0.125f,
                                         0.0f,
                                         500.0f,
                                         {{0.0f, 500.0f},
                                          {1.0f, 500.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 500.0f},
                                          {1.0f, 500.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         -360.0f,
                                         360.0f,
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         0.0f,
                                         0.0f,
                                         256.0f,
                                         256.0f,
                                         NULL,
                                         0.0f,
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         0,
                                         0,
                                         3,
                                         0,
                                         4.0f,
                                         {1.0f, 1.0f, 1.0f},
                                         1.0f,
                                         0.10000000149011612f,
                                         0.0f,
                                         {{0.0f, 0.0f},
                                          {1.0f, 1.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 1.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 1.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {-1, -1, -1, -1, -1, -1, -1, -1},
                                         {-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0},
                                         0,
                                         0,
                                         0,
                                         0,
                                         0.0f,
                                         0.0f,
                                         0.5f,
                                         1.0f,
                                         0};
#pragma data_seg()

#pragma data_seg(".data$DebrisProcessingEnabledPc")
    i32 DebrisProcessingEnabledPc = 1;
#pragma data_seg()

#pragma data_seg(".data$debris_sort_delay")
    i32 debris_sort_delay = 6;
#pragma data_seg()

#pragma data_seg(".data$debris_update_delay")
    i32 debris_update_delay = 3;
#pragma data_seg()

#pragma data_seg(".data$DebrisDmaCleanupIndexPc")
    i32 DebrisDmaCleanupIndexPc = 1;
#pragma data_seg()

#pragma bss_seg(".bss$NuApiEndFrameCallbackPc")
    void(__cdecl *NuApiEndFrameCallbackPc)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$numtoruscolldata")
    i32 numtoruscolldata = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$edpp_cam_pos")
    NUVEC edpp_cam_pos;
#pragma bss_seg()

#pragma bss_seg(".bss$edpp_nearest")
    i32 edpp_nearest = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$numspherecolldata")
    i32 numspherecolldata = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$spherecolldata")
    DEBRIS_COLLISION_ENTRY_PC *spherecolldata;
#pragma bss_seg()

#pragma bss_seg(".bss$debris_collide_pt")
    NUVEC debris_collide_pt;
#pragma bss_seg()

#pragma bss_seg(".bss$toruscolldata")
    DEBRIS_COLLISION_ENTRY_PC *toruscolldata;
#pragma bss_seg()

#pragma bss_seg(".bss$globalframes")
    i32 globalframes = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$DebrisArenaInitializedPc")
    i32 DebrisArenaInitializedPc = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$debris_sort_this_stack")
    i32 debris_sort_this_stack = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$debris_update_this_stack")
    i32 debris_update_this_stack = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$DebrisDmaCleanupDelayPc")
    i32 DebrisDmaCleanupDelayPc = 0;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCDevicePrimitiveUPCallCountPc")
    u32 PCDevicePrimitiveUPCallCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceIndexBufferCachePc")
    PC_BUFFER_RESOURCE_PC *PCDeviceIndexBufferCachePc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceVertexBufferCachePc")
    PC_BUFFER_RESOURCE_PC *PCDeviceVertexBufferCachePc;
#pragma bss_seg()
}

struct SoundQueue;
struct PC_SOUND_COMMIT_PC;
#pragma bss_seg(".bss$music_man")
NuMusic music_man;
#pragma bss_seg()
#pragma bss_seg(".bss$the_music_player")
NuMusic *the_music_player;
#pragma bss_seg()
#pragma bss_seg(".bss$NuSound3StereoStreams")
SoundQueue *NuSound3StereoStreams[2];
#pragma bss_seg()

// Startup and main-loop state; defaults follow the PC executable.
i32 ChallengeMode;
i32 fade_draw_suppressed;
i32 GAMEDEMO;
i32 PLAYERCOUNT;
i32 Level;
i32 Area = -1;
i32 last_area;
i32 NextArea_FreePlay;
i32 FreePlay;
i32 makefreeplaymodellist;
i32 SuperStory;
i32 reset_area;
i32 PlayTrailer = -1;
i32 loadareacharacters_loadedlevel;
i32 level_already_loaded = -1;
i32 next_level;
volatile i32 abort_load;
i32 dagobah_training;
#pragma bss_seg(".bss$party_cant_be_under_cover")
i32 party_cant_be_under_cover;
#pragma bss_seg()
i32 ResetOldFStop;
i32 new_level_from_menu;
i32 reset_restart;
#pragma bss_seg(".bss$NoMoreLoads")
i32 no_more_loads;
#pragma bss_seg()
i32 BonusWinner = -1;
i32 BonusCoinTotal;
i32 NewMode;
i32 reset_load = 1;
i32 newlevel_resumecutaudio = 1;
i32 frameout;
i32 other_level = -1;
volatile i32 waiting_for_level;
i32 gone_through_door_to_new_level;
i32 waiting_for_new_level;
i32 screendump;
i32 MiniCutCam;
i32 pause_rndr_on;
i32 clear_screen_onstill = 1;
i32 HubMainRenderTimeHack;
i32 peak_poly_count;
i32 poly_count;
i32 CUTSTOPGAME;
i32 Paused;
#pragma bss_seg(".bss$NetPaused")
i32 NetPaused;
#pragma bss_seg()
i32 FinishLoop_On = 1;
i32 LevelChange;
i32 LevelChangesInArea;
i32 do_multiframe_update;
#pragma bss_seg(".bss$MainFrameCounters")
MAINFRAMECOUNTERS MainFrameCounters;
#pragma bss_seg()
i32 Arcade;
i32 hub_from_arcade = -1;
i32 UsePlayerList;
i32 Door_UseCutCam;

extern "C" {
    i32 SUPERBUFFERSIZE = 0x09600000;
    u32 PCImplodeWordMask = 0xffffffff;
    NULISTHDR global_aiscripts;
    u16 **BadWords;
    i32 NumBadWords;
    BOLTTYPE_PC GlobalBoltType_Default = {
        "null", 4.0f, 2.0f, 0.0f, 0.125f, 1.0f, 0.1f, -1,   -1,   -1,   0, -1, 0,  1,
        0xff,   0,    0,    NULL, NULL,   NULL, NULL, NULL, NULL, NULL, 0, -1, -1, {0},
    };
    BOLTSYS_PC GlobalBoltSystem_Default = {
        &GlobalBoltType_Default,        1,    NULL, Bolt_Debris_Default, Bolt_GetShootOrigin_Default,
        Bolt_GetShootDirection_Default, NULL, NULL,
    };
    BOLTSYS_PC *BoltSys = &GlobalBoltSystem_Default;
    NUSTRINGBANK StringBank[3];
#pragma data_seg(".data$BoltShootClassId0")
    u16 BoltShootClassId0 = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$BoltShootClassId1")
    u16 BoltShootClassId1 = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$BoltShootClassId2")
    u16 BoltShootClassId2 = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$BoltShootParameter")
    f32 BoltShootParameter = 0.1f;
#pragma data_seg()
#pragma bss_seg(".bss$CurrentStringBank")
    i16 CurrentStringBank;
#pragma bss_seg()
    u32 nustring_format = 1;
    SFX_GROUP_PC NuSound3Groups[128];
    i16 NuSound3GroupSampleIds[512];
    i32 NuSound3GroupSampleCount;
    i32 NuSound3GroupCount;
    BOLT Bolt[32];
    i32 i_bolt;
    f32 spline_seek_pos = 10.0f;
    f32 spline_seek_ang = 10.0f;
    f32 spline_xoffset_speed_seek = 4.0f;
    f32 spline_xoffset_speed_roll = 16384.0f;
    extern const u16 bad_fluff[26] = {
        '!', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=', '{',
        '}', '[', ']', '@', '~', '#', '?', '<', '>', ',', '.', ' ', 0,
    };
    u8 NuAnimDataBitCount[256];
    i32 NuAnimDataBitCountInitialized;
    extern const f64 VehicleTurnPhaseScale = 65536.0;
    extern const f64 VehicleTurnPhaseOffset = 16384.0;
    extern const f64 MoveSplineRunSpeedSentinel = 1000000000.0;
    extern const f32 MoveSplineTurnRate = 8.0f;
    extern const f64 BoltRayAccelerationTimeSquared = 0.04000000283122063;
    extern const f64 BoltTargetRandomDenominator = 65535.0;
    extern const f64 NuAnimDataFloatMax = 3.4028234663852886e+38;
#pragma data_seg(".data$GameObjectRotationBuggyId")
    u16 GameObjectRotationBuggyId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationRadioCarId")
    u16 GameObjectRotationRadioCarId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationPoliceCarId")
    u16 GameObjectRotationPoliceCarId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationMowerId")
    u16 GameObjectRotationMowerId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationAtstId")
    u16 GameObjectRotationAtstId = 0xffff;
#pragma data_seg()
#pragma bss_seg(".bss$temp_xrot")
    u16 temp_xrot;
#pragma bss_seg()
#pragma bss_seg(".bss$temp_zrot")
    u16 temp_zrot;
#pragma bss_seg()
    i32 VehicleArea;
    extern const char NuAnimSourceFileName[48] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuanim.c";
#pragma const_seg(".rdata$AIScriptKeywordInclude")
    extern const char AIScriptKeywordInclude[8] = "INCLUDE";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordDeriveFromScript")
    extern const char AIScriptKeywordDeriveFromScript[17] = "DERIVEFROMSCRIPT";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordActionMacro")
    extern const char AIScriptKeywordActionMacro[12] = "ACTIONMACRO";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordConditionMacro")
    extern const char AIScriptKeywordConditionMacro[15] = "CONDITIONMACRO";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordConst")
    extern const char AIScriptKeywordConst[6] = "CONST";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordParam")
    extern const char AIScriptKeywordParam[6] = "PARAM";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordState")
    extern const char AIScriptKeywordState[6] = "STATE";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordEnd")
    extern const char AIScriptKeywordEnd[2] = "}";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordReferenceScript")
    extern const char AIScriptKeywordReferenceScript[16] = "REFERENCESCRIPT";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordActions")
    extern const char AIScriptKeywordActions[8] = "ACTIONS";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordConditions")
    extern const char AIScriptKeywordConditions[11] = "CONDITIONS";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordBase")
    extern const char AIScriptKeywordBase[5] = "Base";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordGlobal")
    extern const char AIScriptKeywordGlobal[7] = "Global";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordSource")
    extern const char AIScriptKeywordSource[7] = "Source";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordScript")
    extern const char AIScriptKeywordScript[7] = "Script";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordLevel")
    extern const char AIScriptKeywordLevel[6] = "Level";
#pragma const_seg()
    extern const char NuBlankStringValue[2];
    char *NuBlankString = (char *)NuBlankStringValue;
    extern const f64 GameObjectNearFloorNoFloorHeight = 2000000.0;
    extern const f64 GameObjectNearFloorRadiusDivisor = 0.22499999403953552;
    extern const f64 VehicleHoverHeightSeparationScale = 0.6000000238418579;
#pragma const_seg(".rdata$AIPathAngularScale")
    extern const f64 AIPathAngularScale = 10430.3779296875;
#pragma const_seg()
#pragma const_seg(".rdata$PCAtan2RadianScale")
    extern const f64 PCAtan2RadianScale = 9.58738019107841e-05;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient3")
    extern const f64 PCPathArcSineCoefficient3 = (f64)(f32)-0.166667f;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient5")
    extern const f64 PCPathArcSineCoefficient5 = (f64)(f32)-0.075f;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient7")
    extern const f64 PCPathArcSineCoefficient7 = (f64)(f32)-0.0446429f;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient9")
    extern const f64 PCPathArcSineCoefficient9 = (f64)(f32)-0.0303819f;
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptListFilename")
    extern const char AIScriptListFilename[11] = "script.txt";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptListPathFormat")
    extern const char AIScriptListPathFormat[14] = "%s\\script.txt";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptFileExtension")
    extern const char AIScriptFileExtension[5] = ".scp";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptActionNameArgPrefix")
    extern const char AIScriptActionNameArgPrefix[6] = "name=";
#pragma const_seg()
#pragma const_seg(".rdata$PlayCountArgPrefix")
    extern const char PlayCountArgPrefix[11] = "playcount=";
#pragma const_seg()
#pragma const_seg(".rdata$RepeatDelayArgPrefix")
    extern const char RepeatDelayArgPrefix[10] = "repdelay=";
#pragma const_seg()
#pragma const_seg(".rdata$StartDelayArgPrefix")
    extern const char StartDelayArgPrefix[12] = "startdelay=";
#pragma const_seg()
#pragma const_seg(".rdata$XArgPrefix")
    extern const char XArgPrefix[3] = "x=";
#pragma const_seg()
#pragma const_seg(".rdata$YArgPrefix")
    extern const char YArgPrefix[3] = "y=";
#pragma const_seg()
#pragma const_seg(".rdata$ZArgPrefix")
    extern const char ZArgPrefix[3] = "z=";
#pragma const_seg()
#pragma const_seg(".rdata$CharacterPosArgPrefix")
    extern const char CharacterPosArgPrefix[15] = "character_pos=";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptParamPrefix")
    extern const char AIScriptParamPrefix[6] = "param";
#pragma const_seg()
#pragma const_seg(".rdata$NuBlankStringValue")
    extern const char NuBlankStringValue[2] = " ";
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationBuggyOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_BUGGY_OFFSETS[4] = {
        {-0.1175f, 0.0f, 0.1575f},
        {0.1175f, 0.0f, 0.1575f},
        {0.1175f, 0.0f, -0.1575f},
        {-0.1175f, 0.0f, -0.1575f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationRadioCarOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_RADIOCAR_OFFSETS[4] = {
        {-0.12f, 0.0f, 0.08f},
        {0.12f, 0.0f, 0.08f},
        {0.12f, 0.0f, -0.08f},
        {-0.12f, 0.0f, -0.08f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationPoliceCarOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_POLICECAR_OFFSETS[4] = {
        {-0.19f, 0.0f, 0.31f},
        {0.19f, 0.0f, 0.31f},
        {0.19f, 0.0f, -0.31f},
        {-0.19f, 0.0f, -0.31f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationMowerOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_MOWER_OFFSETS[4] = {
        {-0.1175f, 0.0f, 0.155f},
        {0.1175f, 0.0f, 0.155f},
        {0.1175f, 0.0f, -0.155f},
        {-0.1175f, 0.0f, -0.155f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationDefaultOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_DEFAULT_OFFSETS[4] = {
        {-0.2f, 0.0f, 0.2f},
        {0.2f, 0.0f, 0.2f},
        {0.2f, 0.0f, -0.2f},
        {-0.2f, 0.0f, -0.2f},
    };
#pragma const_seg()
    i32 StartupGameFlag0;
    i32 StartupGameFlag1 = 1;
    u8 ForcePlayEndFrame;
#pragma bss_seg(".bss$ForceEulerToQuat")
    u8 ForceEulerToQuat;
#pragma bss_seg()
    ANI3_GLOBALBUFFER_PC *globalbuffer;
#pragma bss_seg(".bss$DdMaxJointsPc")
    i32 DdMaxJointsPc;
#pragma bss_seg()
#pragma data_seg(".data$ModelAnimLayerListDispatchPc")
    i32(__cdecl *ModelAnimLayerListDispatchPc)(CHARACTER_MODEL_LAYERS_PC *, i16 *, u32) = PCBuildModelLayerList;
#pragma data_seg()
#pragma data_seg(".data$DwaWeightCursorPc")
    i32 DwaWeightCursorPc = 61184;
#pragma data_seg()
#pragma data_seg(".data$DwaPointerCursorPc")
    i32 DwaPointerCursorPc = 2048;
#pragma data_seg()
#pragma bss_seg(".bss$DwaWeightPoolPc")
    DWA_WEIGHT_BLOCK_PC DwaWeightPoolPc[60669];
#pragma bss_seg()
#pragma bss_seg(".bss$DwaPointerPoolPc")
    DWA_WEIGHT_BLOCK_PC *DwaPointerPoolPc[2048];
#pragma bss_seg()

#pragma bss_seg(".bss$AnimQuatDepthPc")
    i32 AnimQuatDepthPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimQuatSavedPc")
    u8 AnimQuatSavedPc[4];
#pragma bss_seg()
#pragma bss_seg(".bss$AISysSpecialNodeCallback")
    AISYS_SPECIAL_NODE_CALLBACK_PC AISysSpecialNodeCallback;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimBuffEvalCB")
    ANI3_EVAL_CALLBACK_PC AnimBuffEvalCB;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimBuffEvalData")
    void **AnimBuffEvalData;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimBuffEvalJoint")
    i32 *AnimBuffEvalJoint;
#pragma bss_seg()
#pragma const_seg(".rdata$ANI3_EVAL_IDENTITY_MATRIX")
    extern const NUMTX ANI3_EVAL_IDENTITY_MATRIX = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
#pragma const_seg()
#pragma bss_seg(".bss$ANI3_EVAL_SCALE_ARRAY")
    NUVEC ANI3_EVAL_SCALE_ARRAY[256];
#pragma bss_seg()
#pragma bss_seg(".bss$SoundVoiceManager")
    SoundVoiceManagerPC SoundVoiceManager;
#pragma bss_seg()
#pragma bss_seg(".bss$Bridges")
    NUBRIDGE_PC Bridges[8];
#pragma bss_seg()
#pragma bss_seg(".bss$NuBridgeProc")
    i32 NuBridgeProc;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltShootIdScratch")
    i32 BoltShootIdScratch;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltShootPositionScratch")
    NUVEC BoltShootPositionScratch;
#pragma bss_seg()
#pragma bss_seg(".bss$CurrentTerrain")
    void *CurrentTerrain;
#pragma bss_seg()
#pragma bss_seg(".bss$GetNamedGameObjectCallback")
    GAMEOBJECT_LOOKUP_CALLBACK_PC GetNamedGameObjectCallback;
#pragma bss_seg()
#pragma bss_seg(".bss$RepeatSfxQueueHead")
    i32 RepeatSfxQueueHead;
#pragma bss_seg()
#pragma bss_seg(".bss$RepeatSfxQueue")
    REPEAT_SFX_ENTRY_PC RepeatSfxQueue[32];
#pragma bss_seg()
#pragma data_seg(".data$TerLayer")
    TERRAIN_LAYER_PC TerLayer[17] = {
        {0x00000000, 0x0000ffff, 0x40000000}, {0x00000000, 0x0000ffff, 0x40000000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000001, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000020, 0x0000ffff, 0x3f800000},
        {0x00000001, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x00000000},
    };
#pragma data_seg()
    const FLOWBOX_CALLBACKS_PC flowbox_callbacks[3] = {
        {ResetGizmoFlowBox, ProcessGizmoFlowBox, CheckOutputGizmoFlowBox},
        {NULL, ProcessConditionFlowBox, CheckOutputConditionFlowBox},
        {NULL, ProcessActionFlowBox, CheckOutputActionFlowBox},
    };
    u8 gizflow_loop_checksum;
    GAMEAUDIO_PC GameAudioStorage;
    GAMEAUDIO_PC *GameAudio = &GameAudioStorage;
    SoundTable CurrentSFXTAB;
    NUVEC nusound_special_positions[5];
    NUMTX *NuSound3Listener;
    f32 nusound_fade_start = 2.0f;
    f32 nusound_fade_end = 15.0f;
    u32 nusound_seed = 0x20558057;
#pragma const_seg(".rdata$GizmoDefaultOutputName")
    extern const char GizmoDefaultOutputName[8] = "default";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptFalse")
    extern const char AIScriptFalse[6] = "false";
#pragma const_seg()
    i32 nufile_blkcnt = 1;
    PC_NUFILE_READBLOCK nufile_blkinfo[1024];
    NUDATHDR *StartupDatList[7];
    i32 StartupDatCount;
#pragma bss_seg(".bss$NuSoundDat")
    NUDATHDR *NuSoundDat;
#pragma bss_seg()
    GAMEANIMPROGRESS gameanimsysprogress;
    VARIPTR original_permbuffer_base;
    char InitOnceCurrentDirectory[1];
#pragma const_seg(".rdata$InitOnceAllocSource")
    extern const char InitOnceAllocSource[15] = ".\\pcbatman.cpp";
#pragma const_seg()
#pragma const_seg(".rdata$InitOnceWindowTitle")
    extern const char InitOnceWindowTitle[36] = "LEGO\xae Star Wars\x99: The Complete Saga";
#pragma const_seg()
#pragma const_seg(".rdata$InitOnceHalf")
    extern const f32 InitOnceHalf = 0.5f;
#pragma const_seg()
    void *Game_NuPad_Store[2];
    void **Game_NuPad;
    void *NuTimeBar_SetList[16];
    i32 NuTimeBar_Initialised;

    // PC debris timing uses a single-precision 1/60-second step.
    f32 debris_time_increment = 1.0f / 60.0f;
    void *debris_cutoff_camera_vec;
    i32 debris_render_group;
    f32 FRAMETIME;
    f32 QFONTSCALEX = 1.0f;
    f32 STCOORDSCALE = 1.0f;
    VUFNT *QFont2D;
    VUFNT *QFont2DButtons;
    VUFNT *system_qfont;
    NUTEXDIM_PC **NuTextureDimensions;
    i32 NuPrimVertexCount;
    i32 *NuPrimVertexCountPointer;
    VARIPTR *NuPrimStreamBufferPointer;
    i32 NuPrimCurrentPrimitiveType;
    i32 NuPrimCSPos;
    f32 NuPrimXBias;
    f32 NuPrimYBias;
    f32 NuPrimXScale = 1.0f;
    f32 NuPrimYScale = 1.0f;
#pragma const_seg(".rdata$NuPrimPS2XScale")
    extern const f32 NuPrimPS2XScale = 0.003125f;
#pragma const_seg(".rdata$NuPrimPS2YScale")
    extern const f32 NuPrimPS2YScale = -1.0f / 112.0f;
#pragma const_seg(".rdata$NuPrimTwo")
    extern const f32 NuPrimTwo = 2.0f;
#pragma const_seg(".rdata$NuPrimMinusTwo")
    extern const f32 NuPrimMinusTwo = -2.0f;
#pragma const_seg()
    NUFPAR_PC NuFParRecords[4];
    char NuFParBuffers[4][0x1000];
    NUFPAR_PC *NuFParRecordBase;
    char *NuFParBufferBase;
    i32 file_criticalsection;
    PC_NUDATFILEINFO *unpack_file_info;
    NUDATOPENFILEINFO *unpack_file_odi;
    i64 unpack_file_pos;
    f32 nuqfnt_space_width;
    f32 qfnt_len_scale = 1.0f;
    f32 qfnt_height_scale = 1.0f;
    f32 qfnt_justify_squash = 1.0f;
    f32 qfnt_justify_stretch = 1.0f;
    u32 NuQFntMode;
    i32 NuQFntModeStackIndex;
#pragma bss_seg(".bss$NuQFntModeStack")
    u32 NuQFntModeStack[16];
#pragma bss_seg()
    i32 nuqfnt_cs_mode;
    f32 qfnt_offscale_x;
    f32 qfnt_offscale_y;
    f32 qfnt_rezscale_w = 1.0f;
    f32 qfnt_rezscale_h = 1.0f;
    extern const f32 nu_fsign_minus_one = -1.0f;
#pragma const_seg(".rdata$PCKeyboard2Name_candidate")
    extern const char PCKeyboard2Name_candidate[10] = "Keyboard2";
#pragma const_seg(".rdata$PCKeyboardMouse2Name_candidate")
    extern const char PCKeyboardMouse2Name_candidate[16] = "KeyboardMouse2";
#pragma const_seg(".rdata$PCKeyboard1Name_candidate")
    extern const char PCKeyboard1Name_candidate[10] = "Keyboard1";
#pragma const_seg(".rdata$PCKeyboardMouse1Name_candidate")
    extern const char PCKeyboardMouse1Name_candidate[15] = "KeyboardMouse1";
#pragma const_seg()
#pragma const_seg(".rdata$NuQFntHalfScale")
    extern const f64 NuQFntHalfScale = 0.5;
#pragma const_seg()
    GAMESAVE Game;
    OPTIONSSAVE TempOptions;
    PLAYERPROGRESS PlayerProgress[8];
    i32 MenuLoadOccurred;
    i32 startnewgame_initiated;
    i32 startnewgame;
    i32 saveload_autosave = -1;
    i32 BGLOAD = 1;
    i32 PermDataLoaded = 1;
    i32 LOADEROFF;
    volatile i32 LoadPerm_LanguageSelect;
    NUMTL *legal_mtl;
    i32 legal_tid;
    i32 loadlegal_done;
    i32 LoadPerm_StringsLoaded;
#pragma const_seg(".rdata$PermLegalScale")
    extern const f32 PermLegalScale = 2.2f;
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalBase")
    extern const char PermLegalBase[6] = "legal";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalSeparator")
    extern const char PermLegalSeparator[2] = "_";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalSpanish")
    extern const char PermLegalSpanish[11] = "_usspanish";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalFrench")
    extern const char PermLegalFrench[10] = "_usfrench";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalUS")
    extern const char PermLegalUS[4] = "_us";
#pragma const_seg()
    i32 readpads_always;
    TIMER GlobalTimer;
    i32 saveload_status;
    i32 NOMUSIC;
    MUSICPLAYBACK_PC Music = {0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0, -1, NULL};
    i32 edpp_page_used[8];
    i32 debinf_count;
    DEBRIS_INFO_PC **debtab;
    NUGSCN *save_icon_scene;
    f32 TextScaleX_candidate = 1.0f;
    f32 TextScaleY_candidate = 1.0f;
    void *OldLevelHackData;
    i32(__cdecl *CutScene_StartFn)(CUTINFO *cut);
    void(__cdecl *CutScene_PreUpdateFn)(CUTINFO *cut);
    void(__cdecl *CutScene_PostUpdateFn)(void);
    void(__cdecl *CutScene_StoppedFn)(CUTINFO *cut);
    i32(__cdecl *CutScene_ReplaceCharacterModelFn)(CUTINFO *cut, CUTSCENECHARACTER *character);
    i32(__cdecl *InitBolt_AddMomentumType)(BOLT *bolt, GAMEOBJECT *object, NUVEC *momentum);
    i32(__cdecl *Bolt_HitPlatFn)(BOLT *bolt);
    i32(__cdecl *Bolt_HitCustomFn)(void *bolt, void *points);
    void(__cdecl *GameBlowUpBlownUpFn)(void *blowup);
#pragma bss_seg(".bss$GizObstacle_SetDefaultSFXFn")
    void(__cdecl *GizObstacle_SetDefaultSFXFn)(void *world, GIZOBSTACLE *obstacle);
#pragma bss_seg()
    NUVEC PodAvalanchePeakPos = {-110.0f, -10.0f, -455.0f};
    LEVEL_PROGRESS *LevelProgressData;
    LEVELSPLINE_PC *LevSplList;
    LEVELSPLINE_PC *LevelSplinesTable;
    i32 nudlist_alloc_count;
    u32 nudlist_alloc_base;
    u32 nudlist_alloc_end;
    u32 nudlist_item_base;
    i32 nudlist_frame_active;
    u32 nudlist_field_e80;
    u32 nudlist_field_fc8;
    i32 nudlist_field_fc4;
    u32 nudlist_field_9d0;
    u32 nudlist_field_9d8;
    u32 nudlist_field_fe4;
    NUDISPLAYLISTITEM nudlist_2d_static;
    VARIPTR nudlist_stream_cursor;
    u32 nudlist_field_1598;
    u32 nuprim_field_ba8;
    i32 LEVELSPLINECOUNT;
    i32 levspl_i_start = -1;
    i32 levspl_i_startcam = -1;
    LEVELDATA_PC *KAMINOE_LDATA;
    LEVELDATA_PC *DOGFIGHTA_LDATA;
    void *game_cutscenes_cutscene;
    void *game_cutscenes_podrace_pod_explode;
    void *game_cutscenes_podrace_out_of_time;
    void *game_cutscenes_bonus_gunship_cavalry_explode;
    NUVEC PodRaceADiePos = {350.0f, 0.25f, -15.0f};
    NUVEC PodRaceBDiePos = {138.0f, 0.15f, 178.0f};
    NUVEC PodRaceCDiePos = {140.0f, -7.2f, 52.0f};
    NUVEC GunshipADiePos = {10.0f, -1.0f, -20.0f};
    NUVEC GunshipBDiePos = {0.0f, -1.0f, -150.0f};
    LEVELDATA_PC *PODRACEA_LDATA;
    i32 LSW1;
    LEVELDATA_PC *BONUS_GUNSHIPB_LDATA;
    LEVELDATA_PC *SPEEDERCHASEA_LDATA;
    LEVELDATA_PC *DEATHSTARBATTLED_LDATA;
    i16 id_SPEEDERBIKE = -1;
    KAMINOEPLATE *kamino_plate_1;
    KAMINOEPLATE *kamino_plate_2;
    KAMINOEPLATE *kamino_plate_3;
    KAMINOEPLATE *kamino_plate_4;
    i32 kamino_plate_1_active;
    i32 kamino_plate_2_active;
    i32 kamino_plate_3_active;
    i32 kamino_plate_4_active;
    KAMINOEPLATE *kamino_plate_hit;
    u16 kamino_plate_id;
    i16 id_ANAKINSPOD = -1;
    i16 id_ANAKINSPODGREEN = -1;
    i16 id_ANAKINSNEWPOD = -1;
    i16 id_ANAKINSNEWPODGREEN = -1;
    i16 id_REPUBLICGUNSHIP_GREEN = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP_GREEN = -1;
    i16 id_JEDISTARFIGHTERYELLOWEP3 = -1;
    GAMEOBJECT *player;
    GAMEOBJECT *CutDeadVehiclePlayer;
    APICHARSYS *apicharsys;
    void *game_cutscenes_podrace_sebulba;
    void *game_cutscenes_podsprint_out_of_time;
    void *game_cutscenes_podsprint_sebulba;
    void *game_cutscenes_dogfight_die;
    LEVELDATA_PC *PODSPRINTA_LDATA;
    LEVELDATA_PC *PODRACEB_LDATA;
    LEVELDATA_PC *BONUS_GUNSHIPA_LDATA;
    u16 podrace_die_angle_a;
    u16 podrace_die_angle_b;
    u16 podrace_die_angle_c;
    void *Player[8];
    f32 DEFAULTFPS;
    void *LevelHackData;
    i32 LevelHackDataSize;
    i16 id_DEFAULTCHARACTER[2] = {-1, -1};
    i16 ID_DEFAULTCHARACTER_A = -1;
    i16 ID_DEFAULTCHARACTER_B = -1;
    i16 ID_DEFAULTCHARACTER_LSW_A = -1;
    i16 ID_DEFAULTCHARACTER_LSW_B = -1;
    MEMORYMANAGER theMemoryManager;
    f32 DEFAULTFRAMETIME;
    i32 PAL;
    PART_EMIT_PC part_emits[40];
    i32 part_page_on[8];
    GAMEPAD_INIT_RECORD GamePadStorage[64];
    i32 bgproc_initialized;
    CRITICAL_SECTION bgproc_critical_section;
    HANDLE bgproc_work_event;
    HANDLE bgproc_resume_event;
    BGPROCINFO *bgproc_active;
    DWORD bgproc_thread_id;
    i32 bgproc_frozen;
    i32 bgproc_freeze;
    i32 ShowingError;
    i32 bHaveErr;
    HANDLE pc_file_handles[32] = {
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
    };
    FILEINFO file_info[33];
    FILEBUFF file_buff[4];
    i32 nufile_buffering_enabled;
    i32 datsys_offset;
#pragma bss_seg(".bss$nufile_lasterror")
    i32 nufile_lasterror;
#pragma bss_seg()
#pragma bss_seg(".bss$nufile_try_packed")
    i32 nufile_try_packed;
#pragma bss_seg()
#pragma bss_seg(".bss$fpk_err")
    i32 fpk_err;
#pragma bss_seg()
    i32 file_time_count;
    NUFILE_DEVICE devices[16];
    i32 numdevices;
    NUFILE_DEVICE host_device = {
        0,
        0,
        0x0e,
        0,
        '\\',
        0x400,
        -1,
        -1,
        -1,
        {0},
        {0},
        4,
        6,
        {0},
        "Host pc filesystem",
        {0},
        {0},
        {0},
        DEV_FormatName,
        DEVHOST_Interrogate,
    };
    NUFILE_DEVICE *default_device = &host_device;
    i32 DontDoFileUpperCaseHack;
    NuKeyboardState_PC *nu_input_state;
    HINSTANCE PCInputWindow_candidate;
    HWND PCInputInstance_candidate;
    NuKeyboardOwner_PC nu_keyboard_owner;
    u8 nu_keyboard_pages[0x200];
    u8 nu_keyboard_page_index;
    i32 nu_keyboard_device_flags;
    i32 nu_keyboard_key_index;
    i32 nu_keyboard_repeat_countdown;
    i32 nu_keyboard_previous_key_index;
    i32 nu_keyboard_no_key_state;
    i32 PCThreadCriticalSectionCount_candidate;
    CRITICAL_SECTION PCThreadCriticalSections_candidate[13];
    i32 PCFrameCriticalSectionIndex_candidate;
    NUALLOC_RECORD *NuAllocRecordHead;
    u32 NuAllocTrackedBytes;
    NUALLOC_POOL *NuAllocPool;
    u32 NuAllocRecordSerial;
    NUALLOC_ARENA *NuAllocArena;
#pragma bss_seg(".bss$NuAllocManager")
    void *NuAllocManager;
#pragma bss_seg()
    void *NuAllocPeakEnd0;
    void *NuAllocPeakEnd1;
    u32 NuAllocHeaderMetadata[3] = {0x424d454d, 0x45484b4c, 0x52454441};
    u32 NuAllocTailMetadata[3] = {0x424d454d, 0x41544b4c, 0x31304c49};
#pragma const_seg(".rdata$NuFileDiscSuffix")
    extern const char nufile_disc_suffix[3] = ";1";
#pragma const_seg()
    DAT_HANDLES_PC NuDatHandles;
    i32 finishloop_backdroponly;
#pragma const_seg(".rdata$StartupGameDat")
    extern const char StartupGameDat[9] = "GAME.DAT";
#pragma const_seg(".rdata$StartupEpisodeIDat")
    extern const char StartupEpisodeIDat[14] = "Episode_I.DAT";
#pragma const_seg(".rdata$StartupEpisodeIIDat")
    extern const char StartupEpisodeIIDat[15] = "Episode_II.DAT";
#pragma const_seg(".rdata$StartupEpisodeIIIDat")
    extern const char StartupEpisodeIIIDat[16] = "Episode_III.DAT";
#pragma const_seg(".rdata$StartupEpisodeIVDat")
    extern const char StartupEpisodeIVDat[15] = "Episode_IV.DAT";
#pragma const_seg(".rdata$StartupEpisodeVDat")
    extern const char StartupEpisodeVDat[14] = "Episode_V.DAT";
#pragma const_seg(".rdata$StartupEpisodeVIDat")
    extern const char StartupEpisodeVIDat[15] = "Episode_VI.DAT";
#pragma const_seg()
#pragma data_seg(".data$StartupDatNames")
    const char *StartupDatNames[7] = {
        StartupGameDat,      StartupEpisodeIDat, StartupEpisodeIIDat, StartupEpisodeIIIDat,
        StartupEpisodeIVDat, StartupEpisodeVDat, StartupEpisodeVIDat,
    };
#pragma data_seg()
#pragma const_seg(".rdata$EpisodeIDat")
    extern const char EpisodeIDat[14] = "Episode_I.dat";
#pragma const_seg(".rdata$EpisodeIIDat")
    extern const char EpisodeIIDat[15] = "Episode_II.dat";
#pragma const_seg(".rdata$EpisodeIIIDat")
    extern const char EpisodeIIIDat[16] = "Episode_III.dat";
#pragma const_seg(".rdata$EpisodeIVDat")
    extern const char EpisodeIVDat[15] = "Episode_IV.dat";
#pragma const_seg(".rdata$EpisodeVDat")
    extern const char EpisodeVDat[14] = "Episode_V.dat";
#pragma const_seg(".rdata$EpisodeVIDat")
    extern const char EpisodeVIDat[15] = "Episode_VI.dat";
#pragma const_seg()
#pragma const_seg(".rdata$bgproc_resume_event_name")
    extern const char bgproc_resume_event_name[9] = "BGFreeze";
#pragma const_seg(".rdata$bgproc_work_event_name")
    extern const char bgproc_work_event_name[11] = "BGWorkToDo";
#pragma const_seg()
    VUFNT *QFont3DZ;
    VUFNT *SmartTextFont;
    f32 INTROTEXT_Y = 0.175f;
    f32 INTROTEXT_SCALE = 0.79f;
    void (*Text_GameSetLanguageFn)(i32);
#pragma const_seg(".rdata$Text_TimeHMSC")
    extern const char Text_TimeHMSC[18] = "%i:%.2i:%.2i.%.2i";
#pragma const_seg(".rdata$Text_TimeHMS")
    extern const char Text_TimeHMS[13] = "%i:%.2i:%.2i";
#pragma const_seg(".rdata$Text_TimeMSC")
    extern const char Text_TimeMSC[13] = "%i:%.2i.%.2i";
#pragma const_seg(".rdata$Text_TimeMS")
    extern const char Text_TimeMS[8] = "%i:%.2i";
#pragma const_seg(".rdata$Text_TimeSC")
    extern const char Text_TimeSC[8] = "%i.%.2i";
#pragma const_seg(".rdata$Text_TimeS")
    extern const char Text_TimeS[3] = "%i";
#pragma const_seg()
    f32 nuspecial_const_alpha = 1.0f;
    NUVEC nuspecial_const_tint = {1.0f, 1.0f, 1.0f};
    NUVEC cam_axes = {1.0f, 1.0f, 1.0f};
    NUCAMERA global_camera;
#pragma data_seg(".data$NuCameraProjectionFarClipBias")
    f32 NuCameraProjectionFarClipBias = 0.1f;
#pragma data_seg()
#pragma bss_seg(".bss$NuCameraVPortProjectionScaleX")
    f32 NuCameraVPortProjectionScaleX;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraVPortProjectionScaleY")
    f32 NuCameraVPortProjectionScaleY;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraScissorProjectionScaleX")
    f32 NuCameraScissorProjectionScaleX;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraScissorProjectionScaleY")
    f32 NuCameraScissorProjectionScaleY;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraScissorProjectionScratch")
    NUMTX NuCameraScissorProjectionScratch;
#pragma bss_seg()
#pragma bss_seg(".bss$clip_test_mtx")
    NUMTX clip_test_mtx;
#pragma bss_seg()
#pragma bss_seg(".bss$zx")
    f32 zx;
#pragma bss_seg()
#pragma bss_seg(".bss$zy")
    f32 zy;
#pragma const_seg(".rdata$ClipOutcodeMask32")
    __declspec(align(16)) extern const u32 ClipOutcodeMask32[4] = {32, 32, 32, 32};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask16")
    __declspec(align(16)) extern const u32 ClipOutcodeMask16[4] = {16, 16, 16, 16};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask1")
    __declspec(align(16)) extern const u32 ClipOutcodeMask1[4] = {1, 1, 1, 1};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask2")
    __declspec(align(16)) extern const u32 ClipOutcodeMask2[4] = {2, 2, 2, 2};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask8")
    __declspec(align(16)) extern const u32 ClipOutcodeMask8[4] = {8, 8, 8, 8};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask4")
    __declspec(align(16)) extern const u32 ClipOutcodeMask4[4] = {4, 4, 4, 4};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMinusOne")
    __declspec(align(16)) extern const f32 ClipOutcodeMinusOne[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
#pragma const_seg()
#pragma data_seg(".data$ClipFarLimits")
    __declspec(align(16)) f32 ClipFarLimits[4] = {1.0f, 1.0f, 1.0f, 1.0f};
#pragma data_seg()
#pragma data_seg(".data$PCClipExtentsEnabled")
    i32 PCClipExtentsEnabled = 1;
#pragma data_seg()
#pragma bss_seg(".bss$PCClipUseSSE")
    i32 PCClipUseSSE;
#pragma bss_seg()
#pragma bss_seg(".bss$ClipCornerBlocks")
    __declspec(align(16)) NUVEC4BLOCK_PC ClipCornerBlocks[2];
#pragma bss_seg()
#pragma bss_seg(".bss$ClipTransformedBlocks")
    __declspec(align(16)) NUVEC4BLOCK_PC ClipTransformedBlocks[2];
#pragma bss_seg()
#pragma bss_seg(".bss$clip_outcode_workspace_prefix")
    u32 clip_outcode_workspace_prefix[8];
#pragma bss_seg()
#pragma data_seg(".data$AIPathConnectionMargin")
    f32 AIPathConnectionMargin = 0.05f;
#pragma data_seg()
#pragma bss_seg(".bss$pNuCam")
    NUCAMERA *pNuCam;
#pragma bss_seg()
    // clang-format off: LLVM 18 and newer format initializer macros differently.
#define IDENTITY_NUMTX {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
    // clang-format on
    NUMTX NuMtxSetIdentityTemplate = IDENTITY_NUMTX;
    NUMTX vmtx = IDENTITY_NUMTX;
    NUMTX pmtx = IDENTITY_NUMTX;
    NUMTX cmtx = IDENTITY_NUMTX;
    NUMTX smtx = IDENTITY_NUMTX;
    NUMTX vpmtx = IDENTITY_NUMTX;
    NUMTX vpc_sci_mtx = IDENTITY_NUMTX;
    NUMTX vpc_vport_mtx = IDENTITY_NUMTX;
    NUMTX pc_vport_mtx = IDENTITY_NUMTX;
    NUMTX vpsmtx = IDENTITY_NUMTX;
    NUMTX clip_planes = IDENTITY_NUMTX;
#undef IDENTITY_NUMTX
#pragma bss_seg(".bss$nuspecial_const_alpha_enabled")
    i32 nuspecial_const_alpha_enabled;
#pragma bss_seg()
#pragma bss_seg(".bss$nuspecial_const_tint_enabled")
    i32 nuspecial_const_tint_enabled;
#pragma bss_seg()
#pragma bss_seg(".bss$nuspecial_draw_state")
    i32 nuspecial_draw_state;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTrigTable")
    f32 NuTrigTable[32768];
#pragma bss_seg()

// PC NuSound3 state and loop-hold conversion constants.
#pragma bss_seg(".bss$NuSound3Reverb")
    u8 NuSound3Reverb;
#pragma bss_seg()
    extern const f64 NuScale1000 = 1000.0;
    i32 NuSound3LoopHoldTime = 133;
    i32 NuSound3VolumeOffset = -300;
#pragma data_seg(".data$PCSoundVolumeMultiplier")
    i32 PCSoundVolumeMultiplier = 0x4000;
#pragma data_seg()
#pragma const_seg(".rdata$PCSoundOggExtensionText")
    extern const char PCSoundOggExtensionText[5] = ".ogg";
#pragma const_seg()
#pragma data_seg(".data$PCSoundOggExtension")
    const char *PCSoundOggExtension = PCSoundOggExtensionText;
#pragma data_seg()
#pragma data_seg(".data$SoundVoiceVolumeOffset")
    i32 SoundVoiceVolumeOffset = -300;
#pragma data_seg()
#pragma data_seg(".data$SoundVoiceEnabledGate")
    i32 SoundVoiceEnabledGate = 1;
#pragma data_seg()
#pragma bss_seg(".bss$SoundVoiceCriticalSection")
    CRITICAL_SECTION SoundVoiceCriticalSection;
#pragma bss_seg()
#pragma const_seg(".rdata$SoundVoice3DBufferIID")
    extern const GUID SoundVoice3DBufferIID = {
        0x279afa86,
        0x4981,
        0x11ce,
        {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60},
    };
#pragma const_seg()
#pragma bss_seg(".bss$NuSound3VolumeTable")
    f32 NuSound3VolumeTable[0x4000];
#pragma bss_seg()
    PC_NUDATFILEINFO pc_dat_file_infos[20];
    PC_NUMEMFILE pc_memfiles[20];
    i32 old_line_pos;
#pragma bss_seg(".bss$ADataList")
    AREA_DATA_PC *ADataList;
#pragma bss_seg()
#pragma bss_seg(".bss$AREACOUNT")
    i32 AREACOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$EDataList")
    EPISODEDATA_PC *EDataList;
#pragma bss_seg()
#pragma bss_seg(".bss$EPISODECOUNT")
    i32 EPISODECOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$LEVELCOUNT")
    i32 LEVELCOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$CDataList")
    CHARACTERDATA_PC *CDataList;
#pragma bss_seg()
#pragma bss_seg(".bss$CHARCOUNT")
    i32 CHARCOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$CheatSystem")
    CHEATSYSTEM_PC CheatSystem;
#pragma bss_seg()
#pragma data_seg(".data$CheatSystemPtr")
    CHEATSYSTEM_PC *CheatSystemPtr = &CheatSystem;
#pragma data_seg()
#pragma data_seg(".data$AREA_DEFAULTBONUSTIMETRIALTIME")
    u16 AREA_DEFAULTBONUSTIMETRIALTIME = 300;
#pragma data_seg(".data$AREA_DEFAULTCHALLENGETIME")
    u16 AREA_DEFAULTCHALLENGETIME = 600;
#pragma data_seg()
    AREA_DATA_PC *LastAData;
    AREA_DATA_PC *HUB_ADATA;

#pragma const_seg(".rdata$AreaFixUpName_Jedi")
    extern char AreaFixUpName_Jedi[8] = "Jedi";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Gungan")
    extern char AreaFixUpName_Gungan[8] = "Gungan";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Senate")
    extern char AreaFixUpName_Senate[8] = "senate";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Losttemple")
    extern char AreaFixUpName_Losttemple[12] = "losttemple";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Emperorfight")
    extern char AreaFixUpName_Emperorfight[16] = "emperorfight";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Moseisley")
    extern char AreaFixUpName_Moseisley[12] = "moseisley";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Tatooine")
    extern char AreaFixUpName_Tatooine[12] = "tatooine";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Gunship")
    extern char AreaFixUpName_Gunship[8] = "gunship";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Pursuit")
    extern char AreaFixUpName_Pursuit[8] = "pursuit";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Podsprint")
    extern char AreaFixUpName_Podsprint[12] = "podsprint";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Negotiations")
    extern char AreaFixUpName_Negotiations[16] = "negotiations";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Fairground")
    extern char AreaFixUpName_Fairground[12] = "Fairground";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Rooftops")
    extern char AreaFixUpName_Rooftops[12] = "Rooftops";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Gothamstreets")
    extern char AreaFixUpName_Gothamstreets[16] = "GothamStreets";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Batcave")
    extern char AreaFixUpName_Batcave[8] = "batcave";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Jimtest")
    extern char AreaFixUpName_Jimtest[8] = "JimTest";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_NbKashyyyk")
    extern char AreaFixUpName_NbKashyyyk[12] = "nb_kashyyyk";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_NbKamino")
    extern char AreaFixUpName_NbKamino[12] = "nb_kamino";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_NbDagobah")
    extern char AreaFixUpName_NbDagobah[12] = "nb_dagobah";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Hoth")
    extern char AreaFixUpName_Hoth[8] = "hoth";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Utapau")
    extern char AreaFixUpName_Utapau[8] = "utapau";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_LostTemple")
    extern char AreaFixUpName_LostTemple[12] = "LostTemple";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Bonus")
    extern char AreaFixUpName_Bonus[8] = "Bonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Bonus2")
    extern char AreaFixUpName_Bonus2[8] = "Bonus2";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Anewhope")
    extern char AreaFixUpName_Anewhope[12] = "ANewHope";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_BonusGunship")
    extern char AreaFixUpName_BonusGunship[16] = "Bonus_Gunship";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Anakinsflight")
    extern char AreaFixUpName_Anakinsflight[16] = "AnakinsFlight";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Podrace")
    extern char AreaFixUpName_Podrace[8] = "PodRace";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E6characterbonus")
    extern char AreaFixUpName_E6characterbonus[20] = "E6CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E5characterbonus")
    extern char AreaFixUpName_E5characterbonus[20] = "E5CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E4characterbonus")
    extern char AreaFixUpName_E4characterbonus[20] = "E4CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E3characterbonus")
    extern char AreaFixUpName_E3characterbonus[20] = "E3CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E2characterbonus")
    extern char AreaFixUpName_E2characterbonus[20] = "E2CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E1characterbonus")
    extern char AreaFixUpName_E1characterbonus[20] = "E1CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstar2battle")
    extern char AreaFixUpName_Deathstar2battle[20] = "DeathStar2Battle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Endorbattle")
    extern char AreaFixUpName_Endorbattle[12] = "EndorBattle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Speederchase")
    extern char AreaFixUpName_Speederchase[16] = "SpeederChase";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Sarlaccpit")
    extern char AreaFixUpName_Sarlaccpit[12] = "SarlaccPit";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Jabbaspalace")
    extern char AreaFixUpName_Jabbaspalace[16] = "JabbasPalace";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Cloudcityescape")
    extern char AreaFixUpName_Cloudcityescape[16] = "CloudCityEscape";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Cloudcitytrap")
    extern char AreaFixUpName_Cloudcitytrap[16] = "CloudCityTrap";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Dagobah")
    extern char AreaFixUpName_Dagobah[8] = "Dagobah";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Asteroidchase")
    extern char AreaFixUpName_Asteroidchase[16] = "AsteroidChase";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Hothescape")
    extern char AreaFixUpName_Hothescape[12] = "HothEscape";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Hothbattle")
    extern char AreaFixUpName_Hothbattle[12] = "HothBattle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstarbattle")
    extern char AreaFixUpName_Deathstarbattle[16] = "DeathStarBattle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstarescape")
    extern char AreaFixUpName_Deathstarescape[16] = "DeathStarEscape";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstarrescue")
    extern char AreaFixUpName_Deathstarrescue[16] = "DeathStarRescue";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Blockaderunner")
    extern char AreaFixUpName_Blockaderunner[16] = "BlockadeRunner";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Vader")
    extern char AreaFixUpName_Vader[8] = "Vader";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Temple")
    extern char AreaFixUpName_Temple[8] = "Temple";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Kashyyyk")
    extern char AreaFixUpName_Kashyyyk[12] = "Kashyyyk";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Grievous")
    extern char AreaFixUpName_Grievous[12] = "Grievous";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Cruiser")
    extern char AreaFixUpName_Cruiser[8] = "Cruiser";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Dogfight")
    extern char AreaFixUpName_Dogfight[12] = "Dogfight";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Dooku")
    extern char AreaFixUpName_Dooku[8] = "Dooku";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Factory")
    extern char AreaFixUpName_Factory[8] = "Factory";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Kamino")
    extern char AreaFixUpName_Kamino[8] = "Kamino";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Maul")
    extern char AreaFixUpName_Maul[8] = "Maul";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Retakepalace")
    extern char AreaFixUpName_Retakepalace[16] = "RetakePalace";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Palacerescue")
    extern char AreaFixUpName_Palacerescue[16] = "PalaceRescue";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Map")
    extern char AreaFixUpName_Map[4] = "map";
#pragma const_seg()

#pragma bss_seg(".bss$GOTHAMSTREETS_ADATA")
    AREA_DATA_PC *GOTHAMSTREETS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ROOFTOPS_ADATA")
    AREA_DATA_PC *ROOFTOPS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$FAIRGROUND_ADATA")
    AREA_DATA_PC *FAIRGROUND_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$NEGOTIATIONS_ADATA")
    AREA_DATA_PC *NEGOTIATIONS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$GUNGAN_ADATA")
    AREA_DATA_PC *GUNGAN_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$RESCUE_ADATA")
    AREA_DATA_PC *RESCUE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$PODSPRINT_ADATA")
    AREA_DATA_PC *PODSPRINT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$RETAKE_ADATA")
    AREA_DATA_PC *RETAKE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MAUL_ADATA")
    AREA_DATA_PC *MAUL_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BOUNTYHUNTERPURSUIT_ADATA")
    AREA_DATA_PC *BOUNTYHUNTERPURSUIT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$KAMINO_ADATA")
    AREA_DATA_PC *KAMINO_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$FACTORY_ADATA")
    AREA_DATA_PC *FACTORY_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$JEDI_ADATA")
    AREA_DATA_PC *JEDI_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DOOKU_ADATA")
    AREA_DATA_PC *DOOKU_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DOGFIGHT_ADATA")
    AREA_DATA_PC *DOGFIGHT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BATTLEOVERCORUSCANT_ADATA")
    AREA_DATA_PC *BATTLEOVERCORUSCANT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$CRUISER_ADATA")
    AREA_DATA_PC *CRUISER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$GRIEVOUS_ADATA")
    AREA_DATA_PC *GRIEVOUS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$KASHYYYK_ADATA")
    AREA_DATA_PC *KASHYYYK_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$TEMPLE_ADATA")
    AREA_DATA_PC *TEMPLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$VADER_ADATA")
    AREA_DATA_PC *VADER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BLOCKADERUNNER_ADATA")
    AREA_DATA_PC *BLOCKADERUNNER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$TATOOINE_ADATA")
    AREA_DATA_PC *TATOOINE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MOSEISLEY_ADATA")
    AREA_DATA_PC *MOSEISLEY_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARRESCUE_ADATA")
    AREA_DATA_PC *DEATHSTARRESCUE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARESCAPE_ADATA")
    AREA_DATA_PC *DEATHSTARESCAPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARBATTLE_ADATA")
    AREA_DATA_PC *DEATHSTARBATTLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTHBATTLE_ADATA")
    AREA_DATA_PC *HOTHBATTLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTHBATTLEA_LDATA")
    LEVELDATA_PC *HOTHBATTLEA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$getvehiclehoverheight_hothbattlehack")
    i32 getvehiclehoverheight_hothbattlehack;
#pragma bss_seg()
#pragma bss_seg(".bss$disable_narrow_socks")
    i32 disable_narrow_socks;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTHESCAPE_ADATA")
    AREA_DATA_PC *HOTHESCAPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ASTEROIDCHASE_ADATA")
    AREA_DATA_PC *ASTEROIDCHASE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DAGOBAH_ADATA")
    AREA_DATA_PC *DAGOBAH_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$CLOUDCITYTRAP_ADATA")
    AREA_DATA_PC *CLOUDCITYTRAP_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$CLOUDCITYESCAPE_ADATA")
    AREA_DATA_PC *CLOUDCITYESCAPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$JABBASPALACE_ADATA")
    AREA_DATA_PC *JABBASPALACE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$SARLACCPIT_ADATA")
    AREA_DATA_PC *SARLACCPIT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$SPEEDERCHASE_ADATA")
    AREA_DATA_PC *SPEEDERCHASE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ENDORBATTLE_ADATA")
    AREA_DATA_PC *ENDORBATTLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARBATTLE2_ADATA")
    AREA_DATA_PC *DEATHSTARBATTLE2_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$EMPERORFIGHT_ADATA")
    AREA_DATA_PC *EMPERORFIGHT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E1CHARACTER_ADATA")
    AREA_DATA_PC *E1CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E2CHARACTER_ADATA")
    AREA_DATA_PC *E2CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E3CHARACTER_ADATA")
    AREA_DATA_PC *E3CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E4CHARACTER_ADATA")
    AREA_DATA_PC *E4CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E5CHARACTER_ADATA")
    AREA_DATA_PC *E5CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E6CHARACTER_ADATA")
    AREA_DATA_PC *E6CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E1VEHICLE_ADATA")
    AREA_DATA_PC *E1VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E2VEHICLE_ADATA")
    AREA_DATA_PC *E2VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E3VEHICLE_ADATA")
    AREA_DATA_PC *E3VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E4VEHICLE_ADATA")
    AREA_DATA_PC *E4VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E5VEHICLE_ADATA")
    AREA_DATA_PC *E5VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E6VEHICLE_ADATA")
    AREA_DATA_PC *E6VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$PODRACE_ADATA")
    AREA_DATA_PC *PODRACE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ANAKINSFLIGHT_ADATA")
    AREA_DATA_PC *ANAKINSFLIGHT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUS_GUNSHIP_ADATA")
    AREA_DATA_PC *BONUS_GUNSHIP_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ANEWHOPE_ADATA")
    AREA_DATA_PC *ANEWHOPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$LEGOCITY_ADATA")
    AREA_DATA_PC *LEGOCITY_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$NEWTOWN_ADATA")
    AREA_DATA_PC *NEWTOWN_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$LOSTTEMPLE_ADATA")
    AREA_DATA_PC *LOSTTEMPLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$SENATE_ADATA")
    AREA_DATA_PC *SENATE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$UTAPAU_ADATA")
    AREA_DATA_PC *UTAPAU_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTH_ADATA")
    AREA_DATA_PC *HOTH_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUSDAGOBAH_ADATA")
    AREA_DATA_PC *BONUSDAGOBAH_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUSKAMINO_ADATA")
    AREA_DATA_PC *BONUSKAMINO_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUSKASHYYYK_ADATA")
    AREA_DATA_PC *BONUSKASHYYYK_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$JIMTEST_ADATA")
    AREA_DATA_PC *JIMTEST_ADATA;
#pragma bss_seg()

#pragma bss_seg(".bss$SfxFileTable")
    SFX_FILE_INFO_PC *SfxFileTable;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxFileCount")
    i32 SfxFileCount;
#pragma bss_seg()

#pragma const_seg(".rdata$AreaFixUpLSW")
    AREAFIXUP_PC AreaFixUpLSW[65] = {
        {AreaFixUpName_Map, &HUB_ADATA},
        {AreaFixUpName_Negotiations, &NEGOTIATIONS_ADATA},
        {AreaFixUpName_Gungan, &GUNGAN_ADATA},
        {AreaFixUpName_Palacerescue, &RESCUE_ADATA},
        {AreaFixUpName_Podsprint, &PODSPRINT_ADATA},
        {AreaFixUpName_Retakepalace, &RETAKE_ADATA},
        {AreaFixUpName_Maul, &MAUL_ADATA},
        {AreaFixUpName_Pursuit, &BOUNTYHUNTERPURSUIT_ADATA},
        {AreaFixUpName_Kamino, &KAMINO_ADATA},
        {AreaFixUpName_Factory, &FACTORY_ADATA},
        {AreaFixUpName_Jedi, &JEDI_ADATA},
        {AreaFixUpName_Gunship, &GUNSHIP_ADATA},
        {AreaFixUpName_Dooku, &DOOKU_ADATA},
        {AreaFixUpName_Dogfight, &DOGFIGHT_ADATA},
        {AreaFixUpName_Dogfight, &BATTLEOVERCORUSCANT_ADATA},
        {AreaFixUpName_Cruiser, &CRUISER_ADATA},
        {AreaFixUpName_Grievous, &GRIEVOUS_ADATA},
        {AreaFixUpName_Kashyyyk, &KASHYYYK_ADATA},
        {AreaFixUpName_Temple, &TEMPLE_ADATA},
        {AreaFixUpName_Vader, &VADER_ADATA},
        {AreaFixUpName_Blockaderunner, &BLOCKADERUNNER_ADATA},
        {AreaFixUpName_Tatooine, &TATOOINE_ADATA},
        {AreaFixUpName_Moseisley, &MOSEISLEY_ADATA},
        {AreaFixUpName_Deathstarrescue, &DEATHSTARRESCUE_ADATA},
        {AreaFixUpName_Deathstarescape, &DEATHSTARESCAPE_ADATA},
        {AreaFixUpName_Deathstarbattle, &DEATHSTARBATTLE_ADATA},
        {AreaFixUpName_Hothbattle, &HOTHBATTLE_ADATA},
        {AreaFixUpName_Hothescape, &HOTHESCAPE_ADATA},
        {AreaFixUpName_Asteroidchase, &ASTEROIDCHASE_ADATA},
        {AreaFixUpName_Dagobah, &DAGOBAH_ADATA},
        {AreaFixUpName_Cloudcitytrap, &CLOUDCITYTRAP_ADATA},
        {AreaFixUpName_Cloudcityescape, &CLOUDCITYESCAPE_ADATA},
        {AreaFixUpName_Jabbaspalace, &JABBASPALACE_ADATA},
        {AreaFixUpName_Sarlaccpit, &SARLACCPIT_ADATA},
        {AreaFixUpName_Speederchase, &SPEEDERCHASE_ADATA},
        {AreaFixUpName_Endorbattle, &ENDORBATTLE_ADATA},
        {AreaFixUpName_Deathstar2battle, &DEATHSTARBATTLE2_ADATA},
        {AreaFixUpName_Emperorfight, &EMPERORFIGHT_ADATA},
        {AreaFixUpName_E1characterbonus, &E1CHARACTER_ADATA},
        {AreaFixUpName_E2characterbonus, &E2CHARACTER_ADATA},
        {AreaFixUpName_E3characterbonus, &E3CHARACTER_ADATA},
        {AreaFixUpName_E4characterbonus, &E4CHARACTER_ADATA},
        {AreaFixUpName_E5characterbonus, &E5CHARACTER_ADATA},
        {AreaFixUpName_E6characterbonus, &E6CHARACTER_ADATA},
        {AreaFixUpName_E1characterbonus, &E1VEHICLE_ADATA},
        {AreaFixUpName_E2characterbonus, &E2VEHICLE_ADATA},
        {AreaFixUpName_E3characterbonus, &E3VEHICLE_ADATA},
        {AreaFixUpName_E4characterbonus, &E4VEHICLE_ADATA},
        {AreaFixUpName_E5characterbonus, &E5VEHICLE_ADATA},
        {AreaFixUpName_E6characterbonus, &E6VEHICLE_ADATA},
        {AreaFixUpName_Podrace, &PODRACE_ADATA},
        {AreaFixUpName_Anakinsflight, &ANAKINSFLIGHT_ADATA},
        {AreaFixUpName_BonusGunship, &BONUS_GUNSHIP_ADATA},
        {AreaFixUpName_Anewhope, &ANEWHOPE_ADATA},
        {AreaFixUpName_Bonus2, &LEGOCITY_ADATA},
        {AreaFixUpName_Bonus, &NEWTOWN_ADATA},
        {AreaFixUpName_LostTemple, &LOSTTEMPLE_ADATA},
        {AreaFixUpName_Senate, &SENATE_ADATA},
        {AreaFixUpName_Utapau, &UTAPAU_ADATA},
        {AreaFixUpName_Hoth, &HOTH_ADATA},
        {AreaFixUpName_NbDagobah, &BONUSDAGOBAH_ADATA},
        {AreaFixUpName_NbKamino, &BONUSKAMINO_ADATA},
        {AreaFixUpName_NbKashyyyk, &BONUSKASHYYYK_ADATA},
        {AreaFixUpName_Jimtest, &JIMTEST_ADATA},
        {NULL, NULL},
    };
#pragma const_seg()

#pragma const_seg(".rdata$AreaFixUpLostTemple")
    AREAFIXUP_PC AreaFixUpLostTemple[2] = {
        {AreaFixUpName_Losttemple, &LOSTTEMPLE_ADATA},
        {NULL, NULL},
    };
#pragma const_seg()

#pragma const_seg(".rdata$AreaFixUpGotham")
    AREAFIXUP_PC AreaFixUpGotham[5] = {
        {AreaFixUpName_Batcave, &HUB_ADATA},
        {AreaFixUpName_Gothamstreets, &GOTHAMSTREETS_ADATA},
        {AreaFixUpName_Rooftops, &ROOFTOPS_ADATA},
        {AreaFixUpName_Fairground, &FAIRGROUND_ADATA},
        {NULL, NULL},
    };
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarSource")
    extern const char NuTimeBarSource[51] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutimebar.c";
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarScanlineMicroseconds")
    extern const f64 NuTimeBarScanlineMicroseconds = 63.556;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarScanlinesPerFrame")
    extern const f64 NuTimeBarScanlinesPerFrame = 262.5;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarEngineColours")
    extern const u32 NuTimeBar_EngineColours[7] = {0x7f00007f, 0x7f007f00, 0x7f007f50, 0x7f507f50,
                                                   0x7f000050, 0x7f40407f, 0x7f7f0000};
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarDefaultColours")
    extern const u32 NuTimeBar_DefaultColours[16] = {
        0x00000040, 0x22222240, 0x00004440, 0x00008840, 0x44000040, 0x88000040, 0x44004440, 0x88008840,
        0x00440040, 0x00880040, 0x00444440, 0x00888840, 0x44440040, 0x88880040, 0x44444440, 0x88888840,
    };
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarQuarter")
    extern const f64 NuTimeBarQuarter = 0.25;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarQFontSize")
    extern const f32 NuTimeBarQFontSize = 16.0f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarQFontScale")
    extern const f32 NuTimeBarQFontScale = 0.0625f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarTop")
    extern const f64 NuTimeBarTop = 0.05000000074505806;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarColumnStep")
    extern const f64 NuTimeBarColumnStep = 3.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarSlotWidth")
    extern const f64 NuTimeBarSlotWidth = 0.02500000037252903;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarHorizontalTop")
    extern const f64 NuTimeBarHorizontalTop = 0.15000000596046448;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarValueFormat")
    extern const char NuTimeBarValueFormat[3] = "%d";
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarFontScale")
    extern const f32 NuTimeBarFontScale = 0.8500000238418579f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarVerticalScale")
    extern const f64 NuTimeBarVerticalScale = 120.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarNegativeSlotWidth")
    extern const f64 NuTimeBarNegativeSlotWidth = -0.02500000037252903;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarRight")
    extern const f64 NuTimeBarRight = 0.974999999627471;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarNameMargin")
    extern const f64 NuTimeBarNameMargin = 0.044999999925494194;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarHalfFontScale")
    extern const f32 NuTimeBarHalfFontScale = 0.42500001192092896f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarLinesPerSecond")
    extern const f64 NuTimeBarLinesPerSecond = 15734.0;
#pragma const_seg()
#pragma data_seg(".data$NuTimeBarPeakResetInterval")
    extern i32 NuTimeBarPeakResetInterval = 30;
#pragma data_seg()
#pragma data_seg(".data$NuTimeBarRenderPeakReset")
    extern i32 NuTimeBarRenderPeakReset = 1;
#pragma data_seg()
#pragma data_seg(".data$NuTimeBarHorizontalPeakReset")
    extern i32 NuTimeBarHorizontalPeakReset = 1;
#pragma data_seg()
#pragma bss_seg(".bss$NuTimeBar_EngineEnabled")
    i32 NuTimeBar_EngineEnabled;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimeBar_GpuFrameOutEnabled")
    i32 NuTimeBar_GpuFrameOutEnabled;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimeBar_PeakReset")
    i32 NuTimeBar_PeakReset;
#pragma bss_seg()
#pragma const_seg(".rdata$TextLoadPermTRCPath")
    extern const char TextLoadPermTRCPath[19] = "stuff\\text\\trc.csv";
#pragma const_seg()
#pragma const_seg(".rdata$TextLoadPermAPIExtension")
    extern const char TextLoadPermAPIExtension[9] = "_api.txt";
#pragma const_seg()
#pragma const_seg(".rdata$TextLoadPermTextDirectory")
    extern const char TextLoadPermTextDirectory[12] = "stuff\\text\\";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformXbox")
    extern const char TextPlatformXbox[5] = "xbox";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPC")
    extern const char TextPlatformPC[3] = "pc";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPSP")
    extern const char TextPlatformPSP[4] = "psp";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPS3")
    extern const char TextPlatformPS3[4] = "ps3";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPS2")
    extern const char TextPlatformPS2[4] = "ps2";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformGameCube")
    extern const char TextPlatformGameCube[3] = "gc";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatform360")
    extern const char TextPlatform360[4] = "360";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformWii")
    extern const char TextPlatformWii[4] = "wii";
#pragma const_seg()

#pragma bss_seg(".bss$tALONGTIMEAGO")
    i16 tALONGTIMEAGO;
#pragma bss_seg()
#pragma bss_seg(".bss$NuQFntCSModeStack")
    i32 NuQFntCSModeStack[16];
#pragma bss_seg()
#pragma bss_seg(".bss$NuQFntCSModeStackIndex")
    i32 NuQFntCSModeStackIndex;

#pragma bss_seg(".bss$gizmotypes")
    void *gizmotypes;
#pragma bss_seg()
#pragma data_seg(".data$PS2_VREZ_W")
    i32 PS2_VREZ_W = 640;
#pragma data_seg()
#pragma data_seg(".data$PS2_VREZ_H")
    i32 PS2_VREZ_H = 224;
#pragma data_seg()

    i32 PlayerID[2] = {-1, -1};
    i16 PlayerList[8];
    APICHARACTERMODELLIST Hub_ModelList[6];
    LEVELDATA_PC *LDataList;
    i32 MAXLDATA = 0x64;
    LEVELDATA_PC *NEWGAME_LDATA;
    LEVELDATA_PC *LOADGAME_LDATA;
    LEVELDATA_PC *LastLData;
    LEVELDATA_PC *NewLData;
    LEVELDATA_PC *HUB_LDATA;
    LEVELDATA_PC *STATUS_LDATA;
    LEVELDATA_PC *CREDITS_LDATA;
    f32 MainRenderTime;
    f32 MainRenderTargetTime;
    f32 WaitingForLevelTime;
    f32 WaitingForCharacterTime;
    f32 MAXFRAMETIME;
    i32 save_paused;
    i32 netclient;
    i32 NOSOUND;
    volatile i32 nuvideo_global_vbcnt;
    i32 nurndr_tritot_this_frame;
    i32 openlevels;
    u8 snakebodies[0x440];
#pragma data_seg(".data$gizaimessage_prefix")
    char gizaimessage_prefix[5] = "msg_";
#pragma data_seg()
#pragma const_seg(".rdata$GizAIMessage_NameFormat")
    extern const char GizAIMessage_NameFormat[3] = "%s";
#pragma const_seg(".rdata$GizAIMessage_PrefixedNameFormat")
    extern const char GizAIMessage_PrefixedNameFormat[5] = "%s%s";
#pragma const_seg()
    GIZAIMESSAGESYS *gizaimessagesys;
    f32 PanelProjectionX = 1222.0f;
    f32 PanelProjectionY = 1145.0f;
    f32 PanelWideWidth = 740.0f;
    f32 PanelWideHeight = 387.5f;
#pragma bss_seg(".bss$Panel3DMulX")
    f32 PANEL3DMULX;
#pragma bss_seg()
#pragma bss_seg(".bss$Panel3DMulY")
    f32 PANEL3DMULY;
#pragma bss_seg()
    LANGUAGEDATA Game_LanguageList[7] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}, {-1, 0}};
    LANGUAGEDATA Text_LanguageList_Default[7] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}, {-1, 0}};
    LANGUAGEDATA *Text_LanguageList = Text_LanguageList_Default;
    i32 LANGUAGECOUNT = 6;
    i32 Text_Language = 1;
    i32 INTROTEXT_ID = -1;
    void *Text_AppFont;
    char **TTab_Original;
    char **TTab;
    i32 Text_MaxStrings_Overall;
    u32 *Text_StringBits;
#pragma const_seg(".rdata$Text_ErrorText")
    extern const char Text_ErrorText[7] = "Err...";
#pragma const_seg()
#pragma const_seg(".rdata$Text_UnknownText")
    extern const char Text_UnknownText[2] = "?";
#pragma const_seg()
#pragma const_seg(".rdata$Text_WibbleCodeword")
    extern const char Text_WibbleCodeword[7] = "wibble";
#pragma const_seg()
#pragma const_seg(".rdata$Text_CrossCodeword")
    extern const char Text_CrossCodeword[6] = "cross";
#pragma const_seg()
    void (*buttonmapfn)(char *, char *);
    i32 smarttext_fwn;
#pragma const_seg(".rdata$Text_PathEnglishText")
    extern const char Text_PathEnglishText[8] = "english";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathFrenchText")
    extern const char Text_PathFrenchText[7] = "french";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathGermanText")
    extern const char Text_PathGermanText[7] = "german";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathItalianText")
    extern const char Text_PathItalianText[8] = "italian";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathSpanishText")
    extern const char Text_PathSpanishText[8] = "spanish";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathDanishText")
    extern const char Text_PathDanishText[7] = "danish";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathJapaneseText")
    extern const char Text_PathJapaneseText[9] = "japanese";
#pragma const_seg()
#pragma const_seg(".rdata$Text_EnglishName")
    extern const char Text_EnglishName[8] = "English";
#pragma const_seg()
#pragma const_seg(".rdata$Text_FrenchName")
    extern const char Text_FrenchName[10] = "Fran\xc3\xa7"
                                            "ais";
#pragma const_seg()
#pragma const_seg(".rdata$Text_GermanName")
    extern const char Text_GermanName[8] = "Deutsch";
#pragma const_seg()
#pragma const_seg(".rdata$Text_ItalianName")
    extern const char Text_ItalianName[9] = "Italiano";
#pragma const_seg()
#pragma const_seg(".rdata$Text_SpanishName")
    extern const char Text_SpanishName[9] = "Espa\xc3\xb1"
                                            "ol";
#pragma const_seg()
#pragma const_seg(".rdata$Text_DanishName")
    extern const char Text_DanishName[6] = "Dansk";
#pragma const_seg()
    char *Text_ErrorString = const_cast<char *>(Text_ErrorText);
    char *txt_NULL = const_cast<char *>(Text_UnknownText);
    char *txt_UNKNOWN = const_cast<char *>(Text_UnknownText);
    char *Text_PathEnglish = const_cast<char *>(Text_PathEnglishText);
    char *Text_PathFrench = const_cast<char *>(Text_PathFrenchText);
    char *Text_PathGerman = const_cast<char *>(Text_PathGermanText);
    char *Text_PathItalian = const_cast<char *>(Text_PathItalianText);
    char *Text_PathSpanish = const_cast<char *>(Text_PathSpanishText);
    char *Text_PathDanish = const_cast<char *>(Text_PathDanishText);
    char *Text_PathJapanese = const_cast<char *>(Text_PathJapaneseText);
    char *apitxt_ENGLISH = const_cast<char *>(Text_EnglishName);
    char *apitxt_FRENCH = const_cast<char *>(Text_FrenchName);
    char *apitxt_GERMAN = const_cast<char *>(Text_GermanName);
    char *apitxt_ITALIAN = const_cast<char *>(Text_ItalianName);
    char *apitxt_SPANISH = const_cast<char *>(Text_SpanishName);
    char *apitxt_DANISH = const_cast<char *>(Text_DanishName);
#pragma bss_seg(".bss$tNULL")
    i16 tNULL;
#pragma bss_seg()
#pragma bss_seg(".bss$tUNKNOWN")
    i16 tUNKNOWN;
#pragma bss_seg()
#pragma bss_seg(".bss$tENGLISH")
    i16 tENGLISH;
#pragma bss_seg()
#pragma bss_seg(".bss$tFRENCH")
    i16 tFRENCH;
#pragma bss_seg()
#pragma bss_seg(".bss$tGERMAN")
    i16 tGERMAN;
#pragma bss_seg()
#pragma bss_seg(".bss$tITALIAN")
    i16 tITALIAN;
#pragma bss_seg()
#pragma bss_seg(".bss$tSPANISH")
    i16 tSPANISH;
#pragma bss_seg()
#pragma bss_seg(".bss$tDANISH")
    i16 tDANISH;
#pragma bss_seg()

// NuWaterReset writes these standalone four-byte PC globals.
#pragma bss_seg(".bss$NuWaterResetState")
    i32 nu_water_reset_state;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWaterResetStatus")
    i32 nu_water_reset_status;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimeTickBase")
    i64 NuTime_TickBase;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimePerformanceFrequency")
    i64 NuTime_PerformanceFrequency;
#pragma bss_seg()
#pragma const_seg(".rdata$NuTimeLowScale")
    extern const f64 NuTime_LowScale = 4294967295.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeMicroScale")
    extern const f64 NuTime_MicroScale = 1000000.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeScanlineScale")
    extern const f64 NuTime_ScanlineScale = 272.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeRefreshRate")
    extern const f64 NuTime_RefreshRate = 60.0;
#pragma const_seg()
    void (*nuapi_endframe_callbackfn)(void);
    u8 fstackmem[NU_PORTAL_STACK_SIZE];
    VARIPTR fstack;
    VARIPTR fstack_top;
    VARIPTR fstack_limit;
    i32 DebrisCutSceneMode;
    RTLSET_PC *PartRTL;
#pragma bss_seg(".bss$edbits_base_scene")
    NUGSCN *edbits_base_scene;
#pragma bss_seg()
#pragma bss_seg(".bss$edbits_base_terrain")
    void *edbits_base_terrain;
#pragma bss_seg()
#pragma bss_seg(".bss$NuSound3Initialized")
    i32 NuSound3Initialized;
#pragma bss_seg()
#pragma bss_seg(".bss$MusicInfo")
    struct nusound_filename_info_s *MusicInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxHashHeads")
    i16 *SfxHashHeads;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxInfo")
    SFX_RECORD_PC *SfxInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxCRCTable")
    u32 *SfxCRCTable;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxCRCTableInitialized")
    i32 SfxCRCTableInitialized;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPrimVertexGroupSource")
    NUPRIM_VERTEX_GROUP_SOURCE_PC *NuPrimVertexGroupSource;
#pragma bss_seg()
#pragma bss_seg(".bss$AmbientPairTab")
    i16 *AmbientPairTab;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPrimNeedsOverbrightening")
    u8 NuPrimNeedsOverbrightening;
#pragma bss_seg()
#pragma bss_seg(".bss$levelconfig_ldata")
    LEVELDATA_PC *levelconfig_ldata;
#pragma bss_seg()
#pragma bss_seg(".bss$Level_ConfigBeforeLoad_GameKeywords")
    NUFPCOMJMP *Level_ConfigBeforeLoad_GameKeywords;
#pragma bss_seg()
#pragma bss_seg(".bss$Level_ConfigAfterLoad_GameKeywords")
    NUFPCOMJMP *Level_ConfigAfterLoad_GameKeywords;
#pragma bss_seg()
#pragma bss_seg(".bss$fnInterpreterError")
    void(__cdecl *fnInterpreterError)(NUFPAR_PC *parser);
#pragma bss_seg()
    TERRAIN_SURFACE_PC TerSurface[32] = {
        {1.0f, 0, 0xffff},     {1.0f, 65, 0xffff},   {1.0f, 2, 0xffff},      {1.0f, 8, 0xffff},   {1.0f, 65536, 0xffff},
        {1.0f, 66560, 0xffff}, {1.0f, 0, 0xffff},    {1.0f, 2048, 0xffff},   {1.0f, 0, 0xffff},   {1.0f, 8192, 0xffff},
        {1.0f, 10, 0xffff},    {1.0f, 67, 0xffff},   {1.0f, 16, 0xffff},     {1.0f, 16, 0xffff},  {1.0f, 8192, 0xffff},
        {1.0f, 8192, 0xffff},  {0.1f, 1088, 0xffff}, {0.2f, 131074, 0xffff}, {1.0f, 81, 0xffff},  {1.0f, 512, 0xffff},
        {1.0f, 8192, 0xffff},  {1.0f, 514, 0xffff},  {1.0f, 64, 0xffff},     {1.0f, 192, 0xffff}, {1.0f, 8192, 0xffff},
        {1.0f, 4160, 0xffff},  {1.0f, 0, 0xffff},    {1.0f, 5184, 0xffff},   {1.0f, 256, 0xffff}, {1.0f, 320, 0xffff},
        {1.0f, 64, 0xffff},    {1.0f, 64, 0xffff},
    };
#pragma const_seg(".rdata$GetSfxSwPPadName")
    extern const char GetSfxSwPPadName[7] = "SwPPad";
#pragma const_seg(".rdata$LevelConfigKeywordFile")
    extern const char LevelConfigKeywordFile[5] = "file";
#pragma const_seg(".rdata$LevelConfigKeywordDir")
    extern const char LevelConfigKeywordDir[4] = "dir";
#pragma const_seg(".rdata$LevelConfigKeywordStart")
    extern const char LevelConfigKeywordStart[12] = "level_start";
#pragma const_seg(".rdata$LevelConfigKeywordEnd")
    extern const char LevelConfigKeywordEnd[10] = "level_end";
#pragma const_seg(".rdata$LevelBeforeLoadName")
    extern const char LevelBeforeLoadName[16] = "levelbeforeload";
#pragma const_seg(".rdata$NuCameraAllocSource")
    extern const char NuCameraAllocSource[54] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nucamera_gen.c";
#pragma const_seg(".rdata$LoadPermEpisodesPath")
    extern const char LoadPermEpisodesPath[20] = "levels\\episodes.txt";
#pragma const_seg(".rdata$LoadPermAreasPath")
    extern const char LoadPermAreasPath[17] = "levels\\areas.txt";
#pragma const_seg(".rdata$LoadPermLevelsPath")
    extern const char LoadPermLevelsPath[18] = "levels\\levels.txt";
#pragma const_seg(".rdata$EpisodeConfigKeyword_area")
    extern const char EpisodeConfigKeyword_area[5] = "area";
#pragma const_seg(".rdata$EpisodeConfigKeyword_episode_start")
    extern const char EpisodeConfigKeyword_episode_start[14] = "episode_start";
#pragma const_seg(".rdata$EpisodeConfigKeyword_episode_end")
    extern const char EpisodeConfigKeyword_episode_end[12] = "episode_end";
#pragma const_seg(".rdata$LevelAfterLoadName")
    extern const char LevelAfterLoadName[15] = "levelafterload";
#pragma const_seg(".rdata$LevelConfigKeywordTest")
    extern const char LevelConfigKeywordTest[11] = "test_level";
#pragma const_seg(".rdata$LevelConfigKeywordIntro")
    extern const char LevelConfigKeywordIntro[12] = "intro_level";
#pragma const_seg(".rdata$LevelConfigKeywordMidtro")
    extern const char LevelConfigKeywordMidtro[13] = "midtro_level";
#pragma const_seg(".rdata$LevelConfigKeywordCutscene")
    extern const char LevelConfigKeywordCutscene[15] = "cutscene_level";
#pragma const_seg(".rdata$LevelConfigKeywordOutro")
    extern const char LevelConfigKeywordOutro[12] = "outro_level";
#pragma const_seg(".rdata$LevelConfigKeywordStatus")
    extern const char LevelConfigKeywordStatus[13] = "status_level";
#pragma const_seg(".rdata$LevelConfigKeywordNewGame")
    extern const char LevelConfigKeywordNewGame[14] = "newgame_level";
#pragma const_seg(".rdata$LevelConfigKeywordLoadGame")
    extern const char LevelConfigKeywordLoadGame[15] = "loadgame_level";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_mipmapmode")
    extern const char LevelConfigGenericKeyword_mipmapmode[11] = "mipmapmode";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_fix_strobing_anims")
    extern const char LevelConfigGenericKeyword_fix_strobing_anims[19] = "fix_strobing_anims";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_max_ter_groups")
    extern const char LevelConfigGenericKeyword_max_ter_groups[15] = "max_ter_groups";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_max_ter_platforms")
    extern const char LevelConfigGenericKeyword_max_ter_platforms[18] = "max_ter_platforms";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backr")
    extern const char LevelConfigGenericKeyword_backr[6] = "backr";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backg")
    extern const char LevelConfigGenericKeyword_backg[6] = "backg";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backb")
    extern const char LevelConfigGenericKeyword_backb[6] = "backb";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backr_top")
    extern const char LevelConfigGenericKeyword_backr_top[10] = "backr_top";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backg_top")
    extern const char LevelConfigGenericKeyword_backg_top[10] = "backg_top";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backb_top")
    extern const char LevelConfigGenericKeyword_backb_top[10] = "backb_top";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backr_bottom")
    extern const char LevelConfigGenericKeyword_backr_bottom[13] = "backr_bottom";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backg_bottom")
    extern const char LevelConfigGenericKeyword_backg_bottom[13] = "backg_bottom";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backb_bottom")
    extern const char LevelConfigGenericKeyword_backb_bottom[13] = "backb_bottom";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_farclip_hack")
    extern const char LevelConfigGenericKeyword_farclip_hack[13] = "farclip_hack";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_farclip")
    extern const char LevelConfigGenericKeyword_farclip[8] = "farclip";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_nearclip")
    extern const char LevelConfigGenericKeyword_nearclip[9] = "nearclip";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_farclip_pc")
    extern const char LevelConfigGenericKeyword_farclip_pc[11] = "farclip_pc";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_nearclip_pc")
    extern const char LevelConfigGenericKeyword_nearclip_pc[12] = "nearclip_pc";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_hover_height")
    extern const char LevelConfigGenericKeyword_hover_height[13] = "hover_height";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_blobshadow_alpha")
    extern const char LevelConfigGenericKeyword_blobshadow_alpha[17] = "blobshadow_alpha";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_blobshadow_fadenear")
    extern const char LevelConfigGenericKeyword_blobshadow_fadenear[20] = "blobshadow_fadenear";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_blobshadow_fadefar")
    extern const char LevelConfigGenericKeyword_blobshadow_fadefar[19] = "blobshadow_fadefar";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_reflect_range")
    extern const char LevelConfigGenericKeyword_reflect_range[14] = "reflect_range";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_reflect_y")
    extern const char LevelConfigGenericKeyword_reflect_y[10] = "reflect_y";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_metal")
    extern const char LevelConfigGenericKeyword_metal[6] = "metal";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_in_space")
    extern const char LevelConfigGenericKeyword_in_space[9] = "in_space";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_override_nopickupgravity")
    extern const char LevelConfigGenericKeyword_override_nopickupgravity[25] = "override_nopickupgravity";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_hidden_icons")
    extern const char LevelConfigGenericKeyword_hidden_icons[13] = "hidden_icons";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_pickups_to_panel")
    extern const char LevelConfigGenericKeyword_pickups_to_panel[17] = "pickups_to_panel";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_forget_takeovers")
    extern const char LevelConfigGenericKeyword_forget_takeovers[17] = "forget_takeovers";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_narrow_socks")
    extern const char LevelConfigGenericKeyword_narrow_socks[13] = "narrow_socks";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_camera_rain")
    extern const char LevelConfigGenericKeyword_camera_rain[12] = "camera_rain";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_terrain_rain")
    extern const char LevelConfigGenericKeyword_terrain_rain[13] = "terrain_rain";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_double_score")
    extern const char LevelConfigGenericKeyword_double_score[13] = "double_score";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_flat_terrain")
    extern const char LevelConfigGenericKeyword_flat_terrain[13] = "flat_terrain";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_cam_tilt")
    extern const char LevelConfigGenericKeyword_cam_tilt[9] = "cam_tilt";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_raycaststep")
    extern const char LevelConfigGenericKeyword_raycaststep[12] = "raycaststep";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_plat_scan_dist")
    extern const char LevelConfigGenericKeyword_plat_scan_dist[15] = "plat_scan_dist";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_r")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_r[23] = "waterripple_startcol_r";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_g")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_g[23] = "waterripple_startcol_g";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_b")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_b[23] = "waterripple_startcol_b";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_a")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_a[23] = "waterripple_startcol_a";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_r")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_r[21] = "waterripple_endcol_r";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_g")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_g[21] = "waterripple_endcol_g";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_b")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_b[21] = "waterripple_endcol_b";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_a")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_a[21] = "waterripple_endcol_a";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_life")
    extern const char LevelConfigGenericKeyword_waterripple_life[17] = "waterripple_life";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_cam_pullback_dist")
    extern const char LevelConfigGenericKeyword_cam_pullback_dist[18] = "cam_pullback_dist";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_cam_lateral_dist")
    extern const char LevelConfigGenericKeyword_cam_lateral_dist[17] = "cam_lateral_dist";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_campos_seek")
    extern const char LevelConfigGenericKeyword_campos_seek[12] = "campos_seek";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_camang_seek")
    extern const char LevelConfigGenericKeyword_camang_seek[12] = "camang_seek";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_shadowtype")
    extern const char LevelConfigGenericKeyword_shadowtype[11] = "shadowtype";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_music")
    extern const char LevelConfigGenericKeyword_music[6] = "music";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_music_other")
    extern const char LevelConfigGenericKeyword_music_other[12] = "music_other";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_sfx_ambient")
    extern const char LevelConfigGenericKeyword_sfx_ambient[12] = "sfx_ambient";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_conveyor")
    extern const char LevelConfigGenericKeyword_conveyor[9] = "conveyor";

#pragma const_seg(".rdata$LevelSplinesStartName")
    extern const char LevelSplinesStartName[6] = "start";
#pragma const_seg()
#pragma const_seg(".rdata$LevelSplinesStartCamName")
    extern const char LevelSplinesStartCamName[10] = "start_cam";

#pragma const_seg(".rdata$NuLstCreateTag")
    extern const char NuLstCreateTag[52] = "d:\\projects\\game_saga\\nu2api.saga\\gamelib\\listman.c";
#pragma const_seg()
#pragma const_seg(".rdata$LevelConfigFarDistance")
    extern const f32 LevelConfigFarDistance = 2000000.0f;
#pragma const_seg(".rdata$LevelConfigOff")
    extern const char LevelConfigOff[4] = "off";
#pragma const_seg(".rdata$LevelConfigOn")
    extern const char LevelConfigOn[3] = "on";
#pragma const_seg()
#pragma data_seg(".data$LevelConfigKeywords_BeforeLoad")
    NUFPCOMJMP LevelConfigKeywords_BeforeLoad[45] = {
        {const_cast<char *>(LevelConfigGameKeyword_max_tightropes), LC_BL_max_tightropes},
        {const_cast<char *>(LevelConfigGameKeyword_max_signals), LC_BL_max_signals},
        {const_cast<char *>(LevelConfigGameKeyword_max_levers), LC_BL_max_levers},
        {const_cast<char *>(LevelConfigGameKeyword_max_technos), LC_BL_max_technos},
        {const_cast<char *>(LevelConfigGameKeyword_max_zipups), LC_BL_max_zipups},
        {const_cast<char *>(LevelConfigGameKeyword_max_grapples), LC_BL_max_grapples},
        {const_cast<char *>(LevelConfigGameKeyword_max_obstacles), LC_BL_max_obstacles},
        {const_cast<char *>(LevelConfigGameKeyword_max_obstacle_objects), LC_BL_max_obstacle_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_turrets), LC_BL_max_turrets},
        {const_cast<char *>(LevelConfigGameKeyword_max_buildits), LC_BL_max_buildits},
        {const_cast<char *>(LevelConfigGameKeyword_max_buildit_objects), LC_BL_max_buildit_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_spinneranim_objs), LC_BL_max_spinneranim_objs},
        {const_cast<char *>(LevelConfigGameKeyword_max_shards), LC_BL_max_shards},
        {const_cast<char *>(LevelConfigGameKeyword_max_spinners), LC_BL_max_spinners},
        {const_cast<char *>(LevelConfigGameKeyword_max_minicuts), LC_BL_max_minicuts},
        {const_cast<char *>(LevelConfigGameKeyword_max_minicut_stages), LC_BL_max_minicutParts},
        {const_cast<char *>(LevelConfigGameKeyword_maxgizmoblowuptypes), LC_BL_max_gizmoblowuptypes},
        {const_cast<char *>(LevelConfigGameKeyword_maxgizmoblowups), LC_BL_max_gizmoblowups},
        {const_cast<char *>(LevelConfigGameKeyword_max_climb_objects), LC_BL_max_climb_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_attractos), LC_BL_max_attractos},
        {const_cast<char *>(LevelConfigGameKeyword_max_timers), LC_BL_max_timers},
        {const_cast<char *>(LevelConfigGameKeyword_max_guidelines), LC_BL_max_guidelines},
        {const_cast<char *>(LevelConfigGameKeyword_max_ledges), LC_BL_max_ledges},
        {const_cast<char *>(LevelConfigGameKeyword_max_securitydoors), LC_BL_max_securitydoors},
        {const_cast<char *>(LevelConfigGameKeyword_max_tubes), LC_BL_max_tubes},
        {const_cast<char *>(LevelConfigGameKeyword_max_pickups), LC_BL_max_pickups},
        {const_cast<char *>(LevelConfigGameKeyword_max_gameantinodes), LC_BL_max_gameantinodes},
        {const_cast<char *>(LevelConfigGameKeyword_max_gizpanels), LC_BL_max_gizpanels},
        {const_cast<char *>(LevelConfigGameKeyword_max_force), LC_BL_max_force},
        {const_cast<char *>(LevelConfigGameKeyword_max_force_objects), LC_BL_max_force_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_pushblocks), LC_BL_max_pushblocks},
        {const_cast<char *>(LevelConfigGameKeyword_max_pushblock_endpos), LC_BL_max_pushblock_endpos},
        {const_cast<char *>(LevelConfigGameKeyword_max_gizspecials), LC_BL_max_gizspecials},
        {const_cast<char *>(LevelConfigGameKeyword_max_hatmachines), LC_BL_max_hatmachines},
        {const_cast<char *>(LevelConfigGameKeyword_max_doors), LC_BL_max_doors},
        {const_cast<char *>(LevelConfigGameKeyword_max_teleports), LC_BL_max_teleports},
        {const_cast<char *>(LevelConfigGameKeyword_max_gizrandoms), LC_BL_max_gizrandoms},
        {const_cast<char *>(LevelConfigGameKeyword_max_torpmachines), LC_BL_max_torpmachines},
        {const_cast<char *>(LevelConfigGameKeyword_max_bridges), LC_BL_max_bridges},
        {const_cast<char *>(LevelConfigGameKeyword_max_plugs), LC_BL_max_plugs},
        {const_cast<char *>(LevelConfigGameKeyword_max_bombgen_objects), LC_BL_max_bombgen_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_bombgens), LC_BL_max_bombgens},
        {const_cast<char *>(LevelConfigGameKeyword_windspeed), LC_BL_wind_speed},
        {const_cast<char *>(LevelConfigGameKeyword_windsize), LC_BL_wind_size},
        {NULL, NULL},
    };
#pragma data_seg()
#pragma data_seg(".data$LevelConfig_BeforeLoad_GenericKeywords")
    NUFPCOMJMP LevelConfig_BeforeLoad_GenericKeywords[5] = {
        {const_cast<char *>(LevelConfigGenericKeyword_mipmapmode), LC_BL_mipmapmode},
        {const_cast<char *>(LevelConfigGenericKeyword_fix_strobing_anims), LC_BL_fix_strobing_anims},
        {const_cast<char *>(LevelConfigGenericKeyword_max_ter_groups), LC_BL_max_ter_groups},
        {const_cast<char *>(LevelConfigGenericKeyword_max_ter_platforms), LC_BL_max_ter_platforms},
        {NULL, NULL},
    };
#pragma data_seg()

#pragma data_seg(".data$LevelConfig_AfterLoad_GenericKeywords")
    NUFPCOMJMP LevelConfig_AfterLoad_GenericKeywords[53] = {
        {const_cast<char *>(LevelConfigGenericKeyword_backr), LC_AL_backr},
        {const_cast<char *>(LevelConfigGenericKeyword_backg), LC_AL_backg},
        {const_cast<char *>(LevelConfigGenericKeyword_backb), LC_AL_backb},
        {const_cast<char *>(LevelConfigGenericKeyword_backr_top), LC_AL_backr_top},
        {const_cast<char *>(LevelConfigGenericKeyword_backg_top), LC_AL_backg_top},
        {const_cast<char *>(LevelConfigGenericKeyword_backb_top), LC_AL_backb_top},
        {const_cast<char *>(LevelConfigGenericKeyword_backr_bottom), LC_AL_backr_bottom},
        {const_cast<char *>(LevelConfigGenericKeyword_backg_bottom), LC_AL_backg_bottom},
        {const_cast<char *>(LevelConfigGenericKeyword_backb_bottom), LC_AL_backb_bottom},
        {const_cast<char *>(LevelConfigGenericKeyword_farclip_hack), LC_AL_farclip_hack},
        {const_cast<char *>(LevelConfigGenericKeyword_farclip), LC_AL_farclip},
        {const_cast<char *>(LevelConfigGenericKeyword_nearclip), LC_AL_nearclip},
        {const_cast<char *>(LevelConfigGenericKeyword_farclip_pc), LC_AL_farclip},
        {const_cast<char *>(LevelConfigGenericKeyword_nearclip_pc), LC_AL_nearclip},
        {const_cast<char *>(LevelConfigGenericKeyword_hover_height), LC_AL_hover_height},
        {const_cast<char *>(LevelConfigGenericKeyword_blobshadow_alpha), LC_AL_blobshadow_alpha},
        {const_cast<char *>(LevelConfigGenericKeyword_blobshadow_fadenear), LC_AL_blobshadow_fadenear},
        {const_cast<char *>(LevelConfigGenericKeyword_blobshadow_fadefar), LC_AL_blobshadow_fadefar},
        {const_cast<char *>(LevelConfigGenericKeyword_reflect_range), LC_AL_reflect_range},
        {const_cast<char *>(LevelConfigGenericKeyword_reflect_y), LC_AL_reflect_y},
        {const_cast<char *>(LevelConfigGenericKeyword_metal), LC_AL_metal},
        {const_cast<char *>(LevelConfigGenericKeyword_in_space), LC_AL_in_space},
        {const_cast<char *>(LevelConfigGenericKeyword_override_nopickupgravity), LC_AL_override_nopickupgravity},
        {const_cast<char *>(LevelConfigGenericKeyword_hidden_icons), LC_AL_hidden_icons},
        {const_cast<char *>(LevelConfigGenericKeyword_pickups_to_panel), LC_AL_pickups_to_panel},
        {const_cast<char *>(LevelConfigGenericKeyword_forget_takeovers), LC_AL_forget_takeovers},
        {const_cast<char *>(LevelConfigGenericKeyword_narrow_socks), LC_AL_narrow_socks},
        {const_cast<char *>(LevelConfigGenericKeyword_camera_rain), LC_AL_camera_rain},
        {const_cast<char *>(LevelConfigGenericKeyword_terrain_rain), LC_AL_terrain_rain},
        {const_cast<char *>(LevelConfigGenericKeyword_double_score), LC_AL_double_score},
        {const_cast<char *>(LevelConfigGenericKeyword_flat_terrain), LC_AL_flat_terrain},
        {const_cast<char *>(LevelConfigGenericKeyword_cam_tilt), LC_AL_cam_tilt},
        {const_cast<char *>(LevelConfigGenericKeyword_raycaststep), LC_AL_raycaststep},
        {const_cast<char *>(LevelConfigGenericKeyword_plat_scan_dist), LC_AL_plat_scan_dist},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_r), LC_AL_waterripple_startcol_r},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_g), LC_AL_waterripple_startcol_g},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_b), LC_AL_waterripple_startcol_b},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_a), LC_AL_waterripple_startcol_a},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_r), LC_AL_waterripple_endcol_r},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_g), LC_AL_waterripple_endcol_g},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_b), LC_AL_waterripple_endcol_b},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_a), LC_AL_waterripple_endcol_a},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_life), LC_AL_waterripple_life},
        {const_cast<char *>(LevelConfigGenericKeyword_cam_pullback_dist), LC_AL_cam_pullback_dist},
        {const_cast<char *>(LevelConfigGenericKeyword_cam_lateral_dist), LC_AL_cam_lateral_dist},
        {const_cast<char *>(LevelConfigGenericKeyword_campos_seek), LC_AL_campos_seek},
        {const_cast<char *>(LevelConfigGenericKeyword_camang_seek), LC_AL_camang_seek},
        {const_cast<char *>(LevelConfigGenericKeyword_shadowtype), LC_AL_shadowtype},
        {const_cast<char *>(LevelConfigGenericKeyword_music), LC_AL_music},
        {const_cast<char *>(LevelConfigGenericKeyword_music_other), LC_AL_music_other},
        {const_cast<char *>(LevelConfigGenericKeyword_sfx_ambient), LC_AL_sfx_ambient},
        {const_cast<char *>(LevelConfigGenericKeyword_conveyor), LC_AL_conveyor},
        {NULL, NULL},
    };
#pragma data_seg()

#pragma const_seg(".rdata$LevelConfigGameKeyword_windsize")
    extern const char LevelConfigGameKeyword_windsize[9] = "windsize";
#pragma const_seg(".rdata$LevelConfigGameKeyword_windspeed")
    extern const char LevelConfigGameKeyword_windspeed[10] = "windspeed";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_bombgens")
    extern const char LevelConfigGameKeyword_max_bombgens[13] = "max_bombgens";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_bombgen_objects")
    extern const char LevelConfigGameKeyword_max_bombgen_objects[20] = "max_bombgen_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_plugs")
    extern const char LevelConfigGameKeyword_max_plugs[10] = "max_plugs";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_bridges")
    extern const char LevelConfigGameKeyword_max_bridges[12] = "max_bridges";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_torpmachines")
    extern const char LevelConfigGameKeyword_max_torpmachines[17] = "max_torpmachines";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gizrandoms")
    extern const char LevelConfigGameKeyword_max_gizrandoms[15] = "max_gizrandoms";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_teleports")
    extern const char LevelConfigGameKeyword_max_teleports[14] = "max_teleports";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_doors")
    extern const char LevelConfigGameKeyword_max_doors[10] = "max_doors";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_hatmachines")
    extern const char LevelConfigGameKeyword_max_hatmachines[16] = "max_hatmachines";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gizspecials")
    extern const char LevelConfigGameKeyword_max_gizspecials[16] = "max_gizspecials";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_pushblock_endpos")
    extern const char LevelConfigGameKeyword_max_pushblock_endpos[21] = "max_pushblock_endpos";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_pushblocks")
    extern const char LevelConfigGameKeyword_max_pushblocks[15] = "max_pushblocks";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_force_objects")
    extern const char LevelConfigGameKeyword_max_force_objects[18] = "max_force_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_force")
    extern const char LevelConfigGameKeyword_max_force[10] = "max_force";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gizpanels")
    extern const char LevelConfigGameKeyword_max_gizpanels[14] = "max_gizpanels";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gameantinodes")
    extern const char LevelConfigGameKeyword_max_gameantinodes[18] = "max_gameantinodes";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_pickups")
    extern const char LevelConfigGameKeyword_max_pickups[12] = "max_pickups";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_tubes")
    extern const char LevelConfigGameKeyword_max_tubes[10] = "max_tubes";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_securitydoors")
    extern const char LevelConfigGameKeyword_max_securitydoors[18] = "max_securitydoors";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_ledges")
    extern const char LevelConfigGameKeyword_max_ledges[11] = "max_ledges";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_guidelines")
    extern const char LevelConfigGameKeyword_max_guidelines[15] = "max_guidelines";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_timers")
    extern const char LevelConfigGameKeyword_max_timers[11] = "max_timers";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_attractos")
    extern const char LevelConfigGameKeyword_max_attractos[14] = "max_attractos";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_climb_objects")
    extern const char LevelConfigGameKeyword_max_climb_objects[18] = "max_climb_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_maxgizmoblowups")
    extern const char LevelConfigGameKeyword_maxgizmoblowups[16] = "maxgizmoblowups";
#pragma const_seg(".rdata$LevelConfigGameKeyword_maxgizmoblowuptypes")
    extern const char LevelConfigGameKeyword_maxgizmoblowuptypes[20] = "maxgizmoblowuptypes";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_minicut_stages")
    extern const char LevelConfigGameKeyword_max_minicut_stages[19] = "max_minicut_stages";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_minicuts")
    extern const char LevelConfigGameKeyword_max_minicuts[13] = "max_minicuts";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_spinners")
    extern const char LevelConfigGameKeyword_max_spinners[13] = "max_spinners";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_shards")
    extern const char LevelConfigGameKeyword_max_shards[11] = "max_shards";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_spinneranim_objs")
    extern const char LevelConfigGameKeyword_max_spinneranim_objs[21] = "max_spinneranim_objs";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_buildit_objects")
    extern const char LevelConfigGameKeyword_max_buildit_objects[20] = "max_buildit_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_buildits")
    extern const char LevelConfigGameKeyword_max_buildits[13] = "max_buildits";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_turrets")
    extern const char LevelConfigGameKeyword_max_turrets[12] = "max_turrets";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_obstacle_objects")
    extern const char LevelConfigGameKeyword_max_obstacle_objects[21] = "max_obstacle_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_obstacles")
    extern const char LevelConfigGameKeyword_max_obstacles[14] = "max_obstacles";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_grapples")
    extern const char LevelConfigGameKeyword_max_grapples[13] = "max_grapples";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_zipups")
    extern const char LevelConfigGameKeyword_max_zipups[11] = "max_zipups";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_technos")
    extern const char LevelConfigGameKeyword_max_technos[12] = "max_technos";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_levers")
    extern const char LevelConfigGameKeyword_max_levers[11] = "max_levers";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_signals")
    extern const char LevelConfigGameKeyword_max_signals[12] = "max_signals";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_tightropes")
    extern const char LevelConfigGameKeyword_max_tightropes[15] = "max_tightropes";
#pragma const_seg(".rdata$AreaResidentsDirectory")
    extern const char AreaResidentsDirectory[8] = "levels\\";
#pragma const_seg(".rdata$AreaResidentsExtension")
    extern const char AreaResidentsExtension[5] = ".txt";
#pragma const_seg(".rdata$AreaConfigKeyword_character")
    extern const char AreaConfigKeyword_character[10] = "character";
#pragma const_seg(".rdata$AreaConfigKeyword_area_start")
    extern const char AreaConfigKeyword_area_start[11] = "area_start";
#pragma const_seg(".rdata$AreaConfigKeyword_redbrick_extra")
    extern const char AreaConfigKeyword_redbrick_extra[15] = "redbrick_extra";
#pragma const_seg(".rdata$AreaConfigKeyword_redbrick_cheat")
    extern const char AreaConfigKeyword_redbrick_cheat[15] = "redbrick_cheat";
#pragma const_seg(".rdata$AreaConfigKeyword_timetrial_time")
    extern const char AreaConfigKeyword_timetrial_time[15] = "timetrial_time";
#pragma const_seg(".rdata$AreaConfigKeyword_text_id")
    extern const char AreaConfigKeyword_text_id[8] = "text_id";
#pragma const_seg(".rdata$AreaConfigKeyword_name_id")
    extern const char AreaConfigKeyword_name_id[8] = "name_id";
#pragma const_seg(".rdata$AreaConfigKeyword_no_freeplay")
    extern const char AreaConfigKeyword_no_freeplay[12] = "no_freeplay";
#pragma const_seg(".rdata$AreaConfigKeyword_no_completion_points")
    extern const char AreaConfigKeyword_no_completion_points[21] = "no_completion_points";
#pragma const_seg(".rdata$AreaConfigKeyword_no_gold_brick")
    extern const char AreaConfigKeyword_no_gold_brick[14] = "no_gold_brick";
#pragma const_seg(".rdata$AreaConfigKeyword_nopickupgravity")
    extern const char AreaConfigKeyword_nopickupgravity[16] = "nopickupgravity";
#pragma const_seg(".rdata$AreaConfigKeyword_no_character_collisions")
    extern const char AreaConfigKeyword_no_character_collisions[24] = "no_character_collisions";
#pragma const_seg(".rdata$AreaConfigKeyword_no_character_collision")
    extern const char AreaConfigKeyword_no_character_collision[23] = "no_character_collision";
#pragma const_seg(".rdata$AreaConfigKeyword_nocharactercollisions")
    extern const char AreaConfigKeyword_nocharactercollisions[22] = "nocharactercollisions";
#pragma const_seg(".rdata$AreaConfigKeyword_nocharactercollision")
    extern const char AreaConfigKeyword_nocharactercollision[21] = "nocharactercollision";
#pragma const_seg(".rdata$AreaConfigKeyword_super_bonus_area")
    extern const char AreaConfigKeyword_super_bonus_area[17] = "super_bonus_area";
#pragma const_seg(".rdata$AreaConfigKeyword_bonus_area")
    extern const char AreaConfigKeyword_bonus_area[11] = "bonus_area";
#pragma const_seg(".rdata$AreaConfigKeyword_ending_area")
    extern const char AreaConfigKeyword_ending_area[12] = "ending_area";
#pragma const_seg(".rdata$AreaConfigKeyword_vehicle_area")
    extern const char AreaConfigKeyword_vehicle_area[13] = "vehicle_area";
#pragma const_seg(".rdata$AreaConfigKeyword_override_things_scene")
    extern const char AreaConfigKeyword_override_things_scene[22] = "override_things_scene";
#pragma const_seg(".rdata$AreaConfigKeyword_hub_area")
    extern const char AreaConfigKeyword_hub_area[9] = "hub_area";
#pragma const_seg(".rdata$AreaConfigKeyword_test_area")
    extern const char AreaConfigKeyword_test_area[10] = "test_area";
#pragma const_seg(".rdata$AreaConfigKeyword_minikit")
    extern const char AreaConfigKeyword_minikit[8] = "minikit";
#pragma const_seg(".rdata$AreaConfigKeyword_single_buffer")
    extern const char AreaConfigKeyword_single_buffer[14] = "single_buffer";
#pragma const_seg(".rdata$AreaConfigKeyword_level")
    extern const char AreaConfigKeyword_level[6] = "level";
#pragma const_seg(".rdata$AreaConfigKeyword_area_end")
    extern const char AreaConfigKeyword_area_end[9] = "area_end";
#pragma const_seg(".rdata$AreaConfigKeyword_resident")
    extern const char AreaConfigKeyword_resident[9] = "resident";
#pragma const_seg(".rdata$LevelConfigKeyword_zspeed")
    extern const char LevelConfigKeyword_zspeed[7] = "zspeed";
#pragma const_seg(".rdata$LevelConfigKeyword_xspeed")
    extern const char LevelConfigKeyword_xspeed[7] = "xspeed";
#pragma const_seg()
#pragma bss_seg(".bss$SfxBits")
    u16 SfxBits[100];
#pragma bss_seg()
#pragma bss_seg(".bss$GlobalSfxBits")
    u16 GlobalSfxBits[100];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_page_on")
    i32 edanim_page_on[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_page_used")
    i32 edanim_page_used[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_page_scene")
    NUGSCN *edanim_page_scene[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_next_param")
    i32 edanim_next_param;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_params_used")
    i32 edanim_params_used;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimParams")
    EDANIM_PARAM_PC AnimParams[64];
#pragma bss_seg()
    f32 NuVideoFrameRate;
    OCCLUSIONMANAGER g_OcclusionManager = {0, 1, {0}, {{0}}, 0, 0, 0.3f, -1.0f, 0, 0, {0}};

    // STUBBED: reserved PC menu callbacks remain unavailable.
    MENUFNINFO MenuInfo[100] = {
        {1000, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1001, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1002, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1004, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1003, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1005, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1006, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1007, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1008, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1009, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1010, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0}, {1011, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1012, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1013, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1014, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1015, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1016, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1017, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1018, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1019, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0},
        {1020, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1021, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1022, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1023, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1024, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1025, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1026, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1027, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1028, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1029, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1030, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    };
    MENU GameMenu[10];
    i32 MenuSFX = -1;
    i32 GameMenuLevel = -1;
    i32 MenuLanguages = 1;
    i32 MenusUsed = 31;
    // STUBBED: PC defaults are draw callbacks at 0x5100f0 and 0x5103e0.
    void (*drawslotinfofn)(f32, f32, i32, i32);
    void (*drawslotsfn)(MENU *, f32);
    char MenuHeader[64];
    i32 header_r;
    i32 header_g;
    i32 header_b;
    i32 MenuFadeEnabled;
    i32 MenuDrawDropShadows;
    NUMTL *MenuFadeMtl;
#pragma bss_seg(".bss$MENUHEADERR")
    u8 MENUHEADERR;
#pragma bss_seg()
#pragma data_seg(".data$MENUHEADERG")
    u8 MENUHEADERG = 0x7f;
#pragma data_seg(".data$MENUHEADERB")
    u8 MENUHEADERB = 0xff;
#pragma data_seg()
#pragma const_seg(".rdata$Menu_UsedFormat")
    extern const char Menu_UsedFormat[15] = "Menus used: %d";
#pragma const_seg()
#pragma const_seg(".rdata$Menu_OptionsVolumeFormat")
    extern const char Menu_OptionsVolumeFormat[10] = "%s: %i/%i";
#pragma const_seg(".rdata$Menu_ColonSeparator")
    extern const char Menu_ColonSeparator[3] = ": ";
#pragma const_seg()
#pragma const_seg(".rdata$nu_wind_phase_scale")
    extern const f64 nu_wind_phase_scale = 1.0 / 256.0;
#pragma const_seg(".rdata$nu_wind_scaled_time_multiplier")
    extern const f64 nu_wind_scaled_time_multiplier = 5.0;
#pragma const_seg()
#pragma const_seg(".rdata$PCKeyboardDefaultValue_candidate")
    extern const f32 PCKeyboardDefaultValue_candidate = 0.1f;
#pragma const_seg()
    f32 dme_sy = 1.0f;
    f32 MASTERVOLUME = 1.0f;
#pragma data_seg(".data$AUDIOFADELEVEL")
    f32 AUDIOFADELEVEL = 1.0f;
#pragma data_seg()
    NUMTX numtx_identity = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
#pragma bss_seg(".bss$numtl_defaultmtl2d")
    NUMTL *numtl_defaultmtl2d;
#pragma bss_seg()
#pragma bss_seg(".bss$numtl_defaultmtl3d")
    NUMTL *numtl_defaultmtl3d;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDisplayListStreamCursor")
    VARIPTR *NuDisplayListStreamCursor;
#pragma bss_seg()
#pragma bss_seg(".bss$render_state")
    NUGLOBALRNDRSTATE_PC render_state;
#pragma bss_seg()
    f32 default_targetmomf = 4.0f;
    f32 default_ownermomf = 5.0f;
    f32 default_max_cable_length = 15.0f;
    f32 default_tow_length = 6.0f;
    f32 targetmomf = 4.0f;
    f32 ownermomf = 5.0f;
    f32 max_cable_length = 15.0f;
    f32 tow_length = 6.0f;
    AREA_DATA_PC *GUNSHIP_ADATA;
#pragma bss_seg(".bss$cables")
    u8 cables[0xf60];
#pragma bss_seg()
    i32 menu_flash;
#pragma bss_seg(".bss$tAUDIOVOLUME")
    i16 tAUDIOVOLUME;
#pragma bss_seg(".bss$tMUSIC")
    i16 tMUSIC;
#pragma bss_seg(".bss$tON")
    i16 tON;
#pragma bss_seg(".bss$tOFF")
    i16 tOFF;
#pragma bss_seg(".bss$tACCEPT")
    i16 tACCEPT;
#pragma bss_seg(".bss$tBACK")
    i16 tBACK;
#pragma bss_seg()
    OPTIONSSAVE *Game_OptionsSave;
    f32 opts_sfx_wait = 1.5f;
    i32 opts_sfx_i;
    LEVELDATA_PC *TITLES_LDATA;

    // STUBBED: PC callback bodies are not reconstructed; NULL slots keep them inactive.
    // IDs, remembered cursors, and prompt flags use the 23-record PC table.
    MENUFNINFO GameMenuInfo[23] = {
        {0, MenuEnterTitles, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1, MenuEnterNewGame, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0},
        {2, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {3, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {4, MenuEnterOptions, MenuDrawOptions, MenuUpdateOptions, MenuExitOptions, 0, 0, 0, 1, 1, 0},
        {5, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
        {6, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {7, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
        {8, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {9, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {10, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {11, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {12, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {13, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {14, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {15, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {16, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {17, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {18, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {19, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {20, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {21, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {22, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
    };

#pragma bss_seg(".bss$DebrisGlassInitialized")
    i32 DebrisGlassInitialized;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassCopyMaterial")
    void *DebrisGlassCopyMaterial;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassU1")
    f32 DebrisGlassU1;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassV1")
    f32 DebrisGlassV1;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassTextureWidth")
    i32 DebrisGlassTextureWidth;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassTextureHeight")
    i32 DebrisGlassTextureHeight;
#pragma bss_seg()
#pragma bss_seg(".bss$PS2_VCNTR_X")
    i32 PS2_VCNTR_X;
#pragma bss_seg(".bss$PS2_VCNTR_Y")
    i32 PS2_VCNTR_Y;
#pragma bss_seg()

    extern const f32 DebrisGlassMaterialOpacity = 0.999f;
}
i32 app_tbgameset;
i32 app_tbplayerset;
i32 app_tbaiset;
i32 app_tbdrawset;
NUAPI nuapi;

i32 GAMERAND = 0x1f3ad27f;
u32 NuWindQS = 0x1365;
u32 partseed = 0x5c0999;
u32 debrisseed = 0x5c0999;

i32 argc;
char **argv;
NUDATHDR *curr_dat;
MSG win_message;

i32 permbuffer_size;
#pragma data_seg(".data$CHARACTERBUFFERSIZE")
i32 CHARACTERBUFFERSIZE = 0x02800000;
#pragma data_seg()
i32 EDITBUFFERENDSIZE;
VARIPTR permbuffer_base;
VARIPTR permbuffer_ptr;
VARIPTR permbuffer_end;
VARIPTR characterbuffer_base;
#pragma bss_seg(".bss$CharacterBufferPtr")
VARIPTR characterbuffer_ptr;
#pragma bss_seg()
VARIPTR characterbuffer_end;
VARIPTR superbuffer_base;
VARIPTR superbuffer_ptr;
VARIPTR superbuffer_end;
VARIPTR editbuffer_end;

WORLDINFO WorldInfo[2] = {0};
#pragma data_seg(".data$WORLD")
WORLDINFO *WORLD = &WorldInfo[0];
#pragma data_seg()
#pragma data_seg(".data$LWorld")
WORLDINFO *LWORLD = &WorldInfo[0];
#pragma data_seg()

#pragma data_seg(".data$WaitingForCharacter")
i32 waiting_for_character = -1;
#pragma data_seg()

// CRC-16/IBM lookup values stored as 32-bit words.
u32 crctab[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1,
    0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40,
    0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1,
    0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1,
    0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40,
    0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1,
    0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0,
    0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740,
    0xa501, 0x65c0, 0x6480, 0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0,
    0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1,
    0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140,
    0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0,
    0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0,
    0x4c80, 0x8c41, 0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341,
    0x4100, 0x81c1, 0x8081, 0x4040,
};

// PC walks 8-byte entries to a zero key.
NUKEYCHAR NuKeyChar[65] = {
    {0x2, 0x31, 0x21, {0, 0}},  {0x3, 0x32, 0x22, {0, 0}},  {0x4, 0x33, 0xa3, {0, 0}},  {0x5, 0x34, 0x24, {0, 0}},
    {0x6, 0x35, 0x25, {0, 0}},  {0x7, 0x36, 0x5e, {0, 0}},  {0x8, 0x37, 0x26, {0, 0}},  {0x9, 0x38, 0x2a, {0, 0}},
    {0xa, 0x39, 0x28, {0, 0}},  {0xb, 0x30, 0x29, {0, 0}},  {0xc, 0x2d, 0x5f, {0, 0}},  {0xd, 0x3d, 0x2b, {0, 0}},
    {0xf, 0x09, 0x09, {0, 0}},  {0x10, 0x71, 0x51, {0, 0}}, {0x11, 0x77, 0x57, {0, 0}}, {0x12, 0x65, 0x45, {0, 0}},
    {0x13, 0x72, 0x52, {0, 0}}, {0x14, 0x74, 0x54, {0, 0}}, {0x15, 0x79, 0x59, {0, 0}}, {0x16, 0x75, 0x55, {0, 0}},
    {0x17, 0x69, 0x49, {0, 0}}, {0x18, 0x6f, 0x4f, {0, 0}}, {0x19, 0x70, 0x50, {0, 0}}, {0x1a, 0x5b, 0x7b, {0, 0}},
    {0x1b, 0x5d, 0x7d, {0, 0}}, {0x1e, 0x61, 0x41, {0, 0}}, {0x1f, 0x73, 0x53, {0, 0}}, {0x20, 0x64, 0x44, {0, 0}},
    {0x21, 0x66, 0x46, {0, 0}}, {0x22, 0x67, 0x47, {0, 0}}, {0x23, 0x68, 0x48, {0, 0}}, {0x24, 0x6a, 0x4a, {0, 0}},
    {0x25, 0x6b, 0x4b, {0, 0}}, {0x26, 0x6c, 0x4c, {0, 0}}, {0x27, 0x3b, 0x3a, {0, 0}}, {0x28, 0x27, 0x40, {0, 0}},
    {0x2b, 0x5c, 0x3f, {0, 0}}, {0x2c, 0x7a, 0x5a, {0, 0}}, {0x2d, 0x78, 0x58, {0, 0}}, {0x2e, 0x63, 0x43, {0, 0}},
    {0x2f, 0x76, 0x56, {0, 0}}, {0x30, 0x62, 0x42, {0, 0}}, {0x31, 0x6e, 0x4e, {0, 0}}, {0x32, 0x6d, 0x4d, {0, 0}},
    {0x33, 0x2c, 0x3c, {0, 0}}, {0x34, 0x2e, 0x3e, {0, 0}}, {0x35, 0x2f, 0x00, {0, 0}}, {0x37, 0x2a, 0x2a, {0, 0}},
    {0x39, 0x20, 0x20, {0, 0}}, {0x47, 0x37, 0x37, {0, 0}}, {0x48, 0x38, 0x38, {0, 0}}, {0x49, 0x39, 0x39, {0, 0}},
    {0x4a, 0x2d, 0x2d, {0, 0}}, {0x4b, 0x34, 0x34, {0, 0}}, {0x4c, 0x35, 0x35, {0, 0}}, {0x4d, 0x36, 0x36, {0, 0}},
    {0x4e, 0x2b, 0x2b, {0, 0}}, {0x4f, 0x31, 0x31, {0, 0}}, {0x50, 0x32, 0x32, {0, 0}}, {0x51, 0x33, 0x33, {0, 0}},
    {0x52, 0x30, 0x30, {0, 0}}, {0x53, 0x2e, 0x2e, {0, 0}}, {0x8d, 0x3d, 0x3d, {0, 0}}, {0xb5, 0x2f, 0x2f, {0, 0}},
    {0x0, 0x00, 0x00, {0, 0}},
};

// PC reads pairs of 16-bit code points, ending with a zero pair.
u16 bad_equiv[8][2] = {{'a', '@'}, {'a', '4'}, {'e', '3'}, {'i', '1'}, {'l', '1'}, {'o', '0'}, {'t', '7'}, {0, 0}};

i32 SkinFlipTab[8] = {0, 1, 2, 3, 0, 2, 1, 3};

// Both PC indices are masked to two bits.
i32 TrooperStepFrames[4] = {1, 2, 1, 0};
f32 trooperspeedtab[4] = {0.3f, 0.4f, 0.5f, 0.6f};

// Four explicit PC scalar accesses establish each array's extent.
f32 vehicle_timebase_dist[4] = {50.0f, 25.0f, 0.0f, 0.0f};
i32 vehicle_timebase_nframes[4] = {4, 2, 0, 0};

// PC checks each byte index is less than 16.
const u8 KeyStructSizes[16] = {3, 4, 4, 3, 4, 3, 4, 8, 4, 8, 4, 0, 0, 0, 0, 0};

// Each of the 12 cube edges has two vertex indices and two incident faces.
i32 cubeEdgeIndices[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                              {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
i32 cubeEdgeTopologies[12][2] = {{3, 0}, {4, 0}, {1, 0}, {5, 0}, {2, 3}, {2, 4},
                                 {2, 1}, {2, 5}, {5, 3}, {3, 4}, {4, 1}, {1, 5}};

// 513 angular samples including both endpoints.
u16 ang[513] = {
    0x0000, 0x0014, 0x0029, 0x003d, 0x0051, 0x0066, 0x007a, 0x008f, 0x00a3, 0x00b7, 0x00cc, 0x00e0, 0x00f4, 0x0109,
    0x011d, 0x0131, 0x0146, 0x015a, 0x016f, 0x0183, 0x0197, 0x01ac, 0x01c0, 0x01d4, 0x01e9, 0x01fd, 0x0211, 0x0226,
    0x023a, 0x024e, 0x0262, 0x0277, 0x028b, 0x029f, 0x02b4, 0x02c8, 0x02dc, 0x02f0, 0x0305, 0x0319, 0x032d, 0x0341,
    0x0356, 0x036a, 0x037e, 0x0392, 0x03a7, 0x03bb, 0x03cf, 0x03e3, 0x03f7, 0x040c, 0x0420, 0x0434, 0x0448, 0x045c,
    0x0470, 0x0484, 0x0499, 0x04ad, 0x04c1, 0x04d5, 0x04e9, 0x04fd, 0x0511, 0x0525, 0x0539, 0x054d, 0x0561, 0x0575,
    0x0589, 0x059d, 0x05b1, 0x05c5, 0x05d9, 0x05ed, 0x0601, 0x0615, 0x0629, 0x063d, 0x0651, 0x0665, 0x0678, 0x068c,
    0x06a0, 0x06b4, 0x06c8, 0x06dc, 0x06ef, 0x0703, 0x0717, 0x072b, 0x073e, 0x0752, 0x0766, 0x077a, 0x078d, 0x07a1,
    0x07b5, 0x07c8, 0x07dc, 0x07ef, 0x0803, 0x0817, 0x082a, 0x083e, 0x0851, 0x0865, 0x0878, 0x088c, 0x089f, 0x08b3,
    0x08c6, 0x08da, 0x08ed, 0x0901, 0x0914, 0x0927, 0x093b, 0x094e, 0x0961, 0x0975, 0x0988, 0x099b, 0x09ae, 0x09c2,
    0x09d5, 0x09e8, 0x09fb, 0x0a0e, 0x0a22, 0x0a35, 0x0a48, 0x0a5b, 0x0a6e, 0x0a81, 0x0a94, 0x0aa7, 0x0aba, 0x0acd,
    0x0ae0, 0x0af3, 0x0b06, 0x0b19, 0x0b2c, 0x0b3f, 0x0b51, 0x0b64, 0x0b77, 0x0b8a, 0x0b9d, 0x0baf, 0x0bc2, 0x0bd5,
    0x0be7, 0x0bfa, 0x0c0d, 0x0c1f, 0x0c32, 0x0c45, 0x0c57, 0x0c6a, 0x0c7c, 0x0c8f, 0x0ca1, 0x0cb4, 0x0cc6, 0x0cd9,
    0x0ceb, 0x0cfd, 0x0d10, 0x0d22, 0x0d34, 0x0d47, 0x0d59, 0x0d6b, 0x0d7d, 0x0d90, 0x0da2, 0x0db4, 0x0dc6, 0x0dd8,
    0x0dea, 0x0dfc, 0x0e0f, 0x0e21, 0x0e33, 0x0e45, 0x0e56, 0x0e68, 0x0e7a, 0x0e8c, 0x0e9e, 0x0eb0, 0x0ec2, 0x0ed4,
    0x0ee5, 0x0ef7, 0x0f09, 0x0f1b, 0x0f2c, 0x0f3e, 0x0f50, 0x0f61, 0x0f73, 0x0f84, 0x0f96, 0x0fa7, 0x0fb9, 0x0fca,
    0x0fdc, 0x0fed, 0x0fff, 0x1010, 0x1021, 0x1033, 0x1044, 0x1055, 0x1067, 0x1078, 0x1089, 0x109a, 0x10ab, 0x10bc,
    0x10ce, 0x10df, 0x10f0, 0x1101, 0x1112, 0x1123, 0x1134, 0x1145, 0x1156, 0x1166, 0x1177, 0x1188, 0x1199, 0x11aa,
    0x11bb, 0x11cb, 0x11dc, 0x11ed, 0x11fd, 0x120e, 0x121f, 0x122f, 0x1240, 0x1250, 0x1261, 0x1271, 0x1282, 0x1292,
    0x12a3, 0x12b3, 0x12c3, 0x12d4, 0x12e4, 0x12f4, 0x1305, 0x1315, 0x1325, 0x1335, 0x1345, 0x1355, 0x1366, 0x1376,
    0x1386, 0x1396, 0x13a6, 0x13b6, 0x13c6, 0x13d6, 0x13e6, 0x13f5, 0x1405, 0x1415, 0x1425, 0x1435, 0x1444, 0x1454,
    0x1464, 0x1473, 0x1483, 0x1493, 0x14a2, 0x14b2, 0x14c1, 0x14d1, 0x14e0, 0x14f0, 0x14ff, 0x150f, 0x151e, 0x152d,
    0x153d, 0x154c, 0x155b, 0x156b, 0x157a, 0x1589, 0x1598, 0x15a7, 0x15b7, 0x15c6, 0x15d5, 0x15e4, 0x15f3, 0x1602,
    0x1611, 0x1620, 0x162f, 0x163e, 0x164c, 0x165b, 0x166a, 0x1679, 0x1688, 0x1696, 0x16a5, 0x16b4, 0x16c2, 0x16d1,
    0x16e0, 0x16ee, 0x16fd, 0x170b, 0x171a, 0x1728, 0x1737, 0x1745, 0x1754, 0x1762, 0x1770, 0x177f, 0x178d, 0x179b,
    0x17aa, 0x17b8, 0x17c6, 0x17d4, 0x17e2, 0x17f0, 0x17fe, 0x180d, 0x181b, 0x1829, 0x1837, 0x1845, 0x1853, 0x1860,
    0x186e, 0x187c, 0x188a, 0x1898, 0x18a6, 0x18b3, 0x18c1, 0x18cf, 0x18dd, 0x18ea, 0x18f8, 0x1906, 0x1913, 0x1921,
    0x192e, 0x193c, 0x1949, 0x1957, 0x1964, 0x1972, 0x197f, 0x198c, 0x199a, 0x19a7, 0x19b4, 0x19c2, 0x19cf, 0x19dc,
    0x19e9, 0x19f6, 0x1a04, 0x1a11, 0x1a1e, 0x1a2b, 0x1a38, 0x1a45, 0x1a52, 0x1a5f, 0x1a6c, 0x1a79, 0x1a86, 0x1a93,
    0x1a9f, 0x1aac, 0x1ab9, 0x1ac6, 0x1ad3, 0x1adf, 0x1aec, 0x1af9, 0x1b05, 0x1b12, 0x1b1f, 0x1b2b, 0x1b38, 0x1b44,
    0x1b51, 0x1b5d, 0x1b6a, 0x1b76, 0x1b83, 0x1b8f, 0x1b9c, 0x1ba8, 0x1bb4, 0x1bc1, 0x1bcd, 0x1bd9, 0x1be5, 0x1bf2,
    0x1bfe, 0x1c0a, 0x1c16, 0x1c22, 0x1c2e, 0x1c3a, 0x1c46, 0x1c52, 0x1c5e, 0x1c6a, 0x1c76, 0x1c82, 0x1c8e, 0x1c9a,
    0x1ca6, 0x1cb2, 0x1cbe, 0x1cc9, 0x1cd5, 0x1ce1, 0x1ced, 0x1cf8, 0x1d04, 0x1d10, 0x1d1b, 0x1d27, 0x1d33, 0x1d3e,
    0x1d4a, 0x1d55, 0x1d61, 0x1d6c, 0x1d78, 0x1d83, 0x1d8e, 0x1d9a, 0x1da5, 0x1db1, 0x1dbc, 0x1dc7, 0x1dd3, 0x1dde,
    0x1de9, 0x1df4, 0x1dff, 0x1e0b, 0x1e16, 0x1e21, 0x1e2c, 0x1e37, 0x1e42, 0x1e4d, 0x1e58, 0x1e63, 0x1e6e, 0x1e79,
    0x1e84, 0x1e8f, 0x1e9a, 0x1ea5, 0x1eb0, 0x1eba, 0x1ec5, 0x1ed0, 0x1edb, 0x1ee6, 0x1ef0, 0x1efb, 0x1f06, 0x1f10,
    0x1f1b, 0x1f26, 0x1f30, 0x1f3b, 0x1f45, 0x1f50, 0x1f5a, 0x1f65, 0x1f6f, 0x1f7a, 0x1f84, 0x1f8f, 0x1f99, 0x1fa4,
    0x1fae, 0x1fb8, 0x1fc3, 0x1fcd, 0x1fd7, 0x1fe1, 0x1fec, 0x1ff6, 0x2000};

VARIPTR *aiscript_load_buffer;

VARIPTR *aiscript_load_end_buffer;

AISCRIPT_PC *aiscript_load_script;

AISTATE_PC *aiscript_load_state;

NULISTHDR *aiscript_load_refs;

NULISTHDR *aiscript_load_conditions;

NULISTHDR *aiscript_load_actions;

i32 condition_has_no_goto;

// Movement, object and AI state recovered from the PC image.
extern "C" {
#pragma bss_seg(".bss$HIGHGAMEOBJECT")
    i32 HIGHGAMEOBJECT;
#pragma bss_seg()
#pragma bss_seg(".bss$Obj")
    GAMEOBJECT *Obj;
#pragma bss_seg()
#pragma bss_seg(".bss$plr_lastpos")
    NUVEC plr_lastpos;
#pragma bss_seg()
#pragma bss_seg(".bss$v000")
    NUVEC v000;
#pragma bss_seg()
#pragma bss_seg(".bss$GameTimer")
    TIMER GameTimer;
#pragma bss_seg()
#pragma bss_seg(".bss$party_under_cover")
    i32 party_under_cover;
#pragma bss_seg()
#pragma bss_seg(".bss$alert_obj")
    GAMEOBJECT *alert_obj;
#pragma bss_seg()
#pragma bss_seg(".bss$alert_timer")
    f32 alert_timer;
#pragma bss_seg()
#pragma data_seg(".data$ForcePush_Speed")
    f32 ForcePush_Speed = 2.5f;
#pragma data_seg()
#pragma bss_seg(".bss$ForcePush_SuperPush")
    i32 ForcePush_SuperPush;
#pragma bss_seg()
#pragma const_seg(".rdata$PCContextName_NoContext")
    extern const char PCContextName_NoContext[10] = "NoContext";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Jump")
    extern const char PCContextName_Jump[5] = "Jump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandJump")
    extern const char PCContextName_LandJump[9] = "LandJump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandJump2")
    extern const char PCContextName_LandJump2[10] = "LandJump2";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandFlip")
    extern const char PCContextName_LandFlip[9] = "LandFlip";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandComboJump")
    extern const char PCContextName_LandComboJump[14] = "LandComboJump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Combo")
    extern const char PCContextName_Combo[6] = "Combo";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WeaponIn")
    extern const char PCContextName_WeaponIn[9] = "WeaponIn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WeaponOut")
    extern const char PCContextName_WeaponOut[10] = "WeaponOut";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Force")
    extern const char PCContextName_Force[6] = "Force";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ComboRotate")
    extern const char PCContextName_ComboRotate[12] = "ComboRotate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Shoot")
    extern const char PCContextName_Shoot[6] = "Shoot";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Interface")
    extern const char PCContextName_Interface[10] = "Interface";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Block")
    extern const char PCContextName_Block[6] = "Block";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandLunge")
    extern const char PCContextName_LandLunge[10] = "LandLunge";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandSlam")
    extern const char PCContextName_LandSlam[9] = "LandSlam";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Teleport")
    extern const char PCContextName_Teleport[9] = "Teleport";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Swipe")
    extern const char PCContextName_Swipe[6] = "Swipe";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Tube")
    extern const char PCContextName_Tube[5] = "Tube";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForceThrow")
    extern const char PCContextName_ForceThrow[11] = "ForceThrow";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_HoverUp")
    extern const char PCContextName_HoverUp[8] = "HoverUp";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Rocket")
    extern const char PCContextName_Rocket[7] = "Rocket";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_TakeHit")
    extern const char PCContextName_TakeHit[8] = "TakeHit";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Zap")
    extern const char PCContextName_Zap[4] = "Zap";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Deactivated")
    extern const char PCContextName_Deactivated[12] = "Deactivated";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Hold")
    extern const char PCContextName_Hold[5] = "Hold";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandSpecial")
    extern const char PCContextName_LandSpecial[12] = "LandSpecial";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Communicate")
    extern const char PCContextName_Communicate[12] = "Communicate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForcePush")
    extern const char PCContextName_ForcePush[10] = "ForcePush";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForcePushed")
    extern const char PCContextName_ForcePushed[12] = "ForcePushed";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForceDeflect")
    extern const char PCContextName_ForceDeflect[13] = "ForceDeflect";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForceFrozen")
    extern const char PCContextName_ForceFrozen[12] = "ForceFrozen";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BigJump")
    extern const char PCContextName_BigJump[8] = "BigJump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BackFlip")
    extern const char PCContextName_BackFlip[9] = "BackFlip";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Recoil")
    extern const char PCContextName_Recoil[7] = "Recoil";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForcedBack")
    extern const char PCContextName_ForcedBack[11] = "ForcedBack";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DropIn")
    extern const char PCContextName_DropIn[7] = "DropIn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DropOut")
    extern const char PCContextName_DropOut[8] = "DropOut";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Dodge")
    extern const char PCContextName_Dodge[6] = "Dodge";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Punch")
    extern const char PCContextName_Punch[6] = "Punch";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Push")
    extern const char PCContextName_Push[5] = "Push";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PushSpinner")
    extern const char PCContextName_PushSpinner[12] = "PushSpinner";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandCombatRoll")
    extern const char PCContextName_LandCombatRoll[15] = "LandCombatRoll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Turn")
    extern const char PCContextName_Turn[5] = "Turn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Doomed")
    extern const char PCContextName_Doomed[7] = "Doomed";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Launch")
    extern const char PCContextName_Launch[7] = "Launch";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BuildIt")
    extern const char PCContextName_BuildIt[8] = "BuildIt";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ThrowDetonator")
    extern const char PCContextName_ThrowDetonator[15] = "ThrowDetonator";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Grabbed")
    extern const char PCContextName_Grabbed[8] = "Grabbed";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_SpecialMoveVictim")
    extern const char PCContextName_SpecialMoveVictim[18] = "SpecialMoveVictim";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Roll")
    extern const char PCContextName_Roll[5] = "Roll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_UnRoll")
    extern const char PCContextName_UnRoll[7] = "UnRoll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Slide")
    extern const char PCContextName_Slide[6] = "Slide";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BeenDragged")
    extern const char PCContextName_BeenDragged[12] = "BeenDragged";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ZipDown")
    extern const char PCContextName_ZipDown[8] = "ZipDown";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Loop")
    extern const char PCContextName_Loop[5] = "Loop";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Poo")
    extern const char PCContextName_Poo[4] = "Poo";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Grab")
    extern const char PCContextName_Grab[5] = "Grab";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Eaten")
    extern const char PCContextName_Eaten[6] = "Eaten";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BarrelRoll")
    extern const char PCContextName_BarrelRoll[11] = "BarrelRoll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BeenTakenOver")
    extern const char PCContextName_BeenTakenOver[14] = "BeenTakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_GetIn")
    extern const char PCContextName_GetIn[6] = "GetIn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Flatten")
    extern const char PCContextName_Flatten[8] = "Flatten";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Buck")
    extern const char PCContextName_Buck[5] = "Buck";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Eat")
    extern const char PCContextName_Eat[4] = "Eat";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Disorientate")
    extern const char PCContextName_Disorientate[13] = "Disorientate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Activate")
    extern const char PCContextName_Activate[9] = "Activate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ZappedByFloor")
    extern const char PCContextName_ZappedByFloor[14] = "ZappedByFloor";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Climb")
    extern const char PCContextName_Climb[6] = "Climb";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Tightrope")
    extern const char PCContextName_Tightrope[10] = "Tightrope";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WallShuffle")
    extern const char PCContextName_WallShuffle[12] = "WallShuffle";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Grapple")
    extern const char PCContextName_Grapple[8] = "Grapple";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ZipUp")
    extern const char PCContextName_ZipUp[6] = "ZipUp";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PlaceDetonator")
    extern const char PCContextName_PlaceDetonator[15] = "PlaceDetonator";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PickUpDetonator")
    extern const char PCContextName_PickUpDetonator[16] = "PickUpDetonator";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PullLever")
    extern const char PCContextName_PullLever[10] = "PullLever";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Float")
    extern const char PCContextName_Float[6] = "Float";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Signal")
    extern const char PCContextName_Signal[7] = "Signal";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Batarang")
    extern const char PCContextName_Batarang[9] = "Batarang";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Hang")
    extern const char PCContextName_Hang[5] = "Hang";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Glide")
    extern const char PCContextName_Glide[6] = "Glide";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Catch")
    extern const char PCContextName_Catch[6] = "Catch";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Techno")
    extern const char PCContextName_Techno[7] = "Techno";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_AttractoTarget")
    extern const char PCContextName_AttractoTarget[15] = "AttractoTarget";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_AttractoDeposit")
    extern const char PCContextName_AttractoDeposit[16] = "AttractoDeposit";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Sonar")
    extern const char PCContextName_Sonar[6] = "Sonar";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LedgeTerrain")
    extern const char PCContextName_LedgeTerrain[13] = "LedgeTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Transform")
    extern const char PCContextName_Transform[10] = "Transform";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WallJumpWait")
    extern const char PCContextName_WallJumpWait[13] = "WallJumpWait";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_SuperCarry")
    extern const char PCContextName_SuperCarry[11] = "SuperCarry";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PushObstacle")
    extern const char PCContextName_PushObstacle[13] = "PushObstacle";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Stunned")
    extern const char PCContextName_Stunned[8] = "Stunned";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Ledge")
    extern const char PCContextName_Ledge[6] = "Ledge";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Security")
    extern const char PCContextName_Security[9] = "Security";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Ballooning")
    extern const char PCContextName_Ballooning[11] = "Ballooning";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ThrowQuick")
    extern const char PCContextName_ThrowQuick[11] = "ThrowQuick";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DieAir")
    extern const char PCContextName_DieAir[7] = "DieAir";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DieGround")
    extern const char PCContextName_DieGround[10] = "DieGround";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_HatMachine")
    extern const char PCContextName_HatMachine[11] = "HatMachine";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Whip")
    extern const char PCContextName_Whip[5] = "Whip";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_NetWait")
    extern const char PCContextName_NetWait[8] = "NetWait";
#pragma const_seg()
#pragma data_seg(".data$PCContextInfoTable")
    CHARACTER_CONTEXT_INFO_PC PCContextInfoTable[101] = {
        {PCContextName_NoContext, -1, 0x1000, 0},
        {PCContextName_Jump, -1, 0x1000000, 0},
        {PCContextName_LandJump, -1, 0x15, 0},
        {PCContextName_LandJump2, -1, 0x15, 0},
        {PCContextName_LandFlip, -1, 0x15, 0},
        {PCContextName_LandComboJump, -1, 0x15, 0},
        {PCContextName_Combo, -1, 0x800010, 1},
        {PCContextName_WeaponIn, -1, 0x1013, 0},
        {PCContextName_WeaponOut, -1, 0x1013, 0},
        {PCContextName_Force, -1, 0x222, 0},
        {PCContextName_ComboRotate, -1, 0x11, 0},
        {PCContextName_Shoot, -1, 0x11, 0},
        {PCContextName_Interface, -1, 0x33, 0},
        {PCContextName_Block, -1, 0x4000010, 0},
        {PCContextName_LandLunge, -1, 0x800011, 0},
        {PCContextName_LandSlam, -1, 0x800011, 0},
        {PCContextName_Teleport, -1, 0x1b3, 12},
        {PCContextName_Swipe, -1, 0x800010, 0},
        {PCContextName_Tube, -1, 0x408, 0},
        {PCContextName_ForceThrow, -1, 0x12, 0},
        {PCContextName_HoverUp, -1, 0x30, 0},
        {PCContextName_Rocket, -1, 0x11, 0},
        {PCContextName_TakeHit, -1, 0x10000031, 0},
        {PCContextName_Zap, -1, 0x10, 0},
        {PCContextName_Deactivated, -1, 0x10200031, 0},
        {PCContextName_Hold, -1, 0x4000010, 0},
        {PCContextName_LandSpecial, -1, 0x15, 0},
        {PCContextName_Communicate, -1, 0x33, 0},
        {PCContextName_ForcePush, -1, 0x2, 0},
        {PCContextName_ForcePushed, -1, 0x231, 0},
        {PCContextName_ForceDeflect, -1, 0x2, 0},
        {PCContextName_ForceFrozen, -1, 0xb, 0},
        {PCContextName_BigJump, -1, 0x2400111, 0},
        {PCContextName_BackFlip, -1, 0x91, 0},
        {PCContextName_Recoil, -1, 0x9, 0},
        {PCContextName_ForcedBack, -1, 0x8, 0},
        {PCContextName_DropIn, -1, 0x40113, 0},
        {PCContextName_DropOut, -1, 0x40113, 0},
        {PCContextName_Dodge, -1, 0x92, 1},
        {PCContextName_Punch, -1, 0x10032, 1},
        {PCContextName_Push, -1, 0x2030, 0},
        {PCContextName_PushSpinner, -1, 0x3010, 0},
        {PCContextName_LandCombatRoll, -1, 0xb4, 0},
        {PCContextName_Turn, -1, 0x190, 0},
        {PCContextName_Doomed, -1, 0x400110, 0},
        {PCContextName_Launch, -1, 0x0, 0},
        {PCContextName_BuildIt, -1, 0x10, 0},
        {PCContextName_ThrowDetonator, -1, 0x33, 0},
        {PCContextName_Grabbed, -1, 0x13, 0},
        {PCContextName_SpecialMoveVictim, -1, 0x33, 0},
        {PCContextName_Roll, -1, 0x10, 16},
        {PCContextName_UnRoll, -1, 0x13, 16},
        {PCContextName_Slide, -1, 0x40000030, 0},
        {PCContextName_BeenDragged, -1, 0x0, 0},
        {PCContextName_ZipDown, -1, 0x30, 0},
        {PCContextName_Loop, -1, 0x190, 0},
        {PCContextName_Poo, -1, 0x33, 0},
        {PCContextName_Grab, -1, 0x33, 0},
        {PCContextName_Eaten, -1, 0x60020933, 0},
        {PCContextName_BarrelRoll, -1, 0x190, 0},
        {PCContextName_BeenTakenOver, -1, 0x600209b0, 0},
        {PCContextName_GetIn, -1, 0x60400130, 0},
        {PCContextName_Flatten, -1, 0x33, 0},
        {PCContextName_Buck, -1, 0x33, 0},
        {PCContextName_Eat, -1, 0x33, 0},
        {PCContextName_Disorientate, -1, 0x100, 0},
        {PCContextName_Activate, -1, 0x200033, 0},
        {PCContextName_ZappedByFloor, -1, 0x31, 0},
        {PCContextName_Climb, -1, 0x431, 0},
        {PCContextName_Tightrope, -1, 0x80431, 0},
        {PCContextName_WallShuffle, -1, 0x3031, 0},
        {PCContextName_Grapple, -1, 0x40000430, 0},
        {PCContextName_ZipUp, -1, 0x40000430, 0},
        {PCContextName_PlaceDetonator, -1, 0x33, 0},
        {PCContextName_PickUpDetonator, -1, 0x31, 0},
        {PCContextName_PullLever, -1, 0x31, 0},
        {PCContextName_Float, -1, 0x30, 2},
        {PCContextName_Signal, -1, 0x4000433, 0},
        {PCContextName_Batarang, -1, 0x8000033, 0},
        {PCContextName_Hang, -1, 0x430, 0},
        {PCContextName_Glide, -1, 0x430, 0},
        {PCContextName_Catch, -1, 0x13, 0},
        {PCContextName_Techno, -1, 0x84000031, 0},
        {PCContextName_AttractoTarget, -1, 0x4000033, 0},
        {PCContextName_AttractoDeposit, -1, 0x33, 0},
        {PCContextName_Sonar, -1, 0x33, 0},
        {PCContextName_LedgeTerrain, -1, 0x431, 0},
        {PCContextName_Transform, -1, 0x33, 0},
        {PCContextName_WallJumpWait, -1, 0x2431, 0},
        {PCContextName_SuperCarry, -1, 0x33, 0},
        {PCContextName_PushObstacle, -1, 0x3010, 0},
        {PCContextName_Stunned, -1, 0x10000031, 0},
        {PCContextName_Ledge, -1, 0x431, 0},
        {PCContextName_Security, -1, 0x33, 0},
        {PCContextName_Ballooning, -1, 0x100010, 0},
        {PCContextName_ThrowQuick, -1, 0x33, 0},
        {PCContextName_DieAir, -1, 0x80b1, 0},
        {PCContextName_DieGround, -1, 0x80b1, 0},
        {PCContextName_HatMachine, -1, 0x33, 0},
        {PCContextName_Whip, -1, 0x33, 1},
        {PCContextName_NetWait, -1, 0x23, 0},
    };
#pragma data_seg()
#pragma data_seg(".data$CInfo")
    CHARACTER_CONTEXT_INFO_PC *CInfo = &PCContextInfoTable[1];
#pragma data_seg()
#pragma bss_seg(".bss$dynamic_antinodes")
    AIANTINODE_PC dynamic_antinodes[64];
#pragma bss_seg()
#pragma data_seg(".data$default_path_heighttol")
    f32 default_path_heighttol = 0.2f;
#pragma data_seg()
}

// Additional PC movement, targeting, arcade and callback state.
extern "C" {
#pragma bss_seg(".bss$TargetDist_Near2")
    f32 TargetDist_Near2;
#pragma bss_seg()
#pragma bss_seg(".bss$TargetDist_Mid2")
    f32 TargetDist_Mid2;
#pragma bss_seg()
#pragma bss_seg(".bss$TargetDeg_Near")
    u16 TargetDeg_Near;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTAR2BATTLEA_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ASTEROIDCHASED_LDATA")
    LEVELDATA_PC *ASTEROIDCHASED_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARRESCUEA_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$applygravity_extrahoveroffset")
    f32 applygravity_extrahoveroffset;
#pragma bss_seg()
#pragma data_seg(".data$ApplyGravityContextHoverHeight")
    f32 ApplyGravityContextHoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma const_seg(".rdata$ForcePushSeekRate")
    extern const f32 ForcePushSeekRate = 20.0f;
#pragma const_seg()
#pragma const_seg(".rdata$ForcePushApproachInnerSquare")
    extern const f32 ForcePushApproachInnerSquare = 0.010000000707805157f;
#pragma const_seg()
#pragma const_seg(".rdata$ForcePushApproachOuterSquare")
    extern const f32 ForcePushApproachOuterSquare = 0.04000000283122063f;
#pragma const_seg()
#pragma data_seg(".data$id_GONKDROID")
    u16 id_GONKDROID = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$ONEPLAYERPOWERUPS")
    i32 ONEPLAYERPOWERUPS = 1;
#pragma data_seg()
#pragma bss_seg(".bss$APIOBJECTFromObjIDFn")
    APIOBJECT_FROM_ID_PC APIOBJECTFromObjIDFn;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText0")
    i16 PCArcadeModeText0;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText1")
    i16 PCArcadeModeText1;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText2")
    i16 PCArcadeModeText2;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeLevelText")
    i16 PCArcadeLevelText;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText")
    i16 PCArcadeModeText;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadePlayText")
    i16 PCArcadePlayText;
#pragma bss_seg()
#pragma data_seg(".data$Arcade_Mode")
    ARCADE_MODE_PC Arcade_Mode[7] = {
        {&PCArcadeModeText0, 0, 0x19},   {&PCArcadeModeText1, 10, 0x12}, {&PCArcadeModeText1, 25, 0x12},
        {&PCArcadeModeText1, 50, 0x12},  {&PCArcadeModeText2, 50000, 4}, {&PCArcadeModeText2, 100000, 4},
        {&PCArcadeModeText2, 250000, 4},
    };
#pragma data_seg()
#pragma data_seg(".data$ArcadeItem")
    ARCADE_ITEM_PC ArcadeItem = {&PCArcadeLevelText, 0, 12, 0, &PCArcadeModeText, 0, 7, 0, &PCArcadePlayText, 0, 5, 0};
#pragma data_seg()
#pragma data_seg(".data$gizmoerrorlogsize")
    i32 gizmoerrorlogsize = 2048;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$v001")
    NUVEC v001 = {0.0f, 0.0f, 1.0f};
#pragma data_seg()
}

// PC movement tuning, timers, input masks and parser path.
extern "C" {
#pragma bss_seg(".bss$GamePlayTimer")
    TIMER GamePlayTimer;
#pragma bss_seg()
#pragma data_seg(".data$GAMEPAD_JUMP")
    u32 GAMEPAD_JUMP = 64;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_ACTION")
    u32 GAMEPAD_ACTION = 128;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_SPECIAL")
    u32 GAMEPAD_SPECIAL = 32;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_TAG")
    u32 GAMEPAD_TAG = 16;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityContext22HoverHeight")
    f32 ApplyGravityContext22HoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityContext42HoverHeight")
    f32 ApplyGravityContext42HoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityWallRunAcceleration")
    f32 ApplyGravityWallRunAcceleration = -0.25f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_GRAVITY")
    f32 PODSPRINT_GRAVITY = -35.0f;
#pragma data_seg()
#pragma data_seg(".data$SLAMGRAVITY")
    f32 SLAMGRAVITY = -15.0f;
#pragma data_seg()
#pragma data_seg(".data$id_DRAGBOMB")
    u16 id_DRAGBOMB = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$AIPathDefaultChecks")
    i32 AIPathDefaultChecks = 1;
#pragma data_seg()
#pragma const_seg(".rdata$ApplyGravityVelocityFactor")
    extern const f64 ApplyGravityVelocityFactor = 0.3330000042915344;
#pragma const_seg()
#pragma const_seg(".rdata$ApplyGravityFrameScale")
    extern const f64 ApplyGravityFrameScale = 180.0;
#pragma const_seg()
#pragma const_seg(".rdata$ApplyGravityMinimumFrameTime")
    extern const f32 ApplyGravityMinimumFrameTime = 0.01666666753590107f;
#pragma const_seg()
#pragma const_seg(".rdata$GizmoConfigPathFormat")
    extern const char GizmoConfigPathFormat[7] = "%s.giz";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$AIRespawnOnPathFn")
    AI_RESPAWN_ON_PATH_PC AIRespawnOnPathFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetCharacterGoalSpeedFn")
    AI_CHARACTER_GOAL_SPEED_PC GetCharacterGoalSpeedFn;
#pragma bss_seg()
#pragma bss_seg(".bss$MidSpecialMoveFn")
    AI_MID_SPECIAL_MOVE_PC MidSpecialMoveFn;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$GCDataList")
    PLAYERCHARACTERCONFIG_PC *GCDataList;
#pragma bss_seg()
#pragma bss_seg(".bss$CharacterCustomiser")
    CUSTOMISER_LAYERS_PC *CharacterCustomiser;
#pragma bss_seg()
#pragma data_seg(".data$id_BODYGUARD")
    i16 id_BODYGUARD = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GEONOSIAN")
    i16 id_GEONOSIAN = -1;
#pragma data_seg()
#pragma data_seg(".data$id_CHEWBACCA")
    i16 id_CHEWBACCA = -1;
#pragma data_seg()
#pragma bss_seg(".bss$MAGNETOFFSET")
    f32 MAGNETOFFSET;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$Default_ADDPART")
    ADDPART_PC Default_ADDPART = {
        0,                                    // matrix
        0,                                    // position
        0,                                    // velocity
        0,                                    // owner
        0,                                    // recipient
        0.10000000149011612f,                 // field_14
        0.10000000149011612f,                 // field_18
        -5.0f,                                // gravity
        0.75f,                                // field_20
        0,                                    // special
        -1,                                   // part_id
        0x00000000u,                          // flags
        0,                                    // callback_30
        0,                                    // callback_34
        0,                                    // callback_38
        0,                                    // callback_3c
        0,                                    // callback_40
        0,                                    // callback_44
        0,                                    // callback_48
        0,                                    // callback_4c
        0,                                    // callback_50
        0,                                    // callback_54
        -1,                                   // effect_id_58
        0,                                    // debris_handle
        -1,                                   // effect_id_60
        -1,                                   // effect_id_64
        60.0f,                                // field_68
        60.0f,                                // field_6c
        -1,                                   // effect_id_70
        -1,                                   // effect_id_74
        -1,                                   // effect_id_78
        1.0f,                                 // effect_scale
        0,                                    // lighting_handle
        -1,                                   // lighting_id
        2000000.0f,                           // field_88
        0.01666666753590107f,                 // time_step
        0xffffffffu,                          // hostility_group
        0,                                    // lighting_data
        1.0f,                                 // lighting_scale_x
        1.0f,                                 // lighting_scale_y
        1.0f,                                 // lighting_scale_z
        0.0f,                                 // field_a4
        {0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u}, // field_a8
        1.0f,                                 // field_c0
        0,                                    // field_c4
        {0, 0, 0},                            // padding_c5
    };
#pragma data_seg()

#pragma bss_seg(".bss$Part")
    PART_PC *Part;
#pragma bss_seg()
#pragma bss_seg(".bss$part_scene_pageid")
    i32 part_scene_pageid[32] = {0};
#pragma bss_seg()
#pragma bss_seg(".bss$part_page_used")
    i32 part_page_used[8] = {0};
#pragma bss_seg()
#pragma data_seg(".data$MAXPARTS")
    i32 MAXPARTS = 32;
#pragma data_seg()
#pragma data_seg(".data$part_slot_cursor")
    i32 part_slot_cursor = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$PreparingForSpecialMoveFn")
    AI_PREPARING_SPECIAL_MOVE_PC PreparingForSpecialMoveFn;
#pragma bss_seg()
#pragma data_seg(".data$new_retreat")
    i32 new_retreat = 1;
#pragma data_seg()
#pragma data_seg(".data$ai_moveradius")
    f32 ai_moveradius = 0.10000000149011612f;
#pragma data_seg()
#pragma data_seg(".data$AIMovementInstructionPositiveThreshold")
    f32 AIMovementInstructionPositiveThreshold = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$AIMovementInstructionNegativeThreshold")
    f32 AIMovementInstructionNegativeThreshold = -0.5f;
#pragma data_seg()

#pragma bss_seg(".bss$LevHSpecial")
    NUSPECIALHANDLE LevHSpecial[88];
#pragma bss_seg()
#pragma bss_seg(".bss$LevGizObst")
    GIZOBSTACLE *LevGizObst[8];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$CarWashHack")
    GAMEOBJECT *CarWashHack;
#pragma bss_seg()
#pragma bss_seg(".bss$IDLESPEEDINNARROWSOCKSONLY")
    i32 IDLESPEEDINNARROWSOCKSONLY;
#pragma bss_seg()
#pragma bss_seg(".bss$VehicleAreaRememberSpeed")
    f32 VehicleAreaRememberSpeed;
#pragma bss_seg()
#pragma data_seg(".data$VehicleCollisionDamageSpeedFraction")
    f32 VehicleCollisionDamageSpeedFraction = 0.6000000238418579f;
#pragma data_seg()
#pragma data_seg(".data$VehicleCollisionFrontAngle")
    i32 VehicleCollisionFrontAngle = 45;
#pragma data_seg()
#pragma data_seg(".data$VehicleCollisionRearAngle")
    i32 VehicleCollisionRearAngle = 10;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_WATER_SPEED_MUL")
    f32 VEHICLE_WATER_SPEED_MUL = 0.699999988079071f;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_SUBMARINE_DEPTH")
    f32 VEHICLE_SUBMARINE_DEPTH = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_TUBE_UPSPEED")
    f32 VEHICLE_TUBE_UPSPEED = 5.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_SLOWSPEED")
    f32 PODSPRINT_SLOWSPEED = 15.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_CRUISESPEED")
    f32 PODSPRINT_CRUISESPEED = 25.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_RUNSPEED")
    f32 PODSPRINT_RUNSPEED = 35.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_ACCELTIME")
    f32 PODSPRINT_ACCELTIME = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_DECELTIME")
    f32 PODSPRINT_DECELTIME = 0.25f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_VELOCITYSEEK")
    f32 PODSPRINT_VELOCITYSEEK = 8.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_TURNRATE")
    f32 PODSPRINT_TURNRATE = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_HIGH_TURNRATE")
    f32 PODSPRINT_HIGH_TURNRATE = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$MOSEISLEYB_LDATA")
    LEVELDATA_PC *MOSEISLEYB_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTAR2BATTLEE_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEE_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTAR2BATTLEF_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEF_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTAR2BATTLEG_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEG_LDATA;
#pragma bss_seg()
#pragma data_seg(".data$id_LANDSPEEDER")
    i16 id_LANDSPEEDER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_WOOKIEFLYER")
    i16 id_WOOKIEFLYER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_IMPERIALGUARD")
    i16 id_IMPERIALGUARD = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GRIEVOUS")
    i16 id_GRIEVOUS = -1;
#pragma data_seg()
#pragma data_seg(".data$id_THEEMPEROR")
    i16 id_THEEMPEROR = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BOB")
    i16 id_BOB = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DOOKU")
    i16 id_DOOKU = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DARTHMAUL")
    i16 id_DARTHMAUL = -1;
#pragma data_seg()
#pragma data_seg(".data$id_ANAKINPADAWAN")
    i16 id_ANAKINPADAWAN = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$TURNTIME")
    f32 TURNTIME = 1.0f;
#pragma data_seg()
#pragma const_seg(".rdata$TurnLoopSfx")
    extern const char TurnLoopSfx[17] = "XWing_LoopDeLoop";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$BladeTab")
    BLADE_PC BladeTab[4] = {
        {101, 223, 1, 59, 64, {255, 31, 0}, {0, 0, 0}},
        {103, 221, 2, 60, 65, {30, 191, 15}, {0, 0, 0}},
        {105, 225, 3, 61, 66, {0, 191, 255}, {0, 0, 0}},
        {107, 227, 4, 62, 67, {200, 0, 255}, {0, 0, 0}},
    };
#pragma data_seg()
#pragma data_seg(".data$GameDeflectPartGravity")
    f32 GameDeflectPartGravity = -2.5f;
#pragma data_seg()
#pragma data_seg(".data$GameDeflectPartSpeed")
    f32 GameDeflectPartSpeed = 5.0f;
#pragma data_seg()
#pragma data_seg(".data$PARTSCALEUPTIME")
    f32 PARTSCALEUPTIME = 0.5f;
#pragma data_seg()

#pragma bss_seg(".bss$podsprint")
    PODSPRINT_PC podsprint;
#pragma bss_seg()
#pragma bss_seg(".bss$NarrowSockException")
    NARROWSOCKEXCEPTION_PC *NarrowSockException;
#pragma bss_seg()
#pragma data_seg(".data$id_BASKETCANNON")
    i16 id_BASKETCANNON = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ScriptProcessFirstTimeActionFn")
    AI_SCRIPT_FIRST_ACTION_PC ScriptProcessFirstTimeActionFn;
#pragma bss_seg()
#pragma bss_seg(".bss$current_packet")
    AIPACKET_PC *current_packet;
#pragma bss_seg()
#pragma bss_seg(".bss$current_processor")
    AISCRIPTPROCESS_PC *current_processor;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$SaberSparksTimer")
    f32 SaberSparksTimer;
#pragma bss_seg()
#pragma const_seg(".rdata$SaberSparksName")
    extern const char SaberSparksName[12] = "SaberSparks";
#pragma const_seg()
#pragma bss_seg(".bss$GameParamToFloatFn")
    AI_GAME_PARAM_TO_FLOAT_PC GameParamToFloatFn;
#pragma bss_seg()
#pragma const_seg(".rdata$AIExpressionRandName")
    extern const char AIExpressionRandName[5] = "Rand";
#pragma const_seg()
#pragma const_seg(".rdata$AIExpressionTrueName")
    extern const char AIExpressionTrueName[5] = "true";
#pragma const_seg()
#pragma const_seg(".rdata$AIExpressionMaxName")
    extern const char AIExpressionMaxName[4] = "max";
#pragma const_seg()
#pragma data_seg(".data$rocket_speed")
    f32 rocket_speed = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$rocket_lock_distance")
    f32 rocket_lock_distance = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$GUNSHIPA_LDATA")
    LEVELDATA_PC *GUNSHIPA_LDATA;
#pragma bss_seg()
#pragma const_seg(".rdata$AICharacterInitialiseName")
    extern const char AICharacterInitialiseName[11] = "Initialise";
#pragma const_seg()
#pragma bss_seg(".bss$perm_debrissys")
    APIDEBRISSYS_PC *perm_debrissys;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$id_TIEFIGHTER")
    i16 id_TIEFIGHTER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_XWING")
    i16 id_XWING = -1;
#pragma data_seg()
#pragma data_seg(".data$id_STAP")
    i16 id_STAP = -1;
#pragma data_seg()
#pragma data_seg(".data$id_STAP2")
    i16 id_STAP2 = -1;
#pragma data_seg()
#pragma bss_seg(".bss$AIActionParseSpeedFn")
    AI_PARSE_SPEED_PC AIActionParseSpeedFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetViewRangeFn")
    AI_CHARACTER_DISTANCE_PC GetViewRangeFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetHearDistanceFn")
    AI_CHARACTER_DISTANCE_PC GetHearDistanceFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetMaxViewHeightFn")
    AI_CHARACTER_DISTANCE_PC GetMaxViewHeightFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetMinViewHeightFn")
    AI_CHARACTER_DISTANCE_PC GetMinViewHeightFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetOriginPositionFn")
    AI_ORIGIN_POSITION_PC GetOriginPositionFn;
#pragma bss_seg()
#pragma bss_seg(".bss$CHARPAK")
    i32 CHARPAK;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$LevGizmo")
    GIZMO_PC *LevGizmo[12];
#pragma bss_seg()
#pragma bss_seg(".bss$Explosions")
    EXPLOSION_PC Explosions[8];
#pragma bss_seg()
#pragma bss_seg(".bss$i_explosion")
    i32 i_explosion;
#pragma bss_seg()
#pragma bss_seg(".bss$ShadowMode")
    i32 ShadowMode;
#pragma bss_seg()
#pragma bss_seg(".bss$CheckLostDataFn")
    CHECK_LOST_DATA_PC CheckLostDataFn;
#pragma bss_seg()
// Existing complete working bytes view retained, including the native utility WORD at +0x10000.
// The original owner capacity remains unknown; the CSV extent intentionally stays blank.
#pragma bss_seg(".bss$ConfigBuffer")
    CONFIG_BUFFER_PC ConfigBuffer;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$SURFACEBITS_DUST")
    u16 SURFACEBITS_DUST;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AIActionRandomName")
    const char AIActionRandomName[7] = "Random";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionMinTimeName")
    const char AIActionMinTimeName[8] = "mintime";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionMaxTimeName")
    const char AIActionMaxTimeName[8] = "maxtime";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionTimeName")
    const char AIActionTimeName[5] = "time";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFramesName")
    const char AIActionFramesName[7] = "frames";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIncrementName")
    const char AIActionIncrementName[5] = "inc=";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionDecrementName")
    const char AIActionDecrementName[5] = "dec=";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionStateName")
    const char AIActionStateName[6] = "state";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleTeleportName")
    extern const char AIActionCircleTeleportName[9] = "teleport";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleReverseName")
    extern const char AIActionCircleReverseName[8] = "reverse";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleAnticlockwiseName")
    extern const char AIActionCircleAnticlockwiseName[14] = "anticlockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleClockwiseName")
    extern const char AIActionCircleClockwiseName[10] = "clockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathAnticlockwiseName")
    const char AIPathAnticlockwiseName[14] = "ANTICLOCKWISE";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathClockwiseName")
    const char AIPathClockwiseName[10] = "CLOCKWISE";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathReverseName")
    const char AIPathReverseName[8] = "REVERSE";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathRandomName")
    const char AIPathRandomName[7] = "RANDOM";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathFacingName")
    const char AIPathFacingName[7] = "facing";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathCurrentDistanceName")
    const char AIPathCurrentDistanceName[12] = "currentdist";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathCurrentPositionName")
    const char AIPathCurrentPositionName[17] = "current_position";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathLocatorName")
    const char AIPathLocatorName[9] = "locator=";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathOriginName")
    const char AIPathOriginName[7] = "origin";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathCanGoOffPathName")
    const char AIPathCanGoOffPathName[16] = "can_go_off_path";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathFaceName")
    const char AIPathFaceName[5] = "face";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathWaitTimeName")
    const char AIPathWaitTimeName[9] = "waittime";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathXZRangeCheckName")
    const char AIPathXZRangeCheckName[14] = "xz_rangecheck";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathGoalRangeName")
    const char AIPathGoalRangeName[10] = "goalrange";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFaceOffsetName")
    const char AIActionFaceOffsetName[11] = "faceoffset";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionNearestOpponentName")
    const char AIActionNearestOpponentName[17] = "nearest_opponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionClockwiseName")
    const char AIActionClockwiseName[10] = "Clockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionAnticlockwiseName")
    const char AIActionAnticlockwiseName[14] = "AntiClockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionReverseName")
    const char AIActionReverseName[8] = "Reverse";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFalseName")
    const char AIActionFalseName[6] = "FALSE";
#pragma const_seg()
#pragma bss_seg(".bss$AIActionDiagnosticMode")
    i32 AIActionDiagnosticMode;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackRadiusPc")
    f32 ForceBackRadiusPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackRadius2Pc")
    f32 ForceBackRadius2Pc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackObjPc")
    GAMEOBJECT *ForceBackObjPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackPosPc")
    NUVEC *ForceBackPosPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackTypePc")
    i32 ForceBackTypePc;
#pragma bss_seg()

#pragma data_seg(".data$Tag_Mode")
    i32 Tag_Mode = 2;
#pragma data_seg()
#pragma bss_seg(".bss$player_tag_to")
    GAMEOBJECT *player_tag_to;
#pragma bss_seg()
#pragma bss_seg(".bss$player_tag_from")
    GAMEOBJECT *player_tag_from;
#pragma bss_seg()
#pragma bss_seg(".bss$player_tag_timer")
    f32 player_tag_timer;
#pragma bss_seg()
#pragma bss_seg(".bss$do_player_tag")
    i32 do_player_tag;
#pragma bss_seg()
#pragma bss_seg(".bss$Tag_DrawIconFn")
    TAG_DRAW_ICON_PC Tag_DrawIconFn;
#pragma bss_seg()
#pragma data_seg(".data$TakeOverATATJumpScale")
    f32 TakeOverATATJumpScale = 0.75f;
#pragma data_seg()
#pragma data_seg(".data$SpeederChaseATATExitSpeed")
    f32 SpeederChaseATATExitSpeed = 4.0f;
#pragma data_seg()
#pragma data_seg(".data$SpeederChaseATATExitHeight")
    f32 SpeederChaseATATExitHeight = 2.5f;
#pragma data_seg()
#pragma data_seg(".data$SpeederChaseATATExitLandPos")
    NUVEC SpeederChaseATATExitLandPos = {-159.0f, 6.869999885559082f, -17.600000381469727f};
#pragma data_seg()
#pragma bss_seg(".bss$LastSafePosExtraFn")
    LAST_SAFE_POS_EXTRA_PC LastSafePosExtraFn;
#pragma bss_seg()
#pragma data_seg(".data$LEGOCONTEXT_DOOMED")
    i32 LEGOCONTEXT_DOOMED = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_BEENTAKENOVER")
    i32 LEGOCONTEXT_BEENTAKENOVER = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_GETIN")
    i32 LEGOCONTEXT_GETIN = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_EATEN")
    i32 LEGOCONTEXT_EATEN = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_BUCK")
    i32 LEGOCONTEXT_BUCK = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_JUMP")
    i32 LEGOCONTEXT_JUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_BUCK")
    i16 LEGOACT_BUCK = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_JUMP")
    i16 LEGOACT_JUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$BUCK_RIDERJUMPCLEARANCE")
    f32 BUCK_RIDERJUMPCLEARANCE = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$BuckStartExtraFn")
    TAG_DRAW_ICON_PC BuckStartExtraFn;
#pragma bss_seg()
}

extern "C" {
// Two-entry reconstruction access view; the original full array extent is unknown.
// PC readers and reset producers access these two i32 entries; no original sizeof credit.
#pragma data_seg(".data$LevSafePlatID")
    i32 LevSafePlatID[2] = {-1, -1};
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$id_CLONEWALKER")
    i16 id_CLONEWALKER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TROOPERCANNON")
    i16 id_TROOPERCANNON = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BIGGUN")
    i16 id_BIGGUN = -1;
#pragma data_seg()
#pragma data_seg(".data$id_HEAVYREPEATINGCANNON")
    i16 id_HEAVYREPEATINGCANNON = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TAUNTAUN")
    i16 id_TAUNTAUN = -1;
#pragma data_seg()
#pragma data_seg(".data$id_FLASHSPEEDER")
    i16 id_FLASHSPEEDER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DEWBACK")
    i16 id_DEWBACK = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BOMARRMONK")
    i16 id_BOMARRMONK = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BANTHA")
    i16 id_BANTHA = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GYROCOPTER")
    i16 id_GYROCOPTER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_LUKESKYWALKERDAGOBAH")
    i16 id_LUKESKYWALKERDAGOBAH = -1;
#pragma data_seg()
#pragma data_seg(".data$id_ATAT")
    i16 id_ATAT = -1;
#pragma data_seg()
#pragma data_seg(".data$DEFAULT_PLAYERHITPOINTS")
    i32 DEFAULT_PLAYERHITPOINTS = 8;
#pragma data_seg()
#pragma bss_seg(".bss$VADERC_LDATA")
    LEVELDATA_PC *VADERC_LDATA;
#pragma bss_seg()
#pragma data_seg(".data$TOGGLEHOLDTIME")
    f32 TOGGLEHOLDTIME = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$CUTSKIPLOCK")
    i32 CUTSKIPLOCK;
#pragma bss_seg()
#pragma bss_seg(".bss$players_cannot_exit_speeder")
    i32 players_cannot_exit_speeder;
#pragma bss_seg()
#pragma const_seg(".rdata$TakeOverState_JustBeenTakenOver")
    const char TakeOverState_JustBeenTakenOver[18] = "JustBeenTakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$TakeOverState_MapRunAway")
    const char TakeOverState_MapRunAway[11] = "MapRunAway";
#pragma const_seg()
#pragma const_seg(".rdata$TakeOverState_ReleasedTakeOver")
    const char TakeOverState_ReleasedTakeOver[17] = "ReleasedTakeOver";
#pragma const_seg()
}

extern "C" {
#pragma const_seg(".rdata$AILocatorPersonalFormat")
    const char AILocatorPersonalFormat[6] = "%s_%s";
#pragma const_seg()
#pragma const_seg(".rdata$AILocatorIndexedFormat")
    const char AILocatorIndexedFormat[6] = "%s_%d";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIndexedName")
    const char AIActionIndexedName[8] = "indexed";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionPersonalName")
    const char AIActionPersonalName[9] = "personal";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIDName")
    const char AIActionIDName[3] = "id";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionToName")
    const char AIActionToName[3] = "to";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionBothWaysName")
    const char AIActionBothWaysName[9] = "bothways";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFromName")
    const char AIActionFromName[5] = "from";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIgnoreRadiusName")
    const char AIActionIgnoreRadiusName[14] = "ignore_radius";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionOpenName")
    const char AIActionOpenName[5] = "open";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionLengthName")
    const char AIActionLengthName[7] = "length";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathMustReachDestinationName")
    const char AIPathMustReachDestinationName[23] = "must_reach_destination";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathIgnorePathName")
    const char AIPathIgnorePathName[12] = "ignore_path";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathFaceOpponentName")
    const char AIPathFaceOpponentName[14] = "face_opponent";
#pragma const_seg()
#pragma bss_seg(".bss$AIBigJumpToDestinationFn")
    AI_BIG_JUMP_TO_DESTINATION_PC AIBigJumpToDestinationFn;
#pragma bss_seg()
#pragma bss_seg(".bss$AiSysStateDebugee")
    AISCRIPTPROCESS_PC *AiSysStateDebugee;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$BonusArea")
    i32 BonusArea;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_BIGJUMP")
    i32 LEGOCONTEXT_BIGJUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_FALL")
    i16 LEGOACT_FALL = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_COMBOJUMP")
    i16 LEGOACT_COMBOJUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_JUMP2")
    i16 LEGOACT_JUMP2 = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$aicreature_sets_alive")
    u8 aicreature_sets_alive[16];
#pragma bss_seg()
#pragma bss_seg(".bss$LevGameObject")
    GAMEOBJECT *LevGameObject[8];
#pragma bss_seg()
#pragma bss_seg(".bss$LevFlag")
    i32 LevFlag[4];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_TUBE")
    i32 LEGOCONTEXT_TUBE = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_GLIDE")
    i32 LEGOCONTEXT_GLIDE = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$party_follow_offsets")
    f32 party_follow_offsets[8];
#pragma bss_seg()
#pragma bss_seg(".bss$ai_fighting")
    i32 ai_fighting;
#pragma bss_seg()
#pragma bss_seg(".bss$drop_back_in_timer")
    f32 drop_back_in_timer;
#pragma bss_seg()
#pragma data_seg(".data$aitol")
    f32 aitol = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$direction_scale")
    f32 direction_scale = 100.0f;
#pragma data_seg()
#pragma bss_seg(".bss$player2")
    GAMEOBJECT *player2;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$nethost")
    i32 nethost;
#pragma bss_seg()
#pragma data_seg(".data$id_PKDROID")
    i16 id_PKDROID = -1;
#pragma data_seg()
#pragma bss_seg(".bss$NEGOTIATIONSC_LDATA")
    LEVELDATA_PC *NEGOTIATIONSC_LDATA;
#pragma bss_seg()
#pragma const_seg(".rdata$GameObjectState_InActive")
    const char GameObjectState_InActive[9] = "InActive";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$CurrentSpeed")
    f32 CurrentSpeed = 10.0f;
#pragma data_seg()
#pragma bss_seg(".bss$avg_currentspeed_mul")
    f32 avg_currentspeed_mul;
#pragma bss_seg()
#pragma data_seg(".data$TwistSockRotationModeA")
    i32 TwistSockRotationModeA = 3;
#pragma data_seg()
#pragma data_seg(".data$TwistSockRotationModeB")
    i32 TwistSockRotationModeB = 5;
#pragma data_seg()
#pragma data_seg(".data$TwistYawLeanDegrees")
    i32 TwistYawLeanDegrees = 15;
#pragma data_seg()
#pragma data_seg(".data$TwistPitchLeanDegrees")
    i32 TwistPitchLeanDegrees = 15;
#pragma data_seg()
#pragma data_seg(".data$TwistLateralSpeed")
    f32 TwistLateralSpeed = 8.0f;
#pragma data_seg()
#pragma bss_seg(".bss$TwistScaleLean")
    i32 TwistScaleLean;
#pragma bss_seg()
}

extern "C" {
// Two-entry reconstruction access views; all observed PC readers/producers select 0 or 1.
// Original complete array capacities remain unknown; CSV sizes intentionally stay blank.
#pragma bss_seg(".bss$PlayerPadCurrentSpeed")
    f32 PlayerPadCurrentSpeed[2];
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerPadPreviousSpeed")
    f32 PlayerPadPreviousSpeed[2];
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerPadCurrentAngle")
    u16 PlayerPadCurrentAngle[2];
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerPadPreviousAngle")
    u16 PlayerPadPreviousAngle[2];
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AIGamePunch2Name")
    const char AIGamePunch2Name[8] = "PUNCH_2";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunch1Name")
    const char AIGamePunch1Name[8] = "PUNCH_1";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameSetName")
    const char AIGameSetName[5] = "set=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameCharacterName")
    const char AIGameCharacterName[11] = "character=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameStateName")
    const char AIGameStateName[7] = "state=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameNearestName")
    const char AIGameNearestName[8] = "nearest";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameShootName")
    const char AIGameShootName[6] = "SHOOT";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameBlockName")
    const char AIGameBlockName[6] = "BLOCK";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunchSpecialName")
    const char AIGamePunchSpecialName[14] = "PUNCH_SPECIAL";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunchBehindName")
    const char AIGamePunchBehindName[13] = "PUNCH_BEHIND";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunch3Name")
    const char AIGamePunch3Name[8] = "PUNCH_3";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameAttackOverrideName")
    const char AIGameAttackOverrideName[16] = "attack_override";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameJediGoodieAttackName")
    const char AIGameJediGoodieAttackName[18] = "jediGoodie_attack";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameRangeName")
    const char AIGameRangeName[6] = "range";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameFireIntervalName")
    const char AIGameFireIntervalName[13] = "fireinterval";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameFireRangeName")
    const char AIGameFireRangeName[10] = "firerange";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameEndName")
    const char AIGameEndName[5] = "end=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameStartName")
    const char AIGameStartName[7] = "start=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameHoldSpecialButtonName")
    const char AIGameHoldSpecialButtonName[20] = "hold_special_button";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameTurnName")
    const char AIGameTurnName[5] = "turn";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameRandomTurnDirectionName")
    const char AIGameRandomTurnDirectionName[14] = "rand_turn_dir";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameMaximumTurnName")
    const char AIGameMaximumTurnName[9] = "max_turn";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameMinimumTurnName")
    const char AIGameMinimumTurnName[9] = "min_turn";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameOpponentName")
    const char AIGameOpponentName[9] = "Opponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameTakeOverTargetName")
    const char AIGameTakeOverTargetName[15] = "TakeOverTarget";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$CircleLeanDegrees")
    i32 CircleLeanDegrees = 30;
#pragma data_seg()
#pragma data_seg(".data$PodRollInputRate")
    f32 PodRollInputRate = 6.0f;
#pragma data_seg()
#pragma data_seg(".data$PodRollReleaseRate")
    f32 PodRollReleaseRate = 3.0f;
#pragma data_seg()
#pragma data_seg(".data$PodFacingSeekRate")
    f32 PodFacingSeekRate = 2.0f;
#pragma data_seg()
#pragma data_seg(".data$GUNSHIPAHACK")
    i32 GUNSHIPAHACK = 2;
#pragma data_seg()
#pragma bss_seg(".bss$podrace_section")
    i32 podrace_section;
#pragma bss_seg()
#pragma bss_seg(".bss$newgamecam")
    i32 newgamecam;
#pragma bss_seg()
#pragma data_seg(".data$GAMEPAD_DUP")
    i32 GAMEPAD_DUP = 4096;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_DDOWN")
    i32 GAMEPAD_DDOWN = 16384;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_DLEFT")
    i32 GAMEPAD_DLEFT = 32768;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_DRIGHT")
    i32 GAMEPAD_DRIGHT = 8192;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_LIFT")
    i32 GAMEPAD_LIFT = 8;
#pragma data_seg()
#pragma data_seg(".data$OFFSCREEN_CATCHUP_TIME")
    f32 OFFSCREEN_CATCHUP_TIME = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$LIFTPLAYER")
    i32 LIFTPLAYER;
#pragma bss_seg()
}

extern "C" {
// Hub arrays are two-entry source views; full original capacities remain unknown.
#pragma bss_seg(".bss$pod_roll")
    f32 pod_roll[2];
#pragma bss_seg()
#pragma bss_seg(".bss$pod_roll_target")
    f32 pod_roll_target[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Hub_PadSpeed")
    f32 Hub_PadSpeed[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Hub_PadAngle")
    u16 Hub_PadAngle[2];
#pragma bss_seg()
#pragma bss_seg(".bss$MissionSys")
    MISSIONSYS_PC *MissionSys;
#pragma bss_seg()
#pragma bss_seg(".bss$GameObjectDimensionsExtraFn")
    void(__cdecl *GameObjectDimensionsExtraFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$idealgoalrange")
    f32 idealgoalrange = 1.5f;
#pragma data_seg()
#pragma data_seg(".data$engagefiretime")
    f32 engagefiretime = 1.5f;
#pragma data_seg()
#pragma bss_seg(".bss$DEFAULT_MOVE_RANGE")
    f32 DEFAULT_MOVE_RANGE;
#pragma bss_seg()
#pragma bss_seg(".bss$LevelCharacterTypeIDFn")
    AI_LEVEL_CHARACTER_TYPE_ID_PC LevelCharacterTypeIDFn;
#pragma bss_seg()
#pragma bss_seg(".bss$SpecialRouteCharacterTypeIDFn")
    AI_LEVEL_CHARACTER_TYPE_ID_PC SpecialRouteCharacterTypeIDFn;
#pragma bss_seg()
#pragma bss_seg(".bss$LevelCharacterGlobalIDFn")
    AI_LEVEL_CHARACTER_GLOBAL_ID_PC LevelCharacterGlobalIDFn;
#pragma bss_seg()
#pragma const_seg(".rdata$AICombatMinimumRangeName")
    const char AICombatMinimumRangeName[10] = "minrange=";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatStaticName")
    const char AICombatStaticName[7] = "static";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatOffscreenName")
    const char AICombatOffscreenName[10] = "offscreen";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatNoFireInMiniCutName")
    const char AICombatNoFireInMiniCutName[19] = "no_fire_in_minicut";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatCircleName")
    const char AICombatCircleName[7] = "circle";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatOpponentName")
    const char AICombatOpponentName[10] = "opponent=";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatInstantName")
    const char AICombatInstantName[8] = "instant";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatFrontArcOnlyName")
    const char AICombatFrontArcOnlyName[13] = "frontArcOnly";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatDroidName")
    const char AICombatDroidName[15] = "opponent=droid";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatNearestEnemyName")
    const char AICombatNearestEnemyName[23] = "opponent=nearest_enemy";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatLastAttackerName")
    const char AICombatLastAttackerName[14] = "last_attacker";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatOpponentTypeName")
    const char AICombatOpponentTypeName[13] = "opponentType";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_WHIP")
    i32 LEGOCONTEXT_WHIP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_SUPERCARRY")
    i32 LEGOCONTEXT_SUPERCARRY = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_SPECIALMOVE_ATTACKER")
    i32 LEGOCONTEXT_SPECIALMOVE_ATTACKER = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_SPECIALMOVE_VICTIM")
    i32 LEGOCONTEXT_SPECIALMOVE_VICTIM = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_BUILDIT")
    i32 LEGOCONTEXT_BUILDIT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_PUSHOBSTACLE")
    i32 LEGOCONTEXT_PUSHOBSTACLE = -1;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_THROWSPEED_XZ")
    f32 SUPERCARRY_THROWSPEED_XZ = 3.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_THROWSPEED_Y")
    f32 SUPERCARRY_THROWSPEED_Y = 3.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_RELEASESPEED_XZ")
    f32 SUPERCARRY_RELEASESPEED_XZ = 2.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_DROPSPEED_XZ")
    f32 SUPERCARRY_DROPSPEED_XZ = 0.2f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_DROPSPEED_Y")
    f32 SUPERCARRY_DROPSPEED_Y = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_OBJGRAVITY")
    f32 SUPERCARRY_OBJGRAVITY = -6.0f;
#pragma data_seg()
#pragma data_seg(".data$SuperCarry_KeepObjectLevel")
    i32 SuperCarry_KeepObjectLevel = 1;
#pragma data_seg()
#pragma data_seg(".data$SuperCarry_AlignObject")
    i32 SuperCarry_AlignObject = 1;
#pragma data_seg()
#pragma bss_seg(".bss$Player_ClearContextFn")
    PLAYER_CLEAR_CONTEXT_CALLBACK_PC Player_ClearContextFn;
#pragma bss_seg()
#pragma data_seg(".data$AIFireIntervalMul")
    f32 AIFireIntervalMul = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$GhostLightMul")
    f32 GhostLightMul = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$GhostLightTargetMul")
    f32 GhostLightTargetMul = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$timebase_updates")
    i32 timebase_updates = 1;
#pragma data_seg()
#pragma bss_seg(".bss$tieonsfxwait")
    f32 tieonsfxwait;
#pragma bss_seg()
#pragma bss_seg(".bss$tieoffsfxwait")
    f32 tieoffsfxwait;
#pragma bss_seg()
#pragma bss_seg(".bss$CurrentSpeedOverride")
    f32 CurrentSpeedOverride;
#pragma bss_seg()
#pragma data_seg(".data$BaseCurrentSpeed")
    f32 BaseCurrentSpeed = 10.0f;
#pragma data_seg()
#pragma bss_seg(".bss$DoubleScore")
    i32 DoubleScore;
#pragma bss_seg()
#pragma data_seg(".data$id_SEBULBASPOD")
    i16 id_SEBULBASPOD = -1;
#pragma data_seg()
#pragma bss_seg(".bss$UpdateSockets_LDATA")
    LEVELDATA_PC *UpdateSockets_LDATA;
#pragma bss_seg()
// Mixed-field source access view; complete original adaptive state extent remains unknown.
#pragma data_seg(".data$adtabentries")
    i8 adtabentries[9][4] = {{-1, -1, -1, -1}, {-1, -1, -1, 0}, {-1, -1, 0, 0}, {-1, 0, 0, 0}, {0, 0, 0, 0},
                             {1, 0, 0, 0},     {1, 1, 0, 0},    {1, 1, 1, 0},   {1, 1, 1, 1}};
#pragma data_seg()
#pragma data_seg(".data$adtab")
    i8 (*adtab)[4] = &adtabentries[4];
#pragma data_seg()
#pragma bss_seg(".bss$adaptivedifficulty")
    ADAPTIVEDIFFICULTY_PC adaptivedifficulty;
#pragma bss_seg()
#pragma const_seg(".rdata$AIControlPlayerRunSpeedName")
    const char AIControlPlayerRunSpeedName[17] = "player_run_speed";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlSeekName")
    const char AIControlSeekName[6] = "seek=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMinimumName")
    const char AIControlMinimumName[5] = "min=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMaximumName")
    const char AIControlMaximumName[5] = "max=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlClearName")
    const char AIControlClearName[6] = "clear";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMaximumDefaultName")
    const char AIControlMaximumDefaultName[12] = "max=default";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMultiplyName")
    const char AIControlMultiplyName[10] = "multiply=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlLocatorName")
    const char AIControlLocatorName[8] = "Locator";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlAllNonPartyName")
    const char AIControlAllNonPartyName[14] = "All_Non_Party";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlDefaultTitleName")
    const char AIControlDefaultTitleName[8] = "Default";
#pragma const_seg()
#pragma const_seg(".rdata$GameAIPlayerName")
    const char GameAIPlayerName[7] = "player";
#pragma const_seg()
#pragma const_seg(".rdata$GameAIPlayer2Name")
    const char GameAIPlayer2Name[9] = "player_2";
#pragma const_seg()
#pragma const_seg(".rdata$GameAIPlayerFormat")
    const char GameAIPlayerFormat[9] = "Player%d";
#pragma const_seg()
#pragma data_seg(".data$id_YWING")
    i16 id_YWING = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIYWING")
    i16 id_MINIYWING = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TIEBOMBER")
    i16 id_TIEBOMBER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINITIEBOMBER")
    i16 id_MINITIEBOMBER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINISTARDESTROYER")
    i16 id_MINISTARDESTROYER = -1;
#pragma data_seg()
#pragma bss_seg(".bss$LEGO_AIPATHCNX_FORGOODIES")
    u32 LEGO_AIPATHCNX_FORGOODIES;
#pragma bss_seg()
#pragma bss_seg(".bss$LEGO_AIPATHCNX_FORBADDIES")
    u32 LEGO_AIPATHCNX_FORBADDIES;
#pragma bss_seg()
#pragma data_seg(".data$v100")
    NUVEC v100 = {1.0f, 0.0f, 0.0f};
#pragma data_seg()
#pragma data_seg(".data$v010")
    NUVEC v010 = {0.0f, 1.0f, 0.0f};
#pragma data_seg()
// Eight queue slots are a source access view; the complete original extent is unknown.
#pragma bss_seg(".bss$NewCutInfo")
    CUTINFO *NewCutInfo[8];
#pragma bss_seg()
#pragma bss_seg(".bss$NewCutInfoCount")
    i32 NewCutInfoCount;
#pragma bss_seg()
#pragma bss_seg(".bss$LevelCompletionFallbackFlagPC")
    u32 LevelCompletionFallbackFlagPC;
#pragma bss_seg()
#pragma const_seg(".rdata$AIAnimSpeedValueName")
    const char AIAnimSpeedValueName[7] = "value=";
#pragma const_seg()
#pragma const_seg(".rdata$AIAnimSpeedMultiplyByName")
    const char AIAnimSpeedMultiplyByName[13] = "multiply_by=";
#pragma const_seg()
#pragma const_seg(".rdata$AIUseCurrentSpeedMultiplierName")
    const char AIUseCurrentSpeedMultiplierName[11] = "multiplier";
#pragma const_seg()
#pragma const_seg(".rdata$AIUseCurrentSpeedSnapName")
    const char AIUseCurrentSpeedSnapName[12] = "snaptospeed";
#pragma const_seg()
#pragma const_seg(".rdata$AIScaleOverrideResetName")
    const char AIScaleOverrideResetName[6] = "reset";
#pragma const_seg()
#pragma bss_seg(".bss$SurfaceInfo_ExtraReflectFn")
    void(__cdecl *SurfaceInfo_ExtraReflectFn)(GAMEOBJECT *object);
#pragma bss_seg()
#pragma bss_seg(".bss$ShadPoly")
    TERRAIN_POLYGON_INFO_PC *ShadPoly;
#pragma bss_seg()
#pragma bss_seg(".bss$EShadowPolygonPc")
    TERRAIN_POLYGON_INFO_PC *EShadowPolygonPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ShadowRoofPolygonPc")
    TERRAIN_POLYGON_INFO_PC *ShadowRoofPolygonPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainShadowGroupIndexPc")
    i16 TerrainShadowGroupIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoWaterHeightPc")
    f32 GetSurfaceInfoWaterHeightPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoRoofHeightPc")
    f32 GetSurfaceInfoRoofHeightPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoSurfaceNormalPc")
    NUVEC GetSurfaceInfoSurfaceNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoWaterNormalPc")
    NUVEC GetSurfaceInfoWaterNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoRuntimeMaskPc")
    PC_SURFACE_MASK_PC GetSurfaceInfoRuntimeMaskPc;
#pragma bss_seg()
#pragma data_seg(".data$GetSurfaceInfoDefaultMaskPc")
    PC_SURFACE_MASK_PC GetSurfaceInfoDefaultMaskPc = {0x20};
#pragma data_seg()
}

extern "C" {
// Minimum access view for the reviewed 0x948 and nested 0xd0 requests,
// including their two pointer trailers. The full original extent is unknown.
#pragma bss_seg(".bss$NuScratchBackingAccessPc")
    __declspec(align(4)) u8 NuScratchBackingAccessPc[0xa20];
#pragma bss_seg()
#pragma data_seg(".data$NuScratchCurrentPc")
    u8 *NuScratchCurrentPc = NuScratchBackingAccessPc;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$TerrainSkinSampleCountPc")
    extern const f64 TerrainSkinSampleCountPc = 8.0;
#pragma const_seg(".rdata$TerrainSkinSampleIndexScalePc")
    extern const f64 TerrainSkinSampleIndexScalePc = -8.0;
#pragma const_seg(".rdata$TerrainSkinBoundsMaxPc")
    extern const f32 TerrainSkinBoundsMaxPc = 123456792.0f;
#pragma const_seg(".rdata$TerrainSkinBoundsMinPc")
    extern const f32 TerrainSkinBoundsMinPc = -123456792.0f;
#pragma const_seg()
}

struct TERRAIN_SKIN_INFO_VIEW_PC;
struct TERRAIN_SKIN_USE_RECORD_PC;

extern "C" {
// Accessed terrain state and literals; full original allocation extents remain unknown.
#pragma bss_seg(".bss$TerI")
    void *TerI;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainPlatformsOffPc")
    u16 TerrainPlatformsOffPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainScaledShapeCursorPc")
    TERRAIN_POLYGON_INFO_PC *TerrainScaledShapeCursorPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainShadowFloorIndexPc")
    i16 TerrainShadowFloorIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxPolygonPc")
    TERRAIN_POLYGON_INFO_PC *TerrainCastAuxPolygonPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainShadowHeightPc")
    f32 TerrainShadowHeightPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxValidPc")
    u16 TerrainCastAuxValidPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxIndexPc")
    i16 TerrainCastAuxIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxNormalPc")
    NUVEC TerrainCastAuxNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainWaterCastIndexPc")
    u16 TerrainWaterCastIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainRoofValidPc")
    u16 TerrainRoofValidPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainRoofIndexPc")
    i16 TerrainRoofIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainRoofNormalPc")
    NUVEC TerrainRoofNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSurfaceClassPc")
    u16 TerrainSurfaceClassPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinInfoCountPc")
    i32 TerrainSkinInfoCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinInfosPc")
    struct TERRAIN_SKIN_INFO_VIEW_PC *TerrainSkinInfosPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinUseEpochPc")
    u32 TerrainSkinUseEpochPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinUseRecordsPc")
    struct TERRAIN_SKIN_USE_RECORD_PC *TerrainSkinUseRecordsPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinPoolSlotCountPc")
    i32 TerrainSkinPoolSlotCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinPoolSlotBytesPc")
    i32 TerrainSkinPoolSlotBytesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinPoolBasePc")
    u8 *TerrainSkinPoolBasePc;
#pragma bss_seg()
#pragma const_seg(".rdata$TerrainScanRadiusEpsilon")
    extern const f64 TerrainScanRadiusEpsilon = 9.999999747378752e-06;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainScanTriangleSentinel")
    extern const f32 TerrainScanTriangleSentinel = 65536.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainCastLowerSentinel")
    extern const f32 TerrainCastLowerSentinel = -2000000.0f;
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$TerrainShadowStatusPc")
    i32 TerrainShadowStatusPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainScaleWorkBasePc")
    TERRAIN_POLYGON_INFO_PC *TerrainScaleWorkBasePc;
#pragma bss_seg()
#pragma data_seg(".data$LAYER_HOVERIGNORE")
    u32 LAYER_HOVERIGNORE = 0xffffffffu;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$ActionSnapPlayer1Name")
    extern const char ActionSnapPlayer1Name[8] = "player1";
#pragma const_seg(".rdata$ActionSnapPlayer2Name")
    extern const char ActionSnapPlayer2Name[8] = "player2";
#pragma const_seg(".rdata$ActionSnapRotationYName")
    extern const char ActionSnapRotationYName[5] = "roty";
#pragma const_seg(".rdata$ActionSnapXName")
    extern const char ActionSnapXName[2] = "x";
#pragma const_seg(".rdata$ActionSnapZName")
    extern const char ActionSnapZName[2] = "z";
#pragma const_seg(".rdata$ActionSnapDontCheckTerrainName")
    extern const char ActionSnapDontCheckTerrainName[19] = "dont_check_terrain";
#pragma const_seg(".rdata$ActionSnapPartyName")
    extern const char ActionSnapPartyName[6] = "party";
#pragma const_seg(".rdata$ActionSnapDistanceFromEndName")
    extern const char ActionSnapDistanceFromEndName[18] = "distance_from_end";
#pragma const_seg(".rdata$ActionSnapSockName")
    extern const char ActionSnapSockName[5] = "sock";
#pragma const_seg(".rdata$ActionSnapDxName")
    extern const char ActionSnapDxName[3] = "dx";
#pragma const_seg(".rdata$ActionSnapDyName")
    extern const char ActionSnapDyName[3] = "dy";
#pragma const_seg(".rdata$ActionSnapRotationFactorPc")
    extern const f64 ActionSnapRotationFactorPc = 182.04444885253906;
#pragma const_seg(".rdata$ActionSnapWeightedRandomZeroPc")
    extern const f64 ActionSnapWeightedRandomZeroPc = 0.0;
#pragma const_seg()
}

extern "C" {

#pragma bss_seg(".bss$NuCameraMode")
    i32 NuCameraMode;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraReflection")
    NUMTX NuCameraReflection;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraClipScissorToViewport")
    i32 NuCameraClipScissorToViewport;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraPreviousClipScissorToViewport")
    i32 NuCameraPreviousClipScissorToViewport;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraOrthographic")
    i32 NuCameraOrthographic;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraScalingTemplate")
    NUMTX NuCameraScalingTemplate;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalScaleX")
    f32 NuCameraProjectionGlobalScaleX;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalScaleY")
    f32 NuCameraProjectionGlobalScaleY;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalOffsetX")
    f32 NuCameraProjectionGlobalOffsetX;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalOffsetY")
    f32 NuCameraProjectionGlobalOffsetY;
#pragma bss_seg()

#pragma data_seg(".data$psmtx")
    NUMTX psmtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
#pragma data_seg()

#pragma data_seg(".data$NuCameraOrthoScale")
    f32 NuCameraOrthoScale = 0.1f;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuVpDirtyPc")
    i32 NuVpDirtyPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpMinZPc")
    f32 NuVpMinZPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpMaxZPc")
    f32 NuVpMaxZPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMinXPc")
    f32 NuVpProjectionMinXPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMinYPc")
    f32 NuVpProjectionMinYPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMaxXPc")
    f32 NuVpProjectionMaxXPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMaxYPc")
    f32 NuVpProjectionMaxYPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceLastResult")
    HRESULT PCDeviceLastResult;
#pragma bss_seg()
#pragma data_seg(".data$NuVpIdentityMatrixPc")
    NUMTX NuVpIdentityMatrixPc = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
#pragma data_seg()
#pragma const_seg(".rdata$NuVpFixed16ScalePc")
    extern const f64 NuVpFixed16ScalePc = 1.0 / 65536.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuVpDepthRangePc")
    extern const f64 NuVpDepthRangePc = 16777215.0;
#pragma const_seg()
}

extern "C" {
#pragma const_seg(".rdata$WorldPathSeparator")
    extern const char WorldPathSeparator[2] = "\\";
#pragma const_seg()
#pragma const_seg(".rdata$WorldObjectAnimFormat")
    extern const char WorldObjectAnimFormat[7] = "%s.anm";
#pragma const_seg()
#pragma const_seg(".rdata$WorldLevelConfigFormat")
    extern const char WorldLevelConfigFormat[7] = "%s.txt";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileSourcePathPC")
    extern const char NuFileSourcePathPC[50] = "d:\\projects\\game_saga\\nu2api.saga\\nucore\\nufile.c";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757834")
    extern const char AITransferLiteral757834[13] = "jump_factor=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757874")
    extern const char AITransferLiteral757874[15] = "tgt_character=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757844")
    extern const char AITransferLiteral757844[14] = "check_terrain";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75786c")
    extern const char AITransferLiteral75786c[5] = "slam";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757860")
    extern const char AITransferLiteral757860[10] = "to_origin";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757bb4")
    extern const char AITransferLiteral757bb4[13] = "speed=TIPTOE";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ba8")
    extern const char AITransferLiteral757ba8[11] = "speed=WALK";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757b9c")
    extern const char AITransferLiteral757b9c[10] = "speed=RUN";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ec4")
    extern const char AITransferLiteral757ec4[10] = "keep_out=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ebc")
    extern const char AITransferLiteral757ebc[5] = "TRUE";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a5c")
    extern const char AITransferLiteral757a5c[12] = "dont_avoid=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ee0")
    extern const char AITransferLiteral757ee0[8] = "tag_to=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a7c")
    extern const char AITransferLiteral757a7c[8] = "looping";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a84")
    extern const char AITransferLiteral757a84[8] = "spline=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757cd8")
    extern const char AITransferLiteral757cd8[5] = "mul=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757cd0")
    extern const char AITransferLiteral757cd0[6] = "time=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757cc8")
    extern const char AITransferLiteral757cc8[7] = "speed=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757e68")
    extern const char AITransferLiteral757e68[5] = "path";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757e70")
    extern const char AITransferLiteral757e70[15] = "path=LevelPath";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75772c")
    extern const char AITransferLiteral75772c[5] = "type";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a44")
    extern const char AITransferLiteral757a44[23] = "still_do_take_hit_anim";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCSoundMixSource")
    void *PCSoundMixSource;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundMixSampleCount")
    i32 PCSoundMixSampleCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundMixDestination")
    void *PCSoundMixDestination;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundUpdateClock")
    i32 PCSoundUpdateClock;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundLoopVoiceCount")
    i32 PCSoundLoopVoiceCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundOneShotVoiceCount")
    i32 PCSoundOneShotVoiceCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundMixEvent")
    HANDLE PCSoundMixEvent;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundCommitTarget")
    PC_SOUND_COMMIT_PC *PCSoundCommitTarget;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral7576b8")
    extern const char AITransferLiteral7576b8[7] = "baddie";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576a8")
    extern const char AITransferLiteral7576a8[7] = "goodie";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576b0")
    extern const char AITransferLiteral7576b0[6] = "goody";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757724")
    extern const char AITransferLiteral757724[7] = "range=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75768c")
    extern const char AITransferLiteral75768c[13] = "goodiebaddie";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75769c")
    extern const char AITransferLiteral75769c[11] = "goodybaddy";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75773c")
    extern const char AITransferLiteral75773c[8] = "neutral";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576c0")
    extern const char AITransferLiteral7576c0[6] = "baddy";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576e0")
    extern const char AITransferLiteral7576e0[10] = "maxrange=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576f8")
    extern const char AITransferLiteral7576f8[6] = "type=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576d0")
    extern const char AITransferLiteral7576d0[14] = "ifCanTakeover";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757744")
    extern const char AITransferLiteral757744[7] = "State=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757eb4")
    extern const char AITransferLiteral757eb4[8] = "Restart";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ea8")
    extern const char AITransferLiteral757ea8[10] = "backwards";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583d0")
    extern const char AITransferLiteral7583d0[18] = "attacker=opponent";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583c4")
    extern const char AITransferLiteral7583c4[10] = "attacker=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583b4")
    extern const char AITransferLiteral7583b4[16] = "attacker=player";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a20")
    extern const char AITransferLiteral757a20[8] = "victim=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757f7c")
    extern const char AITransferLiteral757f7c[7] = "myself";
#pragma const_seg()
#pragma data_seg(".data$gizspecial_gizmotype_id")
    i32 gizspecial_gizmotype_id = -1;
#pragma data_seg()
#pragma bss_seg(".bss$APIObjPlaySfxByIdFn")
    void(__cdecl *APIObjPlaySfxByIdFn)(i32, NUVEC *);
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$WorldGrassFormat")
    extern const char WorldGrassFormat[7] = "%s.gra";
#pragma const_seg()
#pragma const_seg(".rdata$WorldBridgeFormat")
    extern const char WorldBridgeFormat[7] = "%s.bri";
#pragma const_seg()
#pragma bss_seg(".bss$Grass_Available")
    i32 Grass_Available;
#pragma bss_seg()
#pragma bss_seg(".bss$APIObjectZeroMomentumPC")
    NUVEC APIObjectZeroMomentumPC;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCDeviceStateCacheEnabled")
    u8 PCDeviceStateCacheEnabled = 1;
#pragma data_seg()
#pragma bss_seg(".bss$PCDeviceRenderStateCache")
    i32 PCDeviceRenderStateCache[12];
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceCacheStateKey")
    i32 PCDeviceCacheStateKey;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceTextureCache")
    void *PCDeviceTextureCache[5];
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceSamplerStateCache")
    i32 PCDeviceSamplerStateCache[8][14];
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceSamplerInvalidation")
    i32 PCDeviceSamplerInvalidation[8][2];
#pragma bss_seg()
#pragma data_seg(".data$nufile_lsn_allowed")
    i32 nufile_lsn_allowed = 1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$BackupGame")
    GAMESAVE BackupGame;
#pragma bss_seg()
#pragma bss_seg(".bss$areaSuitBits")
    u32 areaSuitBits;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$SceneFileExtensionPc")
    extern const char SceneFileExtensionPc[5] = ".gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_dds_PC")
    extern const char NuFileExt_dds_PC[5] = ".dds";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_at3_PC")
    extern const char NuFileExt_at3_PC[5] = ".at3";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_psc_PC")
    extern const char NuFileExt_psc_PC[5] = ".psc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_phg_PC")
    extern const char NuFileExt_phg_PC[5] = ".phg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_nx2_PC")
    extern const char NuFileExt_nx2_PC[5] = ".nx2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_hx2_PC")
    extern const char NuFileExt_hx2_PC[5] = ".hx2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_h4m_PC")
    extern const char NuFileExt_h4m_PC[5] = ".h4m";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_gcm_PC")
    extern const char NuFileExt_gcm_PC[5] = ".gcm";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_dsp_PC")
    extern const char NuFileExt_dsp_PC[5] = ".dsp";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ctx_PC")
    extern const char NuFileExt_ctx_PC[5] = ".ctx";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_csc_PC")
    extern const char NuFileExt_csc_PC[5] = ".csc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_chg_PC")
    extern const char NuFileExt_chg_PC[5] = ".chg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pss_PC")
    extern const char NuFileExt_pss_PC[5] = ".pss";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_vag_PC")
    extern const char NuFileExt_vag_PC[5] = ".vag";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pnt_PC")
    extern const char NuFileExt_pnt_PC[5] = ".pnt";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ghg_PC")
    extern const char NuFileExt_ghg_PC[5] = ".ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pc_ghg_PC")
    extern const char NuFileExt_pc_ghg_PC[8] = "_pc.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_lr_pc_ghg_PC")
    extern const char NuFileExt_lr_pc_ghg_PC[11] = "_lr_pc.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pc_gsc_PC")
    extern const char NuFileExt_pc_gsc_PC[8] = "_pc.gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_bik_PC")
    extern const char NuFileExt_bik_PC[5] = ".bik";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wav_PC")
    extern const char NuFileExt_wav_PC[5] = ".wav";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_cu2_PC")
    extern const char NuFileExt_cu2_PC[5] = ".cu2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pc_dds_PC")
    extern const char NuFileExt_pc_dds_PC[8] = "_pc.dds";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_bsa_PC")
    extern const char NuFileExt_bsa_PC[5] = ".bsa";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_an3_PC")
    extern const char NuFileExt_an3_PC[5] = ".an3";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pam_PC")
    extern const char NuFileExt_pam_PC[5] = ".pam";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_mib_PC")
    extern const char NuFileExt_mib_PC[5] = ".mib";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_msf_PC")
    extern const char NuFileExt_msf_PC[5] = ".msf";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ps3_tex_PC")
    extern const char NuFileExt_ps3_tex_PC[9] = "_ps3.tex";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ps3_gsc_PC")
    extern const char NuFileExt_ps3_gsc_PC[9] = "_ps3.gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ps3_ghg_PC")
    extern const char NuFileExt_ps3_ghg_PC[9] = "_ps3.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wmv_PC")
    extern const char NuFileExt_wmv_PC[5] = ".wmv";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wavm_PC")
    extern const char NuFileExt_wavm_PC[6] = ".wavm";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wavx_PC")
    extern const char NuFileExt_wavx_PC[6] = ".wavx";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_cc2_PC")
    extern const char NuFileExt_cc2_PC[5] = ".cc2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_360_tex_PC")
    extern const char NuFileExt_360_tex_PC[9] = "_360.tex";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_360_gsc_PC")
    extern const char NuFileExt_360_gsc_PC[9] = "_360.gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_360_ghg_PC")
    extern const char NuFileExt_360_ghg_PC[9] = "_360.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_cbs_PC")
    extern const char NuFileExt_cbs_PC[5] = ".cbs";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ca3_PC")
    extern const char NuFileExt_ca3_PC[5] = ".ca3";
#pragma const_seg()
#pragma bss_seg(".bss$extensions")
    PC_FILEEXTINFO extensions[64];
#pragma bss_seg()
#pragma bss_seg(".bss$num_extensions")
    i32 num_extensions;
#pragma bss_seg()
#pragma bss_seg(".bss$Game_Customiser")
    void *Game_Customiser;
#pragma bss_seg()
#pragma bss_seg(".bss$CUTCAM")
    void *CUTCAM;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$APIObjectCharsDirectory")
    extern const char APIObjectCharsDirectory[7] = "chars\\";
#pragma const_seg()
#pragma const_seg(".rdata$APIObjectPakExtension")
    extern const char APIObjectPakExtension[5] = ".fpk";
#pragma const_seg()
#pragma bss_seg(".bss$append")
    i32 append;
#pragma bss_seg()
#pragma bss_seg(".bss$nopakfile")
    i32 nopakfile;
#pragma bss_seg()
#pragma bss_seg(".bss$RedirectAnimFn")
    i32(__cdecl *RedirectAnimFn)(char *, void *, CHARACTERANIM_CONFIG_PC *, char *);
#pragma bss_seg()
#pragma bss_seg(".bss$RedirectAnimList")
    void *RedirectAnimList;
#pragma bss_seg()
#pragma data_seg(".data$RedirectAnimDir")
    char RedirectAnimDir[64] = "chars\\commonanims\\";
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$turret_gizmotype_id")
    i32 turret_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$blowup_gizmotype_id")
    i32 blowup_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$obstacle_gizmotype_id")
    i32 obstacle_gizmotype_id = -1;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral758430")
    extern const char AITransferLiteral758430[5] = "True";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$LSW2")
    i32 LSW2;
#pragma bss_seg()
#pragma data_seg(".data$HIGHJUMPHEIGHT")
    f32 HIGHJUMPHEIGHT = 1.14f;
#pragma data_seg()
#pragma bss_seg(".bss$AreaTimer")
    TIMER AreaTimer;
#pragma bss_seg()
#pragma bss_seg(".bss$OldBonusScore")
    i32 OldBonusScore[2];
#pragma bss_seg()
#pragma bss_seg(".bss$BonusScore")
    i32 BonusScore[2];
#pragma bss_seg()
#pragma bss_seg(".bss$BombGenerator_PlayerBomb")
    GAMEOBJECT *BombGenerator_PlayerBomb[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Lap")
    i32 Lap;
#pragma bss_seg()
#pragma bss_seg(".bss$BuildUpTotal")
    i32 BuildUpTotal;
#pragma bss_seg()
#pragma bss_seg(".bss$BuildUpDone")
    i32 BuildUpDone;
#pragma bss_seg()
#pragma bss_seg(".bss$AreaGlobals_PC")
    AREA_GLOBALS_PC AreaGlobals_PC;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuLgtLaserCnt")
    i32 NuLgtLaserCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtLaserOldCnt")
    i32 NuLgtLaserOldCnt;
#pragma bss_seg()
#pragma data_seg(".data$NuLgtSeed")
    u32 NuLgtSeed = 12345;
#pragma data_seg()
#pragma bss_seg(".bss$NuLgtArcMtl")
    NULGTARCMATERIAL_PC NuLgtArcMtl[4];
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtLaserData")
    NULGTLASER_PC NuLgtLaserData[64];
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserCnt")
    i32 NuLgtArcLaserCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserOldCnt")
    i32 NuLgtArcLaserOldCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserFrame")
    i32 NuLgtArcLaserFrame;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserData")
    NULGTARCLASER_PC NuLgtArcLaserData[16];
#pragma bss_seg()
#pragma bss_seg(".bss$PS2_REZ_W")
    i32 PS2_REZ_W;
#pragma bss_seg()
#pragma bss_seg(".bss$PS2_REZ_H")
    i32 PS2_REZ_H;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral75782c")
    extern const char AITransferLiteral75782c[5] = "name";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583e4")
    extern const char AITransferLiteral7583e4[12] = "controller=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583f0")
    extern const char AITransferLiteral7583f0[8] = "turret=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a38")
    extern const char AITransferLiteral757a38[12] = "hold_button";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a70")
    extern const char AITransferLiteral757a70[11] = "rotational";
#pragma const_seg()
#pragma bss_seg(".bss$BeenAttacked")
    i32 BeenAttacked;
#pragma bss_seg()
#pragma bss_seg(".bss$takeoverobjects")
    u8 takeoverobjects[352];
#pragma bss_seg()
#pragma bss_seg(".bss$num_takeoverobjects")
    i32 num_takeoverobjects;
#pragma bss_seg()
#pragma bss_seg(".bss$DoorExitNamePc")
    char DoorExitNamePc[64];
#pragma bss_seg()
#pragma bss_seg(".bss$Door_Start")
    i32 Door_Start;
#pragma bss_seg()
#pragma data_seg(".data$Door_NextSock")
    i32 Door_NextSock = -1;
#pragma data_seg()
#pragma bss_seg(".bss$minikitCounter_C")
    u8 minikitCounter_C;
#pragma bss_seg()
#pragma bss_seg(".bss$minikitCounter_A")
    u8 minikitCounter_A;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$GizmoSnapOptionNamePc")
    extern const char GizmoSnapOptionNamePc[5] = "SNAP";
#pragma const_seg()
#pragma bss_seg(".bss$debkeydata")
    void *debkeydata;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$Door_Last")
    void *Door_Last;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuPrimCoordSystemStack")
    i32 NuPrimCoordSystemStack[2];
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral7577dc")
    extern const char AITransferLiteral7577dc[8] = "random=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757824")
    extern const char AITransferLiteral757824[7] = "random";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75781c")
    extern const char AITransferLiteral75781c[8] = "player=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757884")
    extern const char AITransferLiteral757884[12] = "take_damage";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a68")
    extern const char AITransferLiteral757a68[6] = "Reset";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c10")
    extern const char AITransferLiteral757c10[13] = "dont_set_max";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c20")
    extern const char AITransferLiteral757c20[12] = "messageval=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral758194")
    extern const char AITransferLiteral758194[11] = "type=CHOKE";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral758188")
    extern const char AITransferLiteral758188[11] = "type=DROID";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581a0")
    extern const char AITransferLiteral7581a0[8] = "radius=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral758174")
    extern const char AITransferLiteral758174[19] = "type=ComboOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581d4")
    extern const char AITransferLiteral7581d4[10] = "rot_rate=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581ec")
    extern const char AITransferLiteral7581ec[9] = "mintime=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581e0")
    extern const char AITransferLiteral7581e0[9] = "maxtime=";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionNamePC")
    extern const char GizObstacleActionNamePC[5] = "Name";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionBackwardPC")
    extern const char GizObstacleActionBackwardPC[9] = "BACKWARD";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionForwardPC")
    extern const char GizObstacleActionForwardPC[8] = "FORWARD";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionStayOpenPC")
    extern const char GizObstacleActionStayOpenPC[9] = "STAYOPEN";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionStayShutPC")
    extern const char GizObstacleActionStayShutPC[9] = "STAYSHUT";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassInvalid")
    extern const char NuMusicDebugClassInvalid[8] = "INVALID";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassQuiet")
    extern const char NuMusicDebugClassQuiet[6] = "QUIET";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassAction")
    extern const char NuMusicDebugClassAction[7] = "ACTION";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassSignature")
    extern const char NuMusicDebugClassSignature[10] = "SIGNATURE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassOverlay")
    extern const char NuMusicDebugClassOverlay[8] = "OVERLAY";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassCutscene")
    extern const char NuMusicDebugClassCutscene[9] = "CUTSCENE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassNoMusic")
    extern const char NuMusicDebugClassNoMusic[8] = "NOMUSIC";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugVoiceFormat")
    extern const char NuMusicDebugVoiceFormat[9] = "VOICE:%d";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugTrackFormat")
    extern const char NuMusicDebugTrackFormat[9] = "TRACK:%s";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassFormat")
    extern const char NuMusicDebugClassFormat[9] = "CLASS:%s";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugTrackNone")
    extern const char NuMusicDebugTrackNone[11] = "TRACK:NONE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassNone")
    extern const char NuMusicDebugClassNone[11] = "CLASS:NONE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatusFormat")
    extern const char NuMusicDebugStatusFormat[10] = "STATUS:%s";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugVolumeFormat")
    extern const char NuMusicDebugVolumeFormat[15] = "VOLUME:%f (%f)";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugFadeFormat")
    extern const char NuMusicDebugFadeFormat[8] = "FADE:%f";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugPositionFormat")
    extern const char NuMusicDebugPositionFormat[7] = "POS:%f";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus0")
    extern const char NuMusicDebugStatus0[13] = "IDLE_PENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus1")
    extern const char NuMusicDebugStatus1[5] = "IDLE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus2")
    extern const char NuMusicDebugStatus2[15] = "PAUSED_PENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus3")
    extern const char NuMusicDebugStatus3[7] = "PAUSED";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus4")
    extern const char NuMusicDebugStatus4[16] = "SEEKING_PENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus5")
    extern const char NuMusicDebugStatus5[8] = "SEEKING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus6")
    extern const char NuMusicDebugStatus6[12] = "PLAYPENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus7")
    extern const char NuMusicDebugStatus7[8] = "PLAYING";
#pragma const_seg()
#pragma data_seg(".data$NuMusicVoiceStatusText")
    const char *NuMusicVoiceStatusText[8] = {NuMusicDebugStatus0, NuMusicDebugStatus1, NuMusicDebugStatus2,
                                             NuMusicDebugStatus3, NuMusicDebugStatus4, NuMusicDebugStatus5,
                                             NuMusicDebugStatus6, NuMusicDebugStatus7};
#pragma data_seg()
#pragma data_seg(".data$NuMusicDebugColour")
    u32 NuMusicDebugColour = 0x80ffffff;
#pragma data_seg()
#pragma bss_seg(".bss$bonus_gunship_store_progress_flag")
    i32 bonus_gunship_store_progress_flag;
#pragma bss_seg()

#pragma data_seg(".data$LEDGETERRAINLOOKAHEAD")
    f32 LEDGETERRAINLOOKAHEAD = 0.02f;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_HANG_MOVE")
    i16 LEGOACT_HANG_MOVE = -1;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral757a94")
    extern const char AITransferLiteral757a94[9] = "splines=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a8c")
    extern const char AITransferLiteral757a8c[7] = "unused";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c64")
    extern const char AITransferLiteral757c64[4] = "dx=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c60")
    extern const char AITransferLiteral757c60[4] = "dy=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c5c")
    extern const char AITransferLiteral757c5c[4] = "dz=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757f28")
    extern const char AITransferLiteral757f28[8] = "target=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583f8")
    extern const char AITransferLiteral7583f8[8] = "release";
#pragma const_seg()
#pragma bss_seg(".bss$part_types")
    PARTTYPE_LOOKUP_PC part_types[128];
#pragma bss_seg()
#pragma bss_seg(".bss$script_spline_selected")
    NUGSPLINE *script_spline_selected;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$edBridges")
    PC_EDBRIDGE edBridges[64];
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_page_used")
    i32 edbri_page_used[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_page_scene")
    NUGSCN *edbri_page_scene[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_bridges_used")
    i32 edbri_bridges_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_nearest")
    i32 edbri_nearest;
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_cam_pos")
    NUVEC edbri_cam_pos;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_used")
    i32 edgra_page_used[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_scene")
    NUGSCN *edgra_page_scene[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_terrain")
    void *edgra_page_terrain[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_matrix_stack")
    NUMTX *edgra_page_matrix_stack[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_vectors_valid")
    i32 edgra_page_vectors_valid[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_nearest")
    i32 edgra_nearest;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_cam_pos")
    NUVEC edgra_cam_pos;
#pragma bss_seg()
#pragma bss_seg(".bss$edbits_editmode")
    i32 edbits_editmode;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_mtxbuffer")
    NUMTX *edgra_mtxbuffer;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_vecbuffer")
    NUVEC *edgra_vecbuffer;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_free_vecbuffer")
    void *edgra_free_vecbuffer;
#pragma bss_seg()
#pragma bss_seg(".bss$EDGRA_MAX_CLUMPS")
    i32 EDGRA_MAX_CLUMPS;
#pragma bss_seg()
#pragma bss_seg(".bss$EDGRA_MAX_INDIVIDUAL_CLUMPS")
    i32 EDGRA_MAX_INDIVIDUAL_CLUMPS;
#pragma bss_seg()
#pragma bss_seg(".bss$EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP")
    i32 EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP;
#pragma bss_seg()
#pragma bss_seg(".bss$GrassClumps")
    PC_EDGRACLUMP *GrassClumps;
#pragma bss_seg()
#pragma bss_seg(".bss$IndGrassClumps")
    PC_EDGRAINDIVIDUAL *IndGrassClumps;
#pragma bss_seg()
#pragma bss_seg(".bss$IndGrassClumpsUsed")
    i32 *IndGrassClumpsUsed;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_ind_clumps_used")
    i32 edgra_ind_clumps_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_clumps_used")
    i32 edgra_clumps_used;
#pragma bss_seg()
#pragma data_seg(".data$edgra_last_clump_in_buffer")
    i32 edgra_last_clump_in_buffer = -1;
#pragma data_seg()
#pragma data_seg(".data$edgra_global_fadein")
    f32 edgra_global_fadein = 15.0f;
#pragma data_seg()
#pragma data_seg(".data$edgra_global_fadeout")
    f32 edgra_global_fadeout = 25.0f;
#pragma data_seg()
#pragma data_seg(".data$edgra_clumpthin")
    i32 edgra_clumpthin = 1;
#pragma data_seg()
#pragma data_seg(".data$edgra_elementthin")
    i32 edgra_elementthin = 1;
#pragma data_seg()
#pragma bss_seg(".bss$edgra_page_on")
    i32 edgra_page_on[8];
#pragma bss_seg()
#pragma bss_seg(".bss$ShadNorm")
    NUVEC ShadNorm;
#pragma bss_seg()
#pragma bss_seg(".bss$rtl_dynamic_pool")
    NULSTHDR *rtl_dynamic_pool;
#pragma bss_seg()
#pragma bss_seg(".bss$rtl_dynamic_max")
    i32 rtl_dynamic_max;
#pragma bss_seg()
#pragma bss_seg(".bss$rtl_dynamic_cnt")
    i32 rtl_dynamic_cnt;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_suspended")
    i32 debris_suspended;
#pragma bss_seg()
#pragma bss_seg(".bss$globaltime")
    f32 globaltime;
#pragma bss_seg()
#pragma bss_seg(".bss$panelglobaltime")
    f32 panelglobaltime;
#pragma bss_seg()
#pragma data_seg(".data$update_debris_enabled")
    i32 update_debris_enabled = 1;
#pragma data_seg()
#pragma bss_seg(".bss$debris_keystack")
    DEBRISKEY_PC *debris_keystack;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebkeyptr")
    i32 freedebkeyptr;
#pragma bss_seg()
#pragma bss_seg(".bss$maxdebkeys")
    i32 maxdebkeys;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebkeys")
    i16 *freedebkeys;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuFadeObjGroup")
    PC_NUFADEGROUP *NuFadeObjGroup;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeMaxGroupsPC")
    i32 NuFadeMaxGroupsPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeMaxDataPC")
    i32 NuFadeMaxDataPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeData")
    i16 *NuFadeData;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjDataIndex")
    i32 NuFadeObjDataIndex;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjWave")
    i32 NuFadeObjWave;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjDir")
    i32 NuFadeObjDir;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjDir2")
    i32 NuFadeObjDir2;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindGroup")
    PC_NUWINDGROUP *NuWindGroup;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindMaxGroupsPC")
    i32 NuWindMaxGroupsPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindWave")
    i32 NuWindWave;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindDir")
    i32 NuWindDir;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindDir2")
    i32 NuWindDir2;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_units_used")
    i32 edgra_units_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_calculate_done")
    i32 edgra_page_calculate_done[8];
#pragma bss_seg()
#pragma data_seg(".data$debrischunks")
    i32 debrischunks = 256;
#pragma data_seg()
#pragma data_seg(".data$debrischunksglass")
    i32 debrischunksglass = 32;
#pragma data_seg()
#pragma bss_seg(".bss$freechunkcontrols")
    DEBRIS_CHUNK_CONTROL_PC **freechunkcontrols;
#pragma bss_seg()
#pragma bss_seg(".bss$freechunkcontrolsptr")
    i32 freechunkcontrolsptr;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_control_stack_lock")
    volatile i32 debris_control_stack_lock;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_chunk_control_stack")
    DEBRIS_CHUNK_CONTROL_PC *debris_chunk_control_stack[2];
#pragma bss_seg()
#pragma bss_seg(".bss$CLOUDCITYTRAPB_LDATA")
    LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MAULA_LDATA")
    LEVELDATA_PC *MAULA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MAULF_LDATA")
    LEVELDATA_PC *MAULF_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DOOKUC_LDATA")
    LEVELDATA_PC *DOOKUC_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$freeDmaDebType")
    i32 freeDmaDebType;
#pragma bss_seg()
#pragma data_seg(".data$EDPP_MAX_DMADEBTYPES")
    i32 EDPP_MAX_DMADEBTYPES = 64;
#pragma data_seg()
#pragma bss_seg(".bss$DmaDebTypes")
    DEBRIS_PART_HEADER_PC **DmaDebTypes;
#pragma bss_seg()
#pragma const_seg(".rdata$ForceThrowExplodeNamePc")
    extern const char ForceThrowExplodeNamePc[9] = "Explode1";
#pragma const_seg()
#pragma data_seg(".data$gensorttab")
    DEBRISGENERATOR_PC gensorttab[13] = {
        GenDebIndex,          GenDebDummy,        GenDebDummy,         GenDebDummy,
        GenDebDummy,          GenDebDummy,        GenDebIndexRadial,   GenDebIndexRadialRotor,
        GenDebIndexSpheroid,  GenDebIndexBounceY, GenDebIndexBounceXZ, GenDebIndexImprovedRadial,
        GenDebIndexRadialStar};
#pragma data_seg()
#pragma data_seg(".data$gencodetab")
    DEBRISMOMENTUMADJUSTER_PC gencodetab[7] = {NULL,
                                               GenDebMomAdjFromPos,
                                               GenDebMomAdjFromPosRev,
                                               GenDebMomAdjFromSplash,
                                               GenDebMomAdjFromAshRock,
                                               GenDebMomAdjFromPosRevTree,
                                               GenDebMomAdjFromPosAll};
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuTextureCapacityPC")
    i32 NuTextureCapacityPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTextureLoadOrderPC")
    i32 *NuTextureLoadOrderPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTextureLoadCountPC")
    i32 NuTextureLoadCountPC;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AIPositionUnsetPc")
    extern const f32 AIPositionUnsetPc = 1000000000.0f;
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$g_overrideList")
    NULSTHDR *g_overrideList;
#pragma bss_seg()
#pragma bss_seg(".bss$Char_MiniKit")
    i32 *Char_MiniKit;
#pragma bss_seg()
#pragma bss_seg(".bss$minikits_savecharacterbufferptr")
    VARIPTR minikits_savecharacterbufferptr;
#pragma bss_seg()
#pragma bss_seg(".bss$forced_debris_thinning")
    i32 forced_debris_thinning;
#pragma bss_seg()
#pragma data_seg(".data$debris_thinning_level")
    f32 debris_thinning_level = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$partglobaltime")
    f32 partglobaltime;
#pragma bss_seg()
#pragma bss_seg(".bss$part_emits_used")
    i32 part_emits_used;
#pragma bss_seg()
#pragma data_seg(".data$SceneUploadBufferSizePC")
    i32 SceneUploadBufferSizePC = 41943040;
#pragma data_seg()
#pragma bss_seg(".bss$SceneUploadBufferStartPC")
    VARIPTR SceneUploadBufferStartPC;
#pragma bss_seg()
#pragma bss_seg(".bss$SceneUploadBufferCursorPC")
    VARIPTR SceneUploadBufferCursorPC;
#pragma bss_seg()
#pragma bss_seg(".bss$SceneUploadBufferEndPC")
    VARIPTR SceneUploadBufferEndPC;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceHardwareVertexProcessingPC")
    i32 PCDeviceHardwareVertexProcessingPC;
#pragma bss_seg()
#pragma bss_seg(".bss$PCFrameLoadingYieldEnabledPC")
    u8 PCFrameLoadingYieldEnabledPC;
#pragma bss_seg()
#pragma const_seg(".rdata$SceneUploadSourcePathPC")
    extern const char SceneUploadSourcePathPC[57] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugscn_dlist.c";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$AxisClipFrustumPlanesPc")
    NUMTX AxisClipFrustumPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipScissorPlanesPc")
    NUMTX AxisClipScissorPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipNearFarPlanesPc")
    NUMTX AxisClipNearFarPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipAbsFrustumPlanesPc")
    NUMTX AxisClipAbsFrustumPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipAbsScissorPlanesPc")
    NUMTX AxisClipAbsScissorPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchunksglass")
    DEBRIS_PARTICLE_CHUNK_PC **freedebchunksglass;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchunks")
    DEBRIS_PARTICLE_CHUNK_PC **freedebchunks;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchkptrg")
    i32 freedebchkptrg;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_chunk_controls")
    DEBRIS_CHUNK_CONTROL_PC *debris_chunk_controls;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchkptr")
    i32 freedebchkptr;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_trash_space")
    u32 debris_trash_space;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_trash_size")
    u32 debris_trash_size;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_setup_called")
    i32 debris_setup_called;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$SceneVideoResourcesPC")
    SCENE_VIDEO_RESOURCE_HEADER_PC SceneVideoResourcesPC;
#pragma bss_seg()
#pragma bss_seg(".bss$ParticleChunkToRender")
    DEBRIS_RENDER_CHUNK_PC *ParticleChunkToRender;
#pragma bss_seg()
#pragma bss_seg(".bss$ParticleChunkRenderStack")
    DEBRIS_RENDER_CHUNK_PC *ParticleChunkRenderStack[5];
#pragma bss_seg()
#pragma data_seg(".data$g_texAnimCriticalSection")
    i32 g_texAnimCriticalSection = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ntal_first")
    PC_TEXTURE_ANIM_LIST *ntal_first;
#pragma bss_seg()
#pragma bss_seg(".bss$ntal_free")
    PC_TEXTURE_ANIM_LIST *ntal_free;
#pragma bss_seg()
#pragma bss_seg(".bss$PCShaderSlots")
    PC_SHADER_SLOT_VIEW *PCShaderSlots;
#pragma bss_seg()
#pragma bss_seg(".bss$PCRegisteredNativeScenes")
    NUGSCN *PCRegisteredNativeScenes[100];
#pragma bss_seg()
#pragma data_seg(".data$NuUnresolvedTextureIDPC")
    i32 NuUnresolvedTextureIDPC = -1;
#pragma data_seg()
#pragma const_seg(".rdata$NuTextureSourcePathPC")
    extern const char NuTextureSourcePathPC[55] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp";
#pragma const_seg()
#pragma const_seg(".rdata$NuTextureAnimationSourcePathPC")
    extern const char NuTextureAnimationSourcePathPC[50] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c";
#pragma const_seg()
#pragma bss_seg(".bss$PCDisplayMaterialDeleteQueue")
    NUMTL **PCDisplayMaterialDeleteQueue;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialDeleteCount")
    i32 PCDisplayMaterialDeleteCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayScenes")
    NUDISPLAY_LIST_PC *PCDisplayScenes[256];
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindMaxMatsPC")
    i32 NuWindMaxMatsPC;
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_page_on")
    i32 edbri_page_on[8];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCDynamicDisplayScene")
    NUDISPLAY_LIST_PC PCDynamicDisplayScene;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$splineStore")
    NUGSPLINE *splineStore;
#pragma bss_seg()
#pragma bss_seg(".bss$numSplinesLoaded")
    i32 numSplinesLoaded;
#pragma bss_seg()
#pragma const_seg(".rdata$EditorSplineFileCheckTextPc")
    extern const char EditorSplineFileCheckTextPc[13] = "EDSPLINE v. ";
#pragma const_seg()
#pragma data_seg(".data$EDSPLINE_FILECHECK")
    const char *EDSPLINE_FILECHECK = EditorSplineFileCheckTextPc;
#pragma data_seg()
#pragma bss_seg(".bss$NuCutSceneDestroyCharacters")
    void(__cdecl *NuCutSceneDestroyCharacters)(void *);
#pragma bss_seg()
#pragma bss_seg(".bss$NuCutSceneCharacterDestroyData")
    void(__cdecl *NuCutSceneCharacterDestroyData)(void *, void *);
#pragma bss_seg()
#pragma bss_seg(".bss$NuCutSceneCharacterEval")
    void(__cdecl *NuCutSceneCharacterEval)(INSTNUGCUTSCENE *, void *, void *, void *, f32);
#pragma bss_seg()
#pragma bss_seg(".bss$NuCutSceneCharacterRelease")
    void(__cdecl *NuCutSceneCharacterRelease)(void *, void *);
#pragma bss_seg()
#pragma bss_seg(".bss$ReleaseLocatorVfxFn")
    void(__cdecl *ReleaseLocatorVfxFn)(i32);
#pragma bss_seg()
#pragma bss_seg(".bss$active_cutscene_instances")
    INSTNUGCUTSCENE *active_cutscene_instances;
#pragma bss_seg()
#pragma bss_seg(".bss$CutSceneCameraCTRL")
    u8 CutSceneCameraCTRL;
#pragma bss_seg()
#pragma bss_seg(".bss$part_types_used")
    i32 part_types_used;
#pragma bss_seg()
#pragma bss_seg(".bss$part_platimpactcnt")
    i32 part_platimpactcnt;
#pragma bss_seg()
#pragma bss_seg(".bss$part_scene")
    NUGSCN *part_scene[32];
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_page_on")
    i32 edpp_page_on[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_instances_used")
    i32 edpp_instances_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_types_used")
    i32 edpp_types_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_ptls")
    EDPP_PARTICLE_PC edpp_ptls[512];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$edbits_anim_page")
    i32 edbits_anim_page;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_particle_mode")
    i32 edanim_particle_mode;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_nearest")
    i32 edanim_nearest;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_nearest_param_id")
    i32 edanim_nearest_param_id;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_cam_pos")
    NUVEC edanim_cam_pos;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$numtl_renderplane")
    u32 numtl_renderplane;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCNativeWarningDebugState")
    PC_NATIVE_DEBUG_STATE_PC PCNativeWarningDebugState;
#pragma bss_seg()
#pragma bss_seg(".bss$PCNativeDebugFormatBuffer")
    char PCNativeDebugFormatBuffer[2048];
#pragma bss_seg()
#pragma bss_seg(".bss$PCNativeDebugOutputBuffer")
    char PCNativeDebugOutputBuffer[2048];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningFormatBuffer")
    char PCWarningFormatBuffer[1024];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningOutputBuffer")
    char PCWarningOutputBuffer[1024];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningMessages")
    char PCWarningMessages[1024];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningHandler")
    void(__cdecl *PCWarningHandler)(char *);
#pragma bss_seg()
#pragma const_seg(".rdata$PCNativeDebugPrefix")
    extern const char PCNativeDebugPrefix[12] = "NUDEBUG: %s";
#pragma const_seg()
#pragma const_seg(".rdata$PCWarningSuffix")
    extern const char PCWarningSuffix[2] = "\n";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$ScaleTerrainT2")
    void *ScaleTerrainT2;
#pragma bss_seg()
#pragma bss_seg(".bss$TempScanStack")
    void *TempScanStack;
#pragma bss_seg()
#pragma bss_seg(".bss$WallSplList")
    void *WallSplList;
#pragma bss_seg()
#pragma bss_seg(".bss$terraincnt")
    i32 terraincnt;
#pragma bss_seg()
#pragma bss_seg(".bss$curSphereter")
    i32 curSphereter;
#pragma bss_seg()
#pragma bss_seg(".bss$platinrange")
    i32 platinrange;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrPoly")
    TERRAIN_POLYGON_INFO_PC *TerrPoly;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrWallInfo")
    u8 TerrWallInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$PlatImpactId")
    i32 PlatImpactId;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrShapeAdjCnt")
    i32 TerrShapeAdjCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$debug_index")
    i16 debug_index;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCrashDataPc")
    u8 *TerrainCrashDataPc;
#pragma bss_seg()
#pragma const_seg(".rdata$TerrainUnusedSpatialBlockPc")
    extern const f32 TerrainUnusedSpatialBlockPc = 2147483648.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainBoundsMinimumPc")
    extern const f32 TerrainBoundsMinimumPc = -200000000.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainBoundsMaximumPc")
    extern const f32 TerrainBoundsMaximumPc = 200000000.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainSteepNormalYPc")
    extern const f32 TerrainSteepNormalYPc = 0.707f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainSourcePathPc")
    extern const char TerrainSourcePathPc[53] = "d:\\projects\\game_saga\\gameapi.saga\\gamelib\\terrain.c";
#pragma const_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_UP")
    i16 LEGOACT_CLIMB_UP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_DOWN")
    i16 LEGOACT_CLIMB_DOWN = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_LEFT")
    i16 LEGOACT_CLIMB_LEFT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_RIGHT")
    i16 LEGOACT_CLIMB_RIGHT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_WALLSHUFFLE_IDLE")
    i16 LEGOACT_WALLSHUFFLE_IDLE = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_WALLSHUFFLE_LEFT")
    i16 LEGOACT_WALLSHUFFLE_LEFT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_WALLSHUFFLE_RIGHT")
    i16 LEGOACT_WALLSHUFFLE_RIGHT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_MAGNET_WALK_METAL")
    i16 LEGOACT_MAGNET_WALK_METAL = -1;
#pragma data_seg()
#pragma const_seg(".rdata$GrappleMomentumNearScalePc")
    extern const f64 GrappleMomentumNearScalePc = 20.0;
#pragma const_seg()
#pragma bss_seg(".bss$LevelHackSendTimer")
    f32 LevelHackSendTimer;
#pragma bss_seg()
#pragma const_seg(".rdata$NuTextureAnimationSlotsWarning")
    extern const char NuTextureAnimationSlotsWarning[31] = "Ran out of texture anim slots!";
#pragma const_seg()
#pragma data_seg(".data$debris_detail_level")
    i32 debris_detail_level = 4;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon1")
    extern const char EpisodeVI_liftrlightbon1[17] = "lift_rlight_bon1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon2")
    extern const char EpisodeVI_liftrlightbon2[17] = "lift_rlight_bon2";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon3")
    extern const char EpisodeVI_liftrlightbon3[17] = "lift_rlight_bon3";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon4")
    extern const char EpisodeVI_liftrlightbon4[17] = "lift_rlight_bon4";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron1")
    extern const char EpisodeVI_liftrarron1[14] = "lift_rarr_on1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron2")
    extern const char EpisodeVI_liftrarron2[14] = "lift_rarr_on2";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron3")
    extern const char EpisodeVI_liftrarron3[14] = "lift_rarr_on3";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron4")
    extern const char EpisodeVI_liftrarron4[14] = "lift_rarr_on4";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton01")
    extern const char EpisodeVI_doton01[10] = "dot_on_01";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton02")
    extern const char EpisodeVI_doton02[10] = "dot_on_02";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton03")
    extern const char EpisodeVI_doton03[10] = "dot_on_03";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton04")
    extern const char EpisodeVI_doton04[10] = "dot_on_04";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton05")
    extern const char EpisodeVI_doton05[10] = "dot_on_05";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton06")
    extern const char EpisodeVI_doton06[10] = "dot_on_06";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton07")
    extern const char EpisodeVI_doton07[10] = "dot_on_07";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton08")
    extern const char EpisodeVI_doton08[10] = "dot_on_08";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton09")
    extern const char EpisodeVI_doton09[10] = "dot_on_09";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton10")
    extern const char EpisodeVI_doton10[10] = "dot_on_10";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton11")
    extern const char EpisodeVI_doton11[10] = "dot_on_11";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton12")
    extern const char EpisodeVI_doton12[10] = "dot_on_12";
#pragma const_seg()
#pragma bss_seg(".bss$emperorfighta_netpacket")
    void *emperorfighta_netpacket;
#pragma bss_seg()
#pragma bss_seg(".bss$LevSfxFlag")
    i32 LevSfxFlag[4];
#pragma bss_seg()
#pragma bss_seg(".bss$debris_object_switches")
    u8 debris_object_switches[128];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOACT_SUPERCARRY_WALK")
    i16 LEGOACT_SUPERCARRY_WALK = -1;
#pragma data_seg()
#pragma const_seg(".rdata$PlayerSlideSpeedPc")
    extern const f64 PlayerSlideSpeedPc = 1.2000000476837158;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerTightRopePhasePc")
    extern const f64 PlayerTightRopePhasePc = 32768.0;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerRandomScalePc")
    extern const f64 PlayerRandomScalePc = 1.5259021893143654e-05;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantZeroX")
    extern const f32 PlayerPanelVariantZeroX = -0.03999999910593033f;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantZeroZ")
    extern const f32 PlayerPanelVariantZeroZ = -0.30000001192092896f;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantOneX")
    extern const f32 PlayerPanelVariantOneX = 0.03500000014901161f;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantOneZ")
    extern const f32 PlayerPanelVariantOneZ = -0.25f;
#pragma const_seg()
#pragma data_seg(".data$LEGOCONTEXT_COMBO")
    i32 LEGOCONTEXT_COMBO = -1;
#pragma data_seg()
#pragma const_seg(".rdata$PlayerStepInnerActionPc")
    extern const f64 PlayerStepInnerActionPc = 1.75;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerStepOuterActionPc")
    extern const f64 PlayerStepOuterActionPc = 2.25;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerStepInnerPc")
    extern const f64 PlayerStepInnerPc = 0.75;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerStepOuterPc")
    extern const f64 PlayerStepOuterPc = 1.25;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerBuildItNegativeLimitPc")
    extern const f32 PlayerBuildItNegativeLimitPc = -1000000000.0f;
#pragma const_seg()
#pragma data_seg(".data$LEGOCONTEXT_PUSHSPINNER")
    i32 LEGOCONTEXT_PUSHSPINNER = -1;
#pragma data_seg()
#pragma data_seg(".data$PlayerPushSpinnerDegreesPc")
    i32 PlayerPushSpinnerDegreesPc = 4;
#pragma data_seg()
#pragma bss_seg(".bss$CanMagnetClimbFn")
    i32(__cdecl *CanMagnetClimbFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerSpecialMoveDataPc")
    void *PlayerSpecialMoveDataPc;
#pragma bss_seg()
#pragma const_seg(".rdata$EpisodeVI_ShowHearts")
    extern const char EpisodeVI_ShowHearts[11] = "ShowHearts";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_ElectricFloor")
    extern const char EpisodeVI_ElectricFloor[14] = "ElectricFloor";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_obstacle21")
    extern const char EpisodeVI_obstacle21[11] = "obstacle21";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_force15")
    extern const char EpisodeVI_force15[8] = "force15";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_force16")
    extern const char EpisodeVI_force16[8] = "force16";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_lift_force_lnull1")
    extern const char EpisodeVI_lift_force_lnull1[18] = "lift_force_lnull1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_lift_force_rnull1")
    extern const char EpisodeVI_lift_force_rnull1[18] = "lift_force_rnull1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_lift_main1")
    extern const char EpisodeVI_lift_main1[11] = "lift_main1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_electric_on")
    extern const char EpisodeVI_electric_on[12] = "electric_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_electric_off")
    extern const char EpisodeVI_electric_off[13] = "electric_off";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_elecpad_zero_format")
    extern const char EpisodeVI_elecpad_zero_format[15] = "elecpad_0%d_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_elecpad_format")
    extern const char EpisodeVI_elecpad_format[14] = "elecpad_%d_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_eFloor_AlwaysOn")
    extern const char EpisodeVI_eFloor_AlwaysOn[16] = "eFloor_AlwaysOn";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_eFloor_AlwaysOf")
    extern const char EpisodeVI_eFloor_AlwaysOf[16] = "eFloor_AlwaysOf";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_eFloor_OnOff")
    extern const char EpisodeVI_eFloor_OnOff[13] = "eFloor_OnOff";
#pragma const_seg()
#pragma data_seg(".data$force_gizmotype_id")
    i32 force_gizmotype_id = -1;
#pragma data_seg()
#pragma bss_seg(".bss$LevAIMessage")
    GIZAIMESSAGE *LevAIMessage[8];
#pragma bss_seg()
#pragma bss_seg(".bss$LevArea")
    AIAREA_PC *LevArea[4];
#pragma bss_seg()
#pragma bss_seg(".bss$routemask_efloor_on")
    u64 routemask_efloor_on;
#pragma bss_seg()
#pragma bss_seg(".bss$routemask_efloor_off")
    u64 routemask_efloor_off;
#pragma bss_seg()
#pragma bss_seg(".bss$eFloor_timer")
    f32 eFloor_timer;
#pragma bss_seg()
#pragma bss_seg(".bss$hspecial_efloor")
    NUSPECIALHANDLE *hspecial_efloor;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccBlowUpTargetCountPc")
    i32 SarlaccBlowUpTargetCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccTakenOverPc")
    i32 SarlaccTakenOverPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccPowerPc")
    i32 SarlaccPowerPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccRechargingPc")
    i32 SarlaccRechargingPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccRechargeTimerPc")
    f32 SarlaccRechargeTimerPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccTargetShieldPc")
    i32 SarlaccTargetShieldPc[2];
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccBlowUpTargetsPc")
    GIZMOBLOWUP_PC *SarlaccBlowUpTargetsPc[4];
#pragma bss_seg()
#pragma const_seg(".rdata$EpisodeVI_magnet_drop")
    extern const char EpisodeVI_magnet_drop[21] = "imp_c3po_magnet_drop";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_magnet_on")
    extern const char EpisodeVI_magnet_on[14] = "env_magnet_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_ffieldoff")
    extern const char EpisodeVI_ffieldoff[10] = "ffieldoff";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_ffield")
    extern const char EpisodeVI_ffield[7] = "ffield";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_cannon_1")
    extern const char EpisodeVI_cannon_1[9] = "cannon_1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_cover_format")
    extern const char EpisodeVI_cover_format[9] = "cover_%d";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_cover_gizmo_format")
    extern const char EpisodeVI_cover_gizmo_format[10] = "cover_%d1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_spinner_format")
    extern const char EpisodeVI_spinner_format[17] = "spinner%d_null_1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_shot_format")
    extern const char EpisodeVI_shot_format[9] = "shot_0%d";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_shot_a_format")
    extern const char EpisodeVI_shot_a_format[10] = "shot_0%da";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_panel2")
    extern const char EpisodeVI_panel2[7] = "panel2";
#pragma const_seg()
#pragma data_seg(".data$gizpanel_gizmotype_id")
    i32 gizpanel_gizmotype_id = -1;
#pragma data_seg()
#pragma bss_seg(".bss$PCDisplayMaterialAddCount")
    i32 PCDisplayMaterialAddCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialAddQueue")
    NUMTL **PCDisplayMaterialAddQueue;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialReferenceCounts")
    u8 *PCDisplayMaterialReferenceCounts;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialStreamStart")
    VARIPTR PCDisplayMaterialStreamStart;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialStreamEnd")
    VARIPTR PCDisplayMaterialStreamEnd;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialStreamUsed")
    u8 *PCDisplayMaterialStreamUsed;
#pragma bss_seg()
#pragma data_seg(".data$PCMaterialBlockItemCount")
    i32 PCMaterialBlockItemCount = 8;
#pragma data_seg()
#pragma data_seg(".data$PCMaterialGeometryItemIndex")
    i32 PCMaterialGeometryItemIndex = 6;
#pragma data_seg()
#pragma data_seg(".data$PCMaterialLinkItemIndex")
    i32 PCMaterialLinkItemIndex = 7;
#pragma data_seg()
#pragma bss_seg(".bss$PCSafeRenderScenes")
    NUDISPLAYLISTRENDERSCENE *PCSafeRenderScenes[24];
#pragma bss_seg()
#pragma bss_seg(".bss$debris_render_global_time")
    f32 debris_render_global_time;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_render_panel_time")
    f32 debris_render_panel_time;
#pragma bss_seg()
#pragma bss_seg(".bss$BossKillDelayTimePc")
    f32 BossKillDelayTimePc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$WallSplCount")
    i32 WallSplCount;
#pragma bss_seg()
#pragma bss_seg(".bss$curPickInst")
    i32 curPickInst;
#pragma bss_seg()
#pragma bss_seg(".bss$IgnoreWallSplines")
    i32 IgnoreWallSplines;
#pragma bss_seg()
#pragma bss_seg(".bss$WallSplinesOnly")
    i32 WallSplinesOnly;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrOverRideScan")
    void *TerrOverRideScan;
#pragma bss_seg()
#pragma const_seg(".rdata$TerrainScanMargin02Pc")
    extern const f64 TerrainScanMargin02Pc = 0.019999999552965164;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainScanMargin10Pc")
    extern const f64 TerrainScanMargin10Pc = 0.10000000149011612;
#pragma const_seg()
#pragma bss_seg(".bss$NuRndr_DebrisRotMtxPtr")
    NUMTX *NuRndr_DebrisRotMtxPtr;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_box_camera_cache")
    NUMTX debris_box_camera_cache;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrStopUpdate")
    i32 NuRndrStopUpdate;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrDebBase")
    NUVEC NuRndrDebBase;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrDebRange")
    NUVEC NuRndrDebRange;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrDebRangeInv")
    NUVEC NuRndrDebRangeInv;
#pragma bss_seg()
#pragma const_seg(".rdata$debris_box_corners")
    extern const NUVEC debris_box_corners[8] = {{-0.2f, -0.2f, -0.05f}, {0.2f, -0.2f, -0.05f}, {-0.2f, 0.2f, -0.05f},
                                                {0.2f, 0.2f, -0.05f},   {-0.5f, -0.5f, 0.8f},  {0.5f, -0.5f, 0.8f},
                                                {-0.5f, 0.5f, 0.8f},    {0.5f, 0.5f, 0.8f}};
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$CRC16TablePC")
    u32 CRC16TablePC[256];
#pragma bss_seg()
#pragma data_seg(".data$ShaderProgramHashPC")
    u32 ShaderProgramHashPC[2] = {1482109315U, 2845988928U};
#pragma data_seg()
#pragma bss_seg(".bss$PCVertexDeclarationCountPC")
    i32 PCVertexDeclarationCountPC;
#pragma bss_seg()
#pragma bss_seg(".bss$PCVertexDeclarationCachePC")
    PC_VERTEX_DECL_ENTRY_PC PCVertexDeclarationCachePC[1];
#pragma bss_seg()
}

#pragma bss_seg(".bss$CRC16InstancePC")
CRC16 CRC16InstancePC;
#pragma bss_seg()

extern "C" {
#pragma const_seg(".rdata$PlayerActionNamePc_001")
    extern const char PlayerActionNamePc_001[5] = "walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_003")
    extern const char PlayerActionNamePc_003[5] = "fire";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_004")
    extern const char PlayerActionNamePc_004[4] = "run";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_005")
    extern const char PlayerActionNamePc_005[7] = "tiptoe";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_006")
    extern const char PlayerActionNamePc_006[5] = "fall";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_007")
    extern const char PlayerActionNamePc_007[5] = "jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_008")
    extern const char PlayerActionNamePc_008[5] = "land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_009")
    extern const char PlayerActionNamePc_009[6] = "idle4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_010")
    extern const char PlayerActionNamePc_010[6] = "jump2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_011")
    extern const char PlayerActionNamePc_011[6] = "land2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_012")
    extern const char PlayerActionNamePc_012[6] = "force";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_013")
    extern const char PlayerActionNamePc_013[5] = "flip";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_014")
    extern const char PlayerActionNamePc_014[9] = "flipland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_015")
    extern const char PlayerActionNamePc_015[6] = "jump3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_016")
    extern const char PlayerActionNamePc_016[6] = "idle2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_017")
    extern const char PlayerActionNamePc_017[9] = "weaponin";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_018")
    extern const char PlayerActionNamePc_018[10] = "weaponout";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_019")
    extern const char PlayerActionNamePc_019[10] = "combojump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_020")
    extern const char PlayerActionNamePc_020[10] = "comboland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_021")
    extern const char PlayerActionNamePc_021[6] = "idle3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_022")
    extern const char PlayerActionNamePc_022[6] = "land3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_023")
    extern const char PlayerActionNamePc_023[6] = "shoot";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_024")
    extern const char PlayerActionNamePc_024[5] = "run2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_025")
    extern const char PlayerActionNamePc_025[10] = "interface";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_026")
    extern const char PlayerActionNamePc_026[11] = "weaponidle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_027")
    extern const char PlayerActionNamePc_027[7] = "block1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_028")
    extern const char PlayerActionNamePc_028[7] = "block2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_029")
    extern const char PlayerActionNamePc_029[7] = "block3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_030")
    extern const char PlayerActionNamePc_030[5] = "run3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_031")
    extern const char PlayerActionNamePc_031[6] = "crawl";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_032")
    extern const char PlayerActionNamePc_032[6] = "lunge";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_033")
    extern const char PlayerActionNamePc_033[10] = "lungeland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_035")
    extern const char PlayerActionNamePc_035[9] = "slamland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_037")
    extern const char PlayerActionNamePc_037[6] = "hover";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_038")
    extern const char PlayerActionNamePc_038[4] = "fly";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_039")
    extern const char PlayerActionNamePc_039[5] = "left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_040")
    extern const char PlayerActionNamePc_040[7] = "force2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_041")
    extern const char PlayerActionNamePc_041[6] = "fall2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_042")
    extern const char PlayerActionNamePc_042[11] = "weaponback";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_043")
    extern const char PlayerActionNamePc_043[3] = "up";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_044")
    extern const char PlayerActionNamePc_044[7] = "pushed";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_045")
    extern const char PlayerActionNamePc_045[3] = "in";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_046")
    extern const char PlayerActionNamePc_046[4] = "out";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_047")
    extern const char PlayerActionNamePc_047[9] = "combo1_1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_048")
    extern const char PlayerActionNamePc_048[10] = "combo1_2a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_049")
    extern const char PlayerActionNamePc_049[10] = "combo1_2b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_050")
    extern const char PlayerActionNamePc_050[10] = "combo1_3a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_051")
    extern const char PlayerActionNamePc_051[10] = "combo1_3b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_052")
    extern const char PlayerActionNamePc_052[10] = "combo1_3c";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_053")
    extern const char PlayerActionNamePc_053[10] = "combo1_3d";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_054")
    extern const char PlayerActionNamePc_054[9] = "combo2_1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_055")
    extern const char PlayerActionNamePc_055[10] = "combo2_2a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_056")
    extern const char PlayerActionNamePc_056[10] = "combo2_2b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_057")
    extern const char PlayerActionNamePc_057[10] = "combo2_3a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_058")
    extern const char PlayerActionNamePc_058[10] = "combo2_3b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_059")
    extern const char PlayerActionNamePc_059[10] = "combo2_3c";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_060")
    extern const char PlayerActionNamePc_060[10] = "combo2_3d";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_061")
    extern const char PlayerActionNamePc_061[7] = "shoot2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_062")
    extern const char PlayerActionNamePc_062[8] = "takehit";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_063")
    extern const char PlayerActionNamePc_063[9] = "takehit2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_064")
    extern const char PlayerActionNamePc_064[8] = "tiptoe2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_065")
    extern const char PlayerActionNamePc_065[6] = "walk2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_066")
    extern const char PlayerActionNamePc_066[12] = "deactivated";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_067")
    extern const char PlayerActionNamePc_067[13] = "deactivated2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_068")
    extern const char PlayerActionNamePc_068[13] = "deactivated3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_069")
    extern const char PlayerActionNamePc_069[13] = "deactivated4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_070")
    extern const char PlayerActionNamePc_070[11] = "interface2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_071")
    extern const char PlayerActionNamePc_071[11] = "interface3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_072")
    extern const char PlayerActionNamePc_072[11] = "interface4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_073")
    extern const char PlayerActionNamePc_073[8] = "hoverup";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_074")
    extern const char PlayerActionNamePc_074[10] = "hoverdown";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_075")
    extern const char PlayerActionNamePc_075[6] = "land4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_076")
    extern const char PlayerActionNamePc_076[6] = "fall3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_077")
    extern const char PlayerActionNamePc_077[6] = "fall4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_078")
    extern const char PlayerActionNamePc_078[6] = "fire2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_079")
    extern const char PlayerActionNamePc_079[4] = "eat";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_080")
    extern const char PlayerActionNamePc_080[6] = "right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_081")
    extern const char PlayerActionNamePc_081[14] = "walkbackwards";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_082")
    extern const char PlayerActionNamePc_082[6] = "punch";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_083")
    extern const char PlayerActionNamePc_083[5] = "push";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_084")
    extern const char PlayerActionNamePc_084[7] = "choked";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_085")
    extern const char PlayerActionNamePc_085[7] = "zapped";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_086")
    extern const char PlayerActionNamePc_086[7] = "punch2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_087")
    extern const char PlayerActionNamePc_087[7] = "punch3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_088")
    extern const char PlayerActionNamePc_088[16] = "combatroll_fire";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_089")
    extern const char PlayerActionNamePc_089[9] = "forwards";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_090")
    extern const char PlayerActionNamePc_090[9] = "fallland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_091")
    extern const char PlayerActionNamePc_091[10] = "shootleft";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_092")
    extern const char PlayerActionNamePc_092[11] = "shootright";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_093")
    extern const char PlayerActionNamePc_093[10] = "shootback";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_094")
    extern const char PlayerActionNamePc_094[10] = "pulllever";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_095")
    extern const char PlayerActionNamePc_095[9] = "helmeton";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_096")
    extern const char PlayerActionNamePc_096[6] = "build";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_097")
    extern const char PlayerActionNamePc_097[6] = "idle5";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_098")
    extern const char PlayerActionNamePc_098[6] = "idle6";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_099")
    extern const char PlayerActionNamePc_099[6] = "idle7";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_100")
    extern const char PlayerActionNamePc_100[6] = "idle8";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_101")
    extern const char PlayerActionNamePc_101[7] = "attack";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_102")
    extern const char PlayerActionNamePc_102[6] = "throw";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_103")
    extern const char PlayerActionNamePc_103[7] = "pickup";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_104")
    extern const char PlayerActionNamePc_104[5] = "drop";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_105")
    extern const char PlayerActionNamePc_105[8] = "grabbed";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_106")
    extern const char PlayerActionNamePc_106[9] = "attacked";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_107")
    extern const char PlayerActionNamePc_107[6] = "slide";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_108")
    extern const char PlayerActionNamePc_108[12] = "communicate";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_109")
    extern const char PlayerActionNamePc_109[5] = "ride";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_110")
    extern const char PlayerActionNamePc_110[10] = "helmeton2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_111")
    extern const char PlayerActionNamePc_111[7] = "throw2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_112")
    extern const char PlayerActionNamePc_112[7] = "throw3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_113")
    extern const char PlayerActionNamePc_113[6] = "ride2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_114")
    extern const char PlayerActionNamePc_114[13] = "extra_tiptoe";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_115")
    extern const char PlayerActionNamePc_115[11] = "extra_walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_116")
    extern const char PlayerActionNamePc_116[10] = "extra_run";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_117")
    extern const char PlayerActionNamePc_117[11] = "extra_fall";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_118")
    extern const char PlayerActionNamePc_118[11] = "extra_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_119")
    extern const char PlayerActionNamePc_119[17] = "extra_weaponidle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_120")
    extern const char PlayerActionNamePc_120[9] = "backflip";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_121")
    extern const char PlayerActionNamePc_121[11] = "extra_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_122")
    extern const char PlayerActionNamePc_122[12] = "extra_jump2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_123")
    extern const char PlayerActionNamePc_123[11] = "extra_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_124")
    extern const char PlayerActionNamePc_124[12] = "extra_land2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_125")
    extern const char PlayerActionNamePc_125[12] = "extra_lunge";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_126")
    extern const char PlayerActionNamePc_126[16] = "extra_lungeland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_127")
    extern const char PlayerActionNamePc_127[15] = "extra_weaponin";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_128")
    extern const char PlayerActionNamePc_128[16] = "extra_weaponout";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_129")
    extern const char PlayerActionNamePc_129[9] = "activate";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_130")
    extern const char PlayerActionNamePc_130[11] = "deactivate";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_131")
    extern const char PlayerActionNamePc_131[6] = "walk3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_132")
    extern const char PlayerActionNamePc_132[6] = "ride3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_133")
    extern const char PlayerActionNamePc_133[6] = "ride4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_134")
    extern const char PlayerActionNamePc_134[6] = "splat";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_135")
    extern const char PlayerActionNamePc_135[6] = "ride5";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_136")
    extern const char PlayerActionNamePc_136[11] = "climb_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_137")
    extern const char PlayerActionNamePc_137[15] = "tightrope_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_138")
    extern const char PlayerActionNamePc_138[15] = "tightrope_move";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_139")
    extern const char PlayerActionNamePc_139[17] = "wallshuffle_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_140")
    extern const char PlayerActionNamePc_140[17] = "wallshuffle_left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_141")
    extern const char PlayerActionNamePc_141[18] = "wallshuffle_right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_142")
    extern const char PlayerActionNamePc_142[8] = "putdown";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_143")
    extern const char PlayerActionNamePc_143[6] = "float";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_144")
    extern const char PlayerActionNamePc_144[16] = "tightrope_geton";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_145")
    extern const char PlayerActionNamePc_145[7] = "target";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_146")
    extern const char PlayerActionNamePc_146[10] = "hang_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_147")
    extern const char PlayerActionNamePc_147[10] = "hang_move";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_148")
    extern const char PlayerActionNamePc_148[6] = "glide";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_149")
    extern const char PlayerActionNamePc_149[13] = "punch_behind";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_150")
    extern const char PlayerActionNamePc_150[17] = "tightrope_getoff";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_151")
    extern const char PlayerActionNamePc_151[7] = "change";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_152")
    extern const char PlayerActionNamePc_152[11] = "throw_wait";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_153")
    extern const char PlayerActionNamePc_153[6] = "catch";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_154")
    extern const char PlayerActionNamePc_154[5] = "hack";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_155")
    extern const char PlayerActionNamePc_155[8] = "attract";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_156")
    extern const char PlayerActionNamePc_156[9] = "transfer";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_157")
    extern const char PlayerActionNamePc_157[6] = "sonar";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_158")
    extern const char PlayerActionNamePc_158[11] = "ledge_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_159")
    extern const char PlayerActionNamePc_159[11] = "ledge_left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_160")
    extern const char PlayerActionNamePc_160[12] = "ledge_right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_161")
    extern const char PlayerActionNamePc_161[10] = "transform";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_162")
    extern const char PlayerActionNamePc_162[14] = "walljump_wait";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_163")
    extern const char PlayerActionNamePc_163[9] = "walljump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_164")
    extern const char PlayerActionNamePc_164[18] = "supercarry_pickup";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_165")
    extern const char PlayerActionNamePc_165[16] = "supercarry_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_166")
    extern const char PlayerActionNamePc_166[16] = "supercarry_walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_167")
    extern const char PlayerActionNamePc_167[17] = "supercarry_throw";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_168")
    extern const char PlayerActionNamePc_168[5] = "stun";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_169")
    extern const char PlayerActionNamePc_169[6] = "stun2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_170")
    extern const char PlayerActionNamePc_170[6] = "stun3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_171")
    extern const char PlayerActionNamePc_171[8] = "stunned";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_172")
    extern const char PlayerActionNamePc_172[9] = "stunned2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_173")
    extern const char PlayerActionNamePc_173[9] = "stunned3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_174")
    extern const char PlayerActionNamePc_174[9] = "security";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_175")
    extern const char PlayerActionNamePc_175[15] = "superpush_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_176")
    extern const char PlayerActionNamePc_176[15] = "superpush_push";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_177")
    extern const char PlayerActionNamePc_177[15] = "superpush_pull";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_178")
    extern const char PlayerActionNamePc_178[11] = "ballooning";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_179")
    extern const char PlayerActionNamePc_179[12] = "throw_quick";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_180")
    extern const char PlayerActionNamePc_180[17] = "backpackfallland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_181")
    extern const char PlayerActionNamePc_181[16] = "combatroll_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_182")
    extern const char PlayerActionNamePc_182[16] = "combatroll_fall";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_183")
    extern const char PlayerActionNamePc_183[16] = "combatroll_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_184")
    extern const char PlayerActionNamePc_184[7] = "recoil";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_185")
    extern const char PlayerActionNamePc_185[8] = "die_air";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_186")
    extern const char PlayerActionNamePc_186[9] = "stun_die";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_187")
    extern const char PlayerActionNamePc_187[13] = "grapple_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_188")
    extern const char PlayerActionNamePc_188[11] = "grapple_up";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_189")
    extern const char PlayerActionNamePc_189[13] = "grapple_down";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_190")
    extern const char PlayerActionNamePc_190[13] = "grapple_hang";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_191")
    extern const char PlayerActionNamePc_191[6] = "idle9";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_192")
    extern const char PlayerActionNamePc_192[7] = "idle10";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_193")
    extern const char PlayerActionNamePc_193[6] = "ride6";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_194")
    extern const char PlayerActionNamePc_194[6] = "ride7";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_195")
    extern const char PlayerActionNamePc_195[6] = "ride8";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_196")
    extern const char PlayerActionNamePc_196[6] = "ride9";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_197")
    extern const char PlayerActionNamePc_197[7] = "ride10";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_198")
    extern const char PlayerActionNamePc_198[18] = "magnet_walk_metal";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_199")
    extern const char PlayerActionNamePc_199[14] = "magnet_tiptoe";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_200")
    extern const char PlayerActionNamePc_200[12] = "magnet_walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_201")
    extern const char PlayerActionNamePc_201[11] = "magnet_run";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_202")
    extern const char PlayerActionNamePc_202[12] = "magnet_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_203")
    extern const char PlayerActionNamePc_203[12] = "magnet_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_204")
    extern const char PlayerActionNamePc_204[7] = "dropin";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_205")
    extern const char PlayerActionNamePc_205[8] = "dropout";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_206")
    extern const char PlayerActionNamePc_206[9] = "climb_up";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_207")
    extern const char PlayerActionNamePc_207[11] = "climb_down";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_208")
    extern const char PlayerActionNamePc_208[11] = "climb_left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_209")
    extern const char PlayerActionNamePc_209[12] = "climb_right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_210")
    extern const char PlayerActionNamePc_210[13] = "ai_override1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_211")
    extern const char PlayerActionNamePc_211[13] = "ai_override2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_212")
    extern const char PlayerActionNamePc_212[13] = "ai_override3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_213")
    extern const char PlayerActionNamePc_213[13] = "ai_override4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_214")
    extern const char PlayerActionNamePc_214[19] = "supercarry_putdown";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_215")
    extern const char PlayerActionNamePc_215[16] = "supercarry_bash";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_216")
    extern const char PlayerActionNamePc_216[16] = "supercarry_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_217")
    extern const char PlayerActionNamePc_217[16] = "supercarry_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_218")
    extern const char PlayerActionNamePc_218[20] = "supercarry_fallland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_219")
    extern const char PlayerActionNamePc_219[11] = "ledge_grab";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_220")
    extern const char PlayerActionNamePc_220[7] = "teeter";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_221")
    extern const char PlayerActionNamePc_221[11] = "whip_start";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_222")
    extern const char PlayerActionNamePc_222[11] = "whip_crack";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_223")
    extern const char PlayerActionNamePc_223[10] = "whip_grab";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_224")
    extern const char PlayerActionNamePc_224[11] = "whip_break";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_225")
    extern const char PlayerActionNamePc_225[17] = "whip_swing_start";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_226")
    extern const char PlayerActionNamePc_226[17] = "whip_swing_swing";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_227")
    extern const char PlayerActionNamePc_227[16] = "whip_swing_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_228")
    extern const char PlayerActionNamePc_228[11] = "crawl_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_229")
    extern const char PlayerActionNamePc_229[11] = "crawl_move";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_230")
    extern const char PlayerActionNamePc_230[5] = "swim";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_231")
    extern const char PlayerActionNamePc_231[5] = "wade";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_232")
    extern const char PlayerActionNamePc_232[4] = "dig";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_233")
    extern const char PlayerActionNamePc_233[6] = "winch";
#pragma const_seg()
#pragma data_seg(".data$PlayerActionInfoListPc")
    API_ACTION_INFO_PC PlayerActionInfoListPc[234] = {
        {Text_UnknownText, 0U},       {PlayerActionNamePc_001, 2U},  {"idle", 0U},
        {PlayerActionNamePc_003, 0U}, {PlayerActionNamePc_004, 4U},  {PlayerActionNamePc_005, 1U},
        {PlayerActionNamePc_006, 0U}, {PlayerActionNamePc_007, 0U},  {PlayerActionNamePc_008, 0U},
        {PlayerActionNamePc_009, 0U}, {PlayerActionNamePc_010, 0U},  {PlayerActionNamePc_011, 0U},
        {PlayerActionNamePc_012, 0U}, {PlayerActionNamePc_013, 0U},  {PlayerActionNamePc_014, 0U},
        {PlayerActionNamePc_015, 0U}, {PlayerActionNamePc_016, 0U},  {PlayerActionNamePc_017, 0U},
        {PlayerActionNamePc_018, 0U}, {PlayerActionNamePc_019, 0U},  {PlayerActionNamePc_020, 0U},
        {PlayerActionNamePc_021, 0U}, {PlayerActionNamePc_022, 0U},  {PlayerActionNamePc_023, 0U},
        {PlayerActionNamePc_024, 4U}, {PlayerActionNamePc_025, 0U},  {PlayerActionNamePc_026, 0U},
        {PlayerActionNamePc_027, 8U}, {PlayerActionNamePc_028, 8U},  {PlayerActionNamePc_029, 8U},
        {PlayerActionNamePc_030, 4U}, {PlayerActionNamePc_031, 0U},  {PlayerActionNamePc_032, 0U},
        {PlayerActionNamePc_033, 0U}, {AITransferLiteral75786c, 0U}, {PlayerActionNamePc_035, 0U},
        {AIActionOpenName, 0U},       {PlayerActionNamePc_037, 0U},  {PlayerActionNamePc_038, 0U},
        {PlayerActionNamePc_039, 0U}, {PlayerActionNamePc_040, 0U},  {PlayerActionNamePc_041, 0U},
        {PlayerActionNamePc_042, 0U}, {PlayerActionNamePc_043, 0U},  {PlayerActionNamePc_044, 0U},
        {PlayerActionNamePc_045, 0U}, {PlayerActionNamePc_046, 0U},  {PlayerActionNamePc_047, 0U},
        {PlayerActionNamePc_048, 0U}, {PlayerActionNamePc_049, 0U},  {PlayerActionNamePc_050, 0U},
        {PlayerActionNamePc_051, 0U}, {PlayerActionNamePc_052, 0U},  {PlayerActionNamePc_053, 0U},
        {PlayerActionNamePc_054, 0U}, {PlayerActionNamePc_055, 0U},  {PlayerActionNamePc_056, 0U},
        {PlayerActionNamePc_057, 0U}, {PlayerActionNamePc_058, 0U},  {PlayerActionNamePc_059, 0U},
        {PlayerActionNamePc_060, 0U}, {PlayerActionNamePc_061, 0U},  {PlayerActionNamePc_062, 0U},
        {PlayerActionNamePc_063, 0U}, {PlayerActionNamePc_064, 1U},  {PlayerActionNamePc_065, 2U},
        {PlayerActionNamePc_066, 0U}, {PlayerActionNamePc_067, 0U},  {PlayerActionNamePc_068, 0U},
        {PlayerActionNamePc_069, 0U}, {PlayerActionNamePc_070, 0U},  {PlayerActionNamePc_071, 0U},
        {PlayerActionNamePc_072, 0U}, {PlayerActionNamePc_073, 0U},  {PlayerActionNamePc_074, 0U},
        {PlayerActionNamePc_075, 0U}, {PlayerActionNamePc_076, 0U},  {PlayerActionNamePc_077, 0U},
        {PlayerActionNamePc_078, 0U}, {PlayerActionNamePc_079, 0U},  {PlayerActionNamePc_080, 0U},
        {PlayerActionNamePc_081, 0U}, {PlayerActionNamePc_082, 0U},  {PlayerActionNamePc_083, 0U},
        {PlayerActionNamePc_084, 0U}, {PlayerActionNamePc_085, 0U},  {PlayerActionNamePc_086, 0U},
        {PlayerActionNamePc_087, 0U}, {PlayerActionNamePc_088, 0U},  {PlayerActionNamePc_089, 0U},
        {PlayerActionNamePc_090, 0U}, {PlayerActionNamePc_091, 0U},  {PlayerActionNamePc_092, 0U},
        {PlayerActionNamePc_093, 0U}, {PlayerActionNamePc_094, 0U},  {PlayerActionNamePc_095, 0U},
        {PlayerActionNamePc_096, 0U}, {PlayerActionNamePc_097, 0U},  {PlayerActionNamePc_098, 0U},
        {PlayerActionNamePc_099, 0U}, {PlayerActionNamePc_100, 0U},  {PlayerActionNamePc_101, 0U},
        {PlayerActionNamePc_102, 0U}, {PlayerActionNamePc_103, 0U},  {PlayerActionNamePc_104, 0U},
        {PlayerActionNamePc_105, 0U}, {PlayerActionNamePc_106, 0U},  {PlayerActionNamePc_107, 0U},
        {PlayerActionNamePc_108, 0U}, {PlayerActionNamePc_109, 0U},  {PlayerActionNamePc_110, 0U},
        {PlayerActionNamePc_111, 0U}, {PlayerActionNamePc_112, 0U},  {PlayerActionNamePc_113, 0U},
        {PlayerActionNamePc_114, 1U}, {PlayerActionNamePc_115, 2U},  {PlayerActionNamePc_116, 4U},
        {PlayerActionNamePc_117, 0U}, {PlayerActionNamePc_118, 0U},  {PlayerActionNamePc_119, 0U},
        {PlayerActionNamePc_120, 0U}, {PlayerActionNamePc_121, 0U},  {PlayerActionNamePc_122, 0U},
        {PlayerActionNamePc_123, 0U}, {PlayerActionNamePc_124, 0U},  {PlayerActionNamePc_125, 0U},
        {PlayerActionNamePc_126, 0U}, {PlayerActionNamePc_127, 0U},  {PlayerActionNamePc_128, 0U},
        {PlayerActionNamePc_129, 0U}, {PlayerActionNamePc_130, 0U},  {PlayerActionNamePc_131, 2U},
        {PlayerActionNamePc_132, 0U}, {PlayerActionNamePc_133, 0U},  {PlayerActionNamePc_134, 0U},
        {PlayerActionNamePc_135, 0U}, {PlayerActionNamePc_136, 0U},  {PlayerActionNamePc_137, 0U},
        {PlayerActionNamePc_138, 0U}, {PlayerActionNamePc_139, 0U},  {PlayerActionNamePc_140, 0U},
        {PlayerActionNamePc_141, 0U}, {PlayerActionNamePc_142, 0U},  {PlayerActionNamePc_143, 0U},
        {PlayerActionNamePc_144, 0U}, {PlayerActionNamePc_145, 0U},  {PlayerActionNamePc_146, 0U},
        {PlayerActionNamePc_147, 0U}, {PlayerActionNamePc_148, 0U},  {PlayerActionNamePc_149, 0U},
        {PlayerActionNamePc_150, 0U}, {PlayerActionNamePc_151, 0U},  {PlayerActionNamePc_152, 0U},
        {PlayerActionNamePc_153, 0U}, {PlayerActionNamePc_154, 0U},  {PlayerActionNamePc_155, 0U},
        {PlayerActionNamePc_156, 0U}, {PlayerActionNamePc_157, 0U},  {PlayerActionNamePc_158, 0U},
        {PlayerActionNamePc_159, 0U}, {PlayerActionNamePc_160, 0U},  {PlayerActionNamePc_161, 0U},
        {PlayerActionNamePc_162, 0U}, {PlayerActionNamePc_163, 0U},  {PlayerActionNamePc_164, 0U},
        {PlayerActionNamePc_165, 0U}, {PlayerActionNamePc_166, 0U},  {PlayerActionNamePc_167, 0U},
        {PlayerActionNamePc_168, 0U}, {PlayerActionNamePc_169, 0U},  {PlayerActionNamePc_170, 0U},
        {PlayerActionNamePc_171, 0U}, {PlayerActionNamePc_172, 0U},  {PlayerActionNamePc_173, 0U},
        {PlayerActionNamePc_174, 0U}, {PlayerActionNamePc_175, 0U},  {PlayerActionNamePc_176, 0U},
        {PlayerActionNamePc_177, 0U}, {PlayerActionNamePc_178, 0U},  {PlayerActionNamePc_179, 0U},
        {PlayerActionNamePc_180, 0U}, {PlayerActionNamePc_181, 0U},  {PlayerActionNamePc_182, 0U},
        {PlayerActionNamePc_183, 0U}, {PlayerActionNamePc_184, 0U},  {PlayerActionNamePc_185, 0U},
        {PlayerActionNamePc_186, 0U}, {PlayerActionNamePc_187, 0U},  {PlayerActionNamePc_188, 0U},
        {PlayerActionNamePc_189, 0U}, {PlayerActionNamePc_190, 0U},  {PlayerActionNamePc_191, 0U},
        {PlayerActionNamePc_192, 0U}, {PlayerActionNamePc_193, 0U},  {PlayerActionNamePc_194, 0U},
        {PlayerActionNamePc_195, 0U}, {PlayerActionNamePc_196, 0U},  {PlayerActionNamePc_197, 0U},
        {PlayerActionNamePc_198, 2U}, {PlayerActionNamePc_199, 1U},  {PlayerActionNamePc_200, 2U},
        {PlayerActionNamePc_201, 4U}, {PlayerActionNamePc_202, 0U},  {PlayerActionNamePc_203, 0U},
        {PlayerActionNamePc_204, 0U}, {PlayerActionNamePc_205, 0U},  {PlayerActionNamePc_206, 0U},
        {PlayerActionNamePc_207, 0U}, {PlayerActionNamePc_208, 0U},  {PlayerActionNamePc_209, 0U},
        {PlayerActionNamePc_210, 0U}, {PlayerActionNamePc_211, 0U},  {PlayerActionNamePc_212, 0U},
        {PlayerActionNamePc_213, 0U}, {PlayerActionNamePc_214, 0U},  {PlayerActionNamePc_215, 0U},
        {PlayerActionNamePc_216, 0U}, {PlayerActionNamePc_217, 0U},  {PlayerActionNamePc_218, 0U},
        {PlayerActionNamePc_219, 0U}, {PlayerActionNamePc_220, 0U},  {PlayerActionNamePc_221, 0U},
        {PlayerActionNamePc_222, 0U}, {PlayerActionNamePc_223, 0U},  {PlayerActionNamePc_224, 0U},
        {PlayerActionNamePc_225, 0U}, {PlayerActionNamePc_226, 0U},  {PlayerActionNamePc_227, 0U},
        {PlayerActionNamePc_228, 0U}, {PlayerActionNamePc_229, 0U},  {PlayerActionNamePc_230, 0U},
        {PlayerActionNamePc_231, 0U}, {PlayerActionNamePc_232, 0U},  {PlayerActionNamePc_233, 0U}};
#pragma data_seg()
#pragma data_seg(".data$PlayerBlockActionInfoPc")
    void *PlayerBlockActionInfoPc = &PlayerActionInfoListPc[1];
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuDebrisVertexBuffers")
    PC_BUFFER_RESOURCE_PC *NuDebrisVertexBuffers[4];
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisSystemBuffers")
    void *NuDebrisSystemBuffers[8];
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisUseSystemMemory")
    i32 NuDebrisUseSystemMemory;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisCurrentBufferIndex")
    u32 NuDebrisCurrentBufferIndex;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisCurrentVertexCount")
    u32 NuDebrisCurrentVertexCount;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisVertexBufferSize")
    u32 NuDebrisVertexBufferSize;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisMaxVertexCount")
    u32 NuDebrisMaxVertexCount;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisVertexData")
    void *NuDebrisVertexData;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisCurrentPacket")
    NATIVE_PARTICLE_PACKET_PC *NuDebrisCurrentPacket;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisLastHeader")
    DEBRIS_PART_HEADER_PC *NuDebrisLastHeader;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisFrameVertexCount")
    u32 NuDebrisFrameVertexCount;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndr_DebrisMtx")
    NUMTX NuRndr_DebrisMtx;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndr_DebrisPlane")
    NUVEC4 NuRndr_DebrisPlane;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayClipSet")
    void *PCDisplayClipSet;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayClipGroupIndex")
    i32 PCDisplayClipGroupIndex;
#pragma bss_seg()
#pragma data_seg(".data$PlayerFloatHeightFractionPc")
    f32 PlayerFloatHeightFractionPc = 0.625f;
#pragma data_seg()
#pragma data_seg(".data$PlayerBackwardMomentumScalePc")
    f32 PlayerBackwardMomentumScalePc = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$PlayerSecondBackwardMomentumScalePc")
    f32 PlayerSecondBackwardMomentumScalePc = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$PlayerGrappleSwingModePc")
    i32 PlayerGrappleSwingModePc = 1;
#pragma data_seg()
#pragma const_seg(".rdata$PlayerGrappleWindSoundPc")
    extern const char PlayerGrappleWindSoundPc[11] = "GrapWindLp";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$GHGTextureProgramsPC")
    void *GHGTextureProgramsPC;
#pragma bss_seg()
#pragma const_seg(".rdata$GHGJointFormatWarningPC")
    extern const char GHGJointFormatWarningPC[95] =
        "Your Character version is %d, whereas the Engine only accepts Character versions from %d to %d";
#pragma const_seg()
#pragma const_seg(".rdata$GHGSceneFormatWarningPC")
    extern const char GHGSceneFormatWarningPC[87] =
        "Your Scene version is %d, whereas the Engine only accepts Scene versions from %d to %d";
#pragma const_seg()
#pragma data_seg(".data$id_GRABCONTROL")
    i16 id_GRABCONTROL = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GRABR2CONTROL")
    i16 id_GRABR2CONTROL = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIATST")
    i16 id_MINIATST = -1;
#pragma data_seg()
#pragma data_seg(".data$id_ATST_LOWRES")
    i16 id_ATST_LOWRES = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIATAT")
    i16 id_MINIATAT = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIATTE")
    i16 id_MINIATTE = -1;
#pragma data_seg()
#pragma data_seg(".data$id_RANCOR")
    i16 id_RANCOR = -1;
#pragma data_seg()
#pragma data_seg(".data$id_YODA")
    i16 id_YODA = -1;
#pragma data_seg()
#pragma data_seg(".data$id_YODAGHOST")
    i16 id_YODAGHOST = -1;
#pragma data_seg()
#pragma data_seg(".data$id_C3PO")
    i16 id_C3PO = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TC14")
    i16 id_TC14 = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DROIDEKA")
    i16 id_DROIDEKA = -1;
#pragma data_seg()
#pragma bss_seg(".bss$VADERA_LDATA")
    LEVELDATA_PC *VADERA_LDATA;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$TerrWallTab")
    u8 TerrWallTab[4];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$CRUISERD_LDATA")
    LEVELDATA_PC *CRUISERD_LDATA;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$TerrainOpposingContactThresholdPc")
    extern const f64 TerrainOpposingContactThresholdPc = (f64)-0.7f;
#pragma const_seg()
#pragma data_seg(".data$TerConTol")
    f32 TerConTol = 0.1f;
#pragma data_seg()
#pragma bss_seg(".bss$TerrImpactPos")
    NUVEC TerrImpactPos;
#pragma bss_seg()
#pragma bss_seg(".bss$terrhitflags")
    i32 terrhitflags;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrImpact")
    i16 TerrImpact;
#pragma bss_seg()
#pragma bss_seg(".bss$SphereData")
    TERRAIN_SPHERE_PC SphereData[16];
#pragma bss_seg()
#pragma bss_seg(".bss$PlatCrush")
    i32 PlatCrush;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrShape")
    TERRAIN_AXIS_FREEDOM_SHAPE_PC *TerrShape;
#pragma bss_seg()
#pragma bss_seg(".bss$tertempvec4")
    NUVEC4 tertempvec4;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrWallNorm")
    NUVEC TerrWallNorm;
#pragma bss_seg()
#pragma bss_seg(".bss$plathitid")
    i32 plathitid;
#pragma bss_seg()
#pragma bss_seg(".bss$PlatImpactTer")
    TERRAIN_POLYGON_INFO_PC PlatImpactTer;
#pragma bss_seg()
#pragma bss_seg(".bss$tertempmat")
    NUMTX tertempmat;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrLastImpact")
    TERRAIN_LAST_IMPACT_PC TerrLastImpact;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrPolyInfo")
    TERRAIN_POLYGON_INFO_PC TerrPolyInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$wallover")
    f32 wallover;
#pragma bss_seg()
#pragma bss_seg(".bss$CurTrackInfo")
    TERRAIN_TRACK_SLOT_PC *CurTrackInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrImpactNormal")
    NUVEC TerrImpactNormal;
#pragma bss_seg()
#pragma bss_seg(".bss$TerImpactDataMax")
    i32 TerImpactDataMax;
#pragma bss_seg()
#pragma bss_seg(".bss$TerImpactDataCount")
    i32 *TerImpactDataCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PlatImpactNorm")
    NUVEC PlatImpactNorm;
#pragma bss_seg()
#pragma bss_seg(".bss$TerImpactData")
    void *TerImpactData;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$DebrisTimeBarName")
    extern const char DebrisTimeBarName[4] = "deb";
#pragma const_seg()
#pragma data_seg(".data$DebrisSuspendDrawObjectSwitch")
    i32 DebrisSuspendDrawObjectSwitch = -1;
#pragma data_seg()
#pragma data_seg(".data$debris_render_scale")
    f32 debris_render_scale = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$edbits_editor_enabled")
    i32 *edbits_editor_enabled = &edbits_editor_state;
#pragma data_seg()
#pragma bss_seg(".bss$NuPortalCameraPlane")
    NUPLANE NuPortalCameraPlane;
#pragma bss_seg()
#pragma bss_seg(".bss$DebMat")
    NUMTL *DebMat[9];
#pragma bss_seg()
#pragma bss_seg(".bss$xzfacingmtx")
    NUMTX xzfacingmtx;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_rt")
    i32 debris_rt;
#pragma bss_seg()
#pragma bss_seg(".bss$g_renderingDebris")
    i32 g_renderingDebris;
#pragma bss_seg()
#pragma bss_seg(".bss$edbits_editor_state")
    i32 edbits_editor_state;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$SuperCarryImpactSpeedPc")
    f32 SuperCarryImpactSpeedPc = 4.0f;
#pragma data_seg()
#pragma bss_seg(".bss$ExtraDieSfxFn")
    void(__cdecl *ExtraDieSfxFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$TerrWallDeflectYScale")
    f32 TerrWallDeflectYScale;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$TerrainPlayerOtherMovementScalePc")
    f32 TerrainPlayerOtherMovementScalePc = 2.5f;
#pragma data_seg()
#pragma data_seg(".data$TerrainPlayerBikeMovementScalePc")
    f32 TerrainPlayerBikeMovementScalePc = 15.0f;
#pragma data_seg()
#pragma data_seg(".data$TerrainPlayerPlatConnectTolerancePc")
    f32 TerrainPlayerPlatConnectTolerancePc = 0.01f;
#pragma data_seg()
#pragma data_seg(".data$TerrainPlayerShadowFixPc")
    i32 TerrainPlayerShadowFixPc = 1;
#pragma data_seg()
#pragma data_seg(".data$MAXBLOWUPRANDSPANG")
    i32 MAXBLOWUPRANDSPANG = 60;
#pragma data_seg()
#pragma bss_seg(".bss$GizmoBlowUp_SfxFn")
    void(__cdecl *GizmoBlowUp_SfxFn)(GIZMOBLOWUP_PC *, NUVEC *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$TerrPlatDis")
    i32 TerrPlatDis = -1;
#pragma data_seg()
#pragma data_seg(".data$PCPortalsEnabled")
    i32 PCPortalsEnabled = 1;
#pragma data_seg()
#pragma data_seg(".data$PartVectorSpreadDegrees_PC")
    i32 PartVectorSpreadDegrees_PC = 30;
#pragma data_seg()
#pragma bss_seg(".bss$AreaPickupGravity")
    f32 AreaPickupGravity;
#pragma bss_seg()
#pragma bss_seg(".bss$AreaPickupScale")
    f32 AreaPickupScale;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$GameMessage")
    GAME_MESSAGE_PC GameMessage[128];
#pragma bss_seg()
#pragma bss_seg(".bss$GameMessageIndex")
    i32 GameMessageIndex;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$ScorePanelName")
    char ScorePanelName[9] = "MK-Panel";
#pragma data_seg()
#pragma const_seg(".rdata$TerrainWaterJumpSoundNamePc")
    extern const char TerrainWaterJumpSoundNamePc[13] = "FS_WaterJump";
#pragma const_seg()
#pragma data_seg(".data$ScoreCounterName")
    char ScoreCounterName[8] = "Counter";
#pragma data_seg()
#pragma data_seg(".data$PickupCoinsLandName")
    char PickupCoinsLandName[10] = "CoinsLand";
#pragma data_seg()
#pragma data_seg(".data$CoinTab")
    u8 CoinTab[4] = {0, 1, 2, 3};
#pragma data_seg()
#pragma data_seg(".data$PCArcadePointGoals")
    i32 PCArcadePointGoals[5] = {5, 10, 25, 50, 0};
#pragma data_seg()
#pragma data_seg(".data$Arcade_NeedTwoPlayers_Scale")
    f32 Arcade_NeedTwoPlayers_Scale = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PCArcadeIconMessageY")
    f32 PCArcadeIconMessageY = -0.550000011920929f;
#pragma data_seg()
#pragma data_seg(".data$PANEL_COINX")
    f32 PANEL_COINX = 0.6499999761581421f;
#pragma data_seg()
#pragma data_seg(".data$PANEL_COINY")
    f32 PANEL_COINY = 0.04500000178813934f;
#pragma data_seg()
#pragma data_seg(".data$PANEL_COINSCALE_END")
    f32 PANEL_COINSCALE_END = 0.3499999940395355f;
#pragma data_seg()
#pragma data_seg(".data$STATSPOSY")
    f32 STATSPOSY = 0.7450000047683716f;
#pragma data_seg()
#pragma data_seg(".data$COINTOTAL_COINSIZE")
    f32 COINTOTAL_COINSIZE = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$cointotal_i_obj")
    i32 cointotal_i_obj[2] = {187, 187};
#pragma data_seg()
#pragma data_seg(".data$TerrainLastChatterSfxPc")
    i32 TerrainLastChatterSfxPc = -1;
#pragma data_seg()
#pragma data_seg(".data$AddGameMsg_Default")
    ADD_GAME_MESSAGE_PC AddGameMsg_Default = {NULL, &v000, &v000, 1.0f, 1.0f, 255,  255, 255,  128,
                                              0,    1.0f,  0.0f,  0,    -1,   NULL, 0,   0.0f, 0.0f,
                                              NULL, NULL,  NULL,  NULL, NULL, -1,   0,   255,  0};
#pragma data_seg()
#pragma data_seg(".data$COINMSGTIME")
    f32 COINMSGTIME = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PCArcadeUnpairedIconModel")
    i32 PCArcadeUnpairedIconModel = -1;
#pragma data_seg()
#pragma data_seg(".data$BigJumpLandFallbackPc")
    i16 BigJumpLandFallbackPc = -1;
#pragma data_seg()
#pragma data_seg(".data$BigJumpLandSecondaryPc")
    i16 BigJumpLandSecondaryPc = -1;
#pragma data_seg()
#pragma data_seg(".data$BigJumpLandPreferredPc")
    i16 BigJumpLandPreferredPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$BonusCoinTarget")
    u32 BonusCoinTarget;
#pragma bss_seg()
#pragma bss_seg(".bss$BonusWinFlag")
    i32 BonusWinFlag;
#pragma bss_seg()
#pragma bss_seg(".bss$Arcade_Points")
    i32 Arcade_Points[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Arcade_Score")
    i32 Arcade_Score[2];
#pragma bss_seg()
#pragma bss_seg(".bss$BuildUpScale")
    f32 BuildUpScale;
#pragma bss_seg()
#pragma bss_seg(".bss$CoinTotalScale")
    f32 CoinTotalScale;
#pragma bss_seg()
#pragma bss_seg(".bss$DrawBuildUpTime")
    f32 DrawBuildUpTime;
#pragma bss_seg()
#pragma bss_seg(".bss$cointotal_x")
    f32 cointotal_x[2];
#pragma bss_seg()
#pragma bss_seg(".bss$MenuStopDraw")
    i32 MenuStopDraw;
#pragma bss_seg()
#pragma bss_seg(".bss$MenuValidated")
    i32 MenuValidated;
#pragma bss_seg()
#pragma bss_seg(".bss$MenuAlpha")
    f32 MenuAlpha;
#pragma bss_seg()
#pragma bss_seg(".bss$MenuA")
    i32 MenuA;
#pragma bss_seg()
#pragma bss_seg(".bss$CurrentMenuId")
    i32 CurrentMenuId;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainChatterWaitPc")
    f32 TerrainChatterWaitPc;
#pragma bss_seg()
#pragma bss_seg(".bss$DoubleScoreTime")
    f32 DoubleScoreTime;
#pragma bss_seg()
#pragma bss_seg(".bss$LookAtBoth")
    i32 LookAtBoth;
#pragma bss_seg()
#pragma bss_seg(".bss$SuperStoryScore")
    u32 SuperStoryScore;
#pragma bss_seg()
#pragma bss_seg(".bss$BigJump_EndOfLandFn")
    void(__cdecl *BigJump_EndOfLandFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$FindSlamOrigin_UseCPosFn")
    i32(__cdecl *FindSlamOrigin_UseCPosFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$AddCoinDelay")
    f32 AddCoinDelay[2];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$alert_time")
    f32 alert_time = 10.0f;
#pragma data_seg()
#pragma bss_seg(".bss$alert_pos")
    NUVEC alert_pos;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PickupTypeLiteral758c00")
    char PickupTypeLiteral758c00[9] = "Power Up";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c0c")
    char PickupTypeLiteral758c0c[8] = "Torpedo";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c14")
    char PickupTypeLiteral758c14[10] = "MK-Pickup";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c20")
    char PickupTypeLiteral758c20[8] = "Charkit";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c28")
    char PickupTypeLiteral758c28[10] = "Red Brick";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c34")
    char PickupTypeLiteral758c34[6] = "Heart";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c3c")
    char PickupTypeLiteral758c3c[10] = "MK-Appear";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c48")
    char PickupTypeLiteral758c48[8] = "Minikit";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c50")
    char PickupTypeLiteral758c50[12] = "Purple Coin";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c5c")
    char PickupTypeLiteral758c5c[12] = "PickupCoinB";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c68")
    char PickupTypeLiteral758c68[10] = "Blue Coin";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c74")
    char PickupTypeLiteral758c74[10] = "Gold Coin";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c80")
    char PickupTypeLiteral758c80[12] = "Silver Coin";
#pragma data_seg()
#pragma data_seg(".data$GizmoPickupType")
    GIZMO_PICKUP_TYPE_PC GizmoPickupType[10] = {
        {PickupTypeLiteral758c80, NULL, NULL, 115, 4, 64, 0, 183, 10, 0.04500000178813934f, 0.04500000178813934f, 10.0f,
         0.0f, NULL, Pup_CollectCoin, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c74, NULL, NULL, 103, 4, 64, 0, 191, 100, 0.04500000178813934f, 0.04500000178813934f,
         10.0f, 0.0f, NULL, Pup_CollectCoin, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c68, PickupTypeLiteral758c5c, NULL, 98, 4, 64, 0, 199, 1000, 0.04500000178813934f,
         0.04500000178813934f, 6.0f, 0.0f, Pup_UpdateBlueCoin, Pup_CollectCoin, 8, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c50, PickupTypeLiteral758c5c, NULL, 112, 4, 64, 0, 213, 10000, 0.04500000178813934f,
         0.04500000178813934f, 6.0f, 0.0f, Pup_UpdatePurpleCoin, Pup_CollectCoin, 8, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c48, PickupTypeLiteral758c3c, NULL, 109, 0, 69, 0, 206, 0, 0.125f, 0.15000000596046448f,
         2.0f, 0.05000000074505806f, NULL, Pup_CollectMinikit, 20, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c34, NULL, NULL, 104, 0, 2, 0, 203, 0, 0.10000000149011612f, 0.07999999821186066f, 4.0f,
         0.0f, NULL, Pup_CollectHeart, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c28, PickupTypeLiteral758c3c, NULL, 114, 0, 73, 0, 210, 0, 0.11999999731779099f,
         0.05999999865889549f, 2.0f, 0.10000000149011612f, NULL, Pup_CollectRedBrick, 99, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c20, PickupTypeLiteral758c3c, PickupTypeLiteral758c14, 99, 0, 33, 0, 207, 0, 0.125f,
         0.15000000596046448f, 2.0f, 0.05000000074505806f, NULL, Pup_CollectCharKit, 20, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c0c, NULL, NULL, 116, 0, 2, 0, 121, 0, 0.10000000149011612f, 0.10000000149011612f, 5.0f,
         0.0f, NULL, NULL, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c00, PickupTypeLiteral758c3c, NULL, 117, 0, 1, 0, 208, 0, 0.125f, 0.125f, 2.0f,
         0.05000000074505806f, Pup_UpdatePowerUp, Pup_CollectPowerUp, -1, 209, 0.0f, 0.0f}};
#pragma data_seg()
#pragma data_seg(".data$POWERUPYPOS1")
    f32 POWERUPYPOS1 = -0.33500000834465027f;
#pragma data_seg()
#pragma data_seg(".data$POWERUPYPOS2")
    f32 POWERUPYPOS2 = -0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$CHEAT_POWERUPTIME")
    f32 CHEAT_POWERUPTIME = 20.0f;
#pragma data_seg()
#pragma bss_seg(".bss$PowerUp_PanelPosMul")
    f32 PowerUp_PanelPosMul[2];
#pragma bss_seg()
#pragma bss_seg(".bss$POWERUP_TEXTID")
    i32 POWERUP_TEXTID;
#pragma bss_seg()
#pragma bss_seg(".bss$hintsys")
    HINTSYS_PC hintsys;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$KITPOSX")
    f32 KITPOSX = -0.7250000238418579f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KITPOSY")
    f32 KITPOSY = -0.699999988079071f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PANEL_MINIKITSCALE")
    f32 PANEL_MINIKITSCALE = 0.25f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PANEL_MINIKITY")
    f32 PANEL_MINIKITY = 0.029999999329447746f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$DrawMiniKitTime")
    f32 DrawMiniKitTime;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$MiniKitScale")
    f32 MiniKitScale;
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$MiniKitModelId_PC")
    i32 MiniKitModelId_PC = -1;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$CharKitModelId_PC")
    i32 CharKitModelId_PC = -1;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$NewMiniPiece")
    NEW_MINI_PIECE_PC NewMiniPiece[10];
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$PANEL_REDBRICKSCALE")
    f32 PANEL_REDBRICKSCALE = 0.4000000059604645f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$REDBRICKPOSX")
    f32 REDBRICKPOSX = 0.7250000238418579f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$REDBRICKPOSY")
    f32 REDBRICKPOSY = -0.7450000047683716f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$DrawRedBrickTime")
    f32 DrawRedBrickTime;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$RedBrickScale")
    f32 RedBrickScale;
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$ICONX")
    f32 ICONX = 0.7749999761581421f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$POWERUPOBJSIZE")
    f32 POWERUPOBJSIZE = 0.25f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$PowerUp_PanelYRot")
    u16 PowerUp_PanelYRot[2];
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$PartExtraCoinRate_PC")
    f32 PartExtraCoinRate_PC = 5.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartLifetime_PC")
    f32 PartVehicleHeartLifetime_PC = 1.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartPodraceHeight_PC")
    f32 PartVehicleHeartPodraceHeight_PC = 2.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartHeight_PC")
    f32 PartVehicleHeartHeight_PC = 3.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartPodraceScale_PC")
    f32 PartVehicleHeartPodraceScale_PC = 4.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartGunshipScale_PC")
    f32 PartVehicleHeartGunshipScale_PC = 7.5f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartScale_PC")
    f32 PartVehicleHeartScale_PC = 5.0f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$coinimpactwait")
    f32 coinimpactwait;
#pragma bss_seg()
}
extern "C" {
#pragma const_seg(".rdata$PowerUpBounceSfx_PC")
    extern const char PowerUpBounceSfx_PC[21] = "Gungan_BlueOrbBounce";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$PickupCoinSfx_PC")
    extern const char PickupCoinSfx_PC[11] = "PickupCoin";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$CoinDropSilverSfx_PC")
    extern const char CoinDropSilverSfx_PC[10] = "CoinDropS";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$CoinDropGoldSfx_PC")
    extern const char CoinDropGoldSfx_PC[10] = "CoinDropG";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$PowerUpLoopSfx_PC")
    extern const char PowerUpLoopSfx_PC[18] = "Grv_GuardWeaponLp";
#pragma const_seg()
}
extern "C" {
#pragma data_seg(".data$id_BOBAFETT")
    i16 id_BOBAFETT = -1;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$id_MINIDROIDEKA")
    i16 id_MINIDROIDEKA = -1;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$SARLACCPITA_LDATA")
    LEVELDATA_PC *SARLACCPITA_LDATA;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsPreviousVariantPc")
    i32 KillPartsPreviousVariantPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsZeroLightingPc")
    PART_LIGHTING_DATA_PC KillPartsZeroLightingPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsBrickImpactWaitPc")
    f32 KillPartsBrickImpactWaitPc;
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsLegoSingleTextPc")
    char KillPartsLegoSingleTextPc[11] = "LegoSingle";
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsCharSceneAreaPc")
    KILLPARTS_CHARSCENE_PC *KillPartsCharSceneAreaPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsAccessoriesPc")
    KILLPARTS_ACCESSORY_PC KillPartsAccessoriesPc[2][9];
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsBikesPc")
    KILLPARTS_BIKE_PC KillPartsBikesPc[4];
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieSpinsTextPc")
    char KillPartsTieSpinsTextPc[10] = "Tie_Spins";
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieSpinRatePc")
    i32 KillPartsTieSpinRatePc = 200000;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieTrailScalePc")
    f32 KillPartsTieTrailScalePc = 0.10000000149011612f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieSpinTranslationPc")
    NUVEC KillPartsTieSpinTranslationPc = {0.0f, 0.0f, -0.05000000074505806f};
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsAtatExplodeTextPc")
    char KillPartsAtatExplodeTextPc[9] = "EXPLODE1";
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$ExtraHurtSfxFn")
    void(__cdecl *ExtraHurtSfxFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$GizBuilditGDeb")
    i16 GizBuilditGDeb[6] = {-1, -1, -1, -1, -1, -1};
#pragma data_seg()
#pragma const_seg(".rdata$PickupMultiplierFormat")
    extern const char PickupMultiplierFormat[4] = "x%i";
#pragma const_seg()
#pragma bss_seg(".bss$DrawCoinTotalTime")
    f32 DrawCoinTotalTime;
#pragma bss_seg()
#pragma data_seg(".data$PANEL_COINSCALE_START")
    f32 PANEL_COINSCALE_START = 1.0f;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$global_windscale")
    f32 global_windscale = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$global_windspeed")
    f32 global_windspeed = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$PCWindFrameCount")
    u32 PCWindFrameCount;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCSpecialClipState")
    i32 PCSpecialClipState = -1;
#pragma data_seg()
#pragma bss_seg(".bss$PCSpecialActiveShadowLights")
    i32 PCSpecialActiveShadowLights;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialShadowClipResultsPresent")
    i32 PCSpecialShadowClipResultsPresent;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialForcedLOD")
    i32 PCSpecialForcedLOD;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialForcedMaterialTable")
    NUMTL **PCSpecialForcedMaterialTable;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialForcedMaterial")
    NUMTL *PCSpecialForcedMaterial;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialReflectionPass")
    i32 PCSpecialReflectionPass;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCSpecialProjectionAdjust")
    PC_SPECIAL_PROJECTION_ADJUST PCSpecialProjectionAdjust = {1.0f, 1.0f, 0.0f, 0.0f};
#pragma data_seg()
#pragma bss_seg(".bss$PCSpecialShadowLights")
    struct PC_SPECIAL_SHADOW_LIGHT *PCSpecialShadowLights[4];
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialShadowClipResults")
    u32 PCSpecialShadowClipResults[4];
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialProjectionTranslate")
    NUMTX PCSpecialProjectionTranslate;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialProjectionScale")
    NUMTX PCSpecialProjectionScale;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$SnakeSegmentHeightPc")
    f32 SnakeSegmentHeightPc = 0.019999999552965164f;
#pragma data_seg()
#pragma data_seg(".data$SnakeDebrisUpwardSpeedPc")
    f32 SnakeDebrisUpwardSpeedPc = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$PCMaterialPoolPc")
    NUMTL *PCMaterialPoolPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCMaterialPoolCountPc")
    i32 PCMaterialPoolCountPc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$ArcadeAiKilledSoundNamePc")
    extern const char ArcadeAiKilledSoundNamePc[16] = "env_padLight_on";
#pragma const_seg()
}

extern "C" {
#pragma const_seg(".rdata$PlayerBeenDeactivatedPc")
    extern const char PlayerBeenDeactivatedPc[16] = "BeenDeactivated";
#pragma const_seg()

#pragma const_seg(".rdata$ShieldHitSfxPc")
    extern const char ShieldHitSfxPc[9] = "DDekaHit";
#pragma const_seg()

#pragma const_seg(".rdata$ShieldFlickerSfxPc")
    extern const char ShieldFlickerSfxPc[13] = "DDekaFlicker";
#pragma const_seg()

#pragma data_seg(".data$id_GAMORREANGUARD")
    i16 id_GAMORREANGUARD = -1;
#pragma data_seg()

#pragma data_seg(".data$id_SNAKE")
    i16 id_SNAKE = -1;
#pragma data_seg()

#pragma data_seg(".data$id_WOOKIEE")
    i16 id_WOOKIEE = -1;
#pragma data_seg()

#pragma data_seg(".data$id_OBIWANKENOBIEP3")
    i16 id_OBIWANKENOBIEP3 = -1;
#pragma data_seg()

#pragma data_seg(".data$id_ANAKINJEDI")
    i16 id_ANAKINJEDI = -1;
#pragma data_seg()

#pragma data_seg(".data$id_ROYALGUARD")
    i16 id_ROYALGUARD = -1;
#pragma data_seg()

#pragma data_seg(".data$CombatNpcKillDifficultyAmountPc")
    f32 CombatNpcKillDifficultyAmountPc = 0.009999999776482582f;
#pragma data_seg()

#pragma data_seg(".data$CombatPlayerDeathDifficultyAmountPc")
    f32 CombatPlayerDeathDifficultyAmountPc = -0.02500000037252903f;
#pragma data_seg()

#pragma data_seg(".data$CombatHitReactionIntervalPc")
    i32 CombatHitReactionIntervalPc = 3;
#pragma data_seg()

#pragma data_seg(".data$CombatReactionDurationPc")
    f32 CombatReactionDurationPc = 3.0f;
#pragma data_seg()

#pragma data_seg(".data$objhit_damage_joints")
    u8 objhit_damage_joints[3] = {6, 8, 7};
#pragma data_seg()

#pragma data_seg(".data$SHIELDFLICKERTIME")
    f32 SHIELDFLICKERTIME = 0.10000000149011612f;
#pragma data_seg()

#pragma data_seg(".data$CombatReactionHorizontalScalePc")
    f32 CombatReactionHorizontalScalePc = 1.5f;
#pragma data_seg()

#pragma data_seg(".data$CombatReactionVerticalScalePc")
    f32 CombatReactionVerticalScalePc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_HOLD")
    i32 LEGOCONTEXT_HOLD = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_LAND_LUNGE")
    i32 LEGOCONTEXT_LAND_LUNGE = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_LAND_SLAM")
    i32 LEGOCONTEXT_LAND_SLAM = -1;
#pragma data_seg()

#pragma bss_seg(".bss$CRUISERC_LDATA")
    LEVELDATA_PC *CRUISERC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$builduptime")
    f32 builduptime;
#pragma bss_seg()

#pragma bss_seg(".bss$ObstacleCamHoldUntilPlayersMove")
    i32 ObstacleCamHoldUntilPlayersMove;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_bolt")
    BOLT *objhitobj_bolt;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_nohurtsfx")
    i32 objhitobj_nohurtsfx;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_noimpactsfx")
    i32 objhitobj_noimpactsfx;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_noattackerrumble")
    i32 objhitobj_noattackerrumble;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_throwkillpartsup")
    i32 objhitobj_throwkillpartsup;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_killparts_yrot")
    i16 *objhitobj_killparts_yrot;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$VehicleGunshipHoverBasePc")
    f32 VehicleGunshipHoverBasePc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipHoverOtherPc")
    f32 VehicleGunshipHoverOtherPc = 1.25f;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipHoverSeparationPc")
    f32 VehicleGunshipHoverSeparationPc = 0.800000011920929f;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$DynamicGrapplePc")
    GRAPPLE_DYNAMIC_PC DynamicGrapplePc[4];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_WEAPONIN")
    i32 LEGOCONTEXT_WEAPONIN = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_WEAPONOUT")
    i32 LEGOCONTEXT_WEAPONOUT = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$TractorBeamEnabledPc")
    u8 TractorBeamEnabledPc;
#pragma bss_seg()

#pragma data_seg(".data$DisorientAxisPc")
    u8 DisorientAxisPc = 7;
#pragma data_seg()

#pragma bss_seg(".bss$WeaponInOut_NoAIJediSfx")
    i32 WeaponInOut_NoAIJediSfx;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$VehicleLoopTimePc")
    f32 VehicleLoopTimePc = 1.5f;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$GizmoBlowUp_NoTargetFn")
    i32(__cdecl *GizmoBlowUp_NoTargetFn)(WORLDINFO *, GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$Fighting_WeaponInActionFn")
    i32(__cdecl *Fighting_WeaponInActionFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$Fighting_WeaponOutActionFn")
    i32(__cdecl *Fighting_WeaponOutActionFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
    extern i16 id_CATAPULT, id_FIRETRUCK;
#pragma data_seg(".data$AwkwardShapeSpherePc")
    AWKWARD_SHAPE_SPHERE_PC AwkwardShapeSpherePc[14] = {{&id_RANCOR, 4, 0, 0.30000001192092896f},
                                                        {&id_DEWBACK, 1, 1, 0.20000000298023224f},
                                                        {&id_DEWBACK, 2, 0, 0.125f},
                                                        {&id_DEWBACK, 3, 0, 0.10000000149011612f},
                                                        {&id_DEWBACK, 4, 0, 0.07500000298023224f},
                                                        {&id_BIGGUN, 3, 0, 0.17499999701976776f},
                                                        {&id_BIGGUN, 4, 0, 0.17499999701976776f},
                                                        {&id_BIGGUN, 5, 0, 0.17499999701976776f},
                                                        {&id_BANTHA, 1, 1, 0.25f},
                                                        {&id_ATAT, 7, 1, 1.2000000476837158f},
                                                        {&id_CATAPULT, 1, 0, 0.25f},
                                                        {&id_FIRETRUCK, 1, 1, 0.30000001192092896f},
                                                        {&id_FIRETRUCK, 2, 1, 0.30000001192092896f},
                                                        {NULL, 0, 0, 0.0f}};
#pragma data_seg()

#pragma data_seg(".data$id_CATAPULT")
    i16 id_CATAPULT = -1;
#pragma data_seg()

#pragma data_seg(".data$id_FIRETRUCK")
    i16 id_FIRETRUCK = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$PCMusicDuckOverride")
    f32 PCMusicDuckOverride = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$PCStereoChannel0B")
    i32 PCStereoChannel0B = 1;
#pragma data_seg()

#pragma data_seg(".data$PCStereoChannel1A")
    i32 PCStereoChannel1A = 2;
#pragma data_seg()

#pragma data_seg(".data$PCStereoChannel1B")
    i32 PCStereoChannel1B = 3;
#pragma data_seg()

#pragma bss_seg(".bss$PCStereoSampleMetadata")
    PC_STEREO_SAMPLE_METADATA *PCStereoSampleMetadata;
#pragma bss_seg()

#pragma bss_seg(".bss$PCStereoChannel0A")
    i32 PCStereoChannel0A;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PossibleForceTargetsPc")
    // Selected input view: cumulative eligible writes <=384; actual PC capacity unknown.
    GIZFORCE_TARGET_RECORD_PC PossibleForceTargetsPc[384];
#pragma bss_seg()

#pragma bss_seg(".bss$LevelTimer")
    TIMER LevelTimer;
#pragma bss_seg()
}

extern "C" {

#pragma data_seg(".data$VehicleBoltLevelObjectIndexPc")
    i16 VehicleBoltLevelObjectIndexPc = 52;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f17d0Pc")
    i16 VehicleBoltId7f17d0Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f17d4Pc")
    i16 VehicleBoltId7f17d4Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f18b4Pc")
    i16 VehicleBoltId7f18b4Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f190cPc")
    i16 VehicleBoltId7f190cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1910Pc")
    i16 VehicleBoltId7f1910Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1924Pc")
    i16 VehicleBoltId7f1924Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1928Pc")
    i16 VehicleBoltId7f1928Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f195cPc")
    i16 VehicleBoltId7f195cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f196cPc")
    i16 VehicleBoltId7f196cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_JAWA")
    i16 id_JAWA = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f19a4Pc")
    i16 VehicleBoltId7f19a4Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f19a8Pc")
    i16 VehicleBoltId7f19a8Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f19e8Pc")
    i16 VehicleBoltId7f19e8Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_REPUBLICGUNSHIP")
    i16 id_REPUBLICGUNSHIP = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a24Pc")
    i16 VehicleBoltId7f1a24Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a28Pc")
    i16 VehicleBoltId7f1a28Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a2cPc")
    i16 VehicleBoltId7f1a2cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_SENTRYDROID")
    i16 id_SENTRYDROID = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a38Pc")
    i16 VehicleBoltId7f1a38Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a9cPc")
    i16 VehicleBoltId7f1a9cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1aa0Pc")
    i16 VehicleBoltId7f1aa0Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1aacPc")
    i16 VehicleBoltId7f1aacPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleSpecialShooterIdPc")
    i16 VehicleSpecialShooterIdPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1ad8Pc")
    i16 VehicleBoltId7f1ad8Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1aecPc")
    i16 VehicleBoltId7f1aecPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1af0Pc")
    i16 VehicleBoltId7f1af0Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b00Pc")
    i16 VehicleBoltId7f1b00Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b04Pc")
    i16 VehicleBoltId7f1b04Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b08Pc")
    i16 VehicleBoltId7f1b08Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b0cPc")
    i16 VehicleBoltId7f1b0cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b34Pc")
    i16 VehicleBoltId7f1b34Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b38Pc")
    i16 VehicleBoltId7f1b38Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_ZAMSSPEEDER")
    i16 id_ZAMSSPEEDER = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleADefaultPc")
    i32 VehicleGunshipAngleADefaultPc = 8;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleAPlayer1Pc")
    i32 VehicleGunshipAngleAPlayer1Pc = 10;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleAPlayer2Pc")
    i32 VehicleGunshipAngleAPlayer2Pc = 5;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleBDefaultPc")
    i32 VehicleGunshipAngleBDefaultPc = 3;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleBPlayer1Pc")
    i32 VehicleGunshipAngleBPlayer1Pc = 4;
#pragma data_seg()

#pragma data_seg(".data$DEACTIVATEDTIME")
    f32 DEACTIVATEDTIME = 8.0f;
#pragma data_seg()

#pragma bss_seg(".bss$GUNSHIPB_LDATA")
    LEVELDATA_PC *GUNSHIPB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ForcePush_SuperMindTrick")
    i32 ForcePush_SuperMindTrick;
#pragma bss_seg()

#pragma bss_seg(".bss$VehicleGunshipAngleBPlayer2Pc")
    i32 VehicleGunshipAngleBPlayer2Pc;
#pragma bss_seg()

#pragma bss_seg(".bss$BOLT_OVERRIDE_PLAYERBOLTSPEED")
    f32 BOLT_OVERRIDE_PLAYERBOLTSPEED;
#pragma bss_seg()

#pragma bss_seg(".bss$BOLT_OVERRIDE_PLAYERBOLTDURATION")
    f32 BOLT_OVERRIDE_PLAYERBOLTDURATION;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCShadowSubmissionEnabled")
    i32 PCShadowSubmissionEnabled = 1;
#pragma data_seg()

#pragma data_seg(".data$PCShadowSurfaceSamplingEnabled")
    i32 PCShadowSurfaceSamplingEnabled = 1;
#pragma data_seg()

#pragma bss_seg(".bss$PCShadowPackets")
    PC_HG_SHADOW_PACKET PCShadowPackets[280];
#pragma bss_seg()

#pragma bss_seg(".bss$PCSpecialForceMaterialCount")
    i32 PCSpecialForceMaterialCount;
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneBoundingBoxTrackRoot")
    u8 CutSceneBoundingBoxTrackRoot;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowPacketSequence")
    i32 PCShadowPacketSequence;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowNearValue")
    f32 PCShadowNearValue;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowFarValue")
    f32 PCShadowFarValue;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowPacketCount")
    i32 PCShadowPacketCount;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowState")
    PC_HG_SHADOW_STATE_VIEW PCShadowState;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_nobsa")
    i32 drawcharactermodel_nobsa;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_noani")
    i32 drawcharactermodel_noani;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_restpose")
    i32 drawcharactermodel_restpose;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_keepmergeaction")
    i32 drawcharactermodel_keepmergeaction;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_locatorsupdated")
    i32 drawcharactermodel_locatorsupdated;
#pragma bss_seg()

#pragma bss_seg(".bss$TempNumJoints")
    u8 TempNumJoints;
#pragma bss_seg()

#pragma bss_seg(".bss$notransparentchardraw")
    i32 notransparentchardraw;
#pragma bss_seg()

#pragma bss_seg(".bss$APIObjResetShadowMapRenderingFn")
    void(__cdecl *APIObjResetShadowMapRenderingFn)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$APIObjEnableShadowMapRenderingFn")
    void(__cdecl *APIObjEnableShadowMapRenderingFn)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$Temphgobj")
    PC_HG_RENDER_VIEW *Temphgobj;
#pragma bss_seg()

#pragma bss_seg(".bss$APITrans_Mtl")
    NUMTL *APITrans_Mtl[2];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$CUTSUBTITLEDEFAULT_R")
    u8 CUTSUBTITLEDEFAULT_R = 255;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_G")
    u8 CUTSUBTITLEDEFAULT_G = 255;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_B")
    u8 CUTSUBTITLEDEFAULT_B = 255;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_A")
    u8 CUTSUBTITLEDEFAULT_A = 128;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_Y")
    f32 CUTSUBTITLEDEFAULT_Y = -0.5f;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_SCALEX")
    f32 CUTSUBTITLEDEFAULT_SCALEX = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_SCALEY")
    f32 CUTSUBTITLEDEFAULT_SCALEY = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_FITWIDTH")
    f32 CUTSUBTITLEDEFAULT_FITWIDTH = 1.7000000476837158f;
#pragma data_seg()

#pragma bss_seg(".bss$CS_area")
    i32 CS_area;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_fadefogcount")
    i32 CS_fadefogcount;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_texanimcount")
    i32 CS_texanimcount;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_buffptr")
    VARIPTR *CS_buffptr;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_CutInfo")
    CUT_LOAD_RECORD_PC *CS_CutInfo;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_worldinfo")
    WORLDINFO *CS_worldinfo;
#pragma bss_seg()

#pragma bss_seg(".bss$CUTSUBTITLEDEFAULT_ALIGN")
    u8 CUTSUBTITLEDEFAULT_ALIGN;
#pragma bss_seg()

#pragma bss_seg(".bss$CUTSUBTITLEDEFAULT_X")
    f32 CUTSUBTITLEDEFAULT_X;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$TextInputKeyNameTextPc7516c0")
    char TextInputKeyNameTextPc7516c0[1] = "";
#pragma bss_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc753370")
    char TextInputKeyNameTextPc753370[2] = "]";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc753374")
    char TextInputKeyNameTextPc753374[2] = "[";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc759770")
    char TextInputKeyNameTextPc759770[2] = "X";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc75ac34")
    char TextInputKeyNameTextPc75ac34[5] = "Down";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc76fde8")
    char TextInputKeyNameTextPc76fde8[7] = "Delete";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719bc")
    char TextInputKeyNameTextPc7719bc[12] = "MEDIASELECT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719c8")
    char TextInputKeyNameTextPc7719c8[5] = "MAIL";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719d0")
    char TextInputKeyNameTextPc7719d0[11] = "MYCOMPUTER";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719dc")
    char TextInputKeyNameTextPc7719dc[8] = "WEBBACK";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719e4")
    char TextInputKeyNameTextPc7719e4[11] = "WEBFORWARD";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719f0")
    char TextInputKeyNameTextPc7719f0[8] = "WEBSTOP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719f8")
    char TextInputKeyNameTextPc7719f8[11] = "WEBREFRESH";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a04")
    char TextInputKeyNameTextPc771a04[13] = "WEBFAVORITES";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a14")
    char TextInputKeyNameTextPc771a14[10] = "WEBSEARCH";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a20")
    char TextInputKeyNameTextPc771a20[5] = "WAKE";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a28")
    char TextInputKeyNameTextPc771a28[6] = "SLEEP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a30")
    char TextInputKeyNameTextPc771a30[6] = "POWER";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a38")
    char TextInputKeyNameTextPc771a38[5] = "APPS";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a40")
    char TextInputKeyNameTextPc771a40[7] = "R. Win";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a48")
    char TextInputKeyNameTextPc771a48[7] = "L. Win";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a50")
    char TextInputKeyNameTextPc771a50[7] = "Insert";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a58")
    char TextInputKeyNameTextPc771a58[9] = "Pg. Down";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a64")
    char TextInputKeyNameTextPc771a64[4] = "End";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a68")
    char TextInputKeyNameTextPc771a68[6] = "Right";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a70")
    char TextInputKeyNameTextPc771a70[5] = "Left";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a78")
    char TextInputKeyNameTextPc771a78[7] = "Pg. Up";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a80")
    char TextInputKeyNameTextPc771a80[3] = "Up";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a84")
    char TextInputKeyNameTextPc771a84[5] = "Home";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a8c")
    char TextInputKeyNameTextPc771a8c[6] = "Pause";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a94")
    char TextInputKeyNameTextPc771a94[7] = "R. Alt";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a9c")
    char TextInputKeyNameTextPc771a9c[6] = "SysRq";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771aa4")
    char TextInputKeyNameTextPc771aa4[7] = "Num. /";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771aac")
    char TextInputKeyNameTextPc771aac[11] = "Num. COMMA";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ab8")
    char TextInputKeyNameTextPc771ab8[8] = "WEBHOME";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ac0")
    char TextInputKeyNameTextPc771ac0[6] = "VOLUP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ac8")
    char TextInputKeyNameTextPc771ac8[8] = "VOLDOWN";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ad0")
    char TextInputKeyNameTextPc771ad0[10] = "MEDIASTOP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771adc")
    char TextInputKeyNameTextPc771adc[5] = "PLAY";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ae4")
    char TextInputKeyNameTextPc771ae4[5] = "CALC";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771aec")
    char TextInputKeyNameTextPc771aec[5] = "MUTE";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771af4")
    char TextInputKeyNameTextPc771af4[8] = "R. Ctrl";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771afc")
    char TextInputKeyNameTextPc771afc[11] = "Num. Enter";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b08")
    char TextInputKeyNameTextPc771b08[10] = "NEXTTRACK";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b14")
    char TextInputKeyNameTextPc771b14[10] = "UNLABELED";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b20")
    char TextInputKeyNameTextPc771b20[3] = "AX";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b24")
    char TextInputKeyNameTextPc771b24[5] = "STOP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b2c")
    char TextInputKeyNameTextPc771b2c[6] = "KANJI";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b34")
    char TextInputKeyNameTextPc771b34[10] = "UNDERLINE";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b40")
    char TextInputKeyNameTextPc771b40[6] = "COLON";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b48")
    char TextInputKeyNameTextPc771b48[3] = "AT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b4c")
    char TextInputKeyNameTextPc771b4c[10] = "PREVTRACK";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b58")
    char TextInputKeyNameTextPc771b58[7] = "Num. =";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b60")
    char TextInputKeyNameTextPc771b60[8] = "ABNT_C2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b68")
    char TextInputKeyNameTextPc771b68[4] = "YEN";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b6c")
    char TextInputKeyNameTextPc771b6c[10] = "NOCONVERT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b78")
    char TextInputKeyNameTextPc771b78[8] = "CONVERT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b80")
    char TextInputKeyNameTextPc771b80[8] = "ABNT_C1";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b88")
    char TextInputKeyNameTextPc771b88[5] = "KANA";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b90")
    char TextInputKeyNameTextPc771b90[4] = "F15";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b94")
    char TextInputKeyNameTextPc771b94[4] = "F14";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b98")
    char TextInputKeyNameTextPc771b98[4] = "F13";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b9c")
    char TextInputKeyNameTextPc771b9c[4] = "F12";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ba0")
    char TextInputKeyNameTextPc771ba0[4] = "F11";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ba4")
    char TextInputKeyNameTextPc771ba4[11] = "Num. Point";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bb0")
    char TextInputKeyNameTextPc771bb0[7] = "Num. 0";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bb8")
    char TextInputKeyNameTextPc771bb8[7] = "Num. 3";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bc0")
    char TextInputKeyNameTextPc771bc0[7] = "Num. 2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bc8")
    char TextInputKeyNameTextPc771bc8[7] = "Num. 1";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bd0")
    char TextInputKeyNameTextPc771bd0[7] = "Num. +";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bd8")
    char TextInputKeyNameTextPc771bd8[7] = "Num. 6";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771be0")
    char TextInputKeyNameTextPc771be0[7] = "Num. 5";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771be8")
    char TextInputKeyNameTextPc771be8[7] = "Num. 4";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bf0")
    char TextInputKeyNameTextPc771bf0[7] = "Num. -";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bf8")
    char TextInputKeyNameTextPc771bf8[7] = "Num. 9";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c00")
    char TextInputKeyNameTextPc771c00[7] = "Num. 8";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c08")
    char TextInputKeyNameTextPc771c08[7] = "Num. 7";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c10")
    char TextInputKeyNameTextPc771c10[12] = "Scroll Lock";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c1c")
    char TextInputKeyNameTextPc771c1c[9] = "Num Lock";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c28")
    char TextInputKeyNameTextPc771c28[4] = "F10";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c2c")
    char TextInputKeyNameTextPc771c2c[3] = "F9";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c30")
    char TextInputKeyNameTextPc771c30[3] = "F8";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c34")
    char TextInputKeyNameTextPc771c34[3] = "F7";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c38")
    char TextInputKeyNameTextPc771c38[3] = "F6";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c3c")
    char TextInputKeyNameTextPc771c3c[3] = "F5";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c40")
    char TextInputKeyNameTextPc771c40[3] = "F4";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c44")
    char TextInputKeyNameTextPc771c44[3] = "F3";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c48")
    char TextInputKeyNameTextPc771c48[3] = "F2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c4c")
    char TextInputKeyNameTextPc771c4c[3] = "F1";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c50")
    char TextInputKeyNameTextPc771c50[10] = "Caps Lock";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c5c")
    char TextInputKeyNameTextPc771c5c[6] = "Space";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c64")
    char TextInputKeyNameTextPc771c64[7] = "L. Alt";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c6c")
    char TextInputKeyNameTextPc771c6c[7] = "Num. *";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c74")
    char TextInputKeyNameTextPc771c74[9] = "R. Shift";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c80")
    char TextInputKeyNameTextPc771c80[11] = "For. Slash";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c8c")
    char TextInputKeyNameTextPc771c8c[5] = "Stop";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c94")
    char TextInputKeyNameTextPc771c94[6] = "Comma";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c9c")
    char TextInputKeyNameTextPc771c9c[2] = "M";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ca0")
    char TextInputKeyNameTextPc771ca0[2] = "N";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ca4")
    char TextInputKeyNameTextPc771ca4[2] = "B";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ca8")
    char TextInputKeyNameTextPc771ca8[2] = "V";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cac")
    char TextInputKeyNameTextPc771cac[2] = "C";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cb0")
    char TextInputKeyNameTextPc771cb0[2] = "#";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cb4")
    char TextInputKeyNameTextPc771cb4[9] = "L. Shift";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cc0")
    char TextInputKeyNameTextPc771cc0[6] = "Grave";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cc8")
    char TextInputKeyNameTextPc771cc8[11] = "Apostrophe";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cd4")
    char TextInputKeyNameTextPc771cd4[10] = "Semicolon";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ce0")
    char TextInputKeyNameTextPc771ce0[2] = "L";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ce4")
    char TextInputKeyNameTextPc771ce4[2] = "K";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ce8")
    char TextInputKeyNameTextPc771ce8[2] = "J";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cec")
    char TextInputKeyNameTextPc771cec[2] = "H";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cf0")
    char TextInputKeyNameTextPc771cf0[2] = "G";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cf4")
    char TextInputKeyNameTextPc771cf4[2] = "F";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cf8")
    char TextInputKeyNameTextPc771cf8[2] = "D";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cfc")
    char TextInputKeyNameTextPc771cfc[2] = "S";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d00")
    char TextInputKeyNameTextPc771d00[2] = "A";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d04")
    char TextInputKeyNameTextPc771d04[8] = "L. Ctrl";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d0c")
    char TextInputKeyNameTextPc771d0c[6] = "Enter";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d14")
    char TextInputKeyNameTextPc771d14[2] = "P";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d18")
    char TextInputKeyNameTextPc771d18[2] = "O";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d1c")
    char TextInputKeyNameTextPc771d1c[2] = "I";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d20")
    char TextInputKeyNameTextPc771d20[2] = "U";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d24")
    char TextInputKeyNameTextPc771d24[2] = "T";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d28")
    char TextInputKeyNameTextPc771d28[2] = "R";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d2c")
    char TextInputKeyNameTextPc771d2c[2] = "E";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d30")
    char TextInputKeyNameTextPc771d30[2] = "W";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d34")
    char TextInputKeyNameTextPc771d34[2] = "Q";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d38")
    char TextInputKeyNameTextPc771d38[4] = "Tab";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d3c")
    char TextInputKeyNameTextPc771d3c[10] = "Backspace";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d48")
    char TextInputKeyNameTextPc771d48[7] = "Equals";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d50")
    char TextInputKeyNameTextPc771d50[6] = "Minus";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d58")
    char TextInputKeyNameTextPc771d58[2] = "0";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d5c")
    char TextInputKeyNameTextPc771d5c[2] = "9";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d60")
    char TextInputKeyNameTextPc771d60[2] = "8";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d64")
    char TextInputKeyNameTextPc771d64[2] = "7";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d68")
    char TextInputKeyNameTextPc771d68[2] = "6";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d6c")
    char TextInputKeyNameTextPc771d6c[2] = "5";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d70")
    char TextInputKeyNameTextPc771d70[2] = "4";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d74")
    char TextInputKeyNameTextPc771d74[2] = "3";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d78")
    char TextInputKeyNameTextPc771d78[2] = "2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d7c")
    char TextInputKeyNameTextPc771d7c[4] = "Esc";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d80")
    char TextInputKeyNameTextPc771d80[8] = "(empty)";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc774180")
    char TextInputKeyNameTextPc774180[2] = "Z";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc774184")
    char TextInputKeyNameTextPc774184[2] = "Y";
#pragma data_seg()

#pragma data_seg(".data$QFONTSCALEY")
    f32 QFONTSCALEY = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$text_preset_r")
    u8 text_preset_r[10] = {128, 128, 0, 0, 0, 128, 128, 128, 0, 0};
#pragma data_seg()

#pragma data_seg(".data$text_preset_g")
    u8 text_preset_g[10] = {128, 0, 128, 0, 128, 0, 128, 96, 0, 0};
#pragma data_seg()

#pragma data_seg(".data$text_preset_b")
    u8 text_preset_b[10] = {128, 0, 0, 128, 128, 128, 0, 0, 0, 0};
#pragma data_seg()

#pragma data_seg(".data$current_rgba")
    u32 current_rgba = 2155905152;
#pragma data_seg()

#pragma data_seg(".data$apitext_half_rgb")
    i32 apitext_half_rgb = 1;
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTablePc")
    char *TextInputKeyNameTablePc[238] = {
        TextInputKeyNameTextPc771d80, TextInputKeyNameTextPc771d7c, "1",
        TextInputKeyNameTextPc771d78, TextInputKeyNameTextPc771d74, TextInputKeyNameTextPc771d70,
        TextInputKeyNameTextPc771d6c, TextInputKeyNameTextPc771d68, TextInputKeyNameTextPc771d64,
        TextInputKeyNameTextPc771d60, TextInputKeyNameTextPc771d5c, TextInputKeyNameTextPc771d58,
        TextInputKeyNameTextPc771d50, TextInputKeyNameTextPc771d48, TextInputKeyNameTextPc771d3c,
        TextInputKeyNameTextPc771d38, TextInputKeyNameTextPc771d34, TextInputKeyNameTextPc771d30,
        TextInputKeyNameTextPc771d2c, TextInputKeyNameTextPc771d28, TextInputKeyNameTextPc771d24,
        TextInputKeyNameTextPc774184, TextInputKeyNameTextPc771d20, TextInputKeyNameTextPc771d1c,
        TextInputKeyNameTextPc771d18, TextInputKeyNameTextPc771d14, TextInputKeyNameTextPc753374,
        TextInputKeyNameTextPc753370, TextInputKeyNameTextPc771d0c, TextInputKeyNameTextPc771d04,
        TextInputKeyNameTextPc771d00, TextInputKeyNameTextPc771cfc, TextInputKeyNameTextPc771cf8,
        TextInputKeyNameTextPc771cf4, TextInputKeyNameTextPc771cf0, TextInputKeyNameTextPc771cec,
        TextInputKeyNameTextPc771ce8, TextInputKeyNameTextPc771ce4, TextInputKeyNameTextPc771ce0,
        TextInputKeyNameTextPc771cd4, TextInputKeyNameTextPc771cc8, TextInputKeyNameTextPc771cc0,
        TextInputKeyNameTextPc771cb4, TextInputKeyNameTextPc771cb0, TextInputKeyNameTextPc774180,
        TextInputKeyNameTextPc759770, TextInputKeyNameTextPc771cac, TextInputKeyNameTextPc771ca8,
        TextInputKeyNameTextPc771ca4, TextInputKeyNameTextPc771ca0, TextInputKeyNameTextPc771c9c,
        TextInputKeyNameTextPc771c94, TextInputKeyNameTextPc771c8c, TextInputKeyNameTextPc771c80,
        TextInputKeyNameTextPc771c74, TextInputKeyNameTextPc771c6c, TextInputKeyNameTextPc771c64,
        TextInputKeyNameTextPc771c5c, TextInputKeyNameTextPc771c50, TextInputKeyNameTextPc771c4c,
        TextInputKeyNameTextPc771c48, TextInputKeyNameTextPc771c44, TextInputKeyNameTextPc771c40,
        TextInputKeyNameTextPc771c3c, TextInputKeyNameTextPc771c38, TextInputKeyNameTextPc771c34,
        TextInputKeyNameTextPc771c30, TextInputKeyNameTextPc771c2c, TextInputKeyNameTextPc771c28,
        TextInputKeyNameTextPc771c1c, TextInputKeyNameTextPc771c10, TextInputKeyNameTextPc771c08,
        TextInputKeyNameTextPc771c00, TextInputKeyNameTextPc771bf8, TextInputKeyNameTextPc771bf0,
        TextInputKeyNameTextPc771be8, TextInputKeyNameTextPc771be0, TextInputKeyNameTextPc771bd8,
        TextInputKeyNameTextPc771bd0, TextInputKeyNameTextPc771bc8, TextInputKeyNameTextPc771bc0,
        TextInputKeyNameTextPc771bb8, TextInputKeyNameTextPc771bb0, TextInputKeyNameTextPc771ba4,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, (char *)WorldPathSeparator,
        TextInputKeyNameTextPc771ba0, TextInputKeyNameTextPc771b9c, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b98, TextInputKeyNameTextPc771b94,
        TextInputKeyNameTextPc771b90, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b88, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b80, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b78, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b6c, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b68,
        TextInputKeyNameTextPc771b60, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b58, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b4c, TextInputKeyNameTextPc771b48, TextInputKeyNameTextPc771b40,
        TextInputKeyNameTextPc771b34, TextInputKeyNameTextPc771b2c, TextInputKeyNameTextPc771b24,
        TextInputKeyNameTextPc771b20, TextInputKeyNameTextPc771b14, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b08, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771afc, TextInputKeyNameTextPc771af4, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771aec, TextInputKeyNameTextPc771ae4,
        TextInputKeyNameTextPc771adc, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ad0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771ac8, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ac0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ab8, TextInputKeyNameTextPc771aac,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771aa4, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771a9c, TextInputKeyNameTextPc771a94, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a8c,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a84, TextInputKeyNameTextPc771a80,
        TextInputKeyNameTextPc771a78, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a70,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a68, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771a64, TextInputKeyNameTextPc75ac34, TextInputKeyNameTextPc771a58,
        TextInputKeyNameTextPc771a50, TextInputKeyNameTextPc76fde8, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771a48, TextInputKeyNameTextPc771a40, TextInputKeyNameTextPc771a38,
        TextInputKeyNameTextPc771a30, TextInputKeyNameTextPc771a28, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a20,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a14, TextInputKeyNameTextPc771a04,
        TextInputKeyNameTextPc7719f8, TextInputKeyNameTextPc7719f0, TextInputKeyNameTextPc7719e4,
        TextInputKeyNameTextPc7719dc, TextInputKeyNameTextPc7719d0, TextInputKeyNameTextPc7719c8,
        TextInputKeyNameTextPc7719bc};
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNamesPc")
    char **TextInputKeyNamesPc = TextInputKeyNameTablePc;
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyboardOverridePc")
    u8 TextInputKeyboardOverridePc[32] = {0, 0, 0, 0, 0, 0, 0, 0, 200, 205, 208, 203, 60, 1, 28, 59,
                                          0, 0, 0, 0, 0, 0, 0, 0, 20,  0,   0,   0,   0,  0, 0,  0};
#pragma data_seg()

#pragma data_seg(".data$TextInputAxisLabelIndexPc")
    i8 TextInputAxisLabelIndexPc[16] = {14, 15, 16, 17, 0, 0, 18, 19, 20, 21, 0, 0, 28, 28, 29, 29};
#pragma data_seg()

#pragma data_seg(".data$TextInputButtonLabelIndexPc")
    i8 TextInputButtonLabelIndexPc[11] = {24, 23, 25, 22, 26, 27, 33, 32, 30, 31, 0};
#pragma data_seg()

#pragma bss_seg(".bss$text3d_height")
    f32 text3d_height;
#pragma bss_seg()

#pragma bss_seg(".bss$text3d_width")
    f32 text3d_width;
#pragma bss_seg()

#pragma bss_seg(".bss$followon_line")
    i32 followon_line;
#pragma bss_seg()

#pragma bss_seg(".bss$TextPrintModePc")
    i32 TextPrintModePc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputAlternateLabelsPc")
    TEXT_INPUT_LABELS_PC *TextInputAlternateLabelsPc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputDefaultLabelsPc")
    TEXT_INPUT_LABELS_PC *TextInputDefaultLabelsPc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputNameCallbackPc")
    void(__cdecl *TextInputNameCallbackPc)(i32, char *);
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputLastDeviceIndexPc")
    i8 TextInputLastDeviceIndexPc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputDeviceLinksPc")
    TEXT_INPUT_DEVICE_LINK_PC TextInputDeviceLinksPc[3];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PunchRangeUnitIdPc")
    i16 PunchRangeUnitIdPc = -1;
#pragma data_seg()

#pragma data_seg(".data$PunchRangeCompanionIdPc")
    i16 PunchRangeCompanionIdPc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_CAPTAINTARPALS")
    i16 id_CAPTAINTARPALS = -1;
#pragma data_seg()

#pragma bss_seg(".bss$CheatPooPartsEnabledPc")
    u8 CheatPooPartsEnabledPc;
#pragma bss_seg()

#pragma bss_seg(".bss$CheatPooCoinsEnabledPc")
    u8 CheatPooCoinsEnabledPc;
#pragma bss_seg()

#pragma data_seg(".data$test_ai_combo")
    i32 test_ai_combo = 1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_PUNCH")
    i32 LEGOCONTEXT_PUNCH = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_LAND_JUMP")
    i32 LEGOCONTEXT_LAND_JUMP = -1;
#pragma data_seg()

#pragma data_seg(".data$LegoContextNetWaitPc")
    i32 LegoContextNetWaitPc = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOACT_FALLLAND")
    i16 LEGOACT_FALLLAND = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOACT_BACKPACKFALLLAND")
    i16 LEGOACT_BACKPACKFALLLAND = -1;
#pragma data_seg()

#pragma data_seg(".data$LegoActPunchBehindPc")
    i16 LegoActPunchBehindPc = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOACT_EXTRA_LAND2")
    i16 LEGOACT_EXTRA_LAND2 = -1;
#pragma data_seg()

#pragma data_seg(".data$PUNCHGAP")
    f32 PUNCHGAP = 0.30000001192092896f;
#pragma data_seg()

#pragma data_seg(".data$PUNCHCHARGAP")
    f32 PUNCHCHARGAP = 0.6000000238418579f;
#pragma data_seg()

#pragma data_seg(".data$CanPunchGirls")
    i32 CanPunchGirls = 1;
#pragma data_seg()

#pragma bss_seg(".bss$MOSEISLEYC_LDATA")
    LEVELDATA_PC *MOSEISLEYC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ComboOpponent_Behind")
    i32 ComboOpponent_Behind;
#pragma bss_seg()

#pragma bss_seg(".bss$ComboOpponent_Range2")
    f32 ComboOpponent_Range2;
#pragma bss_seg()

#pragma bss_seg(".bss$PooPositionPc")
    NUVEC PooPositionPc[2];
#pragma bss_seg()

#pragma bss_seg(".bss$objopponent_ignoreaiopponent")
    i32 objopponent_ignoreaiopponent;
#pragma bss_seg()

#pragma bss_seg(".bss$IsWearingBackPackFn")
    i32(__cdecl *IsWearingBackPackFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$GizmoBlowUpOpponent_Behind")
    i32 GizmoBlowUpOpponent_Behind;
#pragma bss_seg()

#pragma bss_seg(".bss$GizmoBlowUpOpponent_Range2")
    f32 GizmoBlowUpOpponent_Range2;
#pragma bss_seg()

#pragma bss_seg(".bss$PlayerSpecialMoveCountPc")
    i32 PlayerSpecialMoveCountPc;
#pragma bss_seg()

#pragma bss_seg(".bss$UsingExtraActionsFn")
    i32(__cdecl *UsingExtraActionsFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$CanStartHoldFn")
    i32(__cdecl *CanStartHoldFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$Punch_GetDamageFn")
    i32(__cdecl *Punch_GetDamageFn)(GAMEOBJECT *, GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$Punch_HitExtraCodeFn")
    void(__cdecl *Punch_HitExtraCodeFn)(GAMEOBJECT *, NUVEC *);
#pragma bss_seg()

#pragma bss_seg(".bss$Punch_HitHoldFn")
    void(__cdecl *Punch_HitHoldFn)(GAMEOBJECT *, GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NEGOTIATIONSA_LDATA")
    LEVELDATA_PC *NEGOTIATIONSA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$NEGOTIATIONSB_LDATA")
    LEVELDATA_PC *NEGOTIATIONSB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$GUNGAN_A_LDATA")
    LEVELDATA_PC *GUNGAN_A_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RESCUEC_LDATA")
    LEVELDATA_PC *RESCUEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RETAKEB_LDATA")
    LEVELDATA_PC *RETAKEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RETAKED_LDATA")
    LEVELDATA_PC *RETAKED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RETAKEE_LDATA")
    LEVELDATA_PC *RETAKEE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$MAULD_LDATA")
    LEVELDATA_PC *MAULD_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYB_LDATA")
    LEVELDATA_PC *FACTORYB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYD_LDATA")
    LEVELDATA_PC *FACTORYD_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYF_LDATA")
    LEVELDATA_PC *FACTORYF_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYG_LDATA")
    LEVELDATA_PC *FACTORYG_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$KAMINOC_LDATA")
    LEVELDATA_PC *KAMINOC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$KASHYYYKD_LDATA")
    LEVELDATA_PC *KASHYYYKD_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CRUISERG_LDATA")
    LEVELDATA_PC *CRUISERG_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$TATOOINEA_LDATA")
    LEVELDATA_PC *TATOOINEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$TATOOINEB_LDATA")
    LEVELDATA_PC *TATOOINEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$MOSEISLEYA_LDATA")
    LEVELDATA_PC *MOSEISLEYA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARRESCUEB_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARRESCUEC_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARRESCUEE_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPEA_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPEB_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPEC_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPED_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARBATTLEA_LDATA")
    LEVELDATA_PC *DEATHSTARBATTLEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARBATTLEB_LDATA")
    LEVELDATA_PC *DEATHSTARBATTLEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARBATTLEC_LDATA")
    LEVELDATA_PC *DEATHSTARBATTLEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$HOTHESCAPEA_LDATA")
    LEVELDATA_PC *HOTHESCAPEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$HOTHESCAPEC_LDATA")
    LEVELDATA_PC *HOTHESCAPEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CLOUDCITYESCAPEA_LDATA")
    LEVELDATA_PC *CLOUDCITYESCAPEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CLOUDCITYESCAPEC_LDATA")
    LEVELDATA_PC *CLOUDCITYESCAPEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CLOUDCITYTRAPA_LDATA")
    LEVELDATA_PC *CLOUDCITYTRAPA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTAR2BATTLEB_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTAR2BATTLED_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$EMPERORFIGHTA_LDATA")
    LEVELDATA_PC *EMPERORFIGHTA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$SARLACCPITB_LDATA")
    LEVELDATA_PC *SARLACCPITB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$SARLACCPITC_LDATA")
    LEVELDATA_PC *SARLACCPITC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACEA_LDATA")
    LEVELDATA_PC *JABBASPALACEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACEB_LDATA")
    LEVELDATA_PC *JABBASPALACEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACED_LDATA")
    LEVELDATA_PC *JABBASPALACED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACEE_LDATA")
    LEVELDATA_PC *JABBASPALACEE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLEA_LDATA")
    LEVELDATA_PC *ENDORBATTLEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLEB_LDATA")
    LEVELDATA_PC *ENDORBATTLEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLED_LDATA")
    LEVELDATA_PC *ENDORBATTLED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$TorpedoFillEnabledPc")
    u8 TorpedoFillEnabledPc;
#pragma bss_seg()

#pragma data_seg(".data$TorpedoGrabRangeSquaredPc")
    f32 TorpedoGrabRangeSquaredPc = 36.0f;
#pragma data_seg()

#pragma const_seg(".rdata$TorpedoFireSfxPc")
    const char TorpedoFireSfxPc[14] = "XWing_Torpedo";
#pragma const_seg()

#pragma data_seg(".data$TorpedoStealTimerPc")
    f32 TorpedoStealTimerPc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$TorpedoStealRangePc")
    f32 TorpedoStealRangePc = 2.0f;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$CutSceneSingleStreamBufferPc")
    i32 CutSceneSingleStreamBufferPc = 2;
#pragma data_seg()

#pragma bss_seg(".bss$CutSceneBackgroundInstancesPc")
    INSTNUGCUTSCENE *CutSceneBackgroundInstancesPc;
#pragma bss_seg()

#pragma bss_seg(".bss$LookupLocatorVfxFn")
    i32(__cdecl *LookupLocatorVfxFn)(char *);
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneLookupAllDebrisPagesPc")
    i32 CutSceneLookupAllDebrisPagesPc;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneFindCharacters")
    void(__cdecl *NuCutSceneFindCharacters)(CUT_SCENE_LOAD_PC *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneCharacterCreateData")
    void(__cdecl *NuCutSceneCharacterCreateData)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneRequestSFX")
    i32(__cdecl *NuCutSceneRequestSFX)(char *);
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneLocatorFunctionsPc")
    CUT_LOCATOR_FUNCTION_ENTRY_PC *CutSceneLocatorFunctionsPc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$ForcePushSoundNamePc")
    const char ForcePushSoundNamePc[11] = "JForcePush";
#pragma const_seg()

#pragma const_seg(".rdata$VehicleSmartBombChargeNamePc")
    const char VehicleSmartBombChargeNamePc[10] = "JForceUse";
#pragma const_seg()

#pragma const_seg(".rdata$VehicleShieldOnNamePc")
    const char VehicleShieldOnNamePc[10] = "DDekaShOn";
#pragma const_seg()

#pragma const_seg(".rdata$PodBoosterSoundNamePc")
    const char PodBoosterSoundNamePc[13] = "PodX_Booster";
#pragma const_seg()

#pragma data_seg(".data$id_DODGEM")
    i16 id_DODGEM = -1;
#pragma data_seg()

#pragma data_seg(".data$id_SNOWMOB")
    i16 id_SNOWMOB = -1;
#pragma data_seg()

#pragma data_seg(".data$id_TRACTOR")
    i16 id_TRACTOR = -1;
#pragma data_seg()

#pragma data_seg(".data$PCForceGlowRadiusMultiplier")
    f32 PCForceGlowRadiusMultiplier = 1.125f;
#pragma data_seg()

#pragma data_seg(".data$PCForceGlowHoldDuration")
    f32 PCForceGlowHoldDuration = 0.3330000042915344f;
#pragma data_seg()

#pragma data_seg(".data$FORCEGLOWTIME")
    f32 FORCEGLOWTIME = 0.10000000149011612f;
#pragma data_seg()

#pragma data_seg(".data$CableTargetScalePc")
    f32 CableTargetScalePc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$VehicleZamSmokeRatePc")
    f32 VehicleZamSmokeRatePc = 10.0f;
#pragma data_seg()

#pragma data_seg(".data$VehicleShieldTargetPc")
    f32 VehicleShieldTargetPc = 5.0f;
#pragma data_seg()

#pragma data_seg(".data$PodRaceMinimumSpeedPc")
    f32 PodRaceMinimumSpeedPc = 0.3330000042915344f;
#pragma data_seg()

#pragma data_seg(".data$PodRaceBoostSpeedPc")
    f32 PodRaceBoostSpeedPc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$PodSprintMinimumSpeedPc")
    f32 PodSprintMinimumSpeedPc = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$PodSprintBoostSpeedPc")
    f32 PodSprintBoostSpeedPc = 1.399999976158142f;
#pragma data_seg()

#pragma data_seg(".data$PodBoostHeightPc")
    f32 PodBoostHeightPc = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$PodPitchHistoryPc")
    f32 PodPitchHistoryPc[2] = {1.0f, 1.0f};
#pragma data_seg()

#pragma data_seg(".data$PodSprintDebrisIndexPc")
    i32 PodSprintDebrisIndexPc = 117;
#pragma data_seg()

#pragma data_seg(".data$PodCollisionThresholdPc")
    f32 PodCollisionThresholdPc = 0.9990000128746033f;
#pragma data_seg()

#pragma data_seg(".data$PodSprintLiftPositionsPc")
    NUVEC PodSprintLiftPositionsPc[3] = {{-395.260009765625f, -5.480000019073486f, -165.0800018310547f},
                                         {-375.3800048828125f, -5.039999961853027f, -286.5f},
                                         {-354.9100036621094f, -4.849999904632568f, -299.32000732421875f}};
#pragma data_seg()

#pragma bss_seg(".bss$LswForceUsedHintFlagsPc")
    u32 LswForceUsedHintFlagsPc;
#pragma bss_seg()

#pragma bss_seg(".bss$BOUNTYHUNTERPURSUITE_LDATA")
    LEVELDATA_PC *BOUNTYHUNTERPURSUITE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ASTEROIDCHASEC_LDATA")
    LEVELDATA_PC *ASTEROIDCHASEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DAGOBAHC_LDATA")
    LEVELDATA_PC *DAGOBAHC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLEC_LDATA")
    LEVELDATA_PC *ENDORBATTLEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$NOTERRAINSTOP")
    i32 NOTERRAINSTOP;
#pragma bss_seg()

#pragma bss_seg(".bss$VehicleSlowdownCallbackPc")
    void(__cdecl *VehicleSlowdownCallbackPc)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$VehicleLaunchHoverOffsetPc")
    f32 VehicleLaunchHoverOffsetPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PodCollisionMagnitudePc")
    f32 PodCollisionMagnitudePc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCForceGlowDisabledState")
    f32 PCForceGlowDisabledState;
#pragma bss_seg()

#pragma bss_seg(".bss$LEGO_AIPATHCNX_BLOCKAGE")
    i32 LEGO_AIPATHCNX_BLOCKAGE;
#pragma bss_seg()

#pragma bss_seg(".bss$LEGO_AIPATHCNX_WALLSHUFFLE")
    i32 LEGO_AIPATHCNX_WALLSHUFFLE;
#pragma bss_seg()

#pragma bss_seg(".bss$LEGO_AIPATHCNX_FULLTERRAIN")
    i32 LEGO_AIPATHCNX_FULLTERRAIN;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$CS_buffend")
    VARIPTR *CS_buffend;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_cutsys")
    CUT_LOAD_SYS_PC *CS_cutsys;
#pragma bss_seg()

#pragma bss_seg(".bss$CutScene_OverrideConfigFileNameFn")
    void(__cdecl *CutScene_OverrideConfigFileNameFn)(char *, i32, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$CutList")
    CUT_LOAD_LIST_ENTRY_PC *CutList;
#pragma bss_seg()

#pragma bss_seg(".bss$CUTCOUNT")
    i32 CUTCOUNT;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$RenderSplineShopStartPc")
    const char RenderSplineShopStartPc[11] = "shop_start";
#pragma const_seg()

#pragma const_seg(".rdata$RenderSplineBountyStartPc")
    const char RenderSplineBountyStartPc[13] = "bounty_start";
#pragma const_seg()

#pragma data_seg(".data$DrawStatusScaleXPc")
    f32 DrawStatusScaleXPc = 350.0f;
#pragma data_seg()

#pragma data_seg(".data$DrawStatusScaleYPc")
    f32 DrawStatusScaleYPc = 300.0f;
#pragma data_seg()

#pragma data_seg(".data$DrawCharIconPanelPc")
    i32 DrawCharIconPanelPc = -1;
#pragma data_seg()

#pragma data_seg(".data$LegoObjectIconQuestionPc")
    i32 LegoObjectIconQuestionPc = -1;
#pragma data_seg()

#pragma data_seg(".data$DEBPAGE_GENERAL")
    i32 DEBPAGE_GENERAL = -1;
#pragma data_seg()

#pragma data_seg(".data$DEBPAGE_CHARACTER")
    i32 DEBPAGE_CHARACTER = -1;
#pragma data_seg()

#pragma data_seg(".data$DEBPAGE_AREA")
    i32 DEBPAGE_AREA = -1;
#pragma data_seg()

#pragma bss_seg(".bss$CutSceneCameraFocalLengthPc")
    f32 CutSceneCameraFocalLengthPc;
#pragma bss_seg()

#pragma bss_seg(".bss$things_scene")
    NUGSCN *things_scene;
#pragma bss_seg()

#pragma bss_seg(".bss$DrawCharIconFindPc")
    i32 DrawCharIconFindPc;
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackdropSpecialsPc")
    NUSPECIALHANDLE RenderBackdropSpecialsPc[4];
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackgroundColoursPc")
    i32 RenderBackgroundColoursPc[2];
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackdropScenePc")
    NUGSCN *RenderBackdropScenePc;
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackdropAlphaFnPc")
    void(__cdecl *RenderBackdropAlphaFnPc)(f32 *);
#pragma bss_seg()

#pragma bss_seg(".bss$SURFACEBITS_NODUST")
    u16 SURFACEBITS_NODUST;
#pragma bss_seg()

#pragma bss_seg(".bss$RenderHubFromCutscenePlayerPc")
    i32 RenderHubFromCutscenePlayerPc;
#pragma bss_seg()

#pragma bss_seg(".bss$IconScenePc")
    NUGSCN **IconScenePc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$RtlSearchTimebarNamePc")
    const char RtlSearchTimebarNamePc[9] = "RTL srch";
#pragma const_seg()

#pragma data_seg(".data$LightingUnitDirectionPc")
    NUVEC LightingUnitDirectionPc = {1.0f, 0.0f, 0.0f};
#pragma data_seg()

#pragma data_seg(".data$RtlUnitDirectionYPc")
    NUVEC RtlUnitDirectionYPc = {0.0f, 1.0f, 0.0f};
#pragma data_seg()

#pragma data_seg(".data$rtl_dynamic_lights_enabled")
    i32 rtl_dynamic_lights_enabled = 1;
#pragma data_seg()

#pragma data_seg(".data$rtl_shadow_blend_rate")
    f32 rtl_shadow_blend_rate = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$rtl_shadow_flicker")
    NUVEC rtl_shadow_flicker = {0.10000000149011612f, 0.10000000149011612f, 0.10000000149011612f};
#pragma data_seg()

#pragma data_seg(".data$RtlModifierIndexZeroPc")
    f32 RtlModifierIndexZeroPc = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$modifiers")
    f32 *modifiers = &RtlModifierIndexZeroPc;
#pragma data_seg()

#pragma data_seg(".data$rtlwob")
    f32 rtlwob = 0.05000000074505806f;
#pragma data_seg()

#pragma bss_seg(".bss$PCShadowCurrentLight")
    struct PC_SPECIAL_SHADOW_LIGHT *PCShadowCurrentLight;
#pragma bss_seg()

#pragma bss_seg(".bss$PCGlobalLightingState")
    PC_GLOBAL_LIGHTING_STATE PCGlobalLightingState;
#pragma bss_seg()

#pragma bss_seg(".bss$NuRndrShadPolDat")
    PC_SHADOW_QUEUE_RECORD NuRndrShadPolDat[128];
#pragma bss_seg()

#pragma bss_seg(".bss$NuRndrShadowCnt")
    i32 NuRndrShadowCnt;
#pragma bss_seg()

#pragma bss_seg(".bss$rtl_error")
    i32 rtl_error;
#pragma bss_seg()

#pragma bss_seg(".bss$rtl_frametime")
    f32 rtl_frametime;
#pragma bss_seg()

#pragma bss_seg(".bss$rtltimer1")
    i32 rtltimer1;
#pragma bss_seg()

#pragma bss_seg(".bss$dbg_rtlindex")
    RTL_INDEX_PC *dbg_rtlindex;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$UiBackPromptLiteralPc")
    char UiBackPromptLiteralPc[5] = "Back";
#pragma data_seg()

#pragma data_seg(".data$UiSelectPromptLiteralPc")
    char UiSelectPromptLiteralPc[7] = "Select";
#pragma data_seg()

#pragma data_seg(".data$MenuTextScalePc")
    f32 MenuTextScalePc = 0.5f;
#pragma data_seg()

#pragma data_seg(".data$MENUDY")
    f32 MENUDY = -0.10664000362157822f;
#pragma data_seg()

#pragma data_seg(".data$MenuAcceptPromptTextPc")
    char *MenuAcceptPromptTextPc = UiSelectPromptLiteralPc;
#pragma data_seg()

#pragma data_seg(".data$MenuCancelPromptTextPc")
    char *MenuCancelPromptTextPc = UiBackPromptLiteralPc;
#pragma data_seg()

#pragma data_seg(".data$MenuPromptPositionScalePc")
    f32 MenuPromptPositionScalePc = 1.0f;
#pragma data_seg()

#pragma bss_seg(".bss$QFont2DLower")
    VUFNT *QFont2DLower;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$ScreenGrabNeededPc")
    u8 ScreenGrabNeededPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ScreenPauseRenderTargetPc")
    i32 ScreenPauseRenderTargetPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ScreenPauseFadePc")
    f32 ScreenPauseFadePc;
#pragma bss_seg()
#pragma bss_seg(".bss$EdppPageScenesPc")
    NUGSCN *EdppPageScenesPc[8];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$g_enableButtonPrompts")
    i32 g_enableButtonPrompts = 1;
#pragma data_seg()

#pragma data_seg(".data$headerdrawfn")
    void(__cdecl *headerdrawfn)(void) = DrawMenuHeader;
#pragma data_seg()

#pragma data_seg(".data$MenuTopYPc")
    f32 MenuTopYPc = 0.800000011920929f;
#pragma data_seg()

#pragma data_seg(".data$MENUNORMALR")
    u8 MENUNORMALR = 255;
#pragma data_seg()

#pragma data_seg(".data$MENUNORMALG")
    u8 MENUNORMALG = 127;
#pragma data_seg()

#pragma data_seg(".data$SmartTextDefaultWidthPc")
    f32 SmartTextDefaultWidthPc = 1.5f;
#pragma data_seg()

#pragma bss_seg(".bss$SmartTextLongestWidthPc")
    f32 SmartTextLongestWidthPc;
#pragma bss_seg()

#pragma bss_seg(".bss$MenuDisableHeaders")
    i32 MenuDisableHeaders;
#pragma bss_seg()

#pragma bss_seg(".bss$predrawfn")
    void(__cdecl *predrawfn)(MENU *);
#pragma bss_seg()

#pragma bss_seg(".bss$MENUNORMALB")
    u8 MENUNORMALB;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_autosavestarted")
    i32 memcard_autosavestarted;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_autosavepredelay")
    f32 memcard_autosavepredelay;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_autosavepostdelay")
    f32 memcard_autosavepostdelay;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_drawasiconfn")
    void(__cdecl *memcard_drawasiconfn)(void);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$CutSceneDrawSettingsPc")
    CUT_DRAW_SETTINGS_PC CutSceneDrawSettingsPc = {-1, -1, -1, -1, 0};
#pragma data_seg()
#pragma data_seg(".data$dco_id")
    i32 dco_id = -1;
#pragma data_seg()

#pragma data_seg(".data$dco_reflectcoord")
    f32 dco_reflectcoord = 2000000.0f;
#pragma data_seg()

#pragma data_seg(".data$CutSceneSys")
    CUT_DRAW_SETTINGS_PC *CutSceneSys = &CutSceneDrawSettingsPc;
#pragma data_seg()

#pragma bss_seg(".bss$UpdateLocatorVfxFn")
    void(__cdecl *UpdateLocatorVfxFn)(i32, NUMTX *);
#pragma bss_seg()

#pragma bss_seg(".bss$TriggerLocatorVfxFn")
    i32(__cdecl *TriggerLocatorVfxFn)(i32, NUMTX *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneGetHGObj")
    NUHGOBJ_EVAL_PC *(__cdecl *NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneCharacterRender")
    void(__cdecl *NuCutSceneCharacterRender)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneResetCharacters")
    void(__cdecl *NuCutSceneResetCharacters)(INSTNUGCUTSCENE *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneRigidPostRender")
    void(__cdecl *NuCutSceneRigidPostRender)(void *, NUSPECIALHANDLE *, NUMTX *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneSFXUpdate")
    void(__cdecl *NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32,
                                       NUMTX *, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneDebrisAxisSizePc")
    f32 CutSceneDebrisAxisSizePc;
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneDebrisProcessModePc")
    i32 CutSceneDebrisProcessModePc;
#pragma bss_seg()

#pragma bss_seg(".bss$MatrixReflection_CanOverrideFn")
    i32(__cdecl *MatrixReflection_CanOverrideFn)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$dco_gcdata")
    PLAYERCHARACTERCONFIG_PC *dco_gcdata;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_cmodel")
    CHARACTER_MODEL_LAYERS_PC *dco_cmodel;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_wearinghat")
    i32 dco_wearinghat;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_reflectaxis")
    i32 dco_reflectaxis;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_prerotatez")
    u16 dco_prerotatez;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_locatorposonly")
    i32 dco_locatorposonly;
#pragma bss_seg()

#pragma bss_seg(".bss$DisguiseAdjustFn")
    void(__cdecl *DisguiseAdjustFn)(i32, i32, NUVEC *, NUVEC *);
#pragma bss_seg()

#pragma bss_seg(".bss$CutBlobShadowAlpha")
    u8 CutBlobShadowAlpha;
#pragma bss_seg()

#pragma bss_seg(".bss$CutReflectRange2")
    f32 CutReflectRange2;
#pragma bss_seg()

#pragma bss_seg(".bss$CutBlobShadowFadeNear")
    f32 CutBlobShadowFadeNear;
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneWaiting")
    i32 CutSceneWaiting;
#pragma bss_seg()

#pragma bss_seg(".bss$CutBlobShadowFadeFar")
    f32 CutBlobShadowFadeFar;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$effecttypes")
    DEBRIS_INFO_PC *effecttypes;
#pragma bss_seg()

#pragma bss_seg(".bss$debscale")
    DEBRIS_SCALE_CACHE_PC *debscale;
#pragma bss_seg()

#pragma data_seg(".data$EDPP_SCALE_TYPES")
    i32 EDPP_SCALE_TYPES = 20;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$CutStopInfo")
    void *CutStopInfo;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$SuitMetadataName0075314cPc")
    char SuitMetadataName0075314cPc[7] = "batman";
#pragma data_seg()

#pragma bss_seg(".bss$tBATMANSUIT")
    i16 tBATMANSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d280Pc")
    char SuitMetadataName0075d280Pc[14] = "batman_shadow";
#pragma data_seg()

#pragma bss_seg(".bss$tSHADOWSUIT")
    i16 tSHADOWSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d270Pc")
    char SuitMetadataName0075d270Pc[13] = "batman_glide";
#pragma data_seg()

#pragma bss_seg(".bss$tGLIDESUIT")
    i16 tGLIDESUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d25cPc")
    char SuitMetadataName0075d25cPc[18] = "batman_demolition";
#pragma data_seg()

#pragma bss_seg(".bss$tDEMOLITIONSUIT")
    i16 tDEMOLITIONSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d24cPc")
    char SuitMetadataName0075d24cPc[13] = "batman_sonar";
#pragma data_seg()

#pragma bss_seg(".bss$tSONARSUIT")
    i16 tSONARSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName00753144Pc")
    char SuitMetadataName00753144Pc[6] = "robin";
#pragma data_seg()

#pragma bss_seg(".bss$tROBINSUIT")
    i16 tROBINSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d240Pc")
    char SuitMetadataName0075d240Pc[12] = "robin_water";
#pragma data_seg()

#pragma bss_seg(".bss$tWATERSUIT")
    i16 tWATERSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d22cPc")
    char SuitMetadataName0075d22cPc[17] = "robin_technology";
#pragma data_seg()

#pragma bss_seg(".bss$tTECHNOLOGYSUIT")
    i16 tTECHNOLOGYSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d21cPc")
    char SuitMetadataName0075d21cPc[13] = "robin_magnet";
#pragma data_seg()

#pragma bss_seg(".bss$tMAGNETSUIT")
    i16 tMAGNETSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d20cPc")
    char SuitMetadataName0075d20cPc[14] = "robin_attract";
#pragma data_seg()

#pragma bss_seg(".bss$tATTRACTSUIT")
    i16 tATTRACTSUIT;
#pragma bss_seg()

#pragma bss_seg(".bss$NOAICREATURES")
    i32 NOAICREATURES;
#pragma bss_seg()

#pragma bss_seg(".bss$addcreature_override_id_check")
    i32 addcreature_override_id_check;
#pragma bss_seg()

#pragma data_seg(".data$default_mover_extra")
    f32 default_mover_extra = 0.05000000074505806f;
#pragma data_seg()

#pragma data_seg(".data$id_MOSEISLEYCITIZEN")
    i16 id_MOSEISLEYCITIZEN = -1;
#pragma data_seg()

#pragma data_seg(".data$id_CANTINAALIEN")
    i16 id_CANTINAALIEN = -1;
#pragma data_seg()

#pragma data_seg(".data$id_CLOUDCITYCITIZEN")
    i16 id_CLOUDCITYCITIZEN = -1;
#pragma data_seg()

#pragma data_seg(".data$SuitRecordInitPc")
    SUIT_INITIALIZER_RECORD_PC SuitRecordInitPc[10] = {
        {SuitMetadataName0075314cPc, SuitMetadataName0075314cPc, &tBATMANSUIT, 0, 98, 'b', 0x0, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d280Pc, &tSHADOWSUIT, 1, 98, 'i', 0x0, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d270Pc, &tGLIDESUIT, 258, 98, 'a', 0x4, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d25cPc, &tDEMOLITIONSUIT, 260, 98, 'c', 0x0, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d24cPc, &tSONARSUIT, 8, 98, 'h', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName00753144Pc, &tROBINSUIT, 0, 114, 'r', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d240Pc, &tWATERSUIT, 272, 114, 'w', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d22cPc, &tTECHNOLOGYSUIT, 288, 114, 't', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d21cPc, &tMAGNETSUIT, 64, 114, 'm', 0x200000, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d20cPc, &tATTRACTSUIT, 384, 114, 'd', 0x0, 0, 0, 0}};
#pragma data_seg()

#pragma bss_seg(".bss$PlayerStartRecordInitPc")
    PLAYER_START_INITIALIZER_RECORD_PC PlayerStartRecordInitPc[8];
#pragma bss_seg()

#pragma bss_seg(".bss$PodPacemakerPc")
    GAMEOBJECT *PodPacemakerPc;
#pragma bss_seg()

#pragma data_seg(".data$PodRaceSplineIncrementPc")
    f32 PodRaceSplineIncrementPc = 0.0010000000474974513f;
#pragma data_seg()

#pragma data_seg(".data$PodCreateSplineKeywordPc")
    char PodCreateSplineKeywordPc[11] = "spline_id=";
#pragma data_seg()

#pragma data_seg(".data$PodCreateStartKeywordPc")
    char PodCreateStartKeywordPc[14] = "spline_start=";
#pragma data_seg()

#pragma data_seg(".data$PodCreateEndKeywordPc")
    char PodCreateEndKeywordPc[12] = "spline_end=";
#pragma data_seg()

#pragma data_seg(".data$PodCreateTimeKeywordPc")
    char PodCreateTimeKeywordPc[13] = "time_factor=";
#pragma data_seg()

#pragma data_seg(".data$PodCreatePacemakerKeywordPc")
    char PodCreatePacemakerKeywordPc[10] = "pacemaker";
#pragma data_seg()

#pragma data_seg(".data$PodCreateDamagedKeywordPc")
    char PodCreateDamagedKeywordPc[8] = "damaged";
#pragma data_seg()

#pragma bss_seg(".bss$PodRacePc")
    void *PodRacePc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$GizObstacleTriggerLimitMessagePc")
    const char GizObstacleTriggerLimitMessagePc[59] = "MAXGIZOBSTACLETRIGGERS needs increasing, ask a programmer!";
#pragma const_seg()

#pragma data_seg(".data$ShoveMaximumSpeedPc")
    f32 ShoveMaximumSpeedPc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$ShoveMaximumPenetrationPc")
    f32 ShoveMaximumPenetrationPc = 0.10000000149011612f;
#pragma data_seg()

#pragma data_seg(".data$ShoveCollisionExpansionPc")
    f32 ShoveCollisionExpansionPc = 0.10000000149011612f;
#pragma data_seg()

#pragma bss_seg(".bss$ObstacleReturnLevelPc")
    LEVELDATA_PC *ObstacleReturnLevelPc;
#pragma bss_seg()

#pragma bss_seg(".bss$GizObstacleTriggersPc")
    NUVEC *GizObstacleTriggersPc[16];
#pragma bss_seg()

#pragma bss_seg(".bss$GizObstacleExcludeFnPc")
    i32(__cdecl *GizObstacleExcludeFnPc)(GIZOBSTACLE *, GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$gExplodeInitialised")
    i32 gExplodeInitialised = 1;
#pragma data_seg()

#pragma data_seg(".data$GAMEPAD_TOGGLELEFT")
    u32 GAMEPAD_TOGGLELEFT = 5;
#pragma data_seg()

#pragma data_seg(".data$GAMEPAD_TOGGLERIGHT")
    u32 GAMEPAD_TOGGLERIGHT = 10;
#pragma data_seg()

#pragma data_seg(".data$TOGGLEREPEATTIME")
    f32 TOGGLEREPEATTIME = 0.25f;
#pragma data_seg()

#pragma data_seg(".data$LEGOHINT_FREEPLAYTOGGLE")
    i32 LEGOHINT_FREEPLAYTOGGLE = -1;
#pragma data_seg()

#pragma bss_seg(".bss$FULLDEBUGTOGGLE")
    i32 FULLDEBUGTOGGLE;
#pragma bss_seg()

#pragma bss_seg(".bss$Player_ToggleSubCharacterFn")
    i32(__cdecl *Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$Player_ToggledCharacterFn")
    void(__cdecl *Player_ToggledCharacterFn)(GAMEOBJECT *, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$implode_inbuffer")
    char *implode_inbuffer;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_outbuffer")
    char *implode_outbuffer;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_bitbuf")
    u32 implode_bitbuf;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_compsize")
    u32 implode_compsize;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_left")
    u16 implode_left[0x3fb];
#pragma bss_seg()

#pragma bss_seg(".bss$implode_right")
    u16 implode_right[0x3fb];
#pragma bss_seg()

#pragma bss_seg(".bss$implode_origsize")
    u32 implode_origsize;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$GizmoBlowupUnitScalePc")
    NUVEC GizmoBlowupUnitScalePc = {1.0f, 1.0f, 1.0f};
#pragma data_seg()
}

struct PC_OGG_CACHE_RECORD_PC;
extern "C" {
#pragma const_seg(".rdata$PCSoundListenerIIDPc")
    extern const IID PCSoundListenerIIDPc = {
        0x279afa84, 0x4981, 0x11ce, {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60}};
#pragma const_seg()
#pragma const_seg(".rdata$PCSampleBufferAlgorithmPc")
    extern const GUID PCSampleBufferAlgorithmPc = {
        0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#pragma const_seg()
#pragma data_seg(".data$GizmoBlowupHitMultiplierPc")
    u8 GizmoBlowupHitMultiplierPc = 1;
#pragma data_seg()
#pragma data_seg(".data$PCSampleLoadEnabledPc")
    i32 PCSampleLoadEnabledPc = 1;
#pragma data_seg()
#pragma bss_seg(".bss$PCSoundSampleLimitPc")
    i32 PCSoundSampleLimitPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundStreamSerialPc")
    i32 PCSoundStreamSerialPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCOggCachePoolPc")
    struct PC_OGG_CACHE_RECORD_PC *PCOggCachePoolPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCOggCriticalSectionPc")
    i32 PCOggCriticalSectionPc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$GameAntinodeGridPc")
    u64 GameAntinodeGridPc[64];
#pragma bss_seg()
#pragma const_seg(".rdata$ZipUpHookOffsetPc")
    extern const NUVEC ZipUpHookOffsetPc = {0.0f, -0.11429999768733978f, -0.08590000122785568f};
#pragma const_seg()
#pragma data_seg(".data$ZipUpSwingOffsetLengthPc")
    f32 ZipUpSwingOffsetLengthPc = 1.9236159324645996f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingOffsetHeightPc")
    f32 ZipUpSwingOffsetHeightPc = 0.25150299072265625f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRiderOffsetScalePc")
    f32 ZipUpRiderOffsetScalePc = 0.8999999761581421f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRiderHeightPc")
    f32 ZipUpRiderHeightPc = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingDurationPc")
    f32 ZipUpSwingDurationPc = 1.5f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingStartAnimationPc")
    u16 ZipUpSwingStartAnimationPc = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingSwingAnimationPc")
    u16 ZipUpSwingSwingAnimationPc = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingJumpAnimationPc")
    u16 ZipUpSwingJumpAnimationPc = 0xffff;
#pragma data_seg()
#pragma bss_seg(".bss$CanDrawZipUpSwirls")
    i32 CanDrawZipUpSwirls;
#pragma bss_seg()
#pragma bss_seg(".bss$ropemtl")
    NUMTL *ropemtl;
#pragma bss_seg()
#pragma data_seg(".data$ZipUpMarkerOffsetYPc")
    f32 ZipUpMarkerOffsetYPc = 0.8399999737739563f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpMarkerOffsetZPc")
    f32 ZipUpMarkerOffsetZPc = 0.9229000210762024f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRopeHalfWidthPc")
    f32 ZipUpRopeHalfWidthPc = 0.009999999776482582f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRopeCheatColourPc")
    u32 ZipUpRopeCheatColourPc = 0xff103f10;
#pragma data_seg()
#pragma data_seg(".data$ZipUpLineCharacterIdPc")
    i16 ZipUpLineCharacterIdPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ZipUpLineMaterialPc")
    NUMTL *ZipUpLineMaterialPc;
#pragma bss_seg()
#pragma data_seg(".data$ZipUpTypeNamePtrPc")
    char *ZipUpTypeNamePtrPc = (char *)PCContextName_ZipUp;
#pragma data_seg()
#pragma data_seg(".data$zipup_gizmotype_id")
    i32 zipup_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$ZipUpLandCombatRollContextPc")
    i32 ZipUpLandCombatRollContextPc = -1;
#pragma data_seg()
#pragma data_seg(".data$ZipUpWallShuffleContextPc")
    i32 ZipUpWallShuffleContextPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ZipUpRopeModulationPc")
    i32 ZipUpRopeModulationPc;
#pragma bss_seg()
#pragma data_seg(".data$GrapAttach_text")
    char GrapAttach_text[11] = "GrapAttach";
#pragma data_seg()
#pragma data_seg(".data$GrapDetach_text")
    char GrapDetach_text[11] = "GrapDetach";
#pragma data_seg()
#pragma data_seg(".data$GrapSwing_text")
    char GrapSwing_text[10] = "GrapSwing";
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$FindAnglesXFullTempPc")
    i32 FindAnglesXFullTempPc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$MiniCam")
    MINICAM_PC MiniCam;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutSpline")
    NUGSPLINE ObstacleCamCutSpline;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamTargetGuid")
    i32 ObstacleCamTargetGuid;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutPts")
    NUVEC ObstacleCamCutPts[2];
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamSpl")
    NUGSPLINE *ObstacleCamSpl;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamStart")
    f32 ObstacleCamStart;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamEnd")
    f32 ObstacleCamEnd;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamTime")
    f32 ObstacleCamTime;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamRotZ")
    i16 ObstacleCamRotZ;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamBlendInTime")
    f32 ObstacleCamBlendInTime;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamBlendOutTime")
    f32 ObstacleCamBlendOutTime;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutTgtPtr")
    NUVEC *ObstacleCamCutTgtPtr;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutCamPtr")
    NUVEC *ObstacleCamCutCamPtr;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamBorders")
    i32 ObstacleCamBorders;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamAlwaysSnapAnglesPc")
    u8 ObstacleCamAlwaysSnapAnglesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$CutBorderScale")
    f32 CutBorderScale;
#pragma bss_seg()
#pragma const_seg(".rdata$AICameraLiteral758090")
    const char AICameraLiteral758090[19] = "use_current_campos";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758084")
    const char AICameraLiteral758084[12] = "start_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758070")
    const char AICameraLiteral758070[18] = "end_time=infinite";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758064")
    const char AICameraLiteral758064[10] = "end_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758054")
    const char AICameraLiteral758054[15] = "blend_in_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758044")
    const char AICameraLiteral758044[16] = "blend_out_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758034")
    const char AICameraLiteral758034[13] = "tgt_locator=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758028")
    const char AICameraLiteral758028[9] = "tgt_obj=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758018")
    const char AICameraLiteral758018[13] = "cam_locator=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758010")
    const char AICameraLiteral758010[6] = "roty=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758008")
    const char AICameraLiteral758008[6] = "rotx=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757ffc")
    const char AICameraLiteral757ffc[10] = "FollowTgt";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757ff0")
    const char AICameraLiteral757ff0[11] = "dcampos_x=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fe4")
    const char AICameraLiteral757fe4[11] = "dcampos_y=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fd8")
    const char AICameraLiteral757fd8[11] = "dcampos_z=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fcc")
    const char AICameraLiteral757fcc[10] = "campos_x=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fc0")
    const char AICameraLiteral757fc0[10] = "campos_y=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fb4")
    const char AICameraLiteral757fb4[10] = "campos_z=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fa8")
    const char AICameraLiteral757fa8[10] = "tgtpos_x=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757f9c")
    const char AICameraLiteral757f9c[10] = "tgtpos_y=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757f90")
    const char AICameraLiteral757f90[10] = "tgtpos_z=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757f84")
    const char AICameraLiteral757f84[11] = "no_borders";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758160")
    const char AICameraLiteral758160[18] = "attach_to_spline=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral75814c")
    const char AICameraLiteral75814c[20] = "spline_dist_offset=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758140")
    const char AICameraLiteral758140[12] = "blend_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758134")
    const char AICameraLiteral758134[10] = "max_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758128")
    const char AICameraLiteral758128[11] = "hold_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral75811c")
    const char AICameraLiteral75811c[11] = "cont_roty=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758110")
    const char AICameraLiteral758110[11] = "cont_rotx=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758104")
    const char AICameraLiteral758104[11] = "cont_rotz=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580fc")
    const char AICameraLiteral7580fc[6] = "rotz=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580f0")
    const char AICameraLiteral7580f0[12] = "mode=follow";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580e4")
    const char AICameraLiteral7580e4[12] = "mode=static";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580dc")
    const char AICameraLiteral7580dc[7] = "linear";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580cc")
    const char AICameraLiteral7580cc[13] = "slowfastslow";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580c0")
    const char AICameraLiteral7580c0[9] = "slowfast";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580b0")
    const char AICameraLiteral7580b0[13] = "fastslowfast";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580a4")
    const char AICameraLiteral7580a4[9] = "fastslow";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuPortalCameraPc")
    NUCAMERA *NuPortalCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalLocalViewPc")
    NUMTX NuPortalLocalViewPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalProjectionScratchPc")
    NUMTX NuPortalProjectionScratchPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalProjectionInversePc")
    NUMTX NuPortalProjectionInversePc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalFrustaPc")
    NUPORTAL_FRUSTUM_PC **NuPortalFrustaPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalFrustumCountPc")
    i32 *NuPortalFrustumCountPc;
#pragma bss_seg()
#pragma const_seg(".rdata$NuPortalBoundsOnePc")
    const NUVEC NuPortalBoundsOnePc = {1.0f, 1.0f, 1.0f};
#pragma const_seg()
#pragma const_seg(".rdata$NuPortalBoundsMinusOnePc")
    const NUVEC NuPortalBoundsMinusOnePc = {-1.0f, -1.0f, -1.0f};
#pragma const_seg()
#pragma bss_seg(".bss$NuPortalVisibilityBitsPc")
    u8 NuPortalVisibilityBitsPc[625];
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalWorldCameraPc")
    NUVEC NuPortalWorldCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalOverrideCameraPc")
    NUVEC *NuPortalOverrideCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalCameraRoomPc")
    i16 NuPortalCameraRoomPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalRoomsVisitedPc")
    i16 NuPortalRoomsVisitedPc[16];
#pragma bss_seg()
#pragma data_seg(".data$NuPortalDebugDrawPc")
    i32 NuPortalDebugDrawPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$NuPortalNearPlanePc")
    NUPLANE NuPortalNearPlanePc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalDebugEnabledPc")
    i32 NuPortalDebugEnabledPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalCameraSelectPc")
    i32 NuPortalCameraSelectPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalDebugCameraPc")
    NUCAMERA NuPortalDebugCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalRestoreCameraPc")
    NUCAMERA NuPortalRestoreCameraPc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$BoltNoSfxPc")
    i32 BoltNoSfxPc;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltNoObjectMomentumPc")
    i32 BoltNoObjectMomentumPc;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltInitSfxFn")
    i32(__cdecl *BoltInitSfxFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$ExBlowupFlagsPc")
    u32 ExBlowupFlagsPc;
#pragma bss_seg()
#pragma bss_seg(".bss$BlowupExFunc")
    i32(__cdecl *BlowupExFunc)(GIZMOBLOWUP_PC *, i32);
#pragma bss_seg()
#pragma data_seg(".data$BoltBlockContextPc")
    i32 BoltBlockContextPc = -1;
#pragma data_seg()
#pragma data_seg(".data$BoltDeactivatedActionPc")
    i16 BoltDeactivatedActionPc = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$VisibilityEvaluateEnabledPc")
    i32 VisibilityEvaluateEnabledPc = 1;
#pragma data_seg()
#pragma data_seg(".data$VisibilityOcclusionEnabledPc")
    i32 VisibilityOcclusionEnabledPc = 1;
#pragma data_seg()
#pragma data_seg(".data$VisibilityInstanceTreeEnabledPc")
    i32 VisibilityInstanceTreeEnabledPc = 1;
#pragma data_seg()
#pragma bss_seg(".bss$VisibilityCameraLockPc")
    i32 VisibilityCameraLockPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityLoadedOcclusionPc")
    i32 VisibilityLoadedOcclusionPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityUsingOcclusionPc")
    i32 VisibilityUsingOcclusionPc;
#pragma bss_seg()
#pragma data_seg(".data$VisibilityCurrentViewBoxPc")
    i32 VisibilityCurrentViewBoxPc = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$VisibilityTreeInitialisedPc")
    i32 VisibilityTreeInitialisedPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityCurrentResultPc")
    VISIBILITY_RESULT_READER_PC *VisibilityCurrentResultPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityZeroMatrixTemplatePc")
    NUMTX VisibilityZeroMatrixTemplatePc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityPrimaryPlaneMatrixPc")
    NUMTX VisibilityPrimaryPlaneMatrixPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityAlternatePlaneMatrixPc")
    NUMTX VisibilityAlternatePlaneMatrixPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityPrimaryWorldMatrixPc")
    NUMTX VisibilityPrimaryWorldMatrixPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityAlternateWorldMatrixPc")
    NUMTX VisibilityAlternateWorldMatrixPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityPrimaryAbsoluteMatrixPc")
    NUMTX VisibilityPrimaryAbsoluteMatrixPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityAlternateAbsoluteMatrixPc")
    NUMTX VisibilityAlternateAbsoluteMatrixPc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityDepthPlanePc")
    NUPLANE VisibilityDepthPlanePc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityAbsoluteDepthPlanePc")
    NUPLANE VisibilityAbsoluteDepthPlanePc;
#pragma bss_seg()
#pragma bss_seg(".bss$VisibilityAlternateHeightPc")
    f32 VisibilityAlternateHeightPc;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$grapple_attach_frames")
    u8 grapple_attach_frames = 5;
#pragma data_seg()
#pragma bss_seg(".bss$CHARCATEGORYCOUNT")
    i32 CHARCATEGORYCOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$CharCategory")
    CHAR_CATEGORY_RECORD_PC *CharCategory;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$GrabMachineIdPc")
    i16 GrabMachineIdPc = -1;
#pragma data_seg()
#pragma data_seg(".data$GrabMagnetIdPc")
    i16 GrabMagnetIdPc = -1;
#pragma data_seg()
#pragma data_seg(".data$RobotBaseIdPc")
    i16 RobotBaseIdPc = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$DisplaySceneRenderDisabledPc")
    i32 DisplaySceneRenderDisabledPc;
#pragma bss_seg()
#pragma bss_seg(".bss$DisplaySceneSpecialsDisabledPc")
    i32 DisplaySceneSpecialsDisabledPc;
#pragma bss_seg()
#pragma data_seg(".data$VisibilityPortalFilterEnabledPc")
    i32 VisibilityPortalFilterEnabledPc = 1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$PartLoadParticlePagePc")
    i32 PartLoadParticlePagePc = -1;
#pragma data_seg()
#pragma data_seg(".data$PartNullInstanceNamePc")
    char PartNullInstanceNamePc[14] = "NULL instance";
#pragma data_seg()
#pragma bss_seg(".bss$PartNearestEmitterPc")
    i32 PartNearestEmitterPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PartEditorCameraPosPc")
    NUVEC PartEditorCameraPosPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PartNearestEmitterPtrPc")
    PART_EMIT_PC *PartNearestEmitterPtrPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PartNearestTypePtrPc")
    PARTTYPE_LOOKUP_PC *PartNearestTypePtrPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PartGeneralPagePc")
    i32 PartGeneralPagePc;
#pragma bss_seg()
#pragma data_seg(".data$PartLevelPagePc")
    i32 PartLevelPagePc = 1;
#pragma data_seg()
#pragma data_seg(".data$antinode_time")
    f32 antinode_time = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$antinode_reverse_time")
    f32 antinode_reverse_time = 1.0f;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$RenderPostViewportPrefixPc")
    RENDER_POST_VIEWPORT_PREFIX_PC RenderPostViewportPrefixPc = {0.0f,   0.0f,   640.0f, 224.0f, 0.0f, 0.0f,
                                                                 640.0f, 224.0f, 1.0f,   1.0f,   0.0f, 0.0f};
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$PartPlatformImpactListPc")
    i16 PartPlatformImpactListPc[4] = {0, 0, 0, 0};
#pragma data_seg()
#pragma data_seg(".data$PartCollisionIntervalPc")
    f32 PartCollisionIntervalPc = 0.1f;
#pragma data_seg()
#pragma data_seg(".data$PartStopSpeedSquaredPc")
    f32 PartStopSpeedSquaredPc = 0.1f;
#pragma data_seg()
#pragma data_seg(".data$id_UGNAUGHT")
    i16 id_UGNAUGHT = -1;
#pragma data_seg()
#pragma data_seg(".data$OpponentSelectionHysteresisPc")
    f32 OpponentSelectionHysteresisPc = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$nbaddies_can_see_players")
    i32 nbaddies_can_see_players = 0;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$EdppUseMemcardPc")
    i32 EdppUseMemcardPc = 0;
#pragma data_seg()
#pragma data_seg(".data$EdppTypesErrorPc")
    i32 EdppTypesErrorPc = 0;
#pragma data_seg()
#pragma data_seg(".data$EdppUnresolvedInstancesPc")
    i32 EdppUnresolvedInstancesPc = 0;
#pragma data_seg()
#pragma data_seg(".data$PartDebrisNameListPc")
    char **PartDebrisNameListPc = NULL;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$spinner_gizmotype_id")
    i32 spinner_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_GRAPPLE")
    i32 LEGOCONTEXT_GRAPPLE = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$lever_gizmotype_id")
    i32 lever_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$grapple_gizmotype_id")
    i32 grapple_gizmotype_id = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$draw_attention_distance")
    f32 draw_attention_distance = 2.0f;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_START")
    u32 GAMEPAD_START = 2048;
#pragma data_seg()
#pragma data_seg(".data$active_neutral_count")
    i32 active_neutral_count = 0;
#pragma data_seg()
#pragma data_seg(".data$GameAiPartyFollowSpacingPc")
    f32 GameAiPartyFollowSpacingPc = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$JEDI_B_LDATA")
    LEVELDATA_PC *JEDI_B_LDATA = NULL;
#pragma data_seg()
#pragma data_seg(".data$id_JANGOFETT")
    i16 id_JANGOFETT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGO_AIPATHCNX_R2D2GLIDE")
    i32 LEGO_AIPATHCNX_R2D2GLIDE = 0;
#pragma data_seg()
#pragma data_seg(".data$jump_stuck_time")
    f32 jump_stuck_time = 0.10000000149011612f;
#pragma data_seg()
#pragma data_seg(".data$LEGO_AIPATHCNX_JUMP")
    i32 LEGO_AIPATHCNX_JUMP = 0;
#pragma data_seg()
#pragma data_seg(".data$drop_in_teleport")
    i32 drop_in_teleport = 3;
#pragma data_seg()
#pragma data_seg(".data$teleport_all_freeplay_modes")
    i32 teleport_all_freeplay_modes = 1;
#pragma data_seg()
#pragma data_seg(".data$LEGO_AIPATHCNX_DONTTOGGLE")
    i32 LEGO_AIPATHCNX_DONTTOGGLE = 0;
#pragma data_seg()
#pragma data_seg(".data$DEFENDTIME")
    f32 DEFENDTIME = 4.0f;
#pragma data_seg()
#pragma data_seg(".data$debug_ignore_players")
    i32 debug_ignore_players = 0;
#pragma data_seg()
#pragma data_seg(".data$WALKSTEALTH")
    i32 WALKSTEALTH = 0;
#pragma data_seg()
#pragma data_seg(".data$active_goody_count")
    i32 active_goody_count = 0;
#pragma data_seg()
#pragma data_seg(".data$active_baddy_count")
    i32 active_baddy_count = 0;
#pragma data_seg()
#pragma data_seg(".data$GameAiLosDivisorPc")
    i32 GameAiLosDivisorPc = 2;
#pragma data_seg()
#pragma data_seg(".data$timetogetlos")
    f32 timetogetlos = 0.0f;
#pragma data_seg()
#pragma data_seg(".data$test_new_interaction")
    i32 test_new_interaction = 0;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$TextureAnimationRandomPc")
    NURAND TextureAnimationRandomPc = {0};
#pragma data_seg()
#pragma data_seg(".data$TextureAnimationScriptMaskPc")
    u16 TextureAnimationScriptMaskPc = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$NuTextureEnvironmentCubePc")
    PC_TEXTURE_RESOURCE_PC *NuTextureEnvironmentCubePc = NULL;
#pragma data_seg()
#pragma data_seg(".data$NuTexturePhongCubePc")
    PC_TEXTURE_RESOURCE_PC *NuTexturePhongCubePc = NULL;
#pragma data_seg()
#pragma const_seg(".rdata$NuTextureEnvironmentCubePathPc")
    const char NuTextureEnvironmentCubePathPc[28] = "pc\\stuff\\LegoCubeMap_PC.dds";
#pragma const_seg()
#pragma const_seg(".rdata$NuTexturePhongCubePathPc")
    const char NuTexturePhongCubePathPc[25] = "pc\\stuff\\PhongMap_PC.dds";
#pragma const_seg()
#pragma const_seg(".rdata$PCDeviceTextureCubeSourcePathPc")
    const char PCDeviceTextureCubeSourcePathPc[15] = ".\\d3dCalls.cpp";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$RtlFileVersionPc")
    i32 RtlFileVersionPc = 5;
#pragma data_seg()
#pragma data_seg(".data$giztimer_gizmotype_id")
    i32 giztimer_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$gizrandom_gizmotype_id")
    i32 gizrandom_gizmotype_id = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$CameraNearClipPlanePc")
    NUVEC4 CameraNearClipPlanePc = {0.0f, 0.0f, 0.0f, 0.0f};
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$CollisionVehicleExcludedLevelPc")
    LEVELDATA_PC *CollisionVehicleExcludedLevelPc = NULL;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$RippleSetPc")
    RIPPLE_SET_PC *RippleSetPc = NULL;
#pragma data_seg()
#pragma data_seg(".data$RipplePerturbationPc")
    f32 RipplePerturbationPc = 0.15f;
#pragma data_seg()
#pragma data_seg(".data$RippleSfxCooldownPc")
    f32 RippleSfxCooldownPc = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$ForceFieldRippleRatePc")
    f32 ForceFieldRippleRatePc = 100.0f;
#pragma data_seg()
#pragma data_seg(".data$RippleForceFieldSoundNamePc")
    const char RippleForceFieldSoundNamePc[10] = "SabFField";
#pragma data_seg()
#pragma data_seg(".data$PodDustLocatorIndicesPc")
    i32 PodDustLocatorIndicesPc[2] = {1, 2};
#pragma data_seg()
#pragma data_seg(".data$PodDustEmissionRatePc")
    f32 PodDustEmissionRatePc = 100.0f;
#pragma data_seg()
#pragma data_seg(".data$PodDustHeightScalePc")
    f32 PodDustHeightScalePc = 2.0f;
#pragma data_seg()
#pragma data_seg(".data$GunshipDustEmissionRatePc")
    f32 GunshipDustEmissionRatePc = 75.0f;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$CameraSavedStateCountPc")
    i32 CameraSavedStateCountPc = 0;
#pragma data_seg()
#pragma data_seg(".data$CameraSavedStatesPc")
    CAMERA_SAVED_STATE_PC CameraSavedStatesPc[16] = {{0}};
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$PlayPlanePc")
    KEEP_SCREEN_PLANE_PC PlayPlanePc[6] = {};
#pragma data_seg()
#pragma data_seg(".data$KEEPONSCREEN_SIDESONLY")
    i32 KEEPONSCREEN_SIDESONLY = 0;
#pragma data_seg()
#pragma data_seg(".data$GunshipANorm")
    NUVEC GunshipANorm = {};
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$FramePadCorrectDeadZoningPc")
    u8 FramePadCorrectDeadZoningPc = 0;
#pragma data_seg()
#pragma data_seg(".data$FramePadProfilePortPc")
    i32 FramePadProfilePortPc = -1;
#pragma data_seg()
#pragma data_seg(".data$FramePadMaxGamePadsPc")
    i32 FramePadMaxGamePadsPc = 2;
#pragma data_seg()
#pragma data_seg(".data$FrameMaterialAnimationSpeedPc")
    f32 FrameMaterialAnimationSpeedPc = 1.0f;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$bgproc_thread_handle_pc")
    HANDLE bgproc_thread_handle_pc = NULL;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$PCGraphicsRenderTextures")
    GRAPHICS_RENDER_TEXTURE_PC PCGraphicsRenderTextures[32] = {};
#pragma data_seg()
#pragma data_seg(".data$PCGraphicsRenderTextureActive")
    u8 PCGraphicsRenderTextureActive[32] = {};
#pragma data_seg()
#pragma data_seg(".data$PCGraphicsRenderTextureBytes")
    i32 PCGraphicsRenderTextureBytes = 0;
#pragma data_seg()
#pragma data_seg(".data$GizmoPickupSys_Game")
    PICKUP_TYPE_SYSTEM_PC GizmoPickupSys_Game = {GizmoPickupType, CoinTab, 10, 4, 4, 2, 0, 0};
#pragma data_seg()
#pragma data_seg(".data$COINMAGNETSCALE")
    f32 COINMAGNETSCALE = 3.0f;
#pragma data_seg()
#pragma data_seg(".data$GizmoPickups_Collide2DFn")
    f32(__cdecl *GizmoPickups_Collide2DFn)(GAMEOBJECT *) = NULL;
#pragma data_seg()
}
