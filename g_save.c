/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * The savegame system. Unused by the CTF game but nevertheless called
 * during game initialization. Therefor no new savegame code ist 
 * imported.
 *
 * =======================================================================
 */

#include "local.h"

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif

field_t fields[] = { { "classname", FOFS(classname), F_LSTRING }, { "origin",
        FOFS(s.origin), F_VECTOR }, { "model", FOFS(model), F_LSTRING }, {
        "spawnflags", FOFS(spawnflags), F_INT },
        { "speed", FOFS(speed), F_FLOAT }, { "accel", FOFS(accel), F_FLOAT }, {
                "decel", FOFS(decel), F_FLOAT }, { "target", FOFS(target),
                F_LSTRING }, { "targetname", FOFS(targetname), F_LSTRING }, {
                "pathtarget", FOFS(pathtarget), F_LSTRING }, { "deathtarget",
                FOFS(deathtarget), F_LSTRING }, { "killtarget", FOFS(
                killtarget), F_LSTRING }, { "combattarget", FOFS(combattarget),
                F_LSTRING }, { "message", FOFS(message), F_LSTRING }, { "team",
                FOFS(team), F_LSTRING }, { "wait", FOFS(wait), F_FLOAT }, {
                "delay", FOFS(delay), F_FLOAT }, { "random", FOFS(random),
                F_FLOAT }, { "move_origin", FOFS(move_origin), F_VECTOR }, {
                "move_angles", FOFS(move_angles), F_VECTOR }, { "style", FOFS(
                style), F_INT }, { "count", FOFS(count), F_INT }, { "health",
                FOFS(health), F_INT }, { "sounds", FOFS(sounds), F_INT }, {
                "light", 0, F_IGNORE }, { "dmg", FOFS(dmg), F_INT }, { "angles",
                FOFS(s.angles), F_VECTOR }, { "angle", FOFS(s.angles),
                F_ANGLEHACK }, { "mass", FOFS(mass), F_INT }, { "volume", FOFS(
                volume), F_FLOAT },
        { "attenuation", FOFS(attenuation), F_FLOAT }, { "map", FOFS(map),
                F_LSTRING },

        /* temp spawn vars -- only valid when the spawn function is called */
        { "lip", STOFS(lip), F_INT, FFL_SPAWNTEMP }, { "distance", STOFS(
                distance), F_INT, FFL_SPAWNTEMP }, { "height", STOFS(height),
                F_INT, FFL_SPAWNTEMP }, { "noise", STOFS(noise), F_LSTRING,
                FFL_SPAWNTEMP }, { "pausetime", STOFS(pausetime), F_FLOAT,
                FFL_SPAWNTEMP },
        { "item", STOFS(item), F_LSTRING, FFL_SPAWNTEMP }, { "gravity", STOFS(
                gravity), F_LSTRING, FFL_SPAWNTEMP }, { "sky", STOFS(sky),
                F_LSTRING, FFL_SPAWNTEMP }, { "skyrotate", STOFS(skyrotate),
                F_FLOAT, FFL_SPAWNTEMP }, { "skyaxis", STOFS(skyaxis), F_VECTOR,
                FFL_SPAWNTEMP }, { "minyaw", STOFS(minyaw), F_FLOAT,
                FFL_SPAWNTEMP }, { "maxyaw", STOFS(maxyaw), F_FLOAT,
                FFL_SPAWNTEMP }, { "minpitch", STOFS(minpitch), F_FLOAT,
                FFL_SPAWNTEMP }, { "maxpitch", STOFS(maxpitch), F_FLOAT,
                FFL_SPAWNTEMP }, { "nextmap", STOFS(nextmap), F_LSTRING,
                FFL_SPAWNTEMP } };

