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

#include <numeric>

void Player::CreatePet(uint32 entry, bool classcheck)
{
    if (classcheck && getClass() != CLASS_HUNTER)
    {
        SendChatMessage("%s[Mr.Zoo]%s You are no hunter!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return;
    }

    CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(entry);
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