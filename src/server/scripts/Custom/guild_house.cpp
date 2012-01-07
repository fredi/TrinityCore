#include "ScriptPCH.h"
#include "GuildMgr.h"

class guild_house_commandscript : public CommandScript
{
public:
    guild_house_commandscript() : CommandScript("guild_house_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand guildHouseNpcCommandTable[] =
        {
            { "set",            SEC_GAMEMASTER,  false, &HandleNpcSetCommand,           "", NULL },
            { "remove",         SEC_GAMEMASTER,  false, &HandleNpcRemoveCommand,        "", NULL },
            { NULL,             0,               false, NULL,                           "", NULL }
        };
        static ChatCommand guildHouseCommandTable[] =
        {
            { "set",            SEC_GAMEMASTER,  false, &HandleSetCommand,              "", NULL },
            { "remove",         SEC_GAMEMASTER,  false, &HandleRemoveCommand,           "", NULL },
            { "appear",         SEC_GAMEMASTER,  false, &HandleAppearCommand,           "", NULL },
            { "list",           SEC_GAMEMASTER,  false, &HandleListCommand,             "", NULL },
            { "npc",            SEC_GAMEMASTER,  false, NULL,      "", guildHouseNpcCommandTable },
            { NULL,             0,               false, NULL,                           "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "guildhouse",     SEC_GAMEMASTER,  false, NULL,         "", guildHouseCommandTable },
            { NULL,             0,               false, NULL,                           "", NULL }
        };
        return commandTable;
    }

    static bool HandleNpcSetCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Creature* creature = handler->getSelectedCreature();
        if (!creature)
        {
            handler->SendSysMessage("No creature selected.");
            return true;
        }

        QueryResult result = WorldDatabase.PQuery("SELECT guild FROM creature_guild WHERE guid = %u", creature->GetGUIDLow());
        if (result)
        {
            handler->SendSysMessage("This creature already has a guild associated.");
            return true;
        }

        std::string guildName = args;
        Guild* guild = sGuildMgr->GetGuildByName(guildName);
        if (!guild)
        {
            handler->PSendSysMessage("Guild %s not found!", guildName.c_str());
            return true;
        }

        WorldDatabase.PExecute("INSERT INTO creature_guild (guid, guild) VALUES (%u, %u)", creature->GetGUIDLow(), guild->GetId());

        handler->PSendSysMessage("Creature associated to guild %s!", guildName.c_str());
        return true;
    }

    static bool HandleNpcRemoveCommand(ChatHandler* handler, char const* /*args*/)
    {
        Creature* creature = handler->getSelectedCreature();
        if (!creature)
        {
            handler->SendSysMessage("No creature selected.");
            return true;
        }

        QueryResult result = WorldDatabase.PQuery("SELECT guild FROM creature_guild WHERE guid = %u", creature->GetGUIDLow());
        if (!result)
        {
            handler->SendSysMessage("This creature doesn't have a guild associated.");
            return true;
        }

        WorldDatabase.PExecute("DELETE FROM creature_guild WHERE guid = %u", creature->GetGUIDLow());

        handler->SendSysMessage("Creature was removed from guild!");
        return true;
    }

    static bool HandleSetCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string guildName = args;
        Guild* guild = sGuildMgr->GetGuildByName(guildName);
        if (!guild)
        {
            handler->PSendSysMessage("Guild %s not found!", guildName.c_str());
            return true;
        }

        QueryResult result = WorldDatabase.PQuery("SELECT gm, date FROM guild_house WHERE guild = %u", guild->GetId());
        if (result)
        {
            handler->PSendSysMessage("Guild %s has a house set by %s on %s.", guildName.c_str(), (*result)[0].GetCString(), (*result)[1].GetCString());
            return true;
        }

        Player* player = handler->GetSession()->GetPlayer();
        uint32 mapId = player->GetMapId();
        uint32 zoneId = player->GetZoneId();
        float posX = player->GetPositionX();
        float posY = player->GetPositionY();
        float posZ = player->GetPositionZ();

