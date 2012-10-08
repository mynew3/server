/*
 * Copyright (C) 2005-2012 LilleCarl <http://lillecarl.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Custom.h"
#include "World.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "Chat.h"
#include "UpdateFields.h"

#include <numeric>

void Player::CreatePet(uint32 entry, bool classcheck)
{
    if (classcheck && getClass() != CLASS_HUNTER)
    {
        SendChatMessage("%s[Mr.Zoo]%s You are no hunter!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return;
    }

    CreatureInfo const *cinfo = sObjectMgr.GetCreatureTemplate(entry);
    if (!cinfo)
    {
        SendChatMessage("%s[Mr.Zoo]%s This pet seems to be removed from the database. Please report that creature %u is missing.",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,entry);
        return;
    }

    CreatureCreatePos pos(GetSession()->GetPlayer(), GetOrientation());

    Creature* pCreature = new Creature;

    // used guids from specially reserved range (can be 0 if no free values)
    uint32 lowguid = sObjectMgr.GenerateStaticCreatureLowGuid();
    if (!lowguid)
    {
        return;
    }

    if (!pCreature->Create(lowguid, pos, cinfo))
    {
        delete pCreature;
        return;
    }

    //--------------------------------------------------

    if (GetPetGuid())
        UnsummonPetTemporaryIfAny();

    Pet* pet = new Pet(HUNTER_PET);

    if(!pet->CreateBaseAtCreature(pCreature))
    {
        delete pet;
        //PlayerTalkClass->CloseGossip();
        return;
    }

    pet->SetOwnerGuid(GetObjectGuid());
    pet->SetCreatorGuid(GetObjectGuid());
    pet->setFaction(getFaction());
    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, 13481);

    if (IsPvP())
        pet->SetPvP(true);

    if (!pet->InitStatsForLevel(pCreature->getLevel()))
    {
        sLog.outError("Pet::InitStatsForLevel() failed for creature (Entry: %u)!",pCreature->GetEntry());
        delete pet;
        return;
    }

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->AIM_Initialize();
    pet->InitPetCreateSpells();
    pet->SetHealth(pet->GetMaxHealth());

    // add to world
    pet->GetMap()->Add((Creature*)pet);

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL,70);
    int x;
    for (x = 0; x < 6; x++)
    {
        pet->SetPower(POWER_HAPPINESS,66600000);
        pet->ModifyLoyalty(150000);
        pet->TickLoyaltyChange();
        pet->SetTP(10000);
    }

    // caster have pet now
    SetPet(pet);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    PetSpellInitialize();
    pet->learnSpell(27052);
    pet->learnSpell(35698);
    pet->learnSpell(25076);
    pet->learnSpell(27048);
    pet->learnSpell(27053);
    pet->learnSpell(27054);
    pet->learnSpell(27062);
    pet->learnSpell(27047);
    pet->learnSpell(24551);
    delete pCreature;
}

// Player is victim, attackers are fetched inside system.
void Player::HandlePvPKill()
{
    const uint32 uStartTime = WorldTimer::getMSTime();

    const int32 RewardGold = sWorld.getConfig(CONFIG_UINT32_PVPGOLD_BASE);
    uint32 InCombatPlayers = 0;
    uint64 MaxDmgGUID = 0;
    uint64 MaxDmgDmg  = 0;

    // Damage Code Begin
    float TotalHealth = std::accumulate(m_Damagers.begin(), m_Damagers.end(),0,pair_adder); // Summary of all damage done to victim
    for (std::map<uint64, uint32>::const_iterator itr = m_Damagers.begin(); itr != m_Damagers.end(); ++itr)
    {
        uint64  GUID    = itr->first;
        float   Damage  = itr->second;

        Player* pAttacker = sObjectMgr.GetPlayer(GUID);
        if (!pAttacker)
            continue;

        if (Damage > MaxDmgDmg)
        {
            MaxDmgGUID  = GUID;
            MaxDmgDmg   = Damage;
        }

        ++InCombatPlayers;

        if (pAttacker->HandlePvPAntifarm(this))
        {
            int32 Reward = (RewardGold * (Damage / TotalHealth)*((pAttacker->GetKillStreak()/10)+1.0f));
            if (Reward > RewardGold*3)
                Reward = RewardGold*3;
            pAttacker->ModifyMoney(+Reward);
            pAttacker->IncreaseKillStreak();

            pAttacker->SendChatMessage("%s[PvP System]%s You got awarded %g gold for damaging %s",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,Reward/10000.f,GetNameLink().c_str());

            // Damage Code End | Healing Code Begin
            float TotalHealing = std::accumulate(pAttacker->m_Healers.begin(), pAttacker->m_Healers.end(),0,pair_adder); // Summary of all healing done to attacker
            for (std::map<uint64, uint32>::const_iterator itr = pAttacker->m_Healers.begin(); itr != pAttacker->m_Healers.end(); ++itr)
            {

                uint64  GUID        = itr->first;
                float   Healing     = itr->second;
                float   MaxHealth   = pAttacker->GetMaxHealth();

                Player* pHealer = sObjectMgr.GetPlayer(GUID);
                if (!pHealer)
                    continue;

                ++InCombatPlayers;

                float maxhealingPct = (Healing/MaxHealth);
                if (maxhealingPct > 1) maxhealingPct = 1.0f;

                Reward = (Reward * ((Healing / TotalHealing)*((pHealer->GetKillStreak()/10)+1.0f)*maxhealingPct));
                if (Reward > RewardGold*3)
                    Reward = RewardGold*3;
                pHealer->ModifyMoney(+Reward);
                pHealer->IncreaseKillStreak();

                pHealer->SendChatMessage("%s[PvP System]%s You got awarded %g gold for healing %s",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,Reward/10000.f,pAttacker->GetNameLink().c_str());
            }
            // Healing Code End
        }
        else
        {
            pAttacker->SendChatMessage("%s[Antifarming System]%s You triggered out antifarming system. This action was logged.");
            sLog.outError("Player %u %s triggered antifarmsystem with %u %s",pAttacker->GetObjectGuid().GetCounter(),pAttacker->GetName(),GetObjectGuid().GetCounter(),GetName());
        }
    }

    if (Player* pMostDamager = sObjectMgr.GetPlayer(MaxDmgGUID))
    {
        pMostDamager->SendChatMessage("%s[PvP System]%s You did most damage to %s%s (%u)",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,GetNameLink().c_str(),MSG_COLOR_WHITE,MaxDmgDmg);
        SendChatMessage("%s[PvP System]%s Your main attacker was %s%s who did %u damage to you.",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,pMostDamager->GetNameLink().c_str(),MSG_COLOR_WHITE,MaxDmgDmg);
    }

    ClearKillStreak();

    if (InCombatPlayers > 1)
        SendChatMessage("%s[PvP System]%s There were %u players involved in the combat to your death.",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,InCombatPlayers);

    sLog.outDebug("Took %u MS to run PvP System",WorldTimer::getMSTimeDiff(uStartTime, WorldTimer::getMSTime()));
}

bool Player::HandlePvPAntifarm(Player* victim)
{
    if (!isGameMaster() && !victim->isGameMaster())
    {
        if (this == victim)
            return false;
        else if (victim->HasAura(2479))
            return false;
        else if (GetSession()->GetRemoteAddress() == victim->GetSession()->GetRemoteAddress())
            return false;
        else if (victim->GetSession()->GetRemoteAddress() == GetLastAttackerIP())
        {
            IncreaseAttackerLastIPCount();
            if (GetLastAttackerIPCount() >= 6)
                return false;
        }
        else if (GetSession()->GetRemoteAddress() == victim->GetLastVictimIP())
        {
            IncreaseVictimLastIPCount();
            if (victim->GetLastVictimIPCount() >= 6)
                return false;
        }
        else
        {
            ClearAttackerIP();
            victim->ClearVictimIP();
        }
    }
    SetAttackerLastIP(GetSession()->GetRemoteAddress());
    victim->SetVictimLastIP(GetSession()->GetRemoteAddress());
    return true;
}

void Player::CheckCustomVendorRequirements(VendorItem const* crItem, ItemPrototype const* pProto, bool &CanBuy)
{
    Item* pItem = Item::CreateItem(pProto->ItemId,1);
    if (pItem)
        SendChatMessage("You must type %s.togglebuy%s to buy %s",MSG_COLOR_RED,MSG_COLOR_YELLOW,pItem->GetNameLink(true).c_str());
    if (crItem->ReqArenaRating > GetMaxPersonalArenaRatingRequirement())
    {
        if (crItem->ReqArenaRating > 0 )
        {
            // probably not the proper equip err
            SendEquipError(EQUIP_ERR_CANT_EQUIP_RANK, NULL, NULL);
            SendChatMessage("You need %u arena rating to buy this item.",crItem->ReqArenaRating);
            CanBuy = false;
        }
    }
    if (crItem->ReqArenaPoints > GetArenaPoints() || !GetBuyEnabled())
    {
        if (crItem->ReqArenaPoints > 0)
        {
            SendEquipError(EQUIP_ERR_NOT_ENOUGH_ARENA_POINTS, NULL, NULL);
            SendChatMessage("You need %u arena points to buy this item.");
            CanBuy = false;
        }
    }
    if ((!HasItemCount(crItem->ReqItem1,1,false) && !HasItemCount(crItem->ReqItem2,1,false)) || !GetBuyEnabled())
    {
        Item* pItem1 = Item::CreateItem(crItem->ReqItem1,1);
        Item* pItem2 = Item::CreateItem(crItem->ReqItem2,1);
        if (pItem1 && crItem->ReqItem1 > 0 && pItem2 && crItem->ReqItem2 > 0)
        {
            SendEquipError(EQUIP_ERR_VENDOR_MISSING_TURNINS,NULL,NULL,NULL);
            SendChatMessage("You need %s%s or %s%s to buy this item.",pItem1->GetNameLink(true).c_str(),MSG_COLOR_YELLOW,pItem2->GetNameLink(true).c_str(),MSG_COLOR_YELLOW);
            CanBuy = false;
        }
        else if (pItem1 && crItem->ReqItem1 > 0)
        {
            SendEquipError(EQUIP_ERR_VENDOR_MISSING_TURNINS,NULL,NULL,NULL);
            SendChatMessage("You need %s%s to buy this item.",pItem1->GetNameLink(true).c_str(),MSG_COLOR_YELLOW);
            CanBuy = false;
        }
        else if (pItem2 && crItem->ReqItem2 > 0)
        {
            SendEquipError(EQUIP_ERR_VENDOR_MISSING_TURNINS,NULL,NULL,NULL);
            SendChatMessage("You need %s%s to buy this item.",pItem2->GetNameLink(true).c_str(),MSG_COLOR_YELLOW);
            CanBuy = false;
        }
    }
    if (!GetBuyEnabled())
    {
        Item* pItem = Item::CreateItem(pProto->ItemId,1);
        if (pItem)
            SendChatMessage("You must type %s.togglebuy%s to buy %s",MSG_COLOR_RED,MSG_COLOR_YELLOW,pItem->GetNameLink(true).c_str());
        else
            SendChatMessage("You must type %s.togglebuy%s to buy this item",MSG_COLOR_RED,MSG_COLOR_YELLOW);
    }
}

void Player::RemoveCustomVendorRequirements(VendorItem const* crItem)
{
    if (crItem->ReqItem1 > 0)
        DestroyItemCount(crItem->ReqItem1,1,true);
    if (crItem->ReqItem2 > 0)
        DestroyItemCount(crItem->ReqItem2,1,true);
    if (crItem->ReqArenaPoints > 0)
        ModifyArenaPoints(-int32(crItem->ReqArenaPoints));
}

void World::SendBroadcast()
{
    std::string msg;
    msg.reserve(2048);
    msg = MSG_COLOR_MAGENTA"[Server]"MSG_COLOR_WHITE": ";
    QueryResult *result = WorldDatabase.PQuery("SELECT text FROM autobroadcast ORDER BY RAND() LIMIT 1");
    if (result)
    {
        msg = ""+msg+""+result->Fetch()[0].GetString()+"";
        delete result;

        SendServerMessage(SERVER_MSG_CUSTOM,msg.c_str(),NULL);

        sLog.outString("AutoBroadcast: '%s'",msg.c_str());
    }
    else
        sLog.outError("AutoBroadcast enabled, but no broadcast texts was found.");
}

std::string ChatHandler::BuildWorldChatMsg(std::string msg)
{
    std::string StaffString = "";
    if (m_session->GetSecurity() > SEC_PLAYER)
        StaffString = ""MSG_COLOR_WHITE"["MSG_COLOR_MAGENTA"Staff"MSG_COLOR_WHITE"] ";

    std::string message = ""+StaffString+""MSG_COLOR_RED""+m_session->GetPlayer()->GetNameLink(true)+""MSG_COLOR_WHITE": "+msg+"";
    stringReplace(message,"|r",MSG_COLOR_WHITE);

    return message;
}

void ChatHandler::PSendGlobalSysMessage(const char *format, ...)
{
    va_list ap;
    char str [2048];
    va_start(ap, format);
    vsnprintf(str, 2048, format, ap);
    va_end(ap);
    SendGlobalSysMessage(str);
}

bool ChatHandler::HandleExtendedCostCommand(char* args)
{
    uint32 extendedcost;
    if (!ExtractUInt32(&args,extendedcost))
        return false;

    uint32 itemid;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemid))
        return false;

    Item* pItem = Item::CreateItem(itemid, 1);
    if (!pItem)
        return false;
    if (!sItemExtendedCostStore.LookupEntry(extendedcost))
    {
        PSendSysMessage("Extendedcost id %u do not exist.",extendedcost);
    }

    WorldDatabase.PExecute("UPDATE npc_vendor SET extendedcost = %u WHERE item = %u",extendedcost,itemid);
    PSendSysMessage("Updated extendedcost to %u on %s",extendedcost,pItem->GetNameLink(true).c_str());
    return true;
}

bool ChatHandler::HandleReqTitleCommand(char* args)
{
    uint32 titleid;
    if (!ExtractUInt32(&args,titleid))
        return false;

    uint32 itemid;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemid))
        return false;

    Item* pItem = Item::CreateItem(itemid, 1);
    if (!pItem)
        return false;

    WorldDatabase.PExecute("UPDATE item_template SET requiredhonorrank = %u WHERE entry = %u",titleid,itemid);
    PSendSysMessage("Updated required title to %u on %s",titleid,pItem->GetNameLink(true).c_str());
    PSendSysMessage("Remember: Rank ranks are differing +4, so rank 14 should be entered as 18, rank one as 5 etc.");
    return true;
}

bool ChatHandler::HandleReqArenaRatingCommand(char* args)
{
    uint32 rating;
    if (!ExtractUInt32(&args,rating))
        return false;

    uint32 itemid;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemid))
        return false;

    Item* pItem = Item::CreateItem(itemid, 1);
    if (!pItem)
        return false;

    WorldDatabase.PExecute("UPDATE npc_vendor SET ReqArenaRating = %u WHERE item = %u",rating,itemid);
    PSendSysMessage("Updated required arena rating to %u on %s",rating,pItem->GetNameLink(true).c_str());
    return true;
}

bool ChatHandler::HandleReqArenaPointsCommand(char* args)
{
    uint32 points;
    if (!ExtractUInt32(&args,points))
        return false;

    uint32 itemid;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemid))
        return false;

    Item* pItem = Item::CreateItem(itemid, 1);
    if (!pItem)
        return false;

    WorldDatabase.PExecute("UPDATE npc_vendor SET ReqArenaPoints = %u WHERE item = %u",points,itemid);
    PSendSysMessage("Updated required arena points to %u on %s",points,pItem->GetNameLink(true).c_str());
    return true;
}

bool ChatHandler::HandleReqItemCommand(char* args)
{
    uint32 reqitem;
    if (!ExtractUInt32(&args,reqitem))
        return false;

    uint32 itemid;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemid))
        return false;

    Item* pItem = Item::CreateItem(itemid, 1);
    if (!pItem)
        return false;

    Item* pReqItem = Item::CreateItem(reqitem, 1);
    if (!pReqItem)
        return false;

    WorldDatabase.PExecute("UPDATE npc_vendor SET ReqItem = %u WHERE item = %u",reqitem,itemid);
    PSendSysMessage("Updated required item to %s%s on %s",pReqItem->GetNameLink(true).c_str(),MSG_COLOR_YELLOW,pItem->GetNameLink(true).c_str());
    return true;
}

bool ChatHandler::HandleReqItem2Command(char* args)
{
    uint32 reqitem;
    if (!ExtractUInt32(&args,reqitem))
        return false;

    uint32 itemid;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemid))
        return false;

    Item* pItem = Item::CreateItem(itemid, 1);
    if (!pItem)
        return false;

    Item* pReqItem = Item::CreateItem(reqitem, 1);
    if (!pReqItem)
        return false;

    WorldDatabase.PExecute("UPDATE npc_vendor SET ReqItem2 = %u WHERE item = %u",reqitem,itemid);
    PSendSysMessage("Updated required item to %s%s on %s",pReqItem->GetNameLink(true).c_str(),MSG_COLOR_YELLOW,pItem->GetNameLink(true).c_str());
    return true;
}

bool ChatHandler::HandleReqGoldCommand(char* args)
{
    uint32 copper;
    if (!ExtractUInt32(&args,copper))
        return false;

    uint32 itemid;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemid))
        return false;

    Item* pItem = Item::CreateItem(itemid, 1);
    if (!pItem)
        return false;

    WorldDatabase.PExecute("UPDATE item_template SET BuyPrice = %u, SellPrice = %u WHERE entry = %u",copper,copper/4,itemid);

    PSendSysMessage("Updated required gold to %u on %s",copper,pItem->GetNameLink(true).c_str());
    return true;
}

bool ChatHandler::HandleToggleBuyCommand(char* /*args*/)
{
    Player* pPlayer = m_session->GetPlayer();
    if (!pPlayer)
        return false;

    pPlayer->ToggleBuyEnabled();

    if (pPlayer->GetBuyEnabled())
        PSendSysMessage("You can now buy customreq items.");
    else
        PSendSysMessage("You can no longer buy customreq items.");

    return true;
}