field_t savefields[] = { { "", FOFS(classname), F_LSTRING }, { "", FOFS(target),
        F_LSTRING }, { "", FOFS(targetname), F_LSTRING }, { "", FOFS(
        killtarget), F_LSTRING }, { "", FOFS(team), F_LSTRING }, { "", FOFS(
        pathtarget), F_LSTRING }, { "", FOFS(deathtarget), F_LSTRING }, { "",
        FOFS(combattarget), F_LSTRING }, { "", FOFS(model), F_LSTRING }, { "",
        FOFS(map), F_LSTRING }, { "", FOFS(message), F_LSTRING },

{ "", FOFS(client), F_CLIENT }, { "", FOFS(item), F_ITEM },

{ "", FOFS(goalentity), F_EDICT }, { "", FOFS(movetarget), F_EDICT }, { "",
        FOFS(enemy), F_EDICT }, { "", FOFS(oldenemy), F_EDICT }, { "", FOFS(
        activator), F_EDICT }, { "", FOFS(groundentity), F_EDICT }, { "", FOFS(
        teamchain), F_EDICT }, { "", FOFS(teammaster), F_EDICT }, { "", FOFS(
        owner), F_EDICT }, { "", FOFS(mynoise), F_EDICT }, { "", FOFS(mynoise2),
        F_EDICT }, { "", FOFS(target_ent), F_EDICT },
        { "", FOFS(chain), F_EDICT },

        { NULL, 0, F_INT } };

field_t levelfields[] = { { "", LLOFS(changemap), F_LSTRING },

{ "", LLOFS(sight_client), F_EDICT }, { "", LLOFS(sight_entity), F_EDICT }, {
        "", LLOFS(sound_entity), F_EDICT },
        { "", LLOFS(sound2_entity), F_EDICT },

        { NULL, 0, F_INT } };

field_t clientfields[] = { { "", CLOFS(pers.weapon), F_ITEM }, { "", CLOFS(
        pers.lastweapon), F_ITEM }, { "", CLOFS(newweapon), F_ITEM },

{ NULL, 0, F_INT } };



/* ========================================================= */

void WriteField1(FILE *f, field_t *field, byte *base) {
    void *p;
    int len;
    int index;

    p = (void*) (base + field->ofs);

    switch (field->type) {
    case F_INT:
    case F_FLOAT:
    case F_ANGLEHACK:
    case F_VECTOR:
    case F_IGNORE:
        break;

    case F_LSTRING:
    case F_GSTRING:

        if (*(char**) p) {
            len = strlen(*(char**) p) + 1;
        } else {
            len = 0;
        }

        *(int*) p = len;
        break;
    case F_EDICT:

        if (*(edict_t**) p == NULL) {
            index = -1;
        } else {
            index = *(edict_t**) p - g_edicts;
        }

        *(int*) p = index;
        break;
    case F_CLIENT:

        if (*(gclient_t**) p == NULL) {
            index = -1;
        } else {
            index = *(gclient_t**) p - game.clients;
        }

        *(int*) p = index;
        break;
    case F_ITEM:

        if (*(edict_t**) p == NULL) {
            index = -1;
        } else {
            index = *(gitem_t**) p - itemlist;
        }

        *(int*) p = index;
        break;

    default:
        gi.error("WriteEdict: unknown field type");
    }
}

void WriteField2(FILE *f, field_t *field, byte *base) {
    int len;
    void *p;

    p = (void*) (base + field->ofs);

    switch (field->type) {
    case F_LSTRING:
    case F_GSTRING:

        if (*(char**) p) {
            len = strlen(*(char**) p) + 1;
            fwrite(*(char**) p, len, 1, f);
        }

        break;
    default:
        break;
    }
}

void ReadField(FILE *f, field_t *field, byte *base) {
    void *p;
    int len;
    int index;

    p = (void*) (base + field->ofs);

    switch (field->type) {
    case F_INT:
    case F_FLOAT:
    case F_ANGLEHACK:
    case F_VECTOR:
    case F_IGNORE:
        break;

    case F_LSTRING:
        len = *(int*) p;

        if (!len) {
            *(char**) p = NULL;
        } else {
            *(char**) p = gi.TagMalloc(len, TAG_LEVEL);
            fread(*(char**) p, len, 1, f);
        }

        break;
    case F_GSTRING:
        len = *(int*) p;

        if (!len) {
            *(char**) p = NULL;
        } else {
            *(char**) p = gi.TagMalloc(len, TAG_GAME);
            fread(*(char**) p, len, 1, f);
        }

        break;
    case F_EDICT:
        index = *(int*) p;

        if (index == -1) {
            *(edict_t**) p = NULL;
        } else {
            *(edict_t**) p = &g_edicts[index];
        }

        break;
    case F_CLIENT:
        index = *(int*) p;

        if (index == -1) {
            *(gclient_t**) p = NULL;
        } else {
            *(gclient_t**) p = &game.clients[index];
        }

        break;
    case F_ITEM:
        index = *(int*) p;

        if (index == -1) {
            *(gitem_t**) p = NULL;
        } else {
            *(gitem_t**) p = &itemlist[index];
        }

        break;

    default:
        gi.error("ReadEdict: unknown field type");
    }
}