        WorldDatabase.PExecute("INSERT INTO guild_house (guild, mapId, zoneId, posX, posY, posZ, gm) VALUES "
            "(%u, %u, %u, %f, %f, %f, '%s')", guild->GetId(), mapId, zoneId, posX, posY, posZ, player->GetName());

        handler->PSendSysMessage("Guild house set to guild %s (ID: %u).", guildName.c_str(), guild->GetId());

        return true;
    }

    static bool HandleRemoveCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string guildName = args;
        Guild* guild = sGuildMgr->GetGuildByName(guildName);
        if (!guild)
        {
            handler->PSendSysMessage("Guild %s not found!", guildName.c_str());
            return true;
        }

        QueryResult result = WorldDatabase.PQuery("SELECT mapId, posX, posY, posZ FROM guild_house WHERE guild = %u", guild->GetId());
        if (!result)
        {
            handler->PSendSysMessage("Guild %s doesn't have a house.", guildName.c_str());
            return true;
        }

        WorldDatabase.PExecute("DELETE FROM guild_house WHERE guild = %u", guild->GetId());
        WorldDatabase.PExecute("DELETE FROM creature_guild WHERE guild = %u", guild->GetId());

        handler->PSendSysMessage("The house of guild %s was removed along with its NPC links.", guildName.c_str());
        return true;
    }

    static bool HandleAppearCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string guildName = args;
        Guild* guild = sGuildMgr->GetGuildByName(guildName);
        if (!guild)
        {
            handler->PSendSysMessage("Guild %s not found!", guildName.c_str());
            return true;
        }

        QueryResult result = WorldDatabase.PQuery("SELECT mapId, posX, posY, posZ FROM guild_house WHERE guild = %u", guild->GetId());
        if (!result)
        {
            handler->PSendSysMessage("Guild %s doesn't have a house.", guildName.c_str());
            return true;
        }

        Field* fields = result->Fetch();
        uint32 mapId = fields[0].GetUInt32();
        float posX = fields[1].GetFloat();
        float posY = fields[2].GetFloat();
        float posZ = fields[3].GetFloat();

        Player* player = handler->GetSession()->GetPlayer();
        player->TeleportTo(mapId, posX, posY, posZ, player->GetOrientation());

        return true;
    }

    static bool HandleListCommand(ChatHandler* handler, char const* /*args*/)
    {
        QueryResult result = WorldDatabase.Query("SELECT guild, gm, date FROM guild_house");
        if (!result)
        {
            handler->SendSysMessage("No guild has a house.");
            return true;
        }

        do
        {
            Field* fields = result->Fetch();
            Guild* guild = sGuildMgr->GetGuildById(fields[0].GetUInt32());
            if (guild)
                handler->PSendSysMessage("Guild %s set by %s on %s", guild->GetName().c_str(), fields[1].GetCString(), fields[2].GetCString());
        } while (result->NextRow());

        return true;
    }
};

class npc_guild_house : public CreatureScript
{
public:
    npc_guild_house() : CreatureScript("npc_guild_house") {}

