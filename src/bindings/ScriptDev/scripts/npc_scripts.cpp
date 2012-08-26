#include "precompiled.h"
#include "Chat.h"
#include "ObjectMgr.h"

bool GossipHello_telenpc(Player *pPlayer, Creature *pCreature)
{
    if (pPlayer->isInCombat())
    {
        pPlayer->SendChatMessage("%s[Teleporter]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return true;
    }
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(2,"Teleport To: Shopping Mall  ",GOSSIP_SENDER_MAIN,2  ,"",0);
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(2,"Teleport To: Gurubashi Arena",GOSSIP_SENDER_MAIN,3  ,"",0);
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Open Tools                  ",GOSSIP_SENDER_MAIN,100,"",0);
    pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_telenpc(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 action)
{
    if (pPlayer->isInCombat())
    {
        pPlayer->SendChatMessage("%s[Teleporter]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return false;
    }

    if (action == 1)
    {
        GossipHello_telenpc(pPlayer,pCreature);
    }
    else if (action == 2) // Teleport To: Shopping Mall
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        if (pPlayer->GetTeam() == HORDE)
        {
            pPlayer->PlayerTalkClass->CloseGossip();
            pPlayer->SendChatMessage("%s[Teleporter]%s Welcome to Horde shopping mall",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
            pPlayer->TeleportTo(530,    237.497f,   2696.65f,   90.755f,    3.25544f);
        } 
        else
        {
            pPlayer->PlayerTalkClass->CloseGossip();
            pPlayer->SendChatMessage("%s[Teleporter]%s Welcome to Alliance shopping mall",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
            pPlayer->TeleportTo(530,    -796.259f,  2625.9f,    124.095f,   3.70235f);
        }
    }
    else if (action == 3) // Teleport To: Gurubashi Arena
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->SendChatMessage("%s[Teleporter]%s Welcome to Gurubashi Arena",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        pPlayer->TeleportTo(0,  -13235.7f,  213.741f,   31.2181f,   1.14919f);
    }
    else if (action == 100) // Open Tools
    {
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Reset Talents     ",GOSSIP_SENDER_MAIN,101,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Reset Cooldowns   ",GOSSIP_SENDER_MAIN,104,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Max Weapon Skills ",GOSSIP_SENDER_MAIN,103,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Open Bank         ",GOSSIP_SENDER_MAIN,105,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Open Auction House",GOSSIP_SENDER_MAIN,106,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Create a guild    ",GOSSIP_SENDER_MAIN,107,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Design your tabard",GOSSIP_SENDER_MAIN,108,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Back to Main Menu ",GOSSIP_SENDER_MAIN,1  ,"",0);
        pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    }
    else if (action == 101) // Reset Talents
    {
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Are you sure you want to reset your talents?",GOSSIP_SENDER_MAIN,100,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Yes                                         ",GOSSIP_SENDER_MAIN,102,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"No                                          ",GOSSIP_SENDER_MAIN,100,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Back to Main Menu                           ",GOSSIP_SENDER_MAIN,1  ,"",0);
        pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    }
    else if (action == 102) // Talents Reset
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->SendChatMessage("%s[Teleporter]%s Talents Reset",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        pPlayer->resetTalents(true);
    }
    else if (action == 103) // Weapon skills maxed
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->SendChatMessage("%s[Teleporter]%s Weapon skills maxed",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        pPlayer->UpdateSkillsToMaxSkillsForLevel();
    }
    else if (action == 104) // Cooldowns reset
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->SendChatMessage("%s[Teleporter]%s Cooldowns reset",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        pPlayer->RemoveArenaSpellCooldowns();
    }
    else if (action == 105) // Show bank
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->GetSession()->SendShowBank(pCreature->GetObjectGuid());
    }
    else if (action == 106) // Auctioneer
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->GetSession()->SendAuctionHello(pCreature);
    }
    else if (action == 107) // Guild Master
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->GetSession()->SendPetitionShowList(pCreature->GetObjectGuid());
    }
    else if (action == 108) // Tabard Petitioner
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->GetSession()->SendTabardVendorActivate(pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipHello_beast_master(Player *pPlayer, Creature *pCreature)
{
    if (pPlayer->isInCombat())
    {
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return true;
    }

    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Bat         ",GOSSIP_SENDER_MAIN,1 ,"",0); // 16173
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Bear        ",GOSSIP_SENDER_MAIN,2 ,"",0); // 23584
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Boar        ",GOSSIP_SENDER_MAIN,3 ,"",0); // 21878
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Carrion Bird",GOSSIP_SENDER_MAIN,4 ,"",0); // 18707
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Cat         ",GOSSIP_SENDER_MAIN,5 ,"",0); // 20671
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Crab        ",GOSSIP_SENDER_MAIN,6 ,"",0); // 18241
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Crocolisk   ",GOSSIP_SENDER_MAIN,7 ,"",0); // 20773
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Dragonhawk  ",GOSSIP_SENDER_MAIN,8 ,"",0); // 18155
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Gorilla     ",GOSSIP_SENDER_MAIN,9 ,"",0);// 6585
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Hyena       ",GOSSIP_SENDER_MAIN,10,"",0);// 8300
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Nether Ray  ",GOSSIP_SENDER_MAIN,11,"",0);// 17731
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Raptor      ",GOSSIP_SENDER_MAIN,12,"",0);// 20634
    pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Go to Next Page          ",GOSSIP_SENDER_MAIN,13,"",0);
    pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_beast_master(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 action)
{
    if (pPlayer->isInCombat())
    {
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return false;
    }

    if (action == 1) // Bat 16173
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(16173);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(27051); // Screech
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a bat",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 2) // Bear 23584
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(23584);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a bear",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 3) // Boar 21878
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(21878);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27685); // Charge
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->GetPet()->learnSpell(35298); // Gore
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a boar",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 4) // Carrion Bird 18707
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(18707);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(27051); // Screech
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a carrion bird",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 5) // Cat 20671
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20671);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->GetPet()->learnSpell(24453); // Prowl
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a cat",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 6) // Crab 18241
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(18241);
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a crab",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 7) // Crocolisk 20773
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20773);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a crocolisk",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 8) // Dragonhawk 18155
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(18155);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(35323); // Fire Breath
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a dragonhawk",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 9) // Gorilla 6585
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(6585);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27063); // Thunderstomp
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a gorilla",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 10) // Hyena 8300
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(8300);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a hyena",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 11) // Nether Ray 17731
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(17731);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a nether ray",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 12) // Raptor 20634
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20634);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a raptor",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 13)
    {
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Ravager     ",GOSSIP_SENDER_MAIN,14,"",0); // 23326
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Scorpid     ",GOSSIP_SENDER_MAIN,15,"",0); // 21864
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Serpent     ",GOSSIP_SENDER_MAIN,16,"",0); // 19784
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Spider      ",GOSSIP_SENDER_MAIN,17,"",0); // 16170
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Tallstrider ",GOSSIP_SENDER_MAIN,18,"",0); // 22807
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Turtle      ",GOSSIP_SENDER_MAIN,19,"",0); // 5431
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Warp Stalker",GOSSIP_SENDER_MAIN,20,"",0); // 23219
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Wind Serpent",GOSSIP_SENDER_MAIN,21,"",0); // 20673
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(9,"Summon me a: Wolf        ",GOSSIP_SENDER_MAIN,22,"",0); // 17280
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Go to Previous Page      ",GOSSIP_SENDER_MAIN,23,"",0);
        pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    }
    else if (action == 14) // Ravager 23326
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(23326);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->GetPet()->learnSpell(35298); // Gore
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a ravager",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 15) // Scorpid 21864
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(21864);
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(27060); // Scorpid Poision
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a scorpid",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 16) // Serpent 19784
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(19784);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(35392); // Poision Spit
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a serpent",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 17) // Spider 16170
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(16170);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(4167);  // Web
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a spider",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 18) // Tallstrider 22807
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(22807);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a tallstrider",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 19) // Turtle 5431
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(5431);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(26064); // Spell Shield
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a turtle",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 20) // Warp Stalker 23219
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(23219);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(35346); // Warp
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a warp stalker",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 21) // Wind Serpent 20673
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20673);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(25012); // Lightning Breath
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a wind serpent",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 22) // Wolf 17280
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(17280);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->GetPet()->learnSpell(27685); // Charge
        pPlayer->GetPet()->learnSpell(24604); // Furious Howl
        pPlayer->SendChatMessage("%s[Mr.Zoo]%s You have tamed a wolf",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 23)
    {
        GossipHello_beast_master(pPlayer,pCreature);
    }
    return true;
}

