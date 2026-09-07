#include "global.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "gpu_regs.h"
#include "malloc.h"
#include "map_name_popup.h"
#include "map_preview_screen.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "region_map.h"
#include "rtc.h"
#include "script.h"
#include "secret_base.h"
#include "string_util.h"
#include "constants/region_map_sections.h"

static EWRAM_DATA bool8 sHasVisitedMapBefore = FALSE;

static EWRAM_DATA bool8 sAllocedBg0TilemapBuffer = FALSE;

// Frames the player is still held in place for by the FOREST transition, ticked
// down by Task_RunMapPreviewScreenForest. See MPS_FOREST_LOCK_FADE_PERCENT.
static EWRAM_DATA u16 sForestInputLockFrames = 0;

static void Task_RunMapPreviewScreenForest(u8 taskId);

static const u8 sViridianForestMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/viridian_forest/day/tiles.gbapal");
static const u8 sViridianForestMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/viridian_forest/day/tiles.4bpp.smol");
static const u8 sViridianForestMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/viridian_forest/day/tilemap.bin.smolTM");
static const u8 sViridianForestNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/viridian_forest/night/tiles.gbapal");
static const u8 sViridianForestNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/viridian_forest/night/tiles.4bpp.smol");
static const u8 sViridianForestNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/viridian_forest/night/tilemap.bin.smolTM");
static const u8 sRockTunnelMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/rock_tunnel/tiles.gbapal");
static const u8 sRockTunnelMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/rock_tunnel/tiles.4bpp.smol");
static const u8 sRockTunnelMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/rock_tunnel/tilemap.bin.smolTM");
static const u8 sRocketHideoutMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/rocket_hideout/tiles.gbapal");
static const u8 sRocketHideoutMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/rocket_hideout/tiles.4bpp.smol");
static const u8 sRocketHideoutMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/rocket_hideout/tilemap.bin.smolTM");
static const u8 sPowerPlantMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/power_plant/tiles.gbapal");
static const u8 sPowerPlantMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/power_plant/tiles.4bpp.smol");
static const u8 sPowerPlantMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/power_plant/tilemap.bin.smolTM");
static const u8 sPokemonMansionMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/pokemon_mansion/tiles.gbapal");
static const u8 sPokemonMansionMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/pokemon_mansion/tiles.4bpp.smol");
static const u8 sPokemonMansionMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/pokemon_mansion/tilemap.bin.smolTM");
static const u8 sPokemonTowerMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/pokemon_tower/tiles.gbapal");
static const u8 sPokemonTowerMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/pokemon_tower/tiles.4bpp.smol");
static const u8 sPokemonTowerMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/pokemon_tower/tilemap.bin.smolTM");
static const u8 sSilphCoMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/silph_co/tiles.gbapal");
static const u8 sSilphCoMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/silph_co/tiles.4bpp.smol");
static const u8 sSilphCoMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/silph_co/tilemap.bin.smolTM");
static const u8 sMtMoonMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/mt_moon/day/tiles.gbapal");
static const u8 sMtMoonMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/mt_moon/day/tiles.4bpp.smol");
static const u8 sMtMoonMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/mt_moon/day/tilemap.bin.smolTM");
static const u8 sMtMoonNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/mt_moon/night/tiles.gbapal");
static const u8 sMtMoonNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/mt_moon/night/tiles.4bpp.smol");
static const u8 sMtMoonNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/mt_moon/night/tilemap.bin.smolTM");
static const u8 sSeafoamIslandsMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/seafoam_islands/tiles.gbapal");
static const u8 sSeafoamIslandsMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/seafoam_islands/tiles.4bpp.smol");
static const u8 sSeafoamIslandsMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/seafoam_islands/tilemap.bin.smolTM");
static const u8 sRocketWarehouseMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/rocket_warehouse/tiles.gbapal");
static const u8 sRocketWarehouseMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/rocket_warehouse/tiles.4bpp.smol");
static const u8 sRocketWarehouseMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/rocket_warehouse/tilemap.bin.smolTM");
static const u8 sVictoryRoadMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/victory_road/tiles.gbapal");
static const u8 sVictoryRoadMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/victory_road/tiles.4bpp.smol");
static const u8 sVictoryRoadMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/victory_road/tilemap.bin.smolTM");
static const u8 sMtEmberMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/mt_ember/tiles.gbapal");
static const u8 sMtEmberMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/mt_ember/tiles.4bpp.smol");
static const u8 sMtEmberMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/mt_ember/tilemap.bin.smolTM");
static const u8 sSafariZoneMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/safari_zone/tiles.gbapal");
static const u8 sSafariZoneMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/safari_zone/tiles.4bpp.smol");
static const u8 sSafariZoneMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/safari_zone/tilemap.bin.smolTM");
static const u8 sMoneanChamberMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/monean_chamber/tiles.gbapal");
static const u8 sMoneanChamberMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/monean_chamber/tiles.4bpp.smol");
static const u8 sMoneanChamberMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/monean_chamber/tilemap.bin.smolTM");
static const u8 sDottedHoleMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/dotted_hole/day/tiles.gbapal");
static const u8 sDottedHoleMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/dotted_hole/day/tiles.4bpp.smol");
static const u8 sDottedHoleMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/dotted_hole/day/tilemap.bin.smolTM");
static const u8 sDottedHoleNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/dotted_hole/night/tiles.gbapal");
static const u8 sDottedHoleNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/dotted_hole/night/tiles.4bpp.smol");
static const u8 sDottedHoleNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/dotted_hole/night/tilemap.bin.smolTM");
static const u8 sCeruleanCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/cerulean_cave/tiles.gbapal");
static const u8 sCeruleanCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/cerulean_cave/tiles.4bpp.smol");
static const u8 sCeruleanCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/cerulean_cave/tilemap.bin.smolTM");
static const u8 sDiglettsCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/digletts_cave/tiles.gbapal");
static const u8 sDiglettsCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/digletts_cave/tiles.4bpp.smol");
static const u8 sDiglettsCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/digletts_cave/tilemap.bin.smolTM");
static const u8 sLostCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/lost_cave/tiles.gbapal");
static const u8 sLostCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/lost_cave/tiles.4bpp.smol");
static const u8 sLostCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/lost_cave/tilemap.bin.smolTM");
static const u8 sBerryForestMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/berry_forest/tiles.gbapal");
static const u8 sBerryForestMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/berry_forest/tiles.4bpp.smol");
static const u8 sBerryForestMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/berry_forest/tilemap.bin.smolTM");
static const u8 sIcefallCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/icefall_cave/tiles.gbapal");
static const u8 sIcefallCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/icefall_cave/tiles.4bpp.smol");
static const u8 sIcefallCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/icefall_cave/tilemap.bin.smolTM");
static const u8 sIcefallNightCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/icefall_cave/night/tiles.gbapal");
static const u8 sIcefallNightCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/icefall_cave/night/tiles.4bpp.smol");
static const u8 sIcefallNightCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/icefall_cave/night/tiles.bin.smolTM");
static const u8 sAlteringCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/altering_cave/day/tiles.gbapal");
static const u8 sAlteringCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/altering_cave/day/tiles.4bpp.smol");
static const u8 sAlteringCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/altering_cave/day/tilemap.bin.smolTM");
static const u8 sAlteringCaveNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/altering_cave/night/tiles.gbapal");
static const u8 sAlteringCaveNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/altering_cave/night/tiles.4bpp.smol");
static const u8 sAlteringCaveNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/altering_cave/night/tilemap.bin.smolTM");

