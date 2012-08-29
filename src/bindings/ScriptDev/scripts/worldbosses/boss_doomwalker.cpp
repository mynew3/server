/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* ScriptData
SDName: Boss_Doomwalker
SD%Complete: 100
SDComment:
SDCategory: Shadowmoon Valley
EndScriptData */

#include "precompiled.h"


const char* DOOMWALKER_SAY_AGGRO = "Do not proceed. You will be eliminated!";
const char* DOOMWALKER_SAY_EARTHQUAKE_1 = "Tectonic disruption commencing.";
const char* DOOMWALKER_SAY_EARTHQUAKE_2 = "Magnitude set. Release.";
const char* DOOMWALKER_SAY_OVERRUN_1 = "Trajectory locked.";
const char* DOOMWALKER_SAY_OVERRUN_2 = "Engage maximum speed.";
const char* DOOMWALKER_SAY_SLAY_1 = "Threat level zero.";
const char* DOOMWALKER_SAY_SLAY_2 = "Directive accomplished.";
const char* DOOMWALKER_SAY_SLAY_3 = "Target exterminated.";
const char* DOOMWALKER_SAY_DEATH = "System failure in five, f-o-u-r...";

enum
{

    SPELL_EARTHQUAKE = 32686,
    SPELL_CRUSH_ARMOR = 33661,
    SPELL_LIGHTNING_WRATH = 33665,
    SPELL_OVERRUN = 32636,
    SPELL_ENRAGE = 33653,
    SPELL_MARK_OF_DEATH_PLAYER = 37128,
    SPELL_MARK_OF_DEATH_AURA = 37125, // triggers 37131 on target if it has aura 37128
};

struct MANGOS_DLL_DECL boss_doomwalkerAI : public ScriptedAI
{
    boss_doomwalkerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiChainTimer;
    uint32 m_uiOverrunTimer;
    uint32 m_uiQuakeTimer;
    uint32 m_uiArmorTimer;

    bool m_bHasEnrage;

    void Reset()
    {
        m_uiArmorTimer = urand(5000, 13000);
        m_uiChainTimer = urand(10000, 30000);
        m_uiQuakeTimer = urand(25000, 35000);
        m_uiOverrunTimer = urand(30000, 45000);

        m_bHasEnrage = false;
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        pVictim->CastSpell(pVictim, SPELL_MARK_OF_DEATH_PLAYER, true, NULL, NULL, m_creature->GetObjectGuid());

        if (urand(0, 4))
            return;

        switch (urand(0, 2))
        {
            case 0: m_creature->MonsterSay(DOOMWALKER_SAY_SLAY_1,LANG_UNIVERSAL); break;
            case 1: m_creature->MonsterSay(DOOMWALKER_SAY_SLAY_2,LANG_UNIVERSAL); break;
            case 2: m_creature->MonsterSay(DOOMWALKER_SAY_SLAY_3,LANG_UNIVERSAL); break;
        }

    }

    void JustDied(Unit* pKiller)
    {
        m_creature->MonsterSay(DOOMWALKER_SAY_DEATH,LANG_UNIVERSAL);
    }

    void Aggro(Unit* pWho)
    {
        DoCastSpellIfCan(m_creature, SPELL_MARK_OF_DEATH_AURA, CAST_TRIGGERED);
        m_creature->MonsterSay(DOOMWALKER_SAY_AGGRO,LANG_UNIVERSAL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Spell Enrage, when hp <= 20% gain enrage
        if (m_creature->GetHealthPercent() <= 20.0f && !m_bHasEnrage)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                m_bHasEnrage = true;
        }

        // Spell Overrun
        if (m_uiOverrunTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_OVERRUN) == CAST_OK)
            {
                switch (urand(0,1))
                {
                    case 0: m_creature->MonsterSay(DOOMWALKER_SAY_OVERRUN_1,LANG_UNIVERSAL); break;
                    case 1: m_creature->MonsterSay(DOOMWALKER_SAY_OVERRUN_2,LANG_UNIVERSAL); break;
                }
                m_uiOverrunTimer = urand(25000, 40000);
            }
        }
        else
            m_uiOverrunTimer -= uiDiff;

        // Spell Earthquake
        if (m_uiQuakeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EARTHQUAKE) == CAST_OK)
            {
                switch (urand(0,1))
                {
                case 0: m_creature->MonsterSay(DOOMWALKER_SAY_EARTHQUAKE_1,LANG_UNIVERSAL); break;
                case 1: m_creature->MonsterSay(DOOMWALKER_SAY_EARTHQUAKE_2,LANG_UNIVERSAL); break;
                }
                m_uiQuakeTimer = urand(30000, 55000);
            }
        }
        else
            m_uiQuakeTimer -= uiDiff;

        // Spell Chain Lightning
        if (m_uiChainTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            if (!pTarget)
                pTarget = m_creature->getVictim();

            if (pTarget)
            {
                if (DoCastSpellIfCan(pTarget, SPELL_LIGHTNING_WRATH) == CAST_OK)
                    m_uiChainTimer = urand(7000, 27000);
            }
        }
        else
            m_uiChainTimer -= uiDiff;

        // Spell Sunder Armor
        if (m_uiArmorTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSH_ARMOR) == CAST_OK)
                m_uiArmorTimer = urand(10000, 25000);
        }
        else
            m_uiArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_doomwalker(Creature* pCreature)
{
    return new boss_doomwalkerAI(pCreature);
}

void AddSC_boss_doomwalker()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_doomwalker";
    pNewScript->GetAI = &GetAI_boss_doomwalker;
    pNewScript->RegisterSelf();
}