typedef UNORDERED_MAP<ObjectGuid, uint32> AttackerMap;

struct MANGOS_DLL_DECL npc_training_dummyAI : public Scripted_NoMovementAI
{
    npc_training_dummyAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature){}

    AttackerMap m_AttackerMap;

    void Reset() {}

    void DamageTaken(Unit* pDealer, uint32& uiDamage)
    {
        if (pDealer->GetTypeId() == TYPEID_PLAYER)
            m_AttackerMap[pDealer->GetObjectGuid()] = 6000;

        if (m_creature->GetHealthPercent() < 10.0f || m_creature->GetHealthPercent() > 20.0f) // allow players using finishers
            m_creature->SetHealth(m_creature->GetMaxHealth()/5);

        m_creature->SetTargetGuid(ObjectGuid()); // prevent from rotating
    }

    void UpdateAI(const uint32 diff)
    {
        for (AttackerMap::iterator itr = m_AttackerMap.begin(); itr != m_AttackerMap.end();)
        {
            if (itr->second < diff)
            {
                if (Player* pAttacker = m_creature->GetMap()->GetPlayer(itr->first))
                {
                    pAttacker->CombatStop(true);
                    pAttacker->AttackStop(false);
                    pAttacker->CombatStopWithPets(true);
                    pAttacker->ClearInCombat();
                }

                itr = m_AttackerMap.erase(itr);

                if (m_AttackerMap.empty())
                {
                    EnterEvadeMode();
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                }
            }
            else
            {
                itr->second -= diff;
                ++itr;
            }
        }
    }
};
 
CreatureAI* GetAI_npc_training_dummy(Creature* pCreature)
{
    return new npc_training_dummyAI(pCreature);
}


void AddSC_npc_scripts()
{
    Script* pNewScript;

    pNewScript                  = new Script;
    pNewScript->Name            = "npc_teleport1";
    pNewScript->pGossipHello    = &GossipHello_telenpc;
    pNewScript->pGossipSelect   = &GossipSelect_telenpc;
    pNewScript->RegisterSelf();

    pNewScript                  = new Script;
    pNewScript->Name            = "npc_beastmaster";
    pNewScript->pGossipHello    = &GossipHello_beast_master;
    pNewScript->pGossipSelect   = &GossipSelect_beast_master;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "training_dummy";
    pNewScript->GetAI = &GetAI_npc_training_dummy;
    pNewScript->RegisterSelf();
}