static const u8 sBurnedTowerMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/burned_tower/tiles.gbapal");
static const u8 sBurnedTowerMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/burned_tower/tiles.4bpp.smol");
static const u8 sBurnedTowerMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/burned_tower/tilemap.bin.smolTM");
static const u8 sDarkCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/dark_cave/route_31/tiles.gbapal");
static const u8 sDarkCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/dark_cave/route_31/tiles.4bpp.smol");
static const u8 sDarkCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/dark_cave/route_31/tilemap.bin.smolTM");
static const u8 sDarkCaveMap45PreviewPalette[] = INCBIN_U8("graphics/map_preview/dark_cave/route_45/tiles.gbapal");
static const u8 sDarkCaveMap45PreviewTiles[] = INCBIN_U8("graphics/map_preview/dark_cave/route_45/tiles.4bpp.smol");
static const u8 sDarkCaveMap45PreviewTilemap[] = INCBIN_U8("graphics/map_preview/dark_cave/route_45/tilemap.bin.smolTM");
static const u8 sDragonDenMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/dragon_den/tiles.gbapal");
static const u8 sDragonDenMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/dragon_den/tiles.4bpp.smol");
static const u8 sDragonDenMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/dragon_den/tilemap.bin.smolTM");
static const u8 sIcePathMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/ice_path/tiles.gbapal");
static const u8 sIcePathMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/ice_path/tiles.4bpp.smol");
static const u8 sIcePathMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/ice_path/tilemap.bin.smolTM");
static const u8 sIlexForestMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/ilex_forest/day/tiles.gbapal");
static const u8 sIlexForestMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/ilex_forest/day/tiles.4bpp.smol");
static const u8 sIlexForestMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/ilex_forest/day/tilemap.bin.smolTM");
static const u8 sIlexForestNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/ilex_forest/night/tiles.gbapal");
static const u8 sIlexForestNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/ilex_forest/night/tiles.4bpp.smol");
static const u8 sIlexForestNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/ilex_forest/night/tilemap.bin.smolTM");
static const u8 sMtMortarMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/mt_mortar/day/tiles.gbapal");
static const u8 sMtMortarMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/mt_mortar/day/tiles.4bpp.smol");
static const u8 sMtMortarMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/mt_mortar/day/tilemap.bin.smolTM");
static const u8 sMtMortarNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/mt_mortar/night/tiles.gbapal");
static const u8 sMtMortarNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/mt_mortar/night/tiles.4bpp.smol");
static const u8 sMtMortarNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/mt_mortar/night/tilemap.bin.smolTM");
static const u8 sMtSilverMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/mt_silver/tiles.gbapal");
static const u8 sMtSilverMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/mt_silver/tiles.4bpp.smol");
static const u8 sMtSilverMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/mt_silver/tilemap.bin.smolTM");
static const u8 sNationalParkMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/national_park/day/tiles.gbapal");
static const u8 sNationalParkMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/national_park/day/tiles.4bpp.smol");
static const u8 sNationalParkMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/national_park/day/tilemap.bin.smolTM");
static const u8 sNationalParkNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/national_park/night/tiles.gbapal");
static const u8 sNationalParkNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/national_park/night/tiles.4bpp.smol");
static const u8 sNationalParkNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/national_park/night/tilemap.bin.smolTM");
static const u8 sRuinsOfAlphMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/ruins_of_alph/tiles.gbapal");
static const u8 sRuinsOfAlphMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/ruins_of_alph/tiles.4bpp.smol");
static const u8 sRuinsOfAlphMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/ruins_of_alph/tilemap.bin.smolTM");
static const u8 sSlowpokeWellMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/slowpoke_well/tiles.gbapal");
static const u8 sSlowpokeWellMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/slowpoke_well/tiles.4bpp.smol");
static const u8 sSlowpokeWellMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/slowpoke_well/tilemap.bin.smolTM");
static const u8 sSproutTowerMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/sprout_tower/tiles.gbapal");
static const u8 sSproutTowerMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/sprout_tower/tiles.4bpp.smol");
static const u8 sSproutTowerMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/sprout_tower/tilemap.bin.smolTM");
static const u8 sTinTowerMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/tin_tower/day/tiles.gbapal");
static const u8 sTinTowerMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/tin_tower/day/tiles.4bpp.smol");
static const u8 sTinTowerMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/tin_tower/day/tilemap.bin.smolTM");
static const u8 sTinTowerNightMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/tin_tower/night/tiles.gbapal");
static const u8 sTinTowerNightMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/tin_tower/night/tiles.4bpp.smol");
static const u8 sTinTowerNightMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/tin_tower/night/tilemap.bin.smolTM");
static const u8 sTohjoFallsMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/tohjo_falls/tiles.gbapal");
static const u8 sTohjoFallsMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/tohjo_falls/tiles.4bpp.smol");
static const u8 sTohjoFallsMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/tohjo_falls/tilemap.bin.smolTM");
static const u8 sUnionCaveMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/union_cave/tiles.gbapal");
static const u8 sUnionCaveMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/union_cave/tiles.4bpp.smol");
static const u8 sUnionCaveMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/union_cave/tilemap.bin.smolTM");
static const u8 sWhirlIslandsMapPreviewPalette[] = INCBIN_U8("graphics/map_preview/whirl_islands/tiles.gbapal");
static const u8 sWhirlIslandsMapPreviewTiles[] = INCBIN_U8("graphics/map_preview/whirl_islands/tiles.4bpp.smol");
static const u8 sWhirlIslandsMapPreviewTilemap[] = INCBIN_U8("graphics/map_preview/whirl_islands/tilemap.bin.smolTM");