bool ChatHandler::HandleWorldChatCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();
    if (!pPlayer)
        return false;

    char *c_msg = ExtractQuotedOrLiteralArg(&args);
    if (!c_msg)
        return false;

    std::string s_msg = c_msg;
    if (s_msg.empty())
        return false;

    std::string message = BuildWorldChatMsg(s_msg);
    PSendGlobalSysMessage(message.c_str());

    if (m_session->GetSecurity() > SEC_PLAYER)
    {
        WorldPacket data(SMSG_NOTIFICATION, (message.size()+1));
        data << message;
        sWorld.SendGlobalMessage(&data);
    }
    return true;
}

bool ChatHandler::HandleWarpCommand(char* args)
{
    // Based on a concept by Pwntzyou
    if (!*args)
        return false;

    Player* _player = m_session->GetPlayer();

    char* arg1 = strtok((char*)args, " ");
    char* arg2 = strtok(NULL, " ");

    if (!arg1 || !arg2)
        return false;

    char dir = arg1[0];
    float value = float(atof(arg2));
    float x = _player->GetPositionX();
    float y = _player->GetPositionY();
    float z = _player->GetPositionZ();
    float o = _player->GetOrientation();

    if ((dir != 'x' && dir != 'y' && dir != 'z' && dir != 'o') || value == 0.f)
    {
        _player->SendChatMessage("%s[Warp Info]%s Incorrect values, correct direction parameters is: x,y,z and o. Correct range/degree parameter is everything above or under 0.",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,value);
        return true;
    }

    switch (dir)
    {
    case 'x':
        {
            x = x + cos(o-(M_PI/2))*value;
            y = y + sin(o-(M_PI/2))*value;

            _player->NearTeleportTo(x,y,z,o);

            _player->SendChatMessage("%s[Warp Info]%s You teleported %g yards in x direction",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,value);
        }
        break;
    case 'y':
        {
            x = x + cosf(o)*value;
            y = y + sinf(o)*value;

            _player->NearTeleportTo(x,y,z,o);

            _player->SendChatMessage("%s[Warp Info]%s You teleported %g yards in y direction",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,value);
        }
        break;
    case 'z':
        {
            _player->NearTeleportTo(x,y,z+value,o);

            _player->SendChatMessage("%s[Warp Info]%s You teleported %g yards in z direction",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,value);
        }
        break;
    case 'o':
        {
            o = MapManager::NormalizeOrientation((value * M_PI_F/180.0f)+o);

            _player->NearTeleportTo(x,y,z,o);
            _player->SendChatMessage("%s[Warp Info]%s You rotated %g degrees (%g radians)",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,value,value * M_PI / 180.0f);
            _player->SendChatMessage("%sCurrent radian/degree: %g %g",MSG_COLOR_WHITE,o,o*180.0f/M_PI);
        }
        break;
    }
    return true;
}