/* ========================================================= */

/*
 * All pointer variables (except function 
 * pointers) must be handled specially.
 */
void WriteClient(FILE *f, gclient_t *client) {
    field_t *field;
    gclient_t temp;

    /* all of the ints, floats, and vectors stay as they are */
    temp = *client;

    /* change the pointers to lengths or indexes */
    for (field = clientfields; field->name; field++) {
        WriteField1(f, field, (byte*) &temp);
    }

    /* write the block */
    fwrite(&temp, sizeof(temp), 1, f);

    /* now write any allocated data following the edict */
    for (field = clientfields; field->name; field++) {
        WriteField2(f, field, (byte*) client);
    }
}

/*
 * All pointer variables (except function
 * pointers) must be handled specially.
 */
void ReadClient(FILE *f, gclient_t *client) {
    field_t *field;

    fread(client, sizeof(*client), 1, f);

    for (field = clientfields; field->name; field++) {
        ReadField(f, field, (byte*) client);
    }
}

/*
 * This will be called whenever the game goes to a new level,
 * and when the user explicitly saves the game.
 *
 * Game information include cross level data, like multi level
 * triggers, help computer info, and all client states.
 *
 * A single player death will automatically restore from the
 * last save position.
 */
void WriteGame(char *filename, qboolean autosave) {
    FILE *f;
    int i;
    char str[16];

    if (!autosave) {
        SaveClientData();
    }

    f = fopen(filename, "wb");

    if (!f) {
        gi.error("Couldn't open %s", filename);
    }

    memset(str, 0, sizeof(str));
    strcpy(str, BUILD_DATE);
    fwrite(str, sizeof(str), 1, f);

    game.autosaved = autosave;
    fwrite(&game, sizeof(game), 1, f);
    game.autosaved = false;

    for (i = 0; i < game.maxclients; i++) {
        WriteClient(f, &game.clients[i]);
    }

    fclose(f);
}

