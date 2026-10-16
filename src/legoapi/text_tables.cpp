#include "saga.h"
extern "C" {
    extern i16 TextCrawlChapterIdPc;
    extern i16 TextCrawlVehicleBonusIdPc;
    extern i16 TextCrawlCharacterBonusIdPc;
    extern i16 tNULL;
    extern i16 tUNKNOWN;
    extern i16 tENGLISH;
    extern i16 tFRENCH;
    extern i16 tGERMAN;
    extern i16 tITALIAN;
    extern i16 tSPANISH;
    extern i16 tDANISH;
    i16 TextIdPc8806dc = 0;
    i16 TextIdPc8806e0 = 0;
    i16 TextIdPc8806e4 = 0;
    i16 TextIdPc8806e8 = 0;
    i16 TextIdPc8806ec = 0;
    i16 TextIdPc8806f4 = 0;
    i16 TextIdPc8806f8 = 0;
    i16 TextIdPc8806fc = 0;
    i16 TextIdPc880700 = 0;
    i16 TextIdPc880704 = 0;
    i16 TextIdPc880708 = 0;
    i16 TextIdPc88070c = 0;
    i16 TextIdPc880710 = 0;
    i16 TextIdPc880714 = 0;
    i16 TextIdPc880718 = 0;
    i16 TextIdPc88071c = 0;
    i16 TextIdPc880720 = 0;
    i16 TextIdPc880724 = 0;
    i16 TextIdPc880728 = 0;
    i16 TextIdPc88072c = 0;
    i16 TextIdPc880730 = 0;
    i16 TextIdPc880734 = 0;
    i16 TextIdPc880738 = 0;
    i16 TextIdPc88073c = 0;
    i16 TextIdPc880740 = 0;
    i16 TextIdPc880744 = 0;
    i16 TextIdPc880748 = 0;
    i16 TextIdPc88074c = 0;
    i16 TextIdPc880750 = 0;
    i16 TextIdPc880754 = 0;
    i16 TextIdPc880758 = 0;
    i16 TextIdPc88075c = 0;
    i16 TextIdPc880760 = 0;
    i16 TextIdPc880764 = 0;
    i16 TextIdPc880768 = 0;
    i16 TextIdPc88076c = 0;
    i16 TextIdPc880770 = 0;
    i16 TextIdPc880774 = 0;
    i16 TextIdPc880778 = 0;
    i16 TextIdPc88077c = 0;
    i16 TextIdPc880780 = 0;
    i16 TextIdPc880784 = 0;
    i16 TextIdPc880788 = 0;
    i16 TextIdPc88078c = 0;
    i16 TextIdPc880790 = 0;
    i16 TextIdPc880794 = 0;
    i16 TextIdPc880798 = 0;
    i16 TextIdPc88079c = 0;
    i16 TextIdPc8807a0 = 0;
    i16 TextIdPc8807a4 = 0;
    extern i16 PermanentBountyTitleIdPc;
    extern i16 PCArcadeModeText0;
    extern i16 PCArcadeModeText1;
    extern i16 PCArcadeModeText2;
    i16 TextIdPc8807b8 = 0;
    i16 TextIdPc8807bc = 0;
    i16 TextIdPc8807c0 = 0;
    i16 TextIdPc8807c4 = 0;
    i16 TextIdPc8807c8 = 0;
    i16 TextIdPc8807cc = 0;
    i16 TextIdPc8807d0 = 0;
    i16 TextIdPc8807d4 = 0;
    extern i16 PermanentAllTitleIdPc;
    extern i16 PermanentBlasterTitleIdPc;
    extern i16 PCArcadeModeText;
    i16 TextIdPc8807e4 = 0;
    i16 TextIdPc8807e8 = 0;
    i16 TextIdPc8807ec = 0;
    i16 TextIdPc8807f0 = 0;
    i16 TextIdPc8807f4 = 0;
    extern i16 PCArcadePlayText;
    i16 TextIdPc8807fc = 0;
    i16 TextIdPc880800 = 0;
    extern i16 tCLIPi;
    extern i16 tINTRO;
    extern i16 tMIDTRO;
    extern i16 tOUTRO;
    extern i16 tENDING;
    i16 TextIdPc880818 = 0;
    i16 TextIdPc88081c = 0;
    i16 TextIdPc880820 = 0;
    i16 TextIdPc880824 = 0;
    i16 TextIdPc880828 = 0;
    i16 TextIdPc88082c = 0;
    i16 TextIdPc880830 = 0;
    i16 TextIdPc880834 = 0;
    i16 TextIdPc880838 = 0;
    i16 TextIdPc88083c = 0;
    i16 TextIdPc880840 = 0;
    i16 TextIdPc880844 = 0;
    i16 TextIdPc880848 = 0;
    i16 TextIdPc88084c = 0;
    i16 TextIdPc880850 = 0;
    i16 TextIdPc880854 = 0;
    i16 TextIdPc880858 = 0;
    i16 TextIdPc88085c = 0;
    i16 TextIdPc880860 = 0;
    i16 TextIdPc880864 = 0;
    i16 TextIdPc880868 = 0;
    i16 TextIdPc88086c = 0;
    i16 TextIdPc880870 = 0;
    i16 TextIdPc880874 = 0;
    extern i16 tON;
    extern i16 tOFF;
    i16 TextIdPc880880 = 0;
    extern i16 tACCEPT;
    i16 TextIdPc880888 = 0;
    extern i16 tBACK;
    i16 TextIdPc880890 = 0;
    i16 TextIdPc880894 = 0;
    i16 TextIdPc880898 = 0;
    i16 TextIdPc88089c = 0;
    i16 TextIdPc8808a0 = 0;
    i16 TextIdPc8808a4 = 0;
    i16 TextIdPc8808a8 = 0;
    i16 TextIdPc8808ac = 0;
    i16 TextIdPc8808b0 = 0;
    i16 TextIdPc8808b4 = 0;
    i16 TextIdPc8808b8 = 0;
    i16 TextIdPc8808bc = 0;
    i16 TextIdPc8808c0 = 0;
    i16 TextIdPc8808c4 = 0;
    i16 TextIdPc8808c8 = 0;
    i16 TextIdPc8808cc = 0;
    i16 TextIdPc8808d0 = 0;
    i16 TextIdPc8808d4 = 0;
    i16 TextIdPc8808d8 = 0;
    extern i16 PCArcadeLevelText;
    i16 TextIdPc8808e0 = 0;
    i16 TextIdPc8808e4 = 0;
    i16 TextIdPc8808e8 = 0;
    i16 TextIdPc8808ec = 0;
    i16 TextIdPc8808f0 = 0;
    i16 TextIdPc8808f4 = 0;
    i16 TextIdPc8808f8 = 0;
    i16 TextIdPc8808fc = 0;
    i16 TextIdPc880900 = 0;
    i16 TextIdPc880904 = 0;
    i16 TextIdPc880908 = 0;
    i16 TextIdPc88090c = 0;
    i16 TextIdPc880910 = 0;
    i16 TextIdPc880914 = 0;
    extern i16 tAUDIOVOLUME;
    i16 TextIdPc88091c = 0;
    i16 TextIdPc880920 = 0;
    i16 TextIdPc880924 = 0;
    i16 TextIdPc880928 = 0;
    i16 TextIdPc88092c = 0;
    i16 TextIdPc880930 = 0;
    i16 TextIdPc880934 = 0;
    i16 TextIdPc880938 = 0;
    extern i16 tMUSIC;
    i16 TextIdPc880940 = 0;
    i16 TextIdPc880944 = 0;
    i16 TextIdPc880948 = 0;
    i16 TextIdPc88094c = 0;
    i16 TextIdPc880950 = 0;
    i16 TextIdPc880954 = 0;
    i16 TextIdPc880958 = 0;
    i16 TextIdPc88095c = 0;
    i16 TextIdPc880960 = 0;
    extern i16 tBATMANSUIT;
    extern i16 tSHADOWSUIT;
    extern i16 tGLIDESUIT;
    extern i16 tDEMOLITIONSUIT;
    extern i16 tSONARSUIT;
    extern i16 tROBINSUIT;
    extern i16 tWATERSUIT;
    extern i16 tTECHNOLOGYSUIT;
    extern i16 tMAGNETSUIT;
    extern i16 tATTRACTSUIT;
    i16 TextIdPc88098c = 0;
    i16 TextIdPc880990 = 0;
    i16 TextIdPc880994 = 0;
    i16 TextIdPc880998 = 0;
    i16 TextIdPc88099c = 0;
    i16 TextIdPc8809a0 = 0;
    i16 TextIdPc8809a4 = 0;
    i16 TextIdPc8809a8 = 0;
    i16 TextIdPc8809ac = 0;
    i16 TextIdPc8809b0 = 0;
    i16 TextIdPc8809b4 = 0;
    i16 TextIdPc8809b8 = 0;
    i16 TextIdPc8809bc = 0;
    i16 TextIdPc8809c0 = 0;
    i16 TextIdPc8809c4 = 0;
    i16 TextIdPc8809c8 = 0;
    i16 TextIdPc8809cc = 0;
    i16 TextIdPc8809d0 = 0;
    i16 TextIdPc8809d4 = 0;
    i16 TextIdPc8809d8 = 0;
    i16 TextIdPc8809dc = 0;
    i16 TextIdPc8809e0 = 0;
    i16 TextIdPc8809e4 = 0;
    i16 TextIdPc8809e8 = 0;
    i16 TextIdPc8809ec = 0;
    i16 TextIdPc8809f0 = 0;
    i16 TextIdPc8809f4 = 0;
    i16 TextIdPc8809f8 = 0;
    i16 TextIdPc8809fc = 0;
    i16 TextIdPc880a00 = 0;
    i16 TextIdPc880a04 = 0;
    i16 TextIdPc880a08 = 0;
    i16 TextIdPc880a0c = 0;
    i16 TextIdPc880a10 = 0;
    i16 TextIdPc880a14 = 0;
    i16 TextIdPc880a18 = 0;
    i16 TextIdPc880a1c = 0;
    i16 TextIdPc880a20 = 0;
    i16 TextIdPc880a24 = 0;
    i16 TextIdPc880a28 = 0;
    i16 TextIdPc880a2c = 0;
    i16 TextIdPc880a30 = 0;
    i16 TextIdPc880a34 = 0;
    i16 TextIdPc880a38 = 0;
    i16 TextIdPc880a3c = 0;
    i16 TextIdPc880a40 = 0;
    i16 TextIdPc880a44 = 0;
    i16 TextIdPc880a48 = 0;
    i16 TextIdPc880a4c = 0;
    i16 TextIdPc880a50 = 0;
    i16 TextIdPc880a54 = 0;
    i16 TextIdPc880a58 = 0;
    i16 TextIdPc880a5c = 0;
    i16 TextIdPc880a60 = 0;
    i16 TextIdPc880a64 = 0;
    i16 TextIdPc880a68 = 0;
    i16 TextIdPc880a6c = 0;
    i16 TextIdPc880a70 = 0;
    i16 TextIdPc880a74 = 0;
    i16 TextIdPc880a78 = 0;
    i16 TextIdPc880a7c = 0;
    i16 TextIdPc880a80 = 0;
    i16 TextIdPc880a84 = 0;
    i16 TextIdPc880a88 = 0;
    i16 TextIdPc880a8c = 0;
    i16 TextIdPc880a90 = 0;
    i16 TextIdPc880a94 = 0;
    i16 TextIdPc880a98 = 0;
    i16 TextIdPc880a9c = 0;
    i16 TextIdPc880aa0 = 0;
    i16 TextIdPc880aa4 = 0;
    i16 TextIdPc880aa8 = 0;
    i16 TextIdPc880aac = 0;
    i16 TextIdPc880ab0 = 0;
    i16 TextIdPc880ab4 = 0;
    i16 TextIdPc880ab8 = 0;
    i16 TextIdPc880abc = 0;
    i16 TextIdPc880ac0 = 0;
    i16 TextIdPc880ac4 = 0;
    i16 TextIdPc880ac8 = 0;
    i16 TextIdPc880acc = 0;
    i16 TextIdPc880ad0 = 0;
    i16 TextIdPc880ad4 = 0;
    i16 TextIdPc880ad8 = 0;
    i16 TextIdPc880adc = 0;
    i16 TextIdPc880ae0 = 0;
    i16 TextIdPc880ae4 = 0;
    i16 TextIdPc880ae8 = 0;
    i16 TextIdPc880aec = 0;
    i16 TextIdPc880af0 = 0;
    i16 TextIdPc880af4 = 0;
    i16 TextIdPc880af8 = 0;
    i16 TextIdPc880afc = 0;
    i16 TextIdPc880b00 = 0;
    i16 TextIdPc880b04 = 0;
    i16 TextIdPc880b08 = 0;
    i16 TextIdPc880b0c = 0;
    i16 TextIdPc880b10 = 0;
    i16 TextIdPc880b14 = 0;
    i16 TextIdPc880b18 = 0;
    i16 TextIdPc880b1c = 0;
    i16 TextIdPc880b20 = 0;
    i16 TextIdPc880b24 = 0;
    i16 TextIdPc880b28 = 0;
    i16 TextIdPc880b2c = 0;
    i16 TextIdPc880b30 = 0;
    i16 TextIdPc880b34 = 0;
    i16 TextIdPc880b38 = 0;
    i16 TextIdPc880b3c = 0;
    i16 TextIdPc880b40 = 0;
    i16 TextIdPc880b44 = 0;
    i16 TextIdPc880b48 = 0;
    i16 TextIdPc880b4c = 0;
    i16 TextIdPc880b50 = 0;
    i16 TextIdPc880b54 = 0;
    i16 TextIdPc880b58 = 0;
    i16 TextIdPc880b5c = 0;
    i16 TextIdPc880b60 = 0;
    i16 TextIdPc880b64 = 0;
    i16 TextIdPc880b68 = 0;
    i16 TextIdPc880b6c = 0;
    i16 TextIdPc880b70 = 0;
    i16 TextIdPc880b74 = 0;
    i16 TextIdPc880b78 = 0;
    i16 TextIdPc880b7c = 0;
    i16 TextIdPc880b80 = 0;
    i16 TextIdPc880b84 = 0;
    i16 TextIdPc880b88 = 0;
    i16 TextIdPc880b8c = 0;
    i16 TextIdPc880b90 = 0;
    i16 TextIdPc880b94 = 0;
    i16 TextIdPc880b98 = 0;
    i16 TextIdPc880b9c = 0;
    i16 TextIdPc880ba0 = 0;
    i16 TextIdPc880ba4 = 0;
    i16 TextIdPc880ba8 = 0;
    i16 TextIdPc880bac = 0;
    i16 TextIdPc880bb0 = 0;
    i16 TextIdPc880bb4 = 0;
    i16 TextIdPc880bb8 = 0;
    i16 TextIdPc880bbc = 0;
    i16 TextIdPc880bc0 = 0;
    i16 TextIdPc880bc4 = 0;
    i16 TextIdPc880bc8 = 0;
    i16 TextIdPc880bcc = 0;
    i16 TextIdPc880bd0 = 0;
    i16 TextIdPc880bd4 = 0;
    i16 TextIdPc880bd8 = 0;
    i16 TextIdPc880bdc = 0;
    i16 TextIdPc880be0 = 0;
    i16 TextIdPc880be4 = 0;
    i16 TextIdPc880be8 = 0;
    i16 TextIdPc880bec = 0;
    i16 TextIdPc880bf0 = 0;
    i16 TextIdPc880bf4 = 0;
    i16 TextIdPc880bf8 = 0;
    i16 TextIdPc880bfc = 0;
    i16 TextIdPc880c00 = 0;
    i16 TextIdPc880c04 = 0;
    i16 TextIdPc880c08 = 0;
    i16 TextIdPc880c0c = 0;
    i16 TextIdPc880c10 = 0;
    i16 TextIdPc880c14 = 0;
    i16 TextIdPc880c18 = 0;
    i16 TextIdPc880c1c = 0;
    i16 TextIdPc880c20 = 0;
    i16 TextIdPc880c24 = 0;
    i16 TextIdPc880c34 = 0;
    i16 TextIdPc880c38 = 0;
    i16 TextIdPc880c3c = 0;
    i16 TextIdPc880c40 = 0;
    i16 TextIdPc880c44 = 0;
    i16 TextIdPc880c48 = 0;
    i16 TextIdPc880c4c = 0;
    i16 TextIdPc880c50 = 0;
    i16 TextIdPc880c54 = 0;
    i16 TextIdPc880c58 = 0;
    i16 TextIdPc880c5c = 0;
    i16 TextIdPc880c60 = 0;
    i16 TextIdPc880c64 = 0;
    i16 TextIdPc880c68 = 0;
    i16 TextIdPc880c6c = 0;
    i16 TextIdPc880c70 = 0;
    i16 TextIdPc880c74 = 0;
    i16 TextIdPc880c78 = 0;
    i16 TextIdPc880c7c = 0;
    i16 TextIdPc880c80 = 0;
    i16 TextIdPc880c84 = 0;
    extern i16 tTOUCHTOSTART;
    i16 TextIdPc880c8c = 0;
    i16 TextIdPc880c90 = 0;
    i16 TextIdPc880c94 = 0;
    i16 TextIdPc880c98 = 0;
    i16 TextIdPc880c9c = 0;
    i16 TextIdPc880ca0 = 0;
    i16 TextIdPc880ca4 = 0;
    i16 TextIdPc880ca8 = 0;
    i16 TextIdPc880cac = 0;
    i16 TextIdPc880cb0 = 0;
    i16 TextIdPc880cb4 = 0;
    i16 TextIdPc880cb8 = 0;
    i16 TextIdPc880cbc = 0;
    i16 TextIdPc880cc0 = 0;
    i16 TextIdPc880cc4 = 0;
    i16 TextIdPc880cc8 = 0;
    i16 TextIdPc880ccc = 0;
    i16 TextIdPc880cd0 = 0;
    i16 TextIdPc880cd4 = 0;
    i16 TextIdPc880cd8 = 0;
    i16 TextIdPc880cdc = 0;
    i16 TextIdPc880ce0 = 0;
    i16 TextIdPc880ce4 = 0;
    i16 TextIdPc880ce8 = 0;
    i16 TextIdPc880cec = 0;
    i16 TextIdPc880cf0 = 0;
    i16 TextIdPc880cf4 = 0;
    i16 TextIdPc880cf8 = 0;
    i16 TextIdPc880cfc = 0;
    i16 TextIdPc880d00 = 0;
    i16 TextIdPc880d04 = 0;
    i16 TextIdPc880d08 = 0;
    i16 TextIdPc880d0c = 0;
    i16 TextIdPc880d10 = 0;
    i16 TextIdPc880d14 = 0;
    i16 TextIdPc880d18 = 0;
    i16 TextIdPc880d1c = 0;
    i16 TextIdPc880d20 = 0;
    i16 TextIdPc880d24 = 0;
    i16 TextIdPc880d28 = 0;
    i16 TextIdPc880d2c = 0;
    i16 TextIdPc880d30 = 0;
    i16 TextIdPc880d34 = 0;
    i16 TextIdPc880d38 = 0;
    i16 TextIdPc880d3c = 0;
    i16 TextIdPc880d40 = 0;
    i16 TextIdPc880d44 = 0;
    i16 TextIdPc880d48 = 0;
    i16 TextIdPc880d4c = 0;
    i16 TextIdPc880d50 = 0;
    i16 TextIdPc880d54 = 0;
    i16 TextIdPc880d58 = 0;
    i16 TextIdPc880d5c = 0;
    i16 TextIdPc880d60 = 0;
    i16 TextIdPc880d64 = 0;
    i16 TextIdPc880d68 = 0;
    i16 TextIdPc880d6c = 0;
    i16 TextIdPc880d70 = 0;
    i16 TextIdPc880d74 = 0;
    i16 TextIdPc880d78 = 0;
    i16 TextIdPc880d7c = 0;
    i16 TextIdPc880d80 = 0;
    i16 TextIdPc880d84 = 0;
    i16 TextIdPc880d88 = 0;
    i16 TextIdPc880d8c = 0;
    i16 TextIdPc880d90 = 0;
    i16 TextIdPc880d94 = 0;
    i16 TextIdPc880d98 = 0;
    i16 TextIdPc880d9c = 0;
    i16 TextIdPc880da0 = 0;
    i16 TextIdPc880da4 = 0;
    i16 TextIdPc880da8 = 0;
    i16 TextIdPc880dac = 0;
    i16 TextIdPc880db0 = 0;
    i16 TextIdPc880db4 = 0;
    i16 TextIdPc880db8 = 0;
    i16 TextIdPc880dbc = 0;
    i16 TextIdPc880dc0 = 0;
    i16 TextIdPc880dc4 = 0;
    i16 TextIdPc880dc8 = 0;
    i16 TextIdPc880dcc = 0;
    i16 TextIdPc880dd0 = 0;
    i16 TextIdPc880dd4 = 0;
    i16 TextIdPc880dd8 = 0;
    i16 TextIdPc880ddc = 0;
    i16 TextIdPc880de0 = 0;
    i16 TextIdPc880de4 = 0;
    i16 TextIdPc880de8 = 0;
    i16 TextIdPc880dec = 0;
    i16 TextIdPc880df0 = 0;
    i16 TextIdPc880df4 = 0;
    i16 TextIdPc880df8 = 0;
    i16 TextIdPc880dfc = 0;
    i16 TextIdPc880e00 = 0;
    i16 TextIdPc880e04 = 0;
    i16 TextIdPc880e08 = 0;
    i16 TextIdPc880e0c = 0;
    i16 TextIdPc880e10 = 0;
    i16 TextIdPc880e14 = 0;
    i16 TextIdPc880e18 = 0;
    i16 TextIdPc880e1c = 0;
    i16 TextIdPc880e20 = 0;
    i16 TextIdPc880e24 = 0;
    i16 TextIdPc880e28 = 0;
    i16 TextIdPc880e2c = 0;
    i16 TextIdPc880e30 = 0;
    i16 TextIdPc880e34 = 0;
    i16 TextIdPc880e38 = 0;
    i16 TextIdPc880e3c = 0;
    i16 TextIdPc880e40 = 0;
    i16 TextIdPc880e44 = 0;
    i16 TextIdPc880e48 = 0;
    i16 TextIdPc880e4c = 0;
    i16 TextIdPc880e50 = 0;
    i16 TextIdPc880e54 = 0;
    i16 TextIdPc880e58 = 0;
    i16 TextIdPc880e5c = 0;
    i16 TextIdPc880e60 = 0;
    i16 TextIdPc880e64 = 0;
    i16 TextIdPc880e68 = 0;
    i16 TextIdPc880e6c = 0;
    i16 TextIdPc880e70 = 0;
    i16 TextIdPc880e74 = 0;
    i16 TextIdPc880e78 = 0;
    i16 TextIdPc880e7c = 0;
    i16 TextIdPc880e80 = 0;
    i16 TextIdPc880e84 = 0;
    i16 TextIdPc880e88 = 0;
    i16 TextIdPc880e8c = 0;
    i16 TextIdPc880e90 = 0;
    i16 TextIdPc880e94 = 0;
    i16 TextIdPc880e98 = 0;
    i16 TextIdPc880e9c = 0;
    i16 TextIdPc880ea0 = 0;
    i16 TextIdPc880ea4 = 0;
    i16 TextIdPc880ea8 = 0;
    i16 TextIdPc880eac = 0;
    i16 TextIdPc880eb0 = 0;
    i16 TextIdPc880eb4 = 0;
    i16 TextIdPc880eb8 = 0;
    i16 TextIdPc880ebc = 0;
    i16 TextIdPc880ec0 = 0;
    i16 TextIdPc880ec4 = 0;
    i16 TextIdPc880ec8 = 0;
    i16 TextIdPc880ecc = 0;
    i16 TextIdPc880ed0 = 0;
    i16 TextIdPc880ed4 = 0;
    i16 TextIdPc880ed8 = 0;
    i16 TextIdPc880edc = 0;
    i16 TextIdPc880ee0 = 0;
    i16 TextIdPc880ee4 = 0;
    i16 TextIdPc880ee8 = 0;
    i16 TextIdPc880eec = 0;
    i16 TextIdPc880ef0 = 0;
    i16 TextIdPc880ef4 = 0;
    i16 TextIdPc880ef8 = 0;
    i16 TextIdPc880efc = 0;
    i16 TextIdPc880f00 = 0;
    i16 TextIdPc880f04 = 0;
    i16 TextIdPc880f08 = 0;
    i16 TextIdPc880f0c = 0;
    i16 TextIdPc880f10 = 0;
    i16 TextIdPc880f14 = 0;
    i16 TextIdPc880f18 = 0;
    i16 TextIdPc880f1c = 0;
    i16 TextIdPc880f20 = 0;
    i16 TextIdPc880f24 = 0;
    i16 TextIdPc880f28 = 0;
    i16 TextIdPc880f2c = 0;
    i16 TextIdPc880f30 = 0;
    i16 TextIdPc880f34 = 0;
    i16 TextIdPc880f38 = 0;
    i16 TextIdPc880f3c = 0;
    i16 TextIdPc880f40 = 0;
    i16 TextIdPc880f44 = 0;
    i16 TextIdPc880f48 = 0;
    i16 TextIdPc880f4c = 0;
    i16 TextIdPc880f50 = 0;
    i16 TextIdPc880f64 = 0;
    i16 TextIdPc880f68 = 0;
    i16 TextIdPc880f6c = 0;
    i16 TextIdPc880f70 = 0;
    i16 TextIdPc880f74 = 0;
    i16 TextIdPc880f78 = 0;
    i16 TextIdPc880f7c = 0;
    i16 TextIdPc880f80 = 0;
    i16 TextIdPc880f84 = 0;
    i16 TextIdPc880f88 = 0;
    i16 TextIdPc880f8c = 0;
    extern i16 tALONGTIMEAGO;
    i16 TextIdPc880f94 = 0;
    i16 TextIdPc880f98 = 0;
    i16 TextIdPc880f9c = 0;
    i16 TextIdPc880fa0 = 0;
    i16 TextIdPc880fa4 = 0;
    i16 TextIdPc880fa8 = 0;
    i16 TextIdPc880fac = 0;
    i16 TextIdPc880fb0 = 0;
    i16 TextIdPc880fb4 = 0;
    i16 TextIdPc880fb8 = 0;
    i16 TextIdPc880fbc = 0;
    i16 TextIdPc880fc0 = 0;
    i16 TextIdPc880fc4 = 0;
    i16 TextIdPc880fc8 = 0;
    i16 TextIdPc880fcc = 0;
    i16 TextIdPc880fd0 = 0;
    i16 TextIdPc880fd4 = 0;
    i16 TextIdPc880fd8 = 0;
    i16 TextIdPc880fdc = 0;
    i16 TextIdPc880fe0 = 0;
    i16 TextIdPc880fe4 = 0;
    i16 TextIdPc880fe8 = 0;
    i16 TextIdPc880fec = 0;
    i16 TextIdPc880ff0 = 0;
    i16 TextIdPc880ff4 = 0;
    i16 TextIdPc880ff8 = 0;
    i16 TextIdPc880ffc = 0;
    i16 TextIdPc881000 = 0;
    i16 TextIdPc881004 = 0;
    i16 TextIdPc881008 = 0;
    i16 TextIdPc88100c = 0;
    extern i16 MissionMenuInstructionTextPc;
    i16 TextIdPc881014 = 0;
    i16 TextIdPc881018 = 0;
    i16 TextIdPc88101c = 0;
    i16 TextIdPc881020 = 0;
    i16 TextIdPc881024 = 0;
    i16 TextIdPc881028 = 0;
    i16 TextIdPc88102c = 0;
    i16 TextIdPc881030 = 0;
    i16 TextIdPc881034 = 0;
    i16 TextIdPc881038 = 0;
    i16 TextIdPc88103c = 0;
    i16 TextIdPc881040 = 0;
    i16 TextIdPc881044 = 0;
    i16 TextIdPc881048 = 0;
    i16 TextIdPc88104c = 0;
    i16 TextIdPc881050 = 0;
    i16 TextIdPc881054 = 0;
    i16 TextIdPc881058 = 0;
    i16 TextIdPc88105c = 0;
    i16 TextIdPc881060 = 0;
    i16 TextIdPc881064 = 0;
    i16 TextIdPc881068 = 0;
    i16 TextIdPc88106c = 0;
    i16 TextIdPc881070 = 0;
    i16 TextIdPc881074 = 0;
    i16 TextIdPc881078 = 0;
    i16 TextIdPc88107c = 0;
    i16 TextIdPc881080 = 0;
    i16 TextIdPc881084 = 0;
    i16 TextIdPc881088 = 0;
    i16 TextIdPc88108c = 0;
    i16 TextIdPc881090 = 0;
    i16 TextIdPc881094 = 0;
    i16 TextIdPc881098 = 0;
    i16 TextIdPc88109c = 0;
    i16 TextIdPc8810a0 = 0;
    i16 TextIdPc8810a4 = 0;
    i16 TextIdPc8810a8 = 0;
    i16 TextIdPc8810ac = 0;
    i16 TextIdPc8810b0 = 0;
    i16 TextIdPc8810b4 = 0;
    i16 TextIdPc8810b8 = 0;
    extern i16 tPOWERUP;
    i16 TextIdPc8810c0 = 0;
    i16 TextIdPc8810c4 = 0;
    i16 TextIdPc8810c8 = 0;
    i16 TextIdPc8810cc = 0;
    i16 TextIdPc8810d0 = 0;
    i16 TextIdPc8810d4 = 0;
    i16 TextIdPc8810d8 = 0;
    i16 TextIdPc8810dc = 0;
    i16 TextIdPc8810e0 = 0;
    i16 TextIdPc8810e4 = 0;
    i16 TextIdPc8810e8 = 0;
    i16 TextIdPc8810ec = 0;
    i16 TextIdPc8810f0 = 0;
    i16 TextIdPc8810f4 = 0;
    i16 TextIdPc8810f8 = 0;
    i16 TextIdPc8810fc = 0;
    i16 TextIdPc881100 = 0;
    i16 TextIdPc881104 = 0;
    i16 TextIdPc881108 = 0;
    i16 TextIdPc88110c = 0;
    i16 TextIdPc881110 = 0;
    i16 TextIdPc881114 = 0;
    i16 TextIdPc881118 = 0;
    i16 TextIdPc88111c = 0;
    i16 TextIdPc881120 = 0;
    i16 TextIdPc881124 = 0;
    i16 TextIdPc881128 = 0;
    i16 TextIdPc88112c = 0;
    i16 TextIdPc881130 = 0;
    i16 TextIdPc881134 = 0;
    i16 TextIdPc881138 = 0;
    i16 TextIdPc88113c = 0;
    i16 TextIdPc881140 = 0;
    i16 TextIdPc881144 = 0;
    i16 TextIdPc881148 = 0;
    i16 TextIdPc88114c = 0;
    extern i16 PermanentJediTitleIdPc;
    i16 TextIdPc881154 = 0;
    i16 TextIdPc881158 = 0;
    i16 TextIdPc88115c = 0;
    i16 TextIdPc881160 = 0;
    i16 TextIdPc881164 = 0;
    i16 TextIdPc881168 = 0;
    i16 TextIdPc88116c = 0;
    i16 TextIdPc881170 = 0;
    i16 TextIdPc881174 = 0;
    i16 TextIdPc881178 = 0;
    i16 TextIdPc88117c = 0;
    i16 TextIdPc881180 = 0;
    i16 TextIdPc881184 = 0;
    i16 TextIdPc881188 = 0;
    i16 TextIdPc88118c = 0;
    i16 TextIdPc881190 = 0;
    i16 TextIdPc881194 = 0;
    i16 TextIdPc881198 = 0;
    i16 TextIdPc88119c = 0;
    i16 TextIdPc8811a0 = 0;
    i16 TextIdPc8811a4 = 0;
    i16 TextIdPc8811a8 = 0;
    i16 TextIdPc8811ac = 0;
    i16 TextIdPc8811b0 = 0;
    i16 TextIdPc8811b4 = 0;
    i16 TextIdPc8811b8 = 0;
    i16 TextIdPc8811bc = 0;
    i16 TextIdPc8811c0 = 0;
    i16 TextIdPc8811c4 = 0;
    i16 TextIdPc8811c8 = 0;
    i16 TextIdPc8811cc = 0;
    i16 TextIdPc8811d0 = 0;
    i16 TextIdPc8811d4 = 0;
    i16 TextIdPc8811d8 = 0;
    i16 TextIdPc8811dc = 0;
    i16 TextIdPc8811e0 = 0;
    i16 TextIdPc8811e4 = 0;
    i16 TextIdPc8811e8 = 0;
    i16 TextIdPc8811ec = 0;
    i16 TextIdPc8811f0 = 0;
    i16 TextIdPc8811f4 = 0;
    i16 TextIdPc8811f8 = 0;
    TEXTENTRY Text_TableMenu[167] = {
        {&tNULL, 0, 0},
        {&tUNKNOWN, 1, 0},
        {&tENGLISH, 2, 0},
        {&tFRENCH, 3, 0},
        {&tGERMAN, 4, 0},
        {&tITALIAN, 5, 0},
        {&tSPANISH, 6, 0},
        {&tDANISH, 7, 0},
        {&TextIdPc8806dc, 8, 0},
        {&TextIdPc8806e0, 200, 0},
        {&TextIdPc8806e4, -1, 0},
        {&TextIdPc8806e8, -1, 0},
        {&TextIdPc8806ec, -1, 0},
        {&TextIdPc880700, -1, 0},
        {&TextIdPc8806f4, -1, 0},
        {&TextIdPc8806f8, -1, 0},
        {&TextIdPc8806fc, -1, 0},
        {&TextIdPc880704, -1, 0},
        {&TextIdPc880708, -1, 0},
        {&TextIdPc88070c, -1, 0},
        {&TextCrawlChapterIdPc, -1, 0},
        {&TextIdPc880710, -1, 0},
        {&TextIdPc880714, -1, 0},
        {&TextIdPc880718, -1, 0},
        {&TextIdPc88071c, -1, 0},
        {&TextIdPc880720, -1, 0},
        {&TextIdPc880724, -1, 0},
        {&TextCrawlCharacterBonusIdPc, -1, 0},
        {&TextCrawlVehicleBonusIdPc, -1, 0},
        {&TextIdPc880728, -1, 0},
        {&TextIdPc88072c, -1, 0},
        {&TextIdPc880730, -1, 0},
        {&TextIdPc880734, -1, 0},
        {&TextIdPc880738, -1, 0},
        {&TextIdPc88073c, -1, 0},
        {&TextIdPc880740, -1, 0},
        {&TextIdPc880744, -1, 0},
        {&TextIdPc880748, -1, 0},
        {&TextIdPc88074c, -1, 0},
        {&TextIdPc880750, -1, 0},
        {&TextIdPc880754, -1, 0},
        {&TextIdPc880758, -1, 0},
        {&TextIdPc88075c, -1, 0},
        {&TextIdPc880760, -1, 0},
        {&TextIdPc880764, -1, 0},
        {&TextIdPc880768, -1, 0},
        {&TextIdPc88076c, -1, 0},
        {&TextIdPc880774, -1, 0},
        {&TextIdPc880778, -1, 0},
        {&TextIdPc88077c, -1, 0},
        {&TextIdPc880780, -1, 0},
        {&TextIdPc880784, -1, 0},
        {&TextIdPc880788, -1, 0},
        {&TextIdPc88078c, -1, 0},
        {&TextIdPc880790, -1, 0},
        {&TextIdPc880794, -1, 0},
        {&TextIdPc880800, -1, 0},
        {&tCLIPi, -1, 0},
        {&TextIdPc880818, -1, 0},
        {&TextIdPc88081c, -1, 0},
        {&TextIdPc880820, -1, 0},
        {&TextIdPc880824, -1, 0},
        {&TextIdPc880828, -1, 0},
        {&TextIdPc88082c, -1, 0},
        {&TextIdPc880830, -1, 0},
        {&TextIdPc880834, -1, 0},
        {&TextIdPc880838, -1, 0},
        {&TextIdPc88083c, -1, 0},
        {&TextIdPc880840, -1, 0},
        {&TextIdPc880844, -1, 0},
        {&TextIdPc880848, -1, 0},
        {&TextIdPc88084c, -1, 0},
        {&TextIdPc880850, -1, 0},
        {&TextIdPc880854, -1, 0},
        {&TextIdPc880858, -1, 0},
        {&TextIdPc88085c, -1, 0},
        {&TextIdPc880860, -1, 0},
        {&TextIdPc880864, -1, 0},
        {&TextIdPc880868, -1, 0},
        {&TextIdPc88086c, -1, 0},
        {&TextIdPc880870, -1, 0},
        {&TextIdPc880874, -1, 0},
        {&tON, -1, 0},
        {&tOFF, -1, 0},
        {&TextIdPc880880, -1, 0},
        {&tACCEPT, -1, 0},
        {&TextIdPc880888, -1, 0},
        {&tBACK, -1, 0},
        {&TextIdPc880890, -1, 0},
        {&TextIdPc880894, -1, 0},
        {&TextIdPc880898, -1, 0},
        {&TextIdPc88089c, -1, 0},
        {&TextIdPc8808a0, -1, 0},
        {&TextIdPc8808a4, -1, 0},
        {&TextIdPc8808a8, -1, 0},
        {&TextIdPc8808ac, -1, 0},
        {&TextIdPc8808b0, -1, 0},
        {&TextIdPc8808b4, -1, 0},
        {&TextIdPc8808b8, -1, 0},
        {&TextIdPc8808bc, -1, 0},
        {&TextIdPc8808c0, -1, 0},
        {&TextIdPc8808c4, -1, 0},
        {&TextIdPc8808c8, -1, 0},
        {&TextIdPc8808cc, -1, 0},
        {&TextIdPc8808d0, -1, 0},
        {&TextIdPc8808d4, -1, 0},
        {&TextIdPc8808d8, -1, 0},
        {&PCArcadeLevelText, -1, 0},
        {&TextIdPc8808e0, -1, 0},
        {&TextIdPc8808e4, -1, 0},
        {&TextIdPc8808e8, -1, 0},
        {&TextIdPc8808ec, -1, 0},
        {&TextIdPc8808f0, -1, 0},
        {&TextIdPc8808f4, -1, 0},
        {&TextIdPc8808f8, -1, 0},
        {&TextIdPc8808fc, -1, 0},
        {&TextIdPc880900, -1, 0},
        {&TextIdPc880904, -1, 0},
        {&TextIdPc880908, -1, 0},
        {&TextIdPc88090c, -1, 0},
        {&TextIdPc880910, -1, 0},
        {&TextIdPc880914, -1, 0},
        {&tAUDIOVOLUME, -1, 0},
        {&TextIdPc88091c, -1, 0},
        {&TextIdPc880920, -1, 0},
        {&TextIdPc880928, -1, 0},
        {&TextIdPc88092c, -1, 0},
        {&TextIdPc880930, -1, 0},
        {&TextIdPc880934, -1, 0},
        {&TextIdPc880938, -1, 0},
        {&tMUSIC, -1, 0},
        {&TextIdPc880940, -1, 0},
        {&TextIdPc880944, -1, 0},
        {&TextIdPc880948, -1, 0},
        {&TextIdPc88094c, -1, 0},
        {&TextIdPc880950, -1, 0},
        {&TextIdPc880954, -1, 0},
        {&TextIdPc88095c, -1, 0},
        {&TextIdPc880960, -1, 0},
        {&tBATMANSUIT, -1, 0},
        {&tSHADOWSUIT, -1, 0},
        {&tGLIDESUIT, -1, 0},
        {&tDEMOLITIONSUIT, -1, 0},
        {&tSONARSUIT, -1, 0},
        {&tROBINSUIT, -1, 0},
        {&tWATERSUIT, -1, 0},
        {&tTECHNOLOGYSUIT, -1, 0},
        {&tMAGNETSUIT, -1, 0},
        {&tATTRACTSUIT, -1, 0},
        {&TextIdPc88098c, -1, 0},
        {&TextIdPc880990, 600, 0},
        {&TextIdPc880994, -1, 0},
        {&TextIdPc880998, -1, 0},
        {&TextIdPc88099c, -1, 0},
        {&TextIdPc8809a0, -1, 0},
        {&TextIdPc8809a4, -1, 0},
        {&TextIdPc8809a8, -1, 0},
        {&TextIdPc8809ac, -1, 0},
        {&TextIdPc8809b0, -1, 0},
        {&TextIdPc8809b4, -1, 0},
        {&TextIdPc8809b8, -1, 0},
        {&TextIdPc8809bc, -1, 0},
        {&TextIdPc8809c0, -1, 0},
        {&TextIdPc8809c4, -1, 0},
        {&TextIdPc8809c8, -1, 0},
        {&TextIdPc8809cc, -1, 0},
        {NULL, -1, 0},
    };
    TEXTENTRY Text_TableGame[347] = {
        {&TextIdPc8809d4, 800, 0},
        {&TextIdPc8809d8, -1, 0},
        {&TextIdPc8809dc, -1, 0},
        {&TextIdPc8809e0, -1, 0},
        {&TextIdPc8809e4, -1, 0},
        {&TextIdPc8809e8, -1, 0},
        {&TextIdPc8809ec, -1, 0},
        {&TextIdPc8809f0, -1, 0},
        {&TextIdPc8809f4, -1, 0},
        {&TextIdPc8809f8, -1, 0},
        {&TextIdPc8809fc, -1, 0},
        {&TextIdPc880a00, -1, 0},
        {&TextIdPc880a04, -1, 0},
        {&TextIdPc880a08, -1, 0},
        {&TextIdPc880a0c, -1, 0},
        {&TextIdPc880a10, -1, 0},
        {&TextIdPc880a14, -1, 0},
        {&TextIdPc880a18, -1, 0},
        {&TextIdPc880a1c, -1, 0},
        {&TextIdPc880a20, -1, 0},
        {&TextIdPc880a24, -1, 0},
        {&TextIdPc880a28, -1, 0},
        {&TextIdPc880a2c, -1, 0},
        {&TextIdPc880a30, -1, 0},
        {&TextIdPc880a34, -1, 0},
        {&TextIdPc880a38, -1, 0},
        {&TextIdPc880a3c, -1, 0},
        {&TextIdPc880a40, -1, 0},
        {&TextIdPc880a44, -1, 0},
        {&TextIdPc880a48, -1, 0},
        {&TextIdPc880a4c, -1, 0},
        {&TextIdPc880a50, -1, 0},
        {&TextIdPc880a54, -1, 0},
        {&TextIdPc880a58, -1, 0},
        {&TextIdPc880a5c, -1, 0},
        {&TextIdPc880a60, -1, 0},
        {&TextIdPc880a64, -1, 0},
        {&TextIdPc880a68, -1, 0},
        {&TextIdPc880a6c, -1, 0},
        {&TextIdPc880a70, -1, 0},
        {&TextIdPc880a74, -1, 0},
        {&TextIdPc880a78, -1, 0},
        {&TextIdPc880a7c, -1, 0},
        {&TextIdPc880a80, -1, 0},
        {&TextIdPc880a84, -1, 0},
        {&TextIdPc880a88, -1, 0},
        {&TextIdPc880a8c, -1, 0},
        {&TextIdPc880a90, -1, 0},
        {&TextIdPc880a94, -1, 0},
        {&TextIdPc880a98, -1, 0},
        {&TextIdPc880a9c, -1, 0},
        {&TextIdPc880aa0, -1, 0},
        {&TextIdPc880aa4, -1, 0},
        {&TextIdPc880aa8, -1, 0},
        {&TextIdPc880aac, -1, 0},
        {&TextIdPc880ab0, -1, 0},
        {&TextIdPc880ab4, -1, 0},
        {&TextIdPc880ab8, -1, 0},
        {&TextIdPc880abc, -1, 0},
        {&TextIdPc880ac0, -1, 0},
        {&TextIdPc880ac4, -1, 0},
        {&TextIdPc880ac8, -1, 0},
        {&TextIdPc880acc, -1, 0},
        {&TextIdPc880ad0, -1, 0},
        {&TextIdPc880ad4, -1, 0},
        {&TextIdPc880ad8, -1, 0},
        {&TextIdPc880adc, -1, 0},
        {&TextIdPc880ae0, -1, 0},
        {&TextIdPc880ae4, -1, 0},
        {&TextIdPc880ae8, -1, 0},
        {&TextIdPc880aec, -1, 0},
        {&TextIdPc880af0, -1, 0},
        {&TextIdPc880af4, -1, 0},
        {&TextIdPc880af8, -1, 0},
        {&TextIdPc880afc, -1, 0},
        {&TextIdPc880b00, -1, 0},
        {&TextIdPc880b04, -1, 0},
        {&TextIdPc880b08, -1, 0},
        {&TextIdPc880b0c, -1, 0},
        {&TextIdPc880b10, -1, 0},
        {&TextIdPc880b14, -1, 0},
        {&TextIdPc880b18, -1, 0},
        {&TextIdPc880b1c, -1, 0},
        {&TextIdPc880b20, -1, 0},
        {&TextIdPc880b24, -1, 0},
        {&TextIdPc880b28, -1, 0},
        {&TextIdPc880b2c, -1, 0},
        {&TextIdPc880b30, -1, 0},
        {&TextIdPc880b34, -1, 0},
        {&TextIdPc880b38, -1, 0},
        {&TextIdPc880b3c, -1, 0},
        {&TextIdPc880b40, -1, 0},
        {&TextIdPc880b44, -1, 0},
        {&TextIdPc880b48, -1, 0},
        {&TextIdPc880b4c, -1, 0},
        {&TextIdPc880b50, -1, 0},
        {&TextIdPc880b54, -1, 0},
        {&TextIdPc880b58, -1, 0},
        {&TextIdPc880b5c, -1, 0},
        {&TextIdPc880b60, -1, 0},
        {&TextIdPc880b64, -1, 0},
        {&TextIdPc880b68, -1, 0},
        {&TextIdPc880b6c, -1, 0},
        {&TextIdPc880b70, -1, 0},
        {&TextIdPc880b74, -1, 0},
        {&TextIdPc880b78, -1, 0},
        {&TextIdPc880b7c, -1, 0},
        {&TextIdPc880b80, -1, 0},
        {&TextIdPc880b84, -1, 0},
        {&TextIdPc880b88, -1, 0},
        {&TextIdPc880b8c, -1, 0},
        {&TextIdPc880b90, -1, 0},
        {&TextIdPc880b94, -1, 0},
        {&TextIdPc880b98, -1, 0},
        {&TextIdPc880b9c, -1, 0},
        {&TextIdPc880ba0, -1, 0},
        {&TextIdPc880ba4, -1, 0},
        {&TextIdPc880ba8, -1, 0},
        {&TextIdPc880bac, -1, 0},
        {&TextIdPc880bb0, -1, 0},
        {&TextIdPc880bb4, -1, 0},
        {&TextIdPc880bb8, -1, 0},
        {&TextIdPc880bbc, 909, 0},
        {&TextIdPc880bc0, -1, 0},
        {&TextIdPc880bc4, -1, 0},
        {&TextIdPc880bc8, -1, 0},
        {&TextIdPc880bcc, -1, 0},
        {&TextIdPc880bd0, -1, 0},
        {&TextIdPc880bd4, -1, 0},
        {&TextIdPc880bd8, -1, 0},
        {&TextIdPc880bdc, -1, 0},
        {&TextIdPc880be0, -1, 0},
        {&TextIdPc880be4, -1, 0},
        {&TextIdPc880be8, -1, 0},
        {&TextIdPc880bec, -1, 0},
        {&TextIdPc880bf0, -1, 0},
        {&TextIdPc880bf4, -1, 0},
        {&TextIdPc880bf8, -1, 0},
        {&TextIdPc880bfc, -1, 0},
        {&TextIdPc880c00, -1, 0},
        {&TextIdPc880c04, -1, 0},
        {&TextIdPc880c08, -1, 0},
        {&TextIdPc880c0c, -1, 0},
        {&TextIdPc880c10, -1, 0},
        {&TextIdPc880c14, -1, 0},
        {&TextIdPc880c18, -1, 0},
        {&TextIdPc880c1c, 1152, 0},
        {&TextIdPc880c20, -1, 0},
        {&TextIdPc880c24, 1181, 0},
        {&TextIdPc880bc4, 1270, 0},
        {&TextIdPc880f00, -1, 0},
        {&TextIdPc880f04, -1, 0},
        {&TextIdPc880f08, -1, 0},
        {&TextIdPc880f0c, -1, 0},
        {&TextIdPc880f10, -1, 0},
        {&TextIdPc880f14, -1, 0},
        {&TextIdPc880f18, -1, 0},
        {&TextIdPc880f1c, -1, 0},
        {&TextIdPc880f20, -1, 0},
        {&TextIdPc880f44, -1, 0},
        {&TextIdPc880f24, -1, 0},
        {&TextIdPc880f28, -1, 0},
        {&TextIdPc880f2c, -1, 0},
        {&TextIdPc880f30, -1, 0},
        {&TextIdPc880f34, -1, 0},
        {&TextIdPc880f38, -1, 0},
        {&TextIdPc880f3c, -1, 0},
        {&TextIdPc880f40, -1, 0},
        {&TextIdPc880f48, -1, 0},
        {&TextIdPc880f4c, -1, 0},
        {&TextIdPc880f50, -1, 0},
        {&TextIdPc880c34, 1187, 0},
        {&TextIdPc880c38, -1, 0},
        {&TextIdPc880c3c, -1, 0},
        {&TextIdPc880c40, -1, 0},
        {&TextIdPc880c44, -1, 0},
        {&TextIdPc880c48, -1, 0},
        {&TextIdPc880c4c, -1, 0},
        {&TextIdPc880c50, -1, 0},
        {&TextIdPc880c54, -1, 0},
        {&TextIdPc880c58, -1, 0},
        {&TextIdPc880c5c, -1, 0},
        {&TextIdPc880c70, -1, 0},
        {&TextIdPc880c74, -1, 0},
        {&TextIdPc880c78, -1, 0},
        {&TextIdPc880c7c, -1, 0},
        {&TextIdPc880c80, -1, 0},
        {&TextIdPc880c84, -1, 0},
        {&tTOUCHTOSTART, -1, 0},
        {&TextIdPc880c8c, 1211, 0},
        {&TextIdPc880c90, -1, 0},
        {&TextIdPc880c94, -1, 0},
        {&TextIdPc880c98, -1, 0},
        {&TextIdPc880c9c, 1216, 0},
        {&TextIdPc880ca0, -1, 0},
        {&TextIdPc880ca4, 1225, 0},
        {&TextIdPc880ca8, 1226, 0},
        {&TextIdPc880cac, 1227, 0},
        {&TextIdPc880cb0, -1, 0},
        {&TextIdPc880cb4, -1, 0},
        {&TextIdPc880cb8, -1, 0},
        {&TextIdPc880cbc, -1, 0},
        {&TextIdPc880cc0, -1, 0},
        {&TextIdPc880cc4, -1, 0},
        {&TextIdPc880cc8, -1, 0},
        {&TextIdPc880ccc, -1, 0},
        {&TextIdPc880cd0, -1, 0},
        {&TextIdPc880cd4, -1, 0},
        {&TextIdPc880cd8, -1, 0},
        {&TextIdPc880cdc, -1, 0},
        {&TextIdPc880ce0, -1, 0},
        {&TextIdPc880ce4, -1, 0},
        {&TextIdPc880ce8, -1, 0},
        {&TextIdPc880cec, -1, 0},
        {&TextIdPc880cf0, -1, 0},
        {&TextIdPc880cf4, -1, 0},
        {&TextIdPc880cf8, -1, 0},
        {&TextIdPc880cfc, -1, 0},
        {&TextIdPc880d00, 1249, 0},
        {&TextIdPc880d04, -1, 0},
        {&TextIdPc880d08, -1, 0},
        {&TextIdPc880d0c, -1, 0},
        {&TextIdPc880d10, -1, 0},
        {&TextIdPc880d14, -1, 0},
        {&TextIdPc880d18, -1, 0},
        {&TextIdPc880d1c, -1, 0},
        {&TextIdPc880d20, -1, 0},
        {&TextIdPc880d24, -1, 0},
        {&TextIdPc880d28, -1, 0},
        {&TextIdPc880d2c, -1, 0},
        {&TextIdPc880d30, -1, 0},
        {&TextIdPc880d34, -1, 0},
        {&TextIdPc880d38, -1, 0},
        {&TextIdPc880d3c, -1, 0},
        {&TextIdPc880d40, -1, 0},
        {&TextIdPc880d44, -1, 0},
        {&TextIdPc880d48, 1450, 0},
        {&TextIdPc880d4c, -1, 0},
        {&TextIdPc880d50, -1, 0},
        {&TextIdPc880d54, -1, 0},
        {&TextIdPc880d58, -1, 0},
        {&TextIdPc880d5c, -1, 0},
        {&TextIdPc880d60, -1, 0},
        {&TextIdPc880d64, -1, 0},
        {&TextIdPc880d68, -1, 0},
        {&TextIdPc880d6c, -1, 0},
        {&TextIdPc880d70, -1, 0},
        {&TextIdPc880d74, -1, 0},
        {&TextIdPc880d78, -1, 0},
        {&TextIdPc880d7c, -1, 0},
        {&TextIdPc880d80, -1, 0},
        {&TextIdPc880d84, -1, 0},
        {&TextIdPc880d88, -1, 0},
        {&TextIdPc880d8c, -1, 0},
        {&TextIdPc880d90, -1, 0},
        {&TextIdPc880d94, -1, 0},
        {&TextIdPc880d98, 1500, 0},
        {&TextIdPc880d9c, -1, 0},
        {&TextIdPc880da0, -1, 0},
        {&TextIdPc880da4, -1, 0},
        {&TextIdPc880da8, -1, 0},
        {&TextIdPc880dac, -1, 0},
        {&TextIdPc880db0, -1, 0},
        {&TextIdPc880db4, -1, 0},
        {&TextIdPc880db8, -1, 0},
        {&TextIdPc880dbc, -1, 0},
        {&TextIdPc880dc0, -1, 0},
        {&TextIdPc880dc4, -1, 0},
        {&TextIdPc880dc8, -1, 0},
        {&TextIdPc880dcc, -1, 0},
        {&TextIdPc880dd0, -1, 0},
        {&TextIdPc880dd4, -1, 0},
        {&TextIdPc880dd8, -1, 0},
        {&TextIdPc880ddc, -1, 0},
        {&TextIdPc880de0, -1, 0},
        {&TextIdPc880de4, -1, 0},
        {&TextIdPc880de8, -1, 0},
        {&TextIdPc880dec, -1, 0},
        {&TextIdPc880df0, -1, 0},
        {&TextIdPc880df4, -1, 0},
        {&TextIdPc880df8, -1, 0},
        {&TextIdPc880dfc, -1, 0},
        {&TextIdPc880e00, -1, 0},
        {&TextIdPc880e04, -1, 0},
        {&TextIdPc880e08, 1700, 0},
        {&TextIdPc880e0c, 1701, 0},
        {&TextIdPc880e10, -1, 0},
        {&TextIdPc880e14, -1, 0},
        {&TextIdPc880e18, -1, 0},
        {&TextIdPc880e1c, -1, 0},
        {&TextIdPc880e20, -1, 0},
        {&TextIdPc880e24, -1, 0},
        {&TextIdPc880e28, -1, 0},
        {&TextIdPc880e2c, -1, 0},
        {&TextIdPc880e30, 1733, 0},
        {&TextIdPc880e34, 1711, 0},
        {&TextIdPc880e38, -1, 0},
        {&TextIdPc880e3c, -1, 0},
        {&TextIdPc880e40, -1, 0},
        {&TextIdPc880e44, -1, 0},
        {&TextIdPc880e48, -1, 0},
        {&TextIdPc880e4c, -1, 0},
        {&TextIdPc880e50, -1, 0},
        {&TextIdPc880e54, -1, 0},
        {&TextIdPc880e58, -1, 0},
        {&TextIdPc880e5c, -1, 0},
        {&TextIdPc880e60, -1, 0},
        {&TextIdPc880e64, -1, 0},
        {&TextIdPc880e68, -1, 0},
        {&TextIdPc880e6c, -1, 0},
        {&TextIdPc880e70, -1, 0},
        {&TextIdPc880e74, -1, 0},
        {&TextIdPc880e78, -1, 0},
        {&TextIdPc880e7c, -1, 0},
        {&TextIdPc880e80, -1, 0},
        {&TextIdPc880e84, -1, 0},
        {&TextIdPc880e88, -1, 0},
        {&TextIdPc880e8c, 1734, 0},
        {&TextIdPc880e90, -1, 0},
        {&TextIdPc880e94, -1, 0},
        {&TextIdPc880e98, -1, 0},
        {&TextIdPc880e9c, -1, 0},
        {&TextIdPc880ea0, -1, 0},
        {&TextIdPc880ea4, -1, 0},
        {&TextIdPc880ea8, 1742, 0},
        {&TextIdPc880eac, -1, 0},
        {&TextIdPc880eb0, -1, 0},
        {&TextIdPc880eb4, -1, 0},
        {&TextIdPc880eb8, -1, 0},
        {&TextIdPc880ebc, -1, 0},
        {&TextIdPc880ec0, -1, 0},
        {&TextIdPc880ec4, -1, 0},
        {&TextIdPc880ecc, -1, 0},
        {&TextIdPc880ec8, 1751, 0},
        {&TextIdPc880ed0, -1, 0},
        {&TextIdPc880ed4, -1, 0},
        {&TextIdPc880ed8, -1, 0},
        {&TextIdPc880edc, -1, 0},
        {&TextIdPc880ee0, -1, 0},
        {&TextIdPc880ee4, -1, 0},
        {&TextIdPc880ef0, -1, 0},
        {&TextIdPc880ef4, -1, 0},
        {&TextIdPc880ef8, -1, 0},
        {&TextIdPc880ee8, -1, 0},
        {&TextIdPc880eec, -1, 0},
        {&TextIdPc880efc, -1, 0},
        {NULL, -1, 0},
    };
    TEXTENTRY Text_TableMain[706] = {
        {&tNULL, -1, 0},
        {&tUNKNOWN, -1, 0},
        {&tENGLISH, -1, 0},
        {&tFRENCH, -1, 0},
        {&tGERMAN, -1, 0},
        {&tITALIAN, -1, 0},
        {&tSPANISH, -1, 0},
        {&tDANISH, -1, 0},
        {&TextIdPc8806dc, -1, 0},
        {&TextIdPc8806e0, 20, 0},
        {&TextIdPc880f64, 35, 0},
        {&TextIdPc880f68, -1, 0},
        {&TextIdPc880f6c, -1, 0},
        {&TextIdPc880f70, -1, 0},
        {&TextIdPc880f74, -1, 0},
        {&TextIdPc880f78, -1, 0},
        {&TextIdPc880f7c, -1, 0},
        {&TextIdPc880f80, -1, 0},
        {&TextIdPc880f84, -1, 0},
        {&TextIdPc880f88, -1, 0},
        {&TextIdPc8806e4, 49, 0},
        {&TextIdPc8806e8, -1, 0},
        {&TextIdPc880f8c, -1, 0},
        {&tALONGTIMEAGO, -1, 0},
        {&TextIdPc880f94, -1, 0},
        {&TextIdPc8806f4, -1, 0},
        {&TextIdPc8806f8, -1, 0},
        {&TextIdPc8806fc, -1, 0},
        {&TextIdPc880704, -1, 0},
        {&TextIdPc880708, -1, 0},
        {&TextIdPc88070c, -1, 0},
        {&TextCrawlChapterIdPc, -1, 0},
        {&TextIdPc880710, -1, 0},
        {&TextIdPc880714, -1, 0},
        {&TextIdPc880718, -1, 0},
        {&TextIdPc88071c, -1, 0},
        {&TextIdPc880720, -1, 0},
        {&TextIdPc880724, -1, 0},
        {&TextCrawlCharacterBonusIdPc, -1, 0},
        {&TextCrawlVehicleBonusIdPc, -1, 0},
        {&TextIdPc880728, -1, 0},
        {&TextIdPc88072c, -1, 0},
        {&TextIdPc880730, -1, 0},
        {&TextIdPc880734, -1, 0},
        {&TextIdPc880738, -1, 0},
        {&TextIdPc88073c, -1, 0},
        {&TextIdPc880740, -1, 0},
        {&TextIdPc880f98, -1, 0},
        {&TextIdPc880f9c, -1, 0},
        {&TextIdPc880744, -1, 0},
        {&TextIdPc880748, -1, 0},
        {&TextIdPc88074c, -1, 0},
        {&TextIdPc880750, -1, 0},
        {&TextIdPc880754, -1, 0},
        {&TextIdPc880758, -1, 0},
        {&TextIdPc88075c, -1, 0},
        {&TextIdPc880fa0, -1, 0},
        {&TextIdPc880760, -1, 0},
        {&TextIdPc88098c, -1, 0},
        {&TextIdPc880764, -1, 0},
        {&TextIdPc880fa4, -1, 0},
        {&TextIdPc880768, -1, 0},
        {&TextIdPc880fa8, -1, 0},
        {&TextIdPc880fac, -1, 0},
        {&TextIdPc880fb0, -1, 0},
        {&TextIdPc880fb4, -1, 0},
        {&TextIdPc880fb8, -1, 0},
        {&TextIdPc880fbc, -1, 0},
        {&TextIdPc880fc0, -1, 0},
        {&TextIdPc880fc4, -1, 0},
        {&TextIdPc88076c, -1, 0},
        {&TextIdPc880818, 77, 0},
        {&PCArcadeLevelText, 740, 0},
        {&TextIdPc880770, 1656, 0},
        {&TextIdPc8810c0, 245, 0},
        {&tPOWERUP, 295, 0},
        {&TextIdPc88107c, -1, 0},
        {&TextIdPc881078, -1, 0},
        {&TextIdPc881074, -1, 0},
        {&TextIdPc880700, -1, 0},
        {&TextIdPc881080, 1657, 0},
        {&TextIdPc881084, -1, 0},
        {&TextIdPc881088, -1, 0},
        {&TextIdPc88108c, -1, 0},
        {&TextIdPc881090, -1, 0},
        {&TextIdPc881094, -1, 0},
        {&TextIdPc881098, -1, 0},
        {&TextIdPc88109c, -1, 0},
        {&TextIdPc8810a0, -1, 0},
        {&TextIdPc8810a4, -1, 0},
        {&TextIdPc8810a8, -1, 0},
        {&TextIdPc8810ac, -1, 0},
        {&TextIdPc8810b0, -1, 0},
        {&TextIdPc8810b4, -1, 0},
        {&TextIdPc8810b8, -1, 0},
        {&TextIdPc880ff4, 1673, 0},
        {&TextIdPc880ff0, 1674, 0},
        {&TextIdPc8807bc, 1775, 0},
        {&TextIdPc880774, 300, 0},
        {&TextIdPc880fc8, 301, 0},
        {&TextIdPc880fcc, 304, 0},
        {&TextIdPc880fd0, 307, 0},
        {&TextIdPc880fd4, 310, 0},
        {&TextIdPc880fd8, 313, 0},
        {&TextIdPc880fdc, 316, 0},
        {&TextIdPc880fe0, 319, 0},
        {&TextIdPc880fe4, 322, 0},
        {&TextIdPc880fe8, 325, 0},
        {&TextIdPc880fec, 328, 0},
        {&TextIdPc880778, 331, 0},
        {&TextIdPc88077c, -1, 0},
        {&TextIdPc880780, -1, 0},
        {&TextIdPc880784, -1, 0},
        {&TextIdPc880788, -1, 0},
        {&TextIdPc880ff8, -1, 0},
        {&TextIdPc880ffc, -1, 0},
        {&TextIdPc881000, -1, 0},
        {&TextIdPc881004, -1, 0},
        {&TextIdPc88078c, -1, 0},
        {&TextIdPc881008, -1, 0},
        {&TextIdPc88100c, -1, 0},
        {&TextIdPc880790, -1, 0},
        {&TextIdPc880794, -1, 0},
        {&MissionMenuInstructionTextPc, -1, 0},
        {&TextIdPc880800, -1, 0},
        {&tCLIPi, -1, 0},
        {&tINTRO, -1, 0},
        {&tMIDTRO, -1, 0},
        {&tOUTRO, -1, 0},
        {&tENDING, -1, 0},
        {&TextIdPc880798, 1417, 0},
        {&TextIdPc88079c, -1, 0},
        {&TextIdPc8807a0, -1, 0},
        {&TextIdPc8807a4, -1, 0},
        {&PermanentBountyTitleIdPc, -1, 0},
        {&PCArcadeModeText0, -1, 0},
        {&PCArcadeModeText1, -1, 0},
        {&PCArcadeModeText2, -1, 0},
        {&TextIdPc8807b8, -1, 0},
        {&TextIdPc8807c0, -1, 0},
        {&TextIdPc8807c4, -1, 0},
        {&TextIdPc8807c8, -1, 0},
        {&TextIdPc8807cc, -1, 0},
        {&PermanentAllTitleIdPc, -1, 0},
        {&PermanentBlasterTitleIdPc, -1, 0},
        {&PCArcadeModeText, -1, 0},
        {&TextIdPc8807e4, -1, 0},
        {&TextIdPc8807e8, -1, 0},
        {&TextIdPc8807ec, -1, 0},
        {&TextIdPc8807f0, -1, 0},
        {&TextIdPc8807f4, 1441, 0},
        {&PCArcadePlayText, 1442, 0},
        {&TextIdPc8807fc, 1443, 0},
        {&TextIdPc8807d0, 1444, 0},
        {&TextIdPc8807d4, 1445, 0},
        {&TextIdPc881014, 568, 0},
        {&TextIdPc881018, -1, 0},
        {&TextIdPc88101c, 1357, 0},
        {&TextIdPc881020, -1, 0},
        {&TextIdPc881024, 1367, 0},
        {&TextIdPc881028, -1, 0},
        {&TextIdPc88102c, 510, 0},
        {&TextIdPc881030, -1, 0},
        {&TextIdPc881034, 530, 0},
        {&TextIdPc881038, -1, 0},
        {&TextIdPc88103c, 550, 0},
        {&TextIdPc881040, -1, 0},
        {&TextIdPc880904, 660, 0},
        {&TextIdPc880908, -1, 0},
        {&TextIdPc88090c, -1, 0},
        {&TextIdPc8809d4, -1, 0},
        {&TextIdPc8809a0, -1, 0},
        {&TextIdPc8809b4, -1, 0},
        {&TextIdPc8809a4, -1, 0},
        {&TextIdPc881044, -1, 0},
        {&TextIdPc880994, -1, 0},
        {&TextIdPc881048, -1, 0},
        {&TextIdPc8809ac, -1, 0},
        {&TextIdPc88104c, -1, 0},
        {&TextIdPc880990, -1, 0},
        {&TextIdPc8809c0, -1, 0},
        {&TextIdPc881050, -1, 0},
        {&TextIdPc881054, -1, 0},
        {&TextIdPc8809b8, -1, 0},
        {&TextIdPc881058, -1, 0},
        {&TextIdPc88105c, -1, 0},
        {&TextIdPc8809c4, -1, 0},
        {&TextIdPc8809c8, -1, 0},
        {&TextIdPc881060, -1, 0},
        {&TextIdPc880998, -1, 0},
        {&TextIdPc8809b0, -1, 0},
        {&TextIdPc8809a8, -1, 0},
        {&TextIdPc88099c, -1, 0},
        {&TextIdPc881064, -1, 0},
        {&TextIdPc881068, -1, 0},
        {&TextIdPc8809bc, -1, 0},
        {&TextIdPc88106c, -1, 0},
        {&TextIdPc8809cc, -1, 0},
        {&TextIdPc8809d0, 698, 0},
        {&TextIdPc881070, 699, 0},
        {&TextIdPc880820, 700, 0},
        {&TextIdPc880824, -1, 0},
        {&TextIdPc880828, -1, 0},
        {&TextIdPc88082c, -1, 0},
        {&TextIdPc880830, -1, 0},
        {&TextIdPc880834, -1, 0},
        {&TextIdPc880838, -1, 0},
        {&TextIdPc88083c, -1, 0},
        {&TextIdPc880840, -1, 0},
        {&TextIdPc880844, -1, 0},
        {&TextIdPc880848, -1, 0},
        {&TextIdPc88084c, -1, 0},
        {&TextIdPc880850, -1, 0},
        {&TextIdPc880854, -1, 0},
        {&TextIdPc880858, -1, 0},
        {&TextIdPc88085c, -1, 0},
        {&TextIdPc880860, -1, 0},
        {&TextIdPc880864, -1, 0},
        {&TextIdPc880868, -1, 0},
        {&TextIdPc88086c, -1, 0},
        {&TextIdPc880870, -1, 0},
        {&TextIdPc880874, -1, 0},
        {&tON, -1, 0},
        {&tOFF, -1, 0},
        {&TextIdPc880880, -1, 0},
        {&tACCEPT, -1, 0},
        {&TextIdPc880888, -1, 0},
        {&tBACK, -1, 0},
        {&TextIdPc880890, -1, 0},
        {&TextIdPc880928, -1, 0},
        {&TextIdPc88092c, -1, 0},
        {&TextIdPc880930, -1, 0},
        {&TextIdPc880934, -1, 0},
        {&TextIdPc880938, -1, 0},
        {&tMUSIC, -1, 0},
        {&TextIdPc880960, -1, 0},
        {&TextIdPc880958, 1267, 0},
        {&TextIdPc88095c, 735, 0},
        {&TextIdPc8809d8, -1, 0},
        {&TextIdPc880940, -1, 0},
        {&TextIdPc880944, -1, 0},
        {&TextIdPc8809dc, -1, 0},
        {&PCArcadeLevelText, -1, 0},
        {&TextIdPc8808e0, -1, 0},
        {&TextIdPc8808e4, -1, 0},
        {&TextIdPc88081c, -1, 0},
        {&TextIdPc8808e8, -1, 0},
        {&TextIdPc8810c4, -1, 0},
        {&TextIdPc8810c8, -1, 0},
        {&TextIdPc880948, -1, 0},
        {&TextIdPc88094c, -1, 0},
        {&TextIdPc880950, -1, 0},
        {&TextIdPc8809e0, -1, 0},
        {&TextIdPc8809e4, -1, 0},
        {&TextIdPc8809e8, -1, 0},
        {&TextIdPc8809ec, -1, 0},
        {&TextIdPc8808f4, -1, 0},
        {&TextIdPc8808f8, -1, 0},
        {&TextIdPc8809f0, -1, 0},
        {&TextIdPc8808fc, -1, 0},
        {&TextIdPc880900, -1, 0},
        {&TextIdPc8809f4, -1, 0},
        {&TextIdPc8809f8, -1, 0},
        {&TextIdPc8809fc, -1, 0},
        {&TextIdPc880a00, -1, 0},
        {&TextIdPc880a04, -1, 0},
        {&TextIdPc880a08, -1, 0},
        {&TextIdPc880a0c, -1, 0},
        {&TextIdPc880a10, -1, 0},
        {&TextIdPc880a14, -1, 0},
        {&TextIdPc880a18, -1, 0},
        {&TextIdPc880a1c, -1, 0},
        {&TextIdPc880a20, -1, 0},
        {&TextIdPc880a24, -1, 0},
        {&TextIdPc880a28, -1, 0},
        {&TextIdPc880a2c, -1, 0},
        {&TextIdPc880a30, -1, 0},
        {&TextIdPc880a34, -1, 0},
        {&TextIdPc880a38, -1, 0},
        {&TextIdPc880a3c, -1, 0},
        {&TextIdPc880a40, -1, 0},
        {&TextIdPc880a44, -1, 0},
        {&TextIdPc880a48, -1, 0},
        {&TextIdPc880a4c, -1, 0},
        {&TextIdPc880a50, -1, 0},
        {&TextIdPc880a54, -1, 0},
        {&TextIdPc880a58, -1, 0},
        {&TextIdPc880a5c, -1, 0},
        {&TextIdPc880a60, -1, 0},
        {&TextIdPc880a64, -1, 0},
        {&TextIdPc880a68, -1, 0},
        {&TextIdPc880a6c, -1, 0},
        {&TextIdPc880910, -1, 0},
        {&TextIdPc880a70, -1, 0},
        {&TextIdPc880a74, -1, 0},
        {&TextIdPc880a78, -1, 0},
        {&TextIdPc880a7c, -1, 0},
        {&TextIdPc880a80, -1, 0},
        {&TextIdPc880a84, -1, 0},
        {&TextIdPc880a88, -1, 0},
        {&TextIdPc880a8c, -1, 0},
        {&TextIdPc880a90, -1, 0},
        {&TextIdPc880a94, -1, 0},
        {&TextIdPc880a98, -1, 0},
        {&TextIdPc880a9c, -1, 0},
        {&TextIdPc880aa0, -1, 0},
        {&TextIdPc880aa4, -1, 0},
        {&TextIdPc880aa8, -1, 0},
        {&TextIdPc880aac, -1, 0},
        {&TextIdPc880ab0, -1, 0},
        {&TextIdPc880ab4, -1, 0},
        {&TextIdPc880ab8, -1, 0},
        {&TextIdPc880abc, -1, 0},
        {&TextIdPc880ac0, -1, 0},
        {&TextIdPc880ac4, -1, 0},
        {&TextIdPc880ac8, -1, 0},
        {&TextIdPc880acc, -1, 0},
        {&TextIdPc880ad0, -1, 0},
        {&TextIdPc880894, -1, 0},
        {&TextIdPc880898, -1, 0},
        {&TextIdPc880ad4, -1, 0},
        {&TextIdPc88089c, -1, 0},
        {&TextIdPc8808a0, -1, 0},
        {&TextIdPc8808ac, -1, 0},
        {&TextIdPc8808b0, -1, 0},
        {&TextIdPc8808a4, -1, 0},
        {&TextIdPc8808a8, -1, 0},
        {&TextIdPc8808b4, -1, 0},
        {&TextIdPc880ad8, -1, 0},
        {&TextIdPc880adc, -1, 0},
        {&TextIdPc8810cc, -1, 0},
        {&TextIdPc8810d0, -1, 0},
        {&TextIdPc880954, -1, 0},
        {&TextIdPc8808b8, -1, 0},
        {&TextIdPc8808bc, -1, 0},
        {&TextIdPc8808c0, -1, 0},
        {&TextIdPc8808c4, -1, 0},
        {&TextIdPc880ae4, -1, 0},
        {&TextIdPc880ae8, -1, 0},
        {&TextIdPc8808c8, -1, 0},
        {&TextIdPc880aec, -1, 0},
        {&TextIdPc880af0, -1, 0},
        {&TextIdPc880af4, -1, 0},
        {&TextIdPc880af8, -1, 0},
        {&TextIdPc880afc, -1, 0},
        {&TextIdPc880b00, -1, 0},
        {&TextIdPc880b04, -1, 0},
        {&TextIdPc880b08, -1, 0},
        {&TextIdPc880914, -1, 0},
        {&TextIdPc880b0c, -1, 0},
        {&tAUDIOVOLUME, -1, 0},
        {&TextIdPc88091c, -1, 0},
        {&TextIdPc8808cc, -1, 0},
        {&TextIdPc8808d0, -1, 0},
        {&TextIdPc8808d4, -1, 0},
        {&TextIdPc8808d8, -1, 0},
        {&TextIdPc880b10, -1, 0},
        {&TextIdPc880b14, -1, 0},
        {&TextIdPc880920, -1, 0},
        {&TextIdPc8808ec, -1, 0},
        {&TextIdPc8808f0, -1, 0},
        {&TextIdPc880b18, -1, 0},
        {&TextIdPc880b1c, -1, 0},
        {&TextIdPc880b20, -1, 0},
        {&TextIdPc880b24, -1, 0},
        {&TextIdPc880b28, -1, 0},
        {&TextIdPc880b2c, -1, 0},
        {&TextIdPc880b30, -1, 0},
        {&TextIdPc880b34, -1, 0},
        {&TextIdPc880b38, -1, 0},
        {&TextIdPc880b3c, -1, 0},
        {&TextIdPc880b40, -1, 0},
        {&TextIdPc880b44, -1, 0},
        {&TextIdPc880b48, -1, 0},
        {&TextIdPc880b4c, -1, 0},
        {&TextIdPc880b50, -1, 0},
        {&TextIdPc880b54, -1, 0},
        {&TextIdPc880b58, -1, 0},
        {&TextIdPc880b5c, -1, 0},
        {&TextIdPc880b60, -1, 0},
        {&TextIdPc880b64, -1, 0},
        {&TextIdPc880b68, -1, 0},
        {&TextIdPc880b6c, -1, 0},
        {&TextIdPc880b70, -1, 0},
        {&TextIdPc880b74, -1, 0},
        {&TextIdPc880b78, -1, 0},
        {&TextIdPc880b7c, -1, 0},
        {&TextIdPc880b80, -1, 0},
        {&TextIdPc880b84, -1, 0},
        {&TextIdPc880b88, -1, 0},
        {&TextIdPc880b8c, -1, 0},
        {&TextIdPc880b90, -1, 0},
        {&TextIdPc880b94, -1, 0},
        {&TextIdPc880b98, -1, 0},
        {&TextIdPc880b9c, -1, 0},
        {&TextIdPc880ba0, -1, 0},
        {&TextIdPc880ba4, -1, 0},
        {&TextIdPc880ba8, -1, 0},
        {&TextIdPc880bac, -1, 0},
        {&TextIdPc880bb0, -1, 0},
        {&TextIdPc880bb4, -1, 0},
        {&TextIdPc880bb8, -1, 0},
        {&TextIdPc880bbc, 909, 0},
        {&TextIdPc880bc0, -1, 0},
        {&TextIdPc880bc4, -1, 0},
        {&TextIdPc880bc8, -1, 0},
        {&TextIdPc880bcc, -1, 0},
        {&TextIdPc880bd0, -1, 0},
        {&TextIdPc880bd4, -1, 0},
        {&TextIdPc880bd8, -1, 0},
        {&TextIdPc880bdc, -1, 0},
        {&TextIdPc880be0, -1, 0},
        {&TextIdPc880be4, -1, 0},
        {&TextIdPc880be8, -1, 0},
        {&TextIdPc880bec, -1, 0},
        {&TextIdPc880bf0, -1, 0},
        {&TextIdPc880bf4, -1, 0},
        {&TextIdPc880bf8, -1, 0},
        {&TextIdPc880bfc, -1, 0},
        {&TextIdPc880c00, -1, 0},
        {&TextIdPc880c04, -1, 0},
        {&TextIdPc880c08, -1, 0},
        {&TextIdPc880c0c, -1, 0},
        {&TextIdPc880c10, -1, 0},
        {&TextIdPc880c14, -1, 0},
        {&TextIdPc880c18, -1, 0},
        {&TextIdPc880c20, 1153, 0},
        {&TextIdPc880c24, 1181, 0},
        {&TextIdPc880bc4, 1270, 0},
        {&TextIdPc880f00, -1, 0},
        {&TextIdPc880f04, -1, 0},
        {&TextIdPc880f08, -1, 0},
        {&TextIdPc880f0c, -1, 0},
        {&TextIdPc880f10, -1, 0},
        {&TextIdPc880f14, -1, 0},
        {&TextIdPc880f18, -1, 0},
        {&TextIdPc880f1c, -1, 0},
        {&TextIdPc880f20, -1, 0},
        {&TextIdPc880f44, -1, 0},
        {&TextIdPc880f24, -1, 0},
        {&TextIdPc880f28, -1, 0},
        {&TextIdPc880f2c, -1, 0},
        {&TextIdPc880f30, -1, 0},
        {&TextIdPc880f34, -1, 0},
        {&TextIdPc880f38, -1, 0},
        {&TextIdPc880f3c, -1, 0},
        {&TextIdPc880f40, -1, 0},
        {&TextIdPc880f48, -1, 0},
        {&TextIdPc880f4c, -1, 0},
        {&TextIdPc880f50, -1, 0},
        {&TextIdPc880c34, 1187, 0},
        {&TextIdPc880c38, -1, 0},
        {&TextIdPc880c3c, -1, 0},
        {&TextIdPc880c40, -1, 0},
        {&TextIdPc880c44, -1, 0},
        {&TextIdPc880c48, -1, 0},
        {&TextIdPc880c4c, -1, 0},
        {&TextIdPc880c50, -1, 0},
        {&TextIdPc880c54, -1, 0},
        {&TextIdPc880c58, -1, 0},
        {&TextIdPc880c5c, -1, 0},
        {&TextIdPc880c60, 1199, 0},
        {&TextIdPc880c64, -1, 0},
        {&TextIdPc880c68, -1, 0},
        {&TextIdPc880c6c, -1, 0},
        {&TextIdPc880c70, -1, 0},
        {&TextIdPc880c74, -1, 0},
        {&TextIdPc880c78, -1, 0},
        {&TextIdPc880c7c, -1, 0},
        {&TextIdPc880c80, -1, 0},
        {&TextIdPc880c84, -1, 0},
        {&tTOUCHTOSTART, -1, 0},
        {&TextIdPc880c8c, 1211, 0},
        {&TextIdPc880c90, -1, 0},
        {&TextIdPc880c94, -1, 0},
        {&TextIdPc880c98, -1, 0},
        {&TextIdPc880c9c, 1216, 0},
        {&TextIdPc880ca0, -1, 0},
        {&TextIdPc880c1c, 1224, 0},
        {&TextIdPc880ca4, 1225, 0},
        {&TextIdPc880ca8, 1226, 0},
        {&TextIdPc880cac, 1227, 0},
        {&TextIdPc880cb0, -1, 0},
        {&TextIdPc880cb4, -1, 0},
        {&TextIdPc880cb8, -1, 0},
        {&TextIdPc880cbc, -1, 0},
        {&TextIdPc880cc0, -1, 0},
        {&TextIdPc880cc4, -1, 0},
        {&TextIdPc880cc8, -1, 0},
        {&TextIdPc880ccc, -1, 0},
        {&TextIdPc880cd0, -1, 0},
        {&TextIdPc880cd4, -1, 0},
        {&TextIdPc880cd8, -1, 0},
        {&TextIdPc880cdc, -1, 0},
        {&TextIdPc880ce0, -1, 0},
        {&TextIdPc880ce4, -1, 0},
        {&TextIdPc880ce8, -1, 0},
        {&TextIdPc880cec, -1, 0},
        {&TextIdPc880cf0, -1, 0},
        {&TextIdPc880cf4, -1, 0},
        {&TextIdPc880cf8, -1, 0},
        {&TextIdPc880cfc, -1, 0},
        {&TextIdPc880d00, 1249, 0},
        {&TextIdPc880d04, -1, 0},
        {&TextIdPc880d08, -1, 0},
        {&TextIdPc880d0c, -1, 0},
        {&TextIdPc880d10, -1, 0},
        {&TextIdPc880d14, -1, 0},
        {&TextIdPc880d18, -1, 0},
        {&TextIdPc880d1c, -1, 0},
        {&TextIdPc880d20, -1, 0},
        {&TextIdPc880d24, -1, 0},
        {&TextIdPc880d28, -1, 0},
        {&TextIdPc880d2c, -1, 0},
        {&TextIdPc880d30, -1, 0},
        {&TextIdPc880d34, -1, 0},
        {&TextIdPc880d38, -1, 0},
        {&TextIdPc880d3c, -1, 0},
        {&TextIdPc880d40, -1, 0},
        {&TextIdPc880d44, -1, 0},
        {&TextIdPc8810d4, 1300, 0},
        {&TextIdPc8810d8, -1, 0},
        {&TextIdPc8810dc, -1, 0},
        {&TextIdPc8810e0, -1, 0},
        {&TextIdPc8810e4, -1, 0},
        {&TextIdPc8810e8, -1, 0},
        {&TextIdPc8810ec, -1, 0},
        {&TextIdPc8810f0, -1, 0},
        {&TextIdPc8810f4, -1, 0},
        {&TextIdPc8810f8, -1, 0},
        {&TextIdPc8810fc, -1, 0},
        {&TextIdPc881100, -1, 0},
        {&TextIdPc881104, -1, 0},
        {&TextIdPc881108, -1, 0},
        {&TextIdPc88110c, -1, 0},
        {&TextIdPc881110, -1, 0},
        {&TextIdPc881114, -1, 0},
        {&TextIdPc881118, -1, 0},
        {&TextIdPc88111c, -1, 0},
        {&TextIdPc881120, -1, 0},
        {&TextIdPc881124, -1, 0},
        {&TextIdPc881128, -1, 0},
        {&TextIdPc88112c, -1, 0},
        {&TextIdPc881130, -1, 0},
        {&TextIdPc881134, -1, 0},
        {&TextIdPc881138, -1, 0},
        {&TextIdPc88113c, -1, 0},
        {&TextIdPc881140, -1, 0},
        {&TextIdPc881144, -1, 0},
        {&TextIdPc881148, -1, 0},
        {&TextIdPc88114c, -1, 0},
        {&PermanentJediTitleIdPc, -1, 0},
        {&TextIdPc881154, -1, 0},
        {&TextIdPc881158, -1, 0},
        {&TextIdPc88115c, -1, 0},
        {&TextIdPc881160, -1, 0},
        {&TextIdPc881164, -1, 0},
        {&TextIdPc881168, -1, 0},
        {&TextIdPc88116c, -1, 0},
        {&TextIdPc881170, -1, 0},
        {&TextIdPc881174, -1, 0},
        {&TextIdPc881178, -1, 0},
        {&TextIdPc88117c, -1, 0},
        {&TextIdPc881180, 1400, 0},
        {&TextIdPc881184, -1, 0},
        {&TextIdPc881188, -1, 0},
        {&TextIdPc88118c, -1, 0},
        {&TextIdPc881190, -1, 0},
        {&TextIdPc881194, -1, 0},
        {&TextIdPc881198, -1, 0},
        {&TextIdPc88119c, -1, 0},
        {&TextIdPc8811a0, -1, 0},
        {&TextIdPc8811a4, -1, 0},
        {&TextIdPc8811a8, -1, 0},
        {&TextIdPc880d48, 1450, 0},
        {&TextIdPc880d4c, -1, 0},
        {&TextIdPc880d50, -1, 0},
        {&TextIdPc880d54, -1, 0},
        {&TextIdPc880d58, -1, 0},
        {&TextIdPc880d5c, -1, 0},
        {&TextIdPc880d60, -1, 0},
        {&TextIdPc880d64, -1, 0},
        {&TextIdPc880d68, -1, 0},
        {&TextIdPc880d6c, -1, 0},
        {&TextIdPc880d70, -1, 0},
        {&TextIdPc880d74, -1, 0},
        {&TextIdPc880d78, -1, 0},
        {&TextIdPc880d7c, -1, 0},
        {&TextIdPc880d80, -1, 0},
        {&TextIdPc880d84, -1, 0},
        {&TextIdPc880d88, -1, 0},
        {&TextIdPc880d8c, -1, 0},
        {&TextIdPc880d90, -1, 0},
        {&TextIdPc880d94, -1, 0},
        {&TextIdPc880d98, 1500, 0},
        {&TextIdPc880d9c, -1, 0},
        {&TextIdPc880da0, -1, 0},
        {&TextIdPc880da4, -1, 0},
        {&TextIdPc880da8, -1, 0},
        {&TextIdPc880dac, -1, 0},
        {&TextIdPc880db0, -1, 0},
        {&TextIdPc880db4, -1, 0},
        {&TextIdPc880db8, -1, 0},
        {&TextIdPc880dbc, -1, 0},
        {&TextIdPc880dc0, -1, 0},
        {&TextIdPc880dc4, -1, 0},
        {&TextIdPc880dc8, -1, 0},
        {&TextIdPc880dcc, -1, 0},
        {&TextIdPc880dd0, -1, 0},
        {&TextIdPc880dd4, -1, 0},
        {&TextIdPc880dd8, -1, 0},
        {&TextIdPc880ddc, -1, 0},
        {&TextIdPc880de0, -1, 0},
        {&TextIdPc880de4, -1, 0},
        {&TextIdPc880de8, -1, 0},
        {&TextIdPc880dec, -1, 0},
        {&TextIdPc880df0, -1, 0},
        {&TextIdPc880df4, -1, 0},
        {&TextIdPc880df8, -1, 0},
        {&TextIdPc880dfc, -1, 0},
        {&TextIdPc880e00, -1, 0},
        {&TextIdPc880e04, -1, 0},
        {&TextIdPc8811ac, -1, 0},
        {&TextIdPc8811b0, -1, 0},
        {&TextIdPc8811b4, -1, 0},
        {&TextIdPc8811b8, -1, 0},
        {&TextIdPc8811bc, -1, 0},
        {&TextIdPc8811c0, -1, 0},
        {&TextIdPc8811c4, -1, 0},
        {&TextIdPc8811c8, -1, 0},
        {&TextIdPc8811cc, -1, 0},
        {&TextIdPc8811d0, -1, 0},
        {&TextIdPc8811d4, -1, 0},
        {&TextIdPc8811d8, -1, 0},
        {&TextIdPc8811dc, -1, 0},
        {&TextIdPc8811e0, -1, 0},
        {&TextIdPc8811e4, -1, 0},
        {&TextIdPc8811e8, -1, 0},
        {&TextIdPc8811ec, -1, 0},
        {&TextIdPc8811f0, -1, 0},
        {&TextIdPc8811f4, -1, 0},
        {&TextIdPc8811f8, -1, 0},
        {&TextIdPc880e08, 1700, 0},
        {&TextIdPc880e0c, 1701, 0},
        {&TextIdPc880e10, -1, 0},
        {&TextIdPc880e14, -1, 0},
        {&TextIdPc880e18, -1, 0},
        {&TextIdPc880e1c, -1, 0},
        {&TextIdPc880e20, -1, 0},
        {&TextIdPc880e24, -1, 0},
        {&TextIdPc880e28, -1, 0},
        {&TextIdPc880e2c, -1, 0},
        {&TextIdPc880e30, 1733, 0},
        {&TextIdPc880e34, 1711, 0},
        {&TextIdPc880e38, -1, 0},
        {&TextIdPc880e3c, -1, 0},
        {&TextIdPc880e40, -1, 0},
        {&TextIdPc880e44, -1, 0},
        {&TextIdPc880e48, -1, 0},
        {&TextIdPc880e4c, -1, 0},
        {&TextIdPc880e50, -1, 0},
        {&TextIdPc880e54, -1, 0},
        {&TextIdPc880e58, -1, 0},
        {&TextIdPc880e5c, -1, 0},
        {&TextIdPc880e60, -1, 0},
        {&TextIdPc880e64, -1, 0},
        {&TextIdPc880e68, -1, 0},
        {&TextIdPc880e6c, -1, 0},
        {&TextIdPc880e70, -1, 0},
        {&TextIdPc880e74, -1, 0},
        {&TextIdPc880e78, -1, 0},
        {&TextIdPc880e7c, -1, 0},
        {&TextIdPc880e80, -1, 0},
        {&TextIdPc880e84, -1, 0},
        {&TextIdPc880e88, -1, 0},
        {&TextIdPc880e8c, 1734, 0},
        {&TextIdPc880e90, -1, 0},
        {&TextIdPc880e94, -1, 0},
        {&TextIdPc880e98, -1, 0},
        {&TextIdPc880e9c, -1, 0},
        {&TextIdPc880ea0, -1, 0},
        {&TextIdPc880ea4, -1, 0},
        {&TextIdPc880ea8, 1742, 0},
        {&TextIdPc880eac, -1, 0},
        {&TextIdPc880eb0, -1, 0},
        {&TextIdPc880eb4, -1, 0},
        {&TextIdPc880eb8, -1, 0},
        {&TextIdPc880ebc, -1, 0},
        {&TextIdPc880ec0, -1, 0},
        {&TextIdPc880ec4, -1, 0},
        {&TextIdPc880ecc, -1, 0},
        {&TextIdPc880ec8, 1751, 0},
        {&TextIdPc880ed0, -1, 0},
        {&TextIdPc880ed4, -1, 0},
        {&TextIdPc880ed8, -1, 0},
        {&TextIdPc880edc, -1, 0},
        {&TextIdPc880ee0, -1, 0},
        {&TextIdPc880ee4, -1, 0},
        {&TextIdPc880ef0, -1, 0},
        {&TextIdPc880ef4, -1, 0},
        {&TextIdPc880ef8, -1, 0},
        {&TextIdPc880ee8, -1, 0},
        {&TextIdPc880eec, -1, 0},
        {&TextIdPc880efc, -1, 0},
        {&TextIdPc880924, 1343, 0},
        {NULL, -1, 0},
    };
}