void BattleGroundMgr::HandleCrossfactionSendToBattle(Player* pl, BattleGround* bg, uint32 instanceId, BattleGroundTypeId bgTypeId)
{
    if (!pl || !bg)
        return;

    if (sWorld.getConfig(CONFIG_BOOL_BATTLEGROUND_CROSSFACTION_ENABLED))
    {
        Team GrpTeam = TEAM_NONE;
        if (Group *pGroup = pl->GetGroup())
        {
            for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* pGroupGuy = itr->getSource();
                if (!pGroupGuy)
                    continue;

                if (pGroupGuy->GetBattleGround() && pGroupGuy->GetBattleGround()->GetInstanceID() == instanceId && pGroupGuy->GetBattleGround()->GetTypeID() == bgTypeId)
                {
                    GrpTeam = pGroupGuy->GetBGTeam();
                    break;
                }
            }
        }
        if (GrpTeam != TEAM_NONE && bg->GetPlayersCountByTeam(GrpTeam) < bg->GetMaxPlayersPerTeam())
            pl->SetBGTeam(GrpTeam);
        else
        {
            if (bg->GetPlayersCountByTeam(HORDE) < bg->GetMaxPlayersPerTeam() && bg->GetPlayersCountByTeam(HORDE) < bg->GetPlayersCountByTeam(ALLIANCE))
                pl->SetBGTeam(HORDE);
            else if (bg->GetPlayersCountByTeam(ALLIANCE) < bg->GetMaxPlayersPerTeam() && bg->GetPlayersCountByTeam(ALLIANCE) < bg->GetPlayersCountByTeam(HORDE))
                pl->SetBGTeam(ALLIANCE);
        }
        if (pl->GetBGTeam() == HORDE)
            pl->setFaction(2); // orc, and generic for horde
        else if (pl->GetBGTeam() == ALLIANCE)
            pl->setFaction(1); // dwarf/gnome, and generic for alliance
    }

    bg->UpdatePlayersCountByTeam(pl->GetBGTeam(), false); // Add here instead of in AddPlayer, because AddPlayer is not made until loading screen is finished. Which can cause unbalance in the system.
}

