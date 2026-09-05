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
    MPS_BURNED_TOWER,
    MPS_DARK_CAVE,
	MPS_DARK_CAVE_45,
    MPS_DRAGON_DEN,
    MPS_ICE_PATH,
    MPS_ILEX_FOREST,
    MPS_MT_MORTAR,
    MPS_MT_SILVER,
    MPS_NATIONAL_PARK,
    MPS_RUINS_OF_ALPH,
    MPS_SLOWPOKE_WELL,
    MPS_SPROUT_TOWER,
    MPS_TIN_TOWER,
    MPS_TOHJO_FALLS,
    MPS_UNION_CAVE,
    MPS_WHIRL_ISLANDS,
    MPS_COUNT
};

#define MPS_TYPE_CAVE       0
#define MPS_TYPE_FOREST     1
#define MPS_TYPE_ANY        2
#define MPS_TYPE_BASIC      3
#define MPS_TYPE_KANTO_CAVE 4

#define MPS_BASIC_FADE_SPEED    1   // Speed the map preview fades out at when MPS_TYPE_BASIC is used.
                                    // Numbers less than 1 give a shorter fade (negative allowed);
                                    // numbers greater than 1 give a longer fade.

#define MPS_FOREST_FADE_FRAMES_PER_STEP 2   // Frames spent on each of the 16 cross-fade steps when
                                            // MPS_TYPE_FOREST is used (1 = original speed, ~0.8s
                                            // total; 2 = ~1.6s). Higher values slow the fade down
                                            // without changing its stepping pattern.

// Total frames the MPS_TYPE_FOREST cross-fade itself runs for. The fade advances
// one of its 16 alpha steps on each of the first two phases of a 3-phase cycle,
// and finishes on the second phase of the 16th cycle: (15 * 3 + 1) * N = 46 * N.
#define MPS_FOREST_FADE_TOTAL_FRAMES (46 * MPS_FOREST_FADE_FRAMES_PER_STEP)

// How far into the MPS_TYPE_FOREST cross-fade the player stays rooted in place.
// The CAVE and BASIC previews run before the map is handed back, so the player
// simply cannot move during them; the FOREST preview cross-fades over the live
// map, and the warp-exit task returns controls the moment the weather fade-in
// ends - i.e. right as the preview starts its hold, while it is still fully
// opaque. The player is therefore held for the whole hold plus this much of the
// cross-fade, so control only comes back once the map underneath is visible.
// 0 = unlock the instant the fade starts, 100 = stay locked for the whole fade.
#define MPS_FOREST_LOCK_FADE_PERCENT 50

struct MapPreviewScreen
{
    mapsec_u8_t mapsec;
    u8 type;
    u16 flagId;
    const void *tilesptr;
    const void *tilemapptr;
    const void *palptr;
#if IS_HNS
    // Optional night-time variant. Leave NULL (the default) for locations with
    // only one preview image - tilesptr/tilemapptr/palptr above are then used
    // at all times of day.
    const void *nightTilesptr;
    const void *nightTilemapptr;
    const void *nightPalptr;
#endif
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
bool8 MapPreview_ForestFadeIsActive(void);
bool8 MapPreview_ForestInputIsLocked(void);

#endif //GUARD_MAP_PREVIEW_SCREEN_H