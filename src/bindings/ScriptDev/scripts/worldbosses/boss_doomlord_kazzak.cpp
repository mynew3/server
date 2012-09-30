/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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

/* ScriptData
SDName: Boss_Doomlord_Kazzak
SD%Complete: 90
SDComment: Timers
SDCategory: Hellfire Peninsula
EndScriptData */

#include "precompiled.h"

const char* DOOMLORD_SAY_INTRO               = "I remember well the sting of defeat at the conclusion of the Third War. I have waited far too long for my revenge. Now the shadow of the Legion falls over this world. It is only a matter of time until all of your failed creation... is undone.";
const char* DOOMLORD_SAY_AGGRO1              = "The Legion will conquer all!";
const char* DOOMLORD_SAY_AGGRO2              = "All mortals will perish!";
const char* DOOMLORD_SAY_SURPREME1           = "All life must be eradicated!";
const char* DOOMLORD_SAY_SURPREME2           = "I'll rip the flesh from your bones!";
const char* DOOMLORD_SAY_KILL1               = "Kirel Narak!";
const char* DOOMLORD_SAY_KILL2               = "Contemptible wretch!";
const char* DOOMLORD_SAY_KILL3               = "The universe will be remade.";
const char* DOOMLORD_SAY_DEATH               = "The Legion... will never... fall.";
const char* DOOMLORD_EMOTE_GENERIC_FRENZY    = "Doom Lord Kazzak goes into a frenzy!";
const char* DOOMLORD_SAY_RAND1               = "Invaders, you dangle upon the precipice of oblivion! The Burning Legion comes and with it comes your end.";
const char* DOOMLORD_SAY_RAND2               = "Impudent whelps, you only delay the inevitable. Where one has fallen, ten shall rise. Such is the will of Kazzak...";

enum
{
    SPELL_SHADOW_VOLLEY             = 32963,
    SPELL_CLEAVE                    = 31779,
    SPELL_THUNDERCLAP               = 36706,
    SPELL_VOID_BOLT                 = 39329,
    SPELL_MARK_OF_KAZZAK            = 32960,
    SPELL_FRENZY                    = 32964,        // triggers 32963
    SPELL_CAPTURE_SOUL              = 48473,        // procs 32966 on player kill
    SPELL_TWISTED_REFLECTION        = 21063,
    SPELL_BERSERK                   = 32965,        // triggers 32963
};

struct MANGOS_DLL_DECL boss_doomlordkazzakAI : public ScriptedAI
{
    boss_doomlordkazzakAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiShadowVolleyTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiThunderClapTimer;
    uint32 m_uiVoidBoltTimer;
    uint32 m_uiMarkOfKazzakTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiGreatEnrageTimer;
    uint32 m_uiTwistedReflectionTimer;

    void Reset()
    {
        m_uiShadowVolleyTimer       = urand(6000, 10000);
        m_uiCleaveTimer             = 7000;
        m_uiThunderClapTimer        = urand(14000, 18000);
        m_uiVoidBoltTimer           = 30000;
        m_uiMarkOfKazzakTimer       = 25000;
        m_uiEnrageTimer             = 60000;
        m_uiGreatEnrageTimer        = 3*MINUTE*IN_MILLISECONDS;
        m_uiTwistedReflectionTimer  = 33000;                   // Timer may be incorrect
    }

    void JustRespawned()
    {
        m_creature->MonsterSay(DOOMLORD_SAY_INTRO,LANG_UNIVERSAL);
    }

    void Aggro(Unit* pWho)
    {
        switch (urand(0,1))
        {
            case 0: m_creature->MonsterSay(DOOMLORD_SAY_AGGRO1,LANG_UNIVERSAL); break;
            case 1: m_creature->MonsterSay(DOOMLORD_SAY_AGGRO2,LANG_UNIVERSAL); break;
        }
        DoCastSpellIfCan(m_creature, SPELL_CAPTURE_SOUL, CAST_TRIGGERED);
    }

    void KilledUnit(Unit* pVictim)
    {
        // When Kazzak kills a player (not pets/totems), he regens some health
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch(urand(0, 2))
        {
            case 0: m_creature->MonsterSay(DOOMLORD_SAY_KILL1,LANG_UNIVERSAL); break;
            case 1: m_creature->MonsterSay(DOOMLORD_SAY_KILL2,LANG_UNIVERSAL); break;
            case 2: m_creature->MonsterSay(DOOMLORD_SAY_KILL3,LANG_UNIVERSAL); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->MonsterSay(DOOMLORD_SAY_DEATH,LANG_UNIVERSAL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // ShadowVolley_Timer
        if (m_uiShadowVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_VOLLEY) == CAST_OK)
                m_uiShadowVolleyTimer = urand(10000, 30000);
        }
        else
            m_uiShadowVolleyTimer -= uiDiff;

        // Cleave_Timer
        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(8000, 12000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // ThunderClap_Timer
        if (m_uiThunderClapTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_THUNDERCLAP) == CAST_OK)
                m_uiThunderClapTimer = urand(10000, 14000);
        }
        else
            m_uiThunderClapTimer -= uiDiff;

        // VoidBolt_Timer
        if (m_uiVoidBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_VOID_BOLT) == CAST_OK)
                    m_uiVoidBoltTimer = urand(15000, 18000);
            }
        }
        else
            m_uiVoidBoltTimer -= uiDiff;

        // MarkOfKazzak_Timer
        if (m_uiMarkOfKazzakTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MARK_OF_KAZZAK, SELECT_FLAG_POWER_MANA))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MARK_OF_KAZZAK) == CAST_OK)
                    m_uiMarkOfKazzakTimer = 20000;
            }
        }
        else
            m_uiMarkOfKazzakTimer -= uiDiff;

        // Enrage_Timer
        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
                m_creature->MonsterSay(DOOMLORD_EMOTE_GENERIC_FRENZY,LANG_UNIVERSAL);
                m_uiEnrageTimer = 60000;
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        // Great_Enrage_Timer
        if (m_uiGreatEnrageTimer)
        {
            if (m_uiGreatEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiGreatEnrageTimer = 0;
            }
            else
                m_uiGreatEnrageTimer -= uiDiff;
        }

        // Twisted Reflection
        if (m_uiTwistedReflectionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_TWISTED_REFLECTION) == CAST_OK)
                    m_uiTwistedReflectionTimer = 15000;
            }
        }
        else
            m_uiTwistedReflectionTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_doomlordkazzak(Creature* pCreature)
{
    return new boss_doomlordkazzakAI(pCreature);
}

void AddSC_boss_doomlordkazzak()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_doomlord_kazzak";
    pNewScript->GetAI = &GetAI_boss_doomlordkazzak;
    pNewScript->RegisterSelf();
}