void BattleGround::MorphCrossfactionPlayer(Player* plr, bool action)
{
    if (!plr || !plr->IsInWorld())
        return;

    if (plr->GetTeam() != plr->GetBGTeam() && action)
    {
        switch (urand(1,2))
        {
        case 1:
            // Human / Bloodelf
            if (plr->GetBGTeam() == HORDE && plr->getGender() == GENDER_MALE)
            {
                plr->SetDisplayId(19723);
                plr->SetNativeDisplayId(19723);
            }
            else if (plr->GetBGTeam() == HORDE && plr->getGender() == GENDER_FEMALE)
            {
                plr->SetDisplayId(19724);
                plr->SetNativeDisplayId(19724);
            }
            else if (plr->GetBGTeam() == ALLIANCE && plr->getGender() == GENDER_MALE)
            {
                plr->SetDisplayId(20578);
                plr->SetNativeDisplayId(20578);
            }
            else if (plr->GetBGTeam() == ALLIANCE && plr->getGender() == GENDER_FEMALE)
            {
                plr->SetDisplayId(20579);
                plr->SetNativeDisplayId(20579);
            }
            break;
        case 2:
            // Gnome / Tauren
            if (plr->GetBGTeam() == HORDE && plr->getGender() == GENDER_MALE)
            {
                plr->SetDisplayId(20585);
                plr->SetNativeDisplayId(20585);
            }
            else if (plr->GetBGTeam() == HORDE && plr->getGender() == GENDER_FEMALE)
            {
                plr->SetDisplayId(20584);
                plr->SetNativeDisplayId(20584);
            }
            else if (plr->GetBGTeam() == ALLIANCE && plr->getGender() == GENDER_MALE)
            {
                plr->SetDisplayId(20580);
                plr->SetNativeDisplayId(20580);
            }
            else if (plr->GetBGTeam() == ALLIANCE && plr->getGender() == GENDER_FEMALE)
            {
                plr->SetDisplayId(20581);
                plr->SetNativeDisplayId(20581);
            }
            break;
        }
    }
    else if (!action)
    {
        plr->setFactionForRace(plr->getRace());
        plr->InitDisplayIds();
        plr->ClearFakePlayerlist();
    }
}

