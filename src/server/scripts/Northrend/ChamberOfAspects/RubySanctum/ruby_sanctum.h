/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RUBY_SANCTUM_H_
#define RUBY_SANCTUM_H_

#include "SpellScript.h"
#include "Map.h"
#include "Creature.h"

#define RSScriptName "instance_ruby_sanctum"
uint32 const EncounterCount = 4;

Position const HalionControllerSpawnPos = {3156.037f, 533.2656f, 72.97205f, 0.0f};

enum DataTypes
{
    // Encounter States/Boss GUIDs
    DATA_BALTHARUS_THE_WARBORN              = 0,
    DATA_GENERAL_ZARITHRIAN                 = 1,
    DATA_SAVIANA_RAGEFIRE                   = 2,
    DATA_HALION                             = 3,

    // Etc
    DATA_XERESTRASZA                        = 4,
    DATA_CRYSTAL_CHANNEL_TARGET             = 5,
    DATA_BALTHARUS_SHARED_HEALTH            = 6,
    DATA_ZARITHIAN_SPAWN_STALKER_1          = 7,
    DATA_ZARITHIAN_SPAWN_STALKER_2          = 8,
    DATA_HALION_CONTROLLER                  = 9,
    DATA_BURNING_TREE_1                     = 10,
    DATA_BURNING_TREE_2                     = 11,
    DATA_BURNING_TREE_3                     = 12,
    DATA_BURNING_TREE_4                     = 13,
    DATA_FLAME_RING                         = 14,
};

#ifndef DEF_RUBY_SANCTUM_H
#define DEF_RUBY_SANCTUM_H

enum
{
    MAX_ENCOUNTERS                 = 6,

    TYPE_EVENT                     = 0,
    TYPE_RAGEFIRE                  = 1,
    TYPE_BALTHARUS                 = 2,
    TYPE_XERESTRASZA               = 3,
    TYPE_ZARITHRIAN                = 4,
    TYPE_HALION                    = 5,

    TYPE_COUNTER                   = 6, // for WorldUpdateState
    TYPE_HALION_EVENT              = 7,

    TYPE_EVENT_TIMER               = 50,
    TYPE_EVENT_NPC                 = 51,

    NPC_HALION_REAL                = 39863, // Halion Physical Realm NPC
    NPC_HALION_TWILIGHT            = 40142, // Halion Twilight Realm NPC
    NPC_HALION_CONTROL             = 40146,

    NPC_BALTHARUS                  = 39751,
    NPC_CLONE                      = 39899,
    NPC_ZARITHIAN                  = 39746,
    NPC_RAGEFIRE                   = 39747,

    NPC_XERESTRASZA                = 40429,

    NPC_BALTHARUS_TARGET           = 26712,
    NPC_ZARITHIAN_SPAWN_STALKER    = 39794,

	NPC_CHARSCALE_ASSAULTER        = 40419,
	NPC_CHARSCALE_COMMANDER_1      = 93629,
	NPC_CHARSCALE_COMMANDER_2      = 93630,
	NPC_CHARSCALE_ELITE            = 40421,
	NPC_CHARSCALE_INVOKER          = 40417,
	NPC_ZARITHRIAN_STALKER_1       = 397941,
	NPC_ZARITHRIAN_STALKER_2       = 397942,

    // Orb rotation
    NPC_SHADOW_PULSAR_N            = 40083, //spinning orb N spawn
    NPC_SHADOW_PULSAR_S            = 40100, //spinning orb S spawn
    NPC_ORB_CARRIER                = 40081,
    NPC_ORB_ROTATION_FOCUS         = 40091,


    GO_HALION_PORTAL_1             = 202794, //1327 ENTRY
    GO_HALION_PORTAL_2             = 202795, //1327 ENTRY
    GO_HALION_PORTAL_3             = 202796, //1327 EXIT

    GO_FIRE_FIELD                  = 203005,
    GO_FLAME_WALLS                 = 203006,
    GO_FLAME_RING                  = 203007,

    DATA_EVENT_TIMER               = 101,
    DATA_EVENT                     = 102,

    DATA_ORB_DIRECTION             = 110,
    DATA_ORB_S                     = 111,
    DATA_ORB_N                     = 112,

};

enum uiWorldStates
{
    UPDATE_STATE_UI_COUNT_R          = 5049,
    UPDATE_STATE_UI_COUNT_T          = 5050,
    UPDATE_STATE_UI_SHOW             = 5051,
};

#endif