    struct npc_guild_houseAI : public Scripted_NoMovementAI
    {
        npc_guild_houseAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_NORMAL, true);
            creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);

            QueryResult result = WorldDatabase.PQuery("SELECT guild FROM creature_guild WHERE guid = %u", creature->GetGUIDLow());
            _guildId = result ? (*result)[0].GetUInt32() : 0;
        }

        void Reset() {}
        void EnterCombat(Unit* /*who*/) {}
        void AttackStart(Unit* /*who*/) {}
        void UpdateAI(uint32 const /*diff*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || !who->IsInWorld())
                return;

            if (!me->IsWithinDist(who, 65.0f, false))
                return;

            Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();

            if (!player || player->IsBeingTeleported())
                return;

            if (!_guildId && player->isGameMaster())
            {
                QueryResult result = WorldDatabase.PQuery("SELECT guild FROM creature_guild WHERE guid = %u", me->GetGUIDLow());
                _guildId = result ? (*result)[0].GetUInt32() : 0;
            }

            if (_guildId && player->GetGuildId() != _guildId && player->m_positionZ > 180.0f && !player->isGameMaster())
                player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());

            if (QueryResult result = WorldDatabase.PQuery("SELECT posZ FROM guild_house WHERE guild = %u", _guildId))
            {
                if (player->m_positionZ > (*result)[0].GetFloat() - 10.0f)
                {
                    player->CastSpell(player, 12438, true);
                    return;
                }
            }

            me->SetOrientation(me->GetHomePosition().GetOrientation());
            return;
        }

    private:
        uint32 _guildId;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_guild_houseAI(creature);
    }
};

class npc_guild_house_vendor : public CreatureScript
{
public:
    npc_guild_house_vendor() : CreatureScript("npc_guild_house_vendor") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        QueryResult result = WorldDatabase.PQuery("SELECT guild FROM creature_guild WHERE guid = %u", creature->GetGUIDLow());
        if (!result)
            return true;

        if (player->GetGuildId() != (*result)[0].GetUInt32() && !player->isGameMaster())
        {
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
            return true;
        }

        if (creature->isVendor())
        {
            player->CLOSE_GOSSIP_MENU();
            player->GetSession()->SendListInventory(creature->GetGUID());
        }

        return true;
    }
};

class npc_guild_house_services : public CreatureScript
{
public:
    npc_guild_house_services() : CreatureScript("npc_guild_house_services") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        QueryResult result = WorldDatabase.PQuery("SELECT guild FROM creature_guild WHERE guid = %u", creature->GetGUIDLow());
        if (!result)
            return true;

        if (player->GetGuildId() != (*result)[0].GetUInt32() && !player->isGameMaster())
        {
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
            return true;
        }

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Heal",   GOSSIP_SENDER_MAIN, 1201);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK,   "Buffs" , GOSSIP_SENDER_MAIN, 1202);

        if (creature->isVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch(action)
        {
            case 1201: // Heal
                if (player->HasAura(45523))
                {
                    player->CLOSE_GOSSIP_MENU();
                    creature->MonsterWhisper("Desculpe $N, n�o posso fazer isto nesse momento.", player->GetGUID(), false);
                }
                else
                {
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, 39321, true);
                    player->CastSpell(player, 45523, true);
                }
                break;
            case 1202: // Buffs
                player->ADD_GOSSIP_ITEM(5, "Power Word: Fortitude, Rank 8", GOSSIP_SENDER_MAIN, 4000);
                player->ADD_GOSSIP_ITEM(5, "Greater Blessing of Kings", GOSSIP_SENDER_MAIN, 4001);
                player->ADD_GOSSIP_ITEM(5, "Greater Bleesing of Mights", GOSSIP_SENDER_MAIN,4002);
                player->ADD_GOSSIP_ITEM(5, "Greater Blessing of Wisdom", GOSSIP_SENDER_MAIN, 4003);
                player->ADD_GOSSIP_ITEM(5, "Mark of the Wild, Rank 9", GOSSIP_SENDER_MAIN, 4004);
                player->ADD_GOSSIP_ITEM(5, "Arcane Intellect, Rank 7", GOSSIP_SENDER_MAIN, 4005);
                player->ADD_GOSSIP_ITEM(5, "Thorns, Rank 8", GOSSIP_SENDER_MAIN, 4007);
                player->ADD_GOSSIP_ITEM(5, "Divine Spirit, Rank 8", GOSSIP_SENDER_MAIN, 4008);
                player->ADD_GOSSIP_ITEM(5, "Shadow Protection, Rank 5", GOSSIP_SENDER_MAIN, 4009);

                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

                break;
            case 4000:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 48161, true);
                break;
            case 4001:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 25898, true);
                break;
            case 4002:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 48934, true);
                break;
            case 4003:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 48938, true);
                break;
            case 4004:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 48469, true);
                break;
            case 4005:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 42995, true);
                break;
            case 4007:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 53307, true);
                break;
            case 4008:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 48073, true);
                break;
            case 4009:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 48169, true);
                break;
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
        }

        return true;
    }
};

void AddSC_guild_house()
{
    new guild_house_commandscript();
    new npc_guild_house();
    new npc_guild_house_vendor();
    new npc_guild_house_services();
}