void ReadGame(char *filename) {
    FILE *f;
    int i;
    char str[16];

    gi.FreeTags(TAG_GAME);

    f = fopen(filename, "rb");

    if (!f) {
        gi.error("Couldn't open %s", filename);
    }

    fread(str, sizeof(str), 1, f);

    if (strcmp(str, BUILD_DATE)) {
        fclose(f);
        gi.error("Savegame from an older version.\n");
    }

    g_edicts = gi.TagMalloc(game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
    globals.edicts = g_edicts;

    fread(&game, sizeof(game), 1, f);
    game.clients = gi.TagMalloc(game.maxclients * sizeof(game.clients[0]),
            TAG_GAME);

    for (i = 0; i < game.maxclients; i++) {
        ReadClient(f, &game.clients[i]);
    }

    fclose(f);
}

/* ========================================================== */

/*
 * All pointer variables (except function 
 * pointers) must be handled specially.
 */
void WriteEdict(FILE *f, edict_t *ent) {
    field_t *field;
    edict_t temp;

    /* all of the ints, floats, and vectors stay as they are */
    temp = *ent;

    /* change the pointers to lengths or indexes */
    for (field = savefields; field->name; field++) {
        WriteField1(f, field, (byte*) &temp);
    }

    /* write the block */
    fwrite(&temp, sizeof(temp), 1, f);

    /* now write any allocated data following the edict */
    for (field = savefields; field->name; field++) {
        WriteField2(f, field, (byte*) ent);
    }
}

/*
 * All pointer variables (except function
 * pointers) must be handled specially.
 */
void WriteLevelLocals(FILE *f) {
    field_t *field;
    level_locals_t temp;

    /* all of the ints, floats, and vectors stay as they are */
    temp = level;

    /* change the pointers to lengths or indexes */
    for (field = levelfields; field->name; field++) {
        WriteField1(f, field, (byte*) &temp);
    }

    /* write the block */
    fwrite(&temp, sizeof(temp), 1, f);

    /* now write any allocated data following the edict */
    for (field = levelfields; field->name; field++) {
        WriteField2(f, field, (byte*) &level);
    }
}

/*
 * All pointer variables (except function
 * pointers) must be handled specially.
 */
void ReadEdict(FILE *f, edict_t *ent) {
    field_t *field;

    fread(ent, sizeof(*ent), 1, f);

    for (field = savefields; field->name; field++) {
        ReadField(f, field, (byte*) ent);
    }
}

/*
 * All pointer variables (except function
 * pointers) must be handled specially.
 */
void ReadLevelLocals(FILE *f) {
    field_t *field;

    fread(&level, sizeof(level), 1, f);

    for (field = levelfields; field->name; field++) {
        ReadField(f, field, (byte*) &level);
    }
}

void WriteLevel(char *filename) {
    int i;
    edict_t *ent;
    FILE *f;
    void *base;

    f = fopen(filename, "wb");

    if (!f) {
        gi.error("Couldn't open %s", filename);
    }

    /* write out edict size for checking */
    i = sizeof(edict_t);
    fwrite(&i, sizeof(i), 1, f);

    /* write out a function pointer for checking */
    base = (void*) InitGame;
    fwrite(&base, sizeof(base), 1, f);

    /* write out level_locals_t */
    WriteLevelLocals(f);

    /* write out all the entities */
    for (i = 0; i < globals.num_edicts; i++) {
        ent = &g_edicts[i];

        if (!ent->inuse) {
            continue;
        }

        fwrite(&i, sizeof(i), 1, f);
        WriteEdict(f, ent);
    }

    i = -1;
    fwrite(&i, sizeof(i), 1, f);

    fclose(f);
}

/*
 * SpawnEntities will allready have been called on the
 * level the same way it was when the level was saved.
 *
 * That is necessary to get the baselines
 * set up identically.
 *
 * The server will have cleared all of the world links before
 * calling ReadLevel.
 *
 * No clients are connected yet.
 */
void ReadLevel(char *filename) {
    int entnum;
    FILE *f;
    int i;
    void *base;
    edict_t *ent;

    f = fopen(filename, "rb");

    if (!f) {
        gi.error("Couldn't open %s", filename);
    }

    /* free any dynamic memory allocated by
     loading the level base state */
    gi.FreeTags(TAG_LEVEL);

    /* wipe all the entities */
    memset(g_edicts, 0, game.maxentities * sizeof(g_edicts[0]));
    globals.num_edicts = maxclients->value + 1;

    /* check edict size */
    fread(&i, sizeof(i), 1, f);

    if (i != sizeof(edict_t)) {
        fclose(f);
        gi.error("ReadLevel: mismatched edict size");
    }

    /* check function pointer base address */
    fread(&base, sizeof(base), 1, f);

    if (base != (void*) InitGame) {
        fclose(f);
        gi.error("ReadLevel: function pointers have moved");
    }

    /* load the level locals */
    ReadLevelLocals(f);

    /* load all the entities */
    while (1) {
        if (fread(&entnum, sizeof(entnum), 1, f) != 1) {
            fclose(f);
            gi.error("ReadLevel: failed to read entnum");
        }

        if (entnum == -1) {
            break;
        }

        if (entnum >= globals.num_edicts) {
            globals.num_edicts = entnum + 1;
        }

        ent = &g_edicts[entnum];
        ReadEdict(f, ent);

        /* let the server rebuild world links for this ent */
        memset(&ent->area, 0, sizeof(ent->area));
        gi.linkentity(ent);
    }

    fclose(f);

    /* mark all clients as unconnected */
    for (i = 0; i < maxclients->value; i++) {
        ent = &g_edicts[i + 1];
        ent->client = game.clients + i;
        ent->client->pers.connected = false;
    }

    /* do any load time things at this point */
    for (i = 0; i < globals.num_edicts; i++) {
        ent = &g_edicts[i];

        if (!ent->inuse) {
            continue;
        }

        /* fire any cross-level triggers */
        if (ent->classname) {
            if (strcmp(ent->classname, "target_crosslevel_target") == 0) {
                ent->nextthink = level.time + ent->delay;
            }
        }
    }
}