static const struct MapPreviewScreen sMapPreviewScreenData[MPS_COUNT] = {
    [MPS_VIRIDIAN_FOREST] = {
        .mapsec = MAPSEC_VIRIDIAN_FOREST,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_VIRIDIAN_FOREST,
        .tilesptr = sViridianForestMapPreviewTiles,
        .tilemapptr = sViridianForestMapPreviewTilemap,
        .palptr = sViridianForestMapPreviewPalette,
        .nightTilesptr = sViridianForestNightMapPreviewTiles,
        .nightTilemapptr = sViridianForestNightMapPreviewTilemap,
        .nightPalptr = sViridianForestNightMapPreviewPalette
    },
    [MPS_MT_MOON] = {
        .mapsec = MAPSEC_MT_MOON,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_MT_MOON_1F,
        .tilesptr = sMtMoonMapPreviewTiles,
        .tilemapptr = sMtMoonMapPreviewTilemap,
        .palptr = sMtMoonMapPreviewPalette,
        .nightTilesptr = sMtMoonNightMapPreviewTiles,
        .nightTilemapptr = sMtMoonNightMapPreviewTilemap,
        .nightPalptr = sMtMoonNightMapPreviewPalette
    },
    [MPS_DIGLETTS_CAVE] = {
        .mapsec = MAPSEC_DIGLETTS_CAVE,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_DIGLETTS_CAVE_B1F,
        .tilesptr = sDiglettsCaveMapPreviewTiles,
        .tilemapptr = sDiglettsCaveMapPreviewTilemap,
        .palptr = sDiglettsCaveMapPreviewPalette
    },
    [MPS_ROCK_TUNNEL] = {
        .mapsec = MAPSEC_ROCK_TUNNEL,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_ROCK_TUNNEL_1F,
        .tilesptr = sRockTunnelMapPreviewTiles,
        .tilemapptr = sRockTunnelMapPreviewTilemap,
        .palptr = sRockTunnelMapPreviewPalette
    },
    [MPS_POKEMON_TOWER] = {
        .mapsec = MAPSEC_POKEMON_TOWER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_POKEMON_TOWER_1F,
        .tilesptr = sPokemonTowerMapPreviewTiles,
        .tilemapptr = sPokemonTowerMapPreviewTilemap,
        .palptr = sPokemonTowerMapPreviewPalette
    },
    [MPS_SAFARI_ZONE] = {
        .mapsec = MAPSEC_KANTO_SAFARI_ZONE,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_SAFARI_ZONE_CENTER,
        .tilesptr = sSafariZoneMapPreviewTiles,
        .tilemapptr = sSafariZoneMapPreviewTilemap,
        .palptr = sSafariZoneMapPreviewPalette
    },
    [MPS_SEAFOAM_ISLANDS] = {
        .mapsec = MAPSEC_SEAFOAM_ISLANDS,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_SEAFOAM_ISLANDS_1F,
        .tilesptr = sSeafoamIslandsMapPreviewTiles,
        .tilemapptr = sSeafoamIslandsMapPreviewTilemap,
        .palptr = sSeafoamIslandsMapPreviewPalette
    },
    [MPS_POKEMON_MANSION] = {
        .mapsec = MAPSEC_POKEMON_MANSION,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_POKEMON_MANSION_1F,
        .tilesptr = sPokemonMansionMapPreviewTiles,
        .tilemapptr = sPokemonMansionMapPreviewTilemap,
        .palptr = sPokemonMansionMapPreviewPalette
    },
    [MPS_ROCKET_HIDEOUT] = {
        .mapsec = MAPSEC_ROCKET_HIDEOUT_HNS,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_ROCKET_HIDEOUT_B1F,
        .tilesptr = sRocketHideoutMapPreviewTiles,
        .tilemapptr = sRocketHideoutMapPreviewTilemap,
        .palptr = sRocketHideoutMapPreviewPalette
    },
    [MPS_SILPH_CO] = {
        .mapsec = MAPSEC_SILPH_CO,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SILPH_CO_1F,
        .tilesptr = sSilphCoMapPreviewTiles,
        .tilemapptr = sSilphCoMapPreviewTilemap,
        .palptr = sSilphCoMapPreviewPalette
    },
    [MPS_VICTORY_ROAD] = {
        .mapsec = MAPSEC_KANTO_VICTORY_ROAD,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_VICTORY_ROAD_1F,
        .tilesptr = sVictoryRoadMapPreviewTiles,
        .tilemapptr = sVictoryRoadMapPreviewTilemap,
        .palptr = sVictoryRoadMapPreviewPalette
    },
    [MPS_CERULEAN_CAVE] = {
        .mapsec = MAPSEC_CERULEAN_CAVE,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_CERULEAN_CAVE_1F,
        .tilesptr = sCeruleanCaveMapPreviewTiles,
        .tilemapptr = sCeruleanCaveMapPreviewTilemap,
        .palptr = sCeruleanCaveMapPreviewPalette
    },
    [MPS_POWER_PLANT] = {
        .mapsec = MAPSEC_POWER_PLANT,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_POWER_PLANT,
        .tilesptr = sPowerPlantMapPreviewTiles,
        .tilemapptr = sPowerPlantMapPreviewTilemap,
        .palptr = sPowerPlantMapPreviewPalette
    },
    [MPS_MT_EMBER] = {
        .mapsec = MAPSEC_MT_EMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_MT_EMBER_EXTERIOR,
        .tilesptr = sMtEmberMapPreviewTiles,
        .tilemapptr = sMtEmberMapPreviewTilemap,
        .palptr = sMtEmberMapPreviewPalette
    },
    [MPS_ROCKET_WAREHOUSE] = {
        .mapsec = MAPSEC_ROCKET_WAREHOUSE,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_THREE_ISLAND_BERRY_FOREST,
        .tilesptr = sRocketWarehouseMapPreviewTiles,
        .tilemapptr = sRocketWarehouseMapPreviewTilemap,
        .palptr = sRocketWarehouseMapPreviewPalette
    },
    [MPS_MONEAN_CHAMBER] = {
        .mapsec = MAPSEC_MONEAN_CHAMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,
        .tilesptr = sMoneanChamberMapPreviewTiles,
        .tilemapptr = sMoneanChamberMapPreviewTilemap,
        .palptr = sMoneanChamberMapPreviewPalette
    },
    [MPS_DOTTED_HOLE] = {
        .mapsec = MAPSEC_DOTTED_HOLE,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SIX_ISLAND_DOTTED_HOLE_1F,
        .tilesptr = sDottedHoleMapPreviewTiles,
        .tilemapptr = sDottedHoleMapPreviewTilemap,
        .palptr = sDottedHoleMapPreviewPalette,
        .nightTilesptr = sDottedHoleNightMapPreviewTiles,
        .nightTilemapptr = sDottedHoleNightMapPreviewTilemap,
        .nightPalptr = sDottedHoleNightMapPreviewPalette
    },
    [MPS_BERRY_FOREST] = {
        .mapsec = MAPSEC_BERRY_FOREST,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_THREE_ISLAND_BERRY_FOREST,
        .tilesptr = sBerryForestMapPreviewTiles,
        .tilemapptr = sBerryForestMapPreviewTilemap,
        .palptr = sBerryForestMapPreviewPalette
    },
    [MPS_ICEFALL_CAVE] = {
        .mapsec = MAPSEC_ICEFALL_CAVE,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_FOUR_ISLAND_ICEFALL_CAVE_ENTRANCE,
        .tilesptr = sIcefallCaveMapPreviewTiles,
        .tilemapptr = sIcefallCaveMapPreviewTilemap,
        .palptr = sIcefallCaveMapPreviewPalette,
		.nightTilesptr = sIcefallNightCaveMapPreviewTiles,
		.nightTilemapptr = sIcefallNightCaveMapPreviewTilemap,
		.nightPalptr = sIcefallNightCaveMapPreviewPalette
    },
    [MPS_LOST_CAVE] = {
        .mapsec = MAPSEC_LOST_CAVE,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_FIVE_ISLAND_LOST_CAVE_ENTRANCE,
        .tilesptr = sLostCaveMapPreviewTiles,
        .tilemapptr = sLostCaveMapPreviewTilemap,
        .palptr = sLostCaveMapPreviewPalette
    },
    [MPS_ALTERING_CAVE] = {
        .mapsec = MAPSEC_ALTERING_CAVE,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SIX_ISLAND_ALTERING_CAVE,
        .tilesptr = sAlteringCaveMapPreviewTiles,
        .tilemapptr = sAlteringCaveMapPreviewTilemap,
        .palptr = sAlteringCaveMapPreviewPalette,
        .nightTilesptr = sAlteringCaveNightMapPreviewTiles,
        .nightTilemapptr = sAlteringCaveNightMapPreviewTilemap,
        .nightPalptr = sAlteringCaveNightMapPreviewPalette
    },
    [MPS_PATTERN_BUSH] = {
        .mapsec = MAPSEC_PATTERN_BUSH,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_SIX_ISLAND_PATTERN_BUSH,
        .tilesptr = sViridianForestMapPreviewTiles,
        .tilemapptr = sViridianForestMapPreviewTilemap,
        .palptr = sViridianForestMapPreviewPalette
    },
    [MPS_LIPTOO_CHAMBER] = {
        .mapsec = MAPSEC_LIPTOO_CHAMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,
        .tilesptr = sMoneanChamberMapPreviewTiles,
        .tilemapptr = sMoneanChamberMapPreviewTilemap,
        .palptr = sMoneanChamberMapPreviewPalette
    },
    [MPS_WEEPTH_CHAMBER] = {
        .mapsec = MAPSEC_WEEPTH_CHAMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,
        .tilesptr = sMoneanChamberMapPreviewTiles,
        .tilemapptr = sMoneanChamberMapPreviewTilemap,
        .palptr = sMoneanChamberMapPreviewPalette
    },
    [MPS_TDILFORD_CHAMBER] = {
        .mapsec = MAPSEC_DILFORD_CHAMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,
        .tilesptr = sMoneanChamberMapPreviewTiles,
        .tilemapptr = sMoneanChamberMapPreviewTilemap,
        .palptr = sMoneanChamberMapPreviewPalette
    },
    [MPS_SCUFIB_CHAMBER] = {
        .mapsec = MAPSEC_SCUFIB_CHAMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,
        .tilesptr = sMoneanChamberMapPreviewTiles,
        .tilemapptr = sMoneanChamberMapPreviewTilemap,
        .palptr = sMoneanChamberMapPreviewPalette
    },
    [MPS_RIXY_CHAMBER] = {
        .mapsec = MAPSEC_RIXY_CHAMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,
        .tilesptr = sMoneanChamberMapPreviewTiles,
        .tilemapptr = sMoneanChamberMapPreviewTilemap,
        .palptr = sMoneanChamberMapPreviewPalette
    },
    [MPS_VIAPOIS_CHAMBER] = {
        .mapsec = MAPSEC_VIAPOIS_CHAMBER,
        .type = MPS_TYPE_KANTO_CAVE,
        .flagId = FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,
        .tilesptr = sMoneanChamberMapPreviewTiles,
        .tilemapptr = sMoneanChamberMapPreviewTilemap,
        .palptr = sMoneanChamberMapPreviewPalette
    },
 //Johto
    [MPS_BURNED_TOWER] = {
        .mapsec = MAPSEC_BURNED_TOWER,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_BURNED_TOWER,
        .tilesptr = sBurnedTowerMapPreviewTiles,
        .tilemapptr = sBurnedTowerMapPreviewTilemap,
        .palptr = sBurnedTowerMapPreviewPalette
    },
    [MPS_DARK_CAVE] = {
        .mapsec = MAPSEC_DARK_CAVE,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_DARK_CAVE,
        .tilesptr = sDarkCaveMapPreviewTiles,
        .tilemapptr = sDarkCaveMapPreviewTilemap,
        .palptr = sDarkCaveMapPreviewPalette
    },
    [MPS_DARK_CAVE_45] = {
        .mapsec = MAPSEC_DARK_CAVE,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_DARK_CAVE,
        .tilesptr = sDarkCaveMap45PreviewTiles,
        .tilemapptr = sDarkCaveMap45PreviewTilemap,
        .palptr = sDarkCaveMap45PreviewPalette
    },
    [MPS_DRAGON_DEN] = {
        .mapsec = MAPSEC_DRAGONS_DEN,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_DRAGONS_DEN,
        .tilesptr = sDragonDenMapPreviewTiles,
        .tilemapptr = sDragonDenMapPreviewTilemap,
        .palptr = sDragonDenMapPreviewPalette
    },
    [MPS_ICE_PATH] = {
        .mapsec = MAPSEC_ICE_PATH,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_ICE_PATH,
        .tilesptr = sIcePathMapPreviewTiles,
        .tilemapptr = sIcePathMapPreviewTilemap,
        .palptr = sIcePathMapPreviewPalette
    },
    [MPS_ILEX_FOREST] = {
        .mapsec = MAPSEC_ILEX_FOREST,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_ILEX_FOREST,
        .tilesptr = sIlexForestMapPreviewTiles,
        .tilemapptr = sIlexForestMapPreviewTilemap,
        .palptr = sIlexForestMapPreviewPalette,
        .nightTilesptr = sIlexForestNightMapPreviewTiles,
        .nightTilemapptr = sIlexForestNightMapPreviewTilemap,
        .nightPalptr = sIlexForestNightMapPreviewPalette
    },
    [MPS_MT_MORTAR] = {
        .mapsec = MAPSEC_MT_MORTAR,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_MT_MORTAR,
        .tilesptr = sMtMortarMapPreviewTiles,
        .tilemapptr = sMtMortarMapPreviewTilemap,
        .palptr = sMtMortarMapPreviewPalette,
        .nightTilesptr = sMtMortarNightMapPreviewTiles,
        .nightTilemapptr = sMtMortarNightMapPreviewTilemap,
        .nightPalptr = sMtMortarNightMapPreviewPalette
    },
    [MPS_MT_SILVER] = {
        .mapsec = MAPSEC_MT_SILVER,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_MT_SILVER,
        .tilesptr = sMtSilverMapPreviewTiles,
        .tilemapptr = sMtSilverMapPreviewTilemap,
        .palptr = sMtSilverMapPreviewPalette
    },
    [MPS_NATIONAL_PARK] = {
        .mapsec = MAPSEC_NATIONAL_PARK,
        .type = MPS_TYPE_FOREST,
        .flagId = FLAG_WORLD_MAP_NATIONAL_PARK,
        .tilesptr = sNationalParkMapPreviewTiles,
        .tilemapptr = sNationalParkMapPreviewTilemap,
        .palptr = sNationalParkMapPreviewPalette,
        .nightTilesptr = sNationalParkNightMapPreviewTiles,
        .nightTilemapptr = sNationalParkNightMapPreviewTilemap,
        .nightPalptr = sNationalParkNightMapPreviewPalette
    },
    [MPS_RUINS_OF_ALPH] = {
        .mapsec = MAPSEC_RUINS_OF_ALPH_INTERIOR,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_RUINS_OF_ALPH,
        .tilesptr = sRuinsOfAlphMapPreviewTiles,
        .tilemapptr = sRuinsOfAlphMapPreviewTilemap,
        .palptr = sRuinsOfAlphMapPreviewPalette
    },
    [MPS_SLOWPOKE_WELL] = {
        .mapsec = MAPSEC_SLOWPOKE_WELL,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_SLOWPOKE_WELL,
        .tilesptr = sSlowpokeWellMapPreviewTiles,
        .tilemapptr = sSlowpokeWellMapPreviewTilemap,
        .palptr = sSlowpokeWellMapPreviewPalette
    },
    [MPS_SPROUT_TOWER] = {
        .mapsec = MAPSEC_SPROUT_TOWER,
        .type = MPS_TYPE_BASIC,
        .flagId = FLAG_WORLD_MAP_SPROUT_TOWER,
        .tilesptr = sSproutTowerMapPreviewTiles,
        .tilemapptr = sSproutTowerMapPreviewTilemap,
        .palptr = sSproutTowerMapPreviewPalette
    },
    [MPS_TIN_TOWER] = {
        .mapsec = MAPSEC_TIN_TOWER,
        .type = MPS_TYPE_BASIC,
        .flagId = FLAG_WORLD_MAP_TIN_TOWER,
        .tilesptr = sTinTowerMapPreviewTiles,
        .tilemapptr = sTinTowerMapPreviewTilemap,
        .palptr = sTinTowerMapPreviewPalette,
        .nightTilesptr = sTinTowerNightMapPreviewTiles,
        .nightTilemapptr = sTinTowerNightMapPreviewTilemap,
        .nightPalptr = sTinTowerNightMapPreviewPalette
    },
    [MPS_TOHJO_FALLS] = {
        .mapsec = MAPSEC_TOHJO_FALLS,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_TOHJO_FALLS,
        .tilesptr = sTohjoFallsMapPreviewTiles,
        .tilemapptr = sTohjoFallsMapPreviewTilemap,
        .palptr = sTohjoFallsMapPreviewPalette
    },
    [MPS_UNION_CAVE] = {
        .mapsec = MAPSEC_UNION_CAVE,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_UNION_CAVE,
        .tilesptr = sUnionCaveMapPreviewTiles,
        .tilemapptr = sUnionCaveMapPreviewTilemap,
        .palptr = sUnionCaveMapPreviewPalette
    },
    [MPS_WHIRL_ISLANDS] = {
        .mapsec = MAPSEC_WHIRL_ISLANDS,
        .type = MPS_TYPE_CAVE,
        .flagId = FLAG_WORLD_MAP_WHIRL_ISLANDS,
        .tilesptr = sWhirlIslandsMapPreviewTiles,
        .tilemapptr = sWhirlIslandsMapPreviewTilemap,
        .palptr = sWhirlIslandsMapPreviewPalette
	}
};

