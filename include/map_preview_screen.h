#ifndef GUARD_MAP_PREVIEW_SCREEN_H
#define GUARD_MAP_PREVIEW_SCREEN_H

enum MapPreviewScreenId
{
    MPS_VIRIDIAN_FOREST = 0u,
    MPS_MT_MOON,
    MPS_DIGLETTS_CAVE,
    MPS_ROCK_TUNNEL,
    MPS_POKEMON_TOWER,
    MPS_SAFARI_ZONE,
    MPS_SEAFOAM_ISLANDS,
    MPS_POKEMON_MANSION,
    MPS_ROCKET_HIDEOUT,
    MPS_SILPH_CO,
    MPS_VICTORY_ROAD,
    MPS_CERULEAN_CAVE,
    MPS_POWER_PLANT,
    MPS_MT_EMBER,
    MPS_ROCKET_WAREHOUSE,
    MPS_MONEAN_CHAMBER,
    MPS_DOTTED_HOLE,
    MPS_BERRY_FOREST,
    MPS_ICEFALL_CAVE,
    MPS_LOST_CAVE,
    MPS_ALTERING_CAVE,
    MPS_PATTERN_BUSH,
    MPS_LIPTOO_CHAMBER,
    MPS_WEEPTH_CHAMBER,
    MPS_TDILFORD_CHAMBER,
    MPS_SCUFIB_CHAMBER,
    MPS_RIXY_CHAMBER,
    MPS_VIAPOIS_CHAMBER,
//Johto
//    MPS_HGSS_BURNED_TOWER,
//    MPS_HGSS_DARK_CAVE,
//    MPS_HGSS_DRAGON_DEN,
//    MPS_HGSS_ICE_PATH,
//    MPS_HGSS_ILEX_FOREST,
//    MPS_HGSS_MT_MORTAR,
//    MPS_HGSS_MT_SILVER,
//    MPS_HGSS_NATIONAL_PARK,
//    MPS_HGSS_RUINS_OF_ALPH,
//    MPS_HGSS_SLOWPOKE_WELL,
    MPS_SPROUT_TOWER,
//    MPS_HGSS_TIN_TOWER,
//    MPS_HGSS_TOHJO_FALLS,
//    MPS_HGSS_UNION_CAVE,
//    MPS_HGSS_WHIRL_ISLANDS,
    MPS_COUNT
};

#define MPS_TYPE_CAVE   0
#define MPS_TYPE_FOREST 1
#define MPS_TYPE_ANY    2
#define MPS_TYPE_BASIC  3

#define MPS_BASIC_FADE_SPEED    1   // Speed the map preview fades out at when MPS_TYPE_BASIC is used.
                                    // Numbers less than 1 give a shorter fade (negative allowed);
                                    // numbers greater than 1 give a longer fade.

struct MapPreviewScreen
{
    mapsec_u8_t mapsec;
    u8 type;
    u16 flagId;
    const void *tilesptr;
    const void *tilemapptr;
    const void *palptr;
};

u16 MapPreview_CreateMapNameWindow(u8 id);
void MapPreview_SetFlag(u16 a0);
u16 MapPreview_GetDuration(u8 id);
bool8 MapHasPreviewScreen(mapsec_u8_t mapsec, u8 type);
bool32 ForestMapPreviewScreenIsRunning(void);
const struct MapPreviewScreen * GetDungeonMapPreviewScreenInfo(mapsec_u8_t mapsec);
bool32 MapHasPreviewScreen_HandleQLState2(mapsec_u8_t mapsec, u8 type);
void MapPreview_InitBgs(void);
void MapPreview_LoadGfx(mapsec_u8_t mapsec);
bool32 MapPreview_IsGfxLoadFinished(void);
void MapPreview_Unload(s32 windowId);
void MapPreview_UnloadBgOnly(void);
void MapPreview_StartForestTransition(mapsec_u8_t mapsec);

#endif //GUARD_MAP_PREVIEW_SCREEN_H