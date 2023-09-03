/**
 *
 */
#include "local.h"

cvar_t *ctf;
cvar_t *ctf_forcejoin;
cvar_t *competition;
cvar_t *matchlock;
cvar_t *electpercentage;
cvar_t *matchtime;
cvar_t *matchsetuptime;
cvar_t *matchstarttime;
cvar_t *admin_password;
cvar_t *allow_admin;
cvar_t *warp_list;
cvar_t *warn_unbalanced;

/*
 * Returns a pointer to the structure with
 * all entry points and global variables
 */
q_exported game_export_t* GetGameAPI(game_import_t *import) {
    gi = *import;

    globals.apiversion = GAME_API_VERSION;
    globals.Init = InitGame;
    globals.Shutdown = ShutdownGame;
    globals.SpawnEntities = SpawnEntities;

    globals.WriteGame = WriteGame;
    globals.ReadGame = ReadGame;
    globals.WriteLevel = WriteLevel;
    globals.ReadLevel = ReadLevel;

    globals.ClientThink = ClientThink;
    globals.ClientConnect = ClientConnect;
    globals.ClientUserinfoChanged = ClientUserinfoChanged;
    globals.ClientDisconnect = ClientDisconnect;
    globals.ClientBegin = ClientBegin;
    globals.ClientCommand = ClientCommand;

    globals.RunFrame = G_RunFrame;

    globals.ServerCommand = ServerCommand;

    globals.edict_size = sizeof(edict_t);

    return &globals;
}

/*
 * This will be called when the dll is first loaded, which
 * only happens when a new game is started or a save game
 * is loaded.
 */
void InitGame(void) {
    gi.dprintf("Game is starting up.\n");
    gi.dprintf("Game is ctf built on %s.\n", GAMEVERSION, BUILD_DATE);

    gun_x = gi.cvar("gun_x", "0", 0);
    gun_y = gi.cvar("gun_y", "0", 0);
    gun_z = gi.cvar("gun_z", "0", 0);
    sv_rollspeed = gi.cvar("sv_rollspeed", "200", 0);
    sv_rollangle = gi.cvar("sv_rollangle", "2", 0);
    sv_maxvelocity = gi.cvar("sv_maxvelocity", "2000", 0);
    sv_gravity = gi.cvar("sv_gravity", "800", 0);

    /* noset vars */
    dedicated = gi.cvar("dedicated", "0", CVAR_NOSET);

    /* latched vars */
    sv_cheats = gi.cvar("cheats", "0", CVAR_SERVERINFO | CVAR_LATCH);
    gi.cvar("gamename", GAMEVERSION, CVAR_SERVERINFO | CVAR_LATCH);
    gi.cvar("gamedate", BUILD_DATE, CVAR_SERVERINFO | CVAR_LATCH);
    maxclients = gi.cvar("maxclients", "4", CVAR_SERVERINFO | CVAR_LATCH);
    deathmatch = gi.cvar("deathmatch", "0", CVAR_LATCH);
    coop = gi.cvar("coop", "0", CVAR_LATCH);
    skill = gi.cvar("skill", "1", CVAR_LATCH);
    maxentities = gi.cvar("maxentities", "1024", CVAR_LATCH);

    /* This game.dll only supports deathmatch */
    if (!deathmatch->value) {
        gi.dprintf("Forcing deathmatch.\n");
        gi.cvar_set("deathmatch", "1");
    }

    /* force coop off */
    if (coop->value) {
        gi.cvar_set("coop", "0");
    }

    /* change anytime vars */
    dmflags = gi.cvar("dmflags", "0", CVAR_SERVERINFO);
    fraglimit = gi.cvar("fraglimit", "0", CVAR_SERVERINFO);
    timelimit = gi.cvar("timelimit", "0", CVAR_SERVERINFO);
    capturelimit = gi.cvar("capturelimit", "0", CVAR_SERVERINFO);
    instantweap = gi.cvar("instantweap", "0", CVAR_SERVERINFO);
    password = gi.cvar("password", "", CVAR_USERINFO);
    filterban = gi.cvar("filterban", "1", 0);
    g_select_empty = gi.cvar("g_select_empty", "0", CVAR_ARCHIVE);
    run_pitch = gi.cvar("run_pitch", "0.002", 0);
    run_roll = gi.cvar("run_roll", "0.005", 0);
    bob_up = gi.cvar("bob_up", "0.005", 0);
    bob_pitch = gi.cvar("bob_pitch", "0.002", 0);
    bob_roll = gi.cvar("bob_roll", "0.002", 0);

    /* flood control */
    flood_msgs = gi.cvar("flood_msgs", "4", 0);
    flood_persecond = gi.cvar("flood_persecond", "4", 0);
    flood_waitdelay = gi.cvar("flood_waitdelay", "10", 0);

    /* dm map list */
    sv_maplist = gi.cvar("sv_maplist", "", 0);

    /* others */
    aimfix = gi.cvar("aimfix", "0", CVAR_ARCHIVE);

    /* items */
    InitItems();

    Com_sprintf(game.helpmessage1, sizeof(game.helpmessage1), "");
    Com_sprintf(game.helpmessage2, sizeof(game.helpmessage2), "");

    /* initialize all entities for this game */
    game.maxentities = maxentities->value;
    g_edicts = gi.TagMalloc(game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
    globals.edicts = g_edicts;
    globals.max_edicts = game.maxentities;

    /* initialize all clients for this game */
    game.maxclients = maxclients->value;
    game.clients = gi.TagMalloc(game.maxclients * sizeof(game.clients[0]),
            TAG_GAME);
    globals.num_edicts = game.maxclients + 1;

    CTFInit();
}

void CTFInit(void) {
    ctf = gi.cvar("ctf", "1", CVAR_SERVERINFO);
    ctf_forcejoin = gi.cvar("ctf_forcejoin", "", 0);
    competition = gi.cvar("competition", "0", CVAR_SERVERINFO);
    matchlock = gi.cvar("matchlock", "1", CVAR_SERVERINFO);
    electpercentage = gi.cvar("electpercentage", "66", 0);
    matchtime = gi.cvar("matchtime", "20", CVAR_SERVERINFO);
    matchsetuptime = gi.cvar("matchsetuptime", "10", 0);
    matchstarttime = gi.cvar("matchstarttime", "20", 0);
    admin_password = gi.cvar("admin_password", "", 0);
    allow_admin = gi.cvar("allow_admin", "1", 0);
    warp_list = gi.cvar("warp_list", "q2ctf1 q2ctf2 q2ctf3 q2ctf4 q2ctf5", 0);
    warn_unbalanced = gi.cvar("warn_unbalanced", "1", 0);
}