static const struct WindowTemplate sMapNameWindow = {
    .bg = 0,
    .tilemapLeft = 0,
    .tilemapTop = 0,
    .width = 13,
    .height = 2,
    .paletteNum = 14,
    .baseBlock = 0x1C2
};

static const struct BgTemplate sMapPreviewBgTemplate[1] = {
    {
        .mapBaseIndex = 31
    }
};

static u8 GetMapPreviewScreenIdx(mapsec_u8_t mapsec)
{
    s32 i;

#if IS_HNS
    // Dark Cave has two distinct entrances (Route 31 south side, Route 45 north side)
    // sharing one mapsec, so this is the one location needing an entrance-specific preview.
    if (mapsec == MAPSEC_DARK_CAVE && GetLastUsedWarpMapSectionId() == MAPSEC_ROUTE_45)
        return MPS_DARK_CAVE_45;
#endif

    for (i = 0; i < MPS_COUNT; i++)
    {
        if (sMapPreviewScreenData[i].mapsec == mapsec)
        {
            return i;
        }
    }
    return MPS_COUNT;
}

bool8 MapHasPreviewScreen(mapsec_u8_t mapsec, u8 type)
{
    u8 idx;

    idx = GetMapPreviewScreenIdx(mapsec);
    if (idx != MPS_COUNT)
    {
        if (type == MPS_TYPE_ANY)
        {
            return TRUE;
        }
        else
        {
            return sMapPreviewScreenData[idx].type == type ? TRUE : FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

bool32 MapHasPreviewScreen_HandleQLState2(mapsec_u8_t mapsec, u8 type)
{
    return MapHasPreviewScreen(mapsec, type);
}

void MapPreview_InitBgs(void)
{
    InitBgsFromTemplates(0, sMapPreviewBgTemplate, NELEMS(sMapPreviewBgTemplate));
    ShowBg(0);
}

void MapPreview_LoadGfx(mapsec_u8_t mapsec)
{
    u8 idx;
    const void *tilesptr;
    const void *tilemapptr;
    const void *palptr;

    idx = GetMapPreviewScreenIdx(mapsec);
    if (idx != MPS_COUNT)
    {
       tilesptr = sMapPreviewScreenData[idx].tilesptr;
       tilemapptr = sMapPreviewScreenData[idx].tilemapptr;
       palptr = sMapPreviewScreenData[idx].palptr;
#if IS_HNS
       // Locations without a night variant leave these NULL, so they always
       // fall through to the day/default assets above.
       if (sMapPreviewScreenData[idx].nightTilesptr != NULL && GetTimeOfDay() == TIME_NIGHT)
       {
           tilesptr = sMapPreviewScreenData[idx].nightTilesptr;
           tilemapptr = sMapPreviewScreenData[idx].nightTilemapptr;
           palptr = sMapPreviewScreenData[idx].nightPalptr;
       }
#endif
       ResetTempTileDataBuffers();
       if (sMapPreviewScreenData[idx].type == MPS_TYPE_FOREST
		   || sMapPreviewScreenData[idx].type == MPS_TYPE_KANTO_CAVE)
           LoadPalette(palptr, BG_PLTT_ID(13), 3 * PLTT_SIZE_4BPP);
       else
           LoadPalette(palptr, BG_PLTT_ID(0), 16 * PLTT_SIZE_4BPP);
       DecompressAndCopyTileDataToVram(0, tilesptr, 0, 0, 0);
       if (GetBgTilemapBuffer(0) == NULL)
       {
           SetBgTilemapBuffer(0, Alloc(BG_SCREEN_SIZE));
           sAllocedBg0TilemapBuffer = TRUE;
       }
       else
       {
           sAllocedBg0TilemapBuffer = FALSE;
       }
       CopyToBgTilemapBuffer(0, tilemapptr, 0, 0x000);
       CopyBgTilemapBufferToVram(0);
    }
}

void MapPreview_UnloadBgOnly(void)
{
    if (sAllocedBg0TilemapBuffer)
    {
        Free(GetBgTilemapBuffer(0));
    }
}


void MapPreview_Unload(s32 windowId)
{
    RemoveWindow(windowId);
    if (sAllocedBg0TilemapBuffer)
    {
        Free(GetBgTilemapBuffer(0));
    }
}

bool32 MapPreview_IsGfxLoadFinished(void)
{
    return FreeTempTileDataBuffersIfPossible();
}

void MapPreview_StartForestTransition(mapsec_u8_t mapsec)
{
    u8 taskId;

    taskId = CreateTask(Task_RunMapPreviewScreenForest, 0);
    gTasks[taskId].data[2] = GetBgAttribute(0, BG_ATTR_PRIORITY);
    gTasks[taskId].data[4] = GetGpuReg(REG_OFFSET_BLDCNT);
    gTasks[taskId].data[5] = GetGpuReg(REG_OFFSET_BLDALPHA);
    gTasks[taskId].data[3] = GetGpuReg(REG_OFFSET_DISPCNT);
    gTasks[taskId].data[6] = GetGpuReg(REG_OFFSET_WININ);
    gTasks[taskId].data[7] = GetGpuReg(REG_OFFSET_WINOUT);
    gTasks[taskId].data[10] = MapPreview_GetDuration(mapsec);
    gTasks[taskId].data[8] = 16;
    gTasks[taskId].data[9] = 0;
    SetBgAttribute(0, BG_ATTR_PRIORITY, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
    SetGpuRegBits(REG_OFFSET_WININ, WININ_WIN0_CLR | WININ_WIN1_CLR);
    SetGpuRegBits(REG_OFFSET_WINOUT, WINOUT_WIN01_CLR);
    PreservePaletteInWeather(13);
//    gTasks[taskId].data[11] = MapPreview_CreateMapNameWindow(mapsec);
    LockPlayerFieldControls();
    // This lock does not survive on its own: the warp-exit task (Task_ExitNonDoor
    // and friends) unlocks controls as soon as the weather fade-in finishes, which
    // is exactly when the preview's hold begins - so without the window below the
    // player can walk around behind a still-opaque preview. Counted down from the
    // start of the hold, not from here, since states 0-2 are still fading in.
    sForestInputLockFrames = gTasks[taskId].data[10]
                           + MPS_FOREST_FADE_TOTAL_FRAMES * MPS_FOREST_LOCK_FADE_PERCENT / 100;
}

u16 MapPreview_CreateMapNameWindow(mapsec_u8_t mapsec)
{
    u16 windowId;
    u32 xctr;
    #ifdef BUGFIX
    // Fixes access violations indicated below.
    u8 color[3];
    #else
    u8 color[0];
    #endif

    windowId = AddWindow(&sMapNameWindow);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    PutWindowTilemap(windowId);
    color[0] = TEXT_COLOR_WHITE; // Access violation
    color[1] = TEXT_COLOR_RED; // Access violation
    color[2] = TEXT_COLOR_LIGHT_GRAY; // Access violation
    GetMapName(gStringVar4, mapsec, 0);
    xctr = 104 - GetStringWidth(FONT_NORMAL, gStringVar4, 0);
    AddTextPrinterParameterized4(windowId, FONT_NORMAL, xctr / 2, 2, 0, 0, color/* Access violation */, -1, gStringVar4);
    return windowId;
}

bool32 ForestMapPreviewScreenIsRunning(void)
{
    if (FuncIsActiveTask(Task_RunMapPreviewScreenForest) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// Whether the FOREST transition is still holding the player in place. Gated on
// the task actually running so a transition that never reaches its cleanup can
// not leave the player permanently frozen.
bool8 MapPreview_ForestInputIsLocked(void)
{
    return (sForestInputLockFrames != 0 && ForestMapPreviewScreenIsRunning() == TRUE);
}

static void Task_RunMapPreviewScreenForest(u8 taskId)
{
    s16 * data;

    data = gTasks[taskId].data;
    // Only ticks from the hold (state 3) onward - during states 0-2 the screen is
    // still fading in and the warp-exit task has not handed controls back yet, so
    // counting there would shorten the window the player actually feels.
    if (data[0] >= 3 && sForestInputLockFrames != 0)
        sForestInputLockFrames--;

    // The preview owns BLDALPHA for as long as this task lives, so re-assert it every
    // frame rather than only while the cross-fade is stepping. data[8]/data[9] hold
    // 16/0 - fully opaque - until state 4 starts moving them, which keeps the preview
    // solid through the gfx load and the hold. Without this the FadeInFromBlack() call
    // in state 1 left the whole preview part-transparent until state 4's first write.
    // State 4 writes again after stepping, so the fade itself is unchanged.
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(data[8], data[9]));

    switch (data[0])
    {
    case 0:
        if (!MapPreview_IsGfxLoadFinished() && !IsDma3ManagerBusyWithBgCopy())
        {
            data[0]++;
        }
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            FadeInFromBlack();
            data[0]++;
        }
        break;
    case 2:
        if (IsWeatherNotFadingIn())
        {
            Overworld_PlaySpecialMapMusic();
            data[0]++;
        }
        break;
    case 3:
        data[1]++;
        if (data[1] > data[10])
        {
            data[1] = 0;
            data[0]++;
        }
        break;
    case 4:
        // Each of the three phases below (increment, decrement, idle) now spans
        // MPS_FOREST_FADE_FRAMES_PER_STEP frames instead of always 1, so the fade
        // can be slowed down without changing its stepping pattern. The value 1
        // reproduces the original frame-for-frame behaviour exactly.
        if (data[1] % MPS_FOREST_FADE_FRAMES_PER_STEP == 0)
        {
            switch (data[1] / MPS_FOREST_FADE_FRAMES_PER_STEP)
            {
            case 0:
                data[9]++;
                if (data[9] > 16)
                {
                    data[9] = 16;
                }
                break;
            case 1:
                data[8]--;
                if (data[8] < 0)
                {
                    data[8] = 0;
                }
                break;
            }
        }
        data[1] = (data[1] + 1) % (MPS_FOREST_FADE_FRAMES_PER_STEP * 3);
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(data[8], data[9]));
        if (data[8] == 0 && data[9] == 16)
        {
            FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 32, 32);
            CopyBgTilemapBufferToVram(0);
            data[0]++;
        }
        break;
    case 5:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            MapPreview_UnloadBgOnly();
            ResetPaletteColorMapType(13);
            sForestInputLockFrames = 0;
            SetBgAttribute(0, BG_ATTR_PRIORITY, data[2]);
            SetGpuReg(REG_OFFSET_DISPCNT, data[3]);
            SetGpuReg(REG_OFFSET_BLDCNT, data[4]);
            SetGpuReg(REG_OFFSET_BLDALPHA, data[5]);
            SetGpuReg(REG_OFFSET_WININ, data[6]);
            SetGpuReg(REG_OFFSET_WINOUT, data[7]);
            // Now that the preview is gone, announce the map the normal way.
            if (gMapHeader.showMapName == TRUE && SecretBaseMapPopupEnabled() == TRUE)
                ShowMapNamePopup();
            DestroyTask(taskId);
        }
        break;
    }
}

const struct MapPreviewScreen * GetDungeonMapPreviewScreenInfo(mapsec_u8_t mapsec)
{
    u8 idx;

    idx = GetMapPreviewScreenIdx(mapsec);
    if (idx == MPS_COUNT)
    {
        return NULL;
    }
    else
    {
        return &sMapPreviewScreenData[idx];
    }
}

u16 MapPreview_GetDuration(mapsec_u8_t mapsec)
{
    u8 idx;
    u16 flagId;

    idx = GetMapPreviewScreenIdx(mapsec);
    if (idx == MPS_COUNT)
    {
        return 0;
    }
    flagId = sMapPreviewScreenData[idx].flagId;
    if (sMapPreviewScreenData[idx].type == MPS_TYPE_CAVE)
    {
        if (!FlagGet(flagId))
        {
            return 120;
        }
        else
        {
            return 40;
        }
    }
    else {
        if (sHasVisitedMapBefore)
        {
            return 120;
        }
        else
        {
            return 40;
        }
    }
}

bool8 MapPreview_ForestFadeIsActive(void)
{
#if IS_HNS
    return MapHasPreviewScreen_HandleQLState2(gMapHeader.regionMapSectionId, MPS_TYPE_FOREST);
#else
    return FALSE;
#endif
}

void MapPreview_SetFlag(u16 flagId)
{
    if (!FlagGet(flagId))
    {
        sHasVisitedMapBefore = TRUE;
    }
    else
    {
        sHasVisitedMapBefore = FALSE;
    }
    FlagSet(flagId);
}