void BattleGround::ForgetPlayers(Player* plr)
{
    for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player* pPlayer = sObjectMgr.GetPlayer(itr->first,true);
        if (!pPlayer)
            continue;

        if (pPlayer != plr)
        {
            WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
            data << plr->GetObjectGuid();
            pPlayer->GetSession()->SendPacket(&data);
            WorldPacket data2(SMSG_INVALIDATE_PLAYER, 8);
            data2 << pPlayer->GetObjectGuid();
            plr->GetSession()->SendPacket(&data2);
        }
    }
}

uint8 Player::GetFakeRace()
{
    if (GetNativeDisplayId() == 19723 || GetNativeDisplayId() == 19724)
        return RACE_BLOODELF;
    else if (GetNativeDisplayId() == 20578 || GetNativeDisplayId() == 20579)
        return RACE_HUMAN;
    else if (GetNativeDisplayId() == 20584 || GetNativeDisplayId() == 20585)
        return RACE_TAUREN;
    else if (GetNativeDisplayId() == 20580 || GetNativeDisplayId() == 20581)
        return RACE_GNOME;

    return GetByteValue(UNIT_FIELD_BYTES_0, 0);
}

void Player::ClearFakePlayerlist()
{
    std::vector<Player*>::iterator iter;
    for (iter = m_FakedPlayers.begin(); iter != m_FakedPlayers.end(); ++iter)
    {
        Player* pPlayer = *iter;
        if (pPlayer->GetTeam() == pPlayer->GetBGTeam())
        {
            WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
            data << pPlayer->GetObjectGuid();
            GetSession()->SendPacket(&data);
        }
    }
    m_FakedPlayers.clear();
}