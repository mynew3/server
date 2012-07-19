#include "precompiled.h"
#include "Chat.h"
#include "ObjectMgr.h"

bool GossipHello_telenpc(Player *pPlayer, Creature *pCreature)
{
    if (pPlayer->isInCombat())
    {
        ChatHandler(pPlayer).PSendSysMessage("%s[Teleporter]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
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
        ChatHandler(pPlayer).PSendSysMessage("%s[Teleporter]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
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
            ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Teleporter]%s Welcome to Horde shopping mall",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
            pPlayer->TeleportTo(530,    237.497f,   2696.65f,   90.755f,    3.25544f);
        } 
        else
        {
            pPlayer->PlayerTalkClass->CloseGossip();
            ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Teleporter]%s Welcome to Alliance shopping mall",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
            pPlayer->TeleportTo(530,    -796.259f,  2625.9f,    124.095f,   3.70235f);
        }
    }
    else if (action == 3) // Teleport To: Gurubashi Arena
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Teleporter]%s Welcome to Gurubashi Arena",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
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
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Show ingame GMs   ",GOSSIP_SENDER_MAIN,109,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Buy insurance     ",GOSSIP_SENDER_MAIN,110,"",0);
        //pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Place a bounty    ",GOSSIP_SENDER_MAIN,121,"",0);
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
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Teleporter]%s Talents Reset",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        pPlayer->resetTalents(true);
    }
    else if (action == 103) // Weapon skills maxed
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Teleporter]%s Weapon skills maxed",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        pPlayer->UpdateSkillsToMaxSkillsForLevel();
    }
    else if (action == 104) // Cooldowns reset
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Teleporter]%s Cooldowns reset",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
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
    else if (action == 109) // Show Gamemasters
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->InGamemasterGossip(pCreature);
    }
    else if (action == 110)
    {
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"100G/Item 50G/ItemRefresh",GOSSIP_SENDER_MAIN,1  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 2 items.                            ",GOSSIP_SENDER_MAIN,111  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 3 items.                            ",GOSSIP_SENDER_MAIN,112  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 4 items.                            ",GOSSIP_SENDER_MAIN,113  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 5 items.                            ",GOSSIP_SENDER_MAIN,114  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 6 items.                            ",GOSSIP_SENDER_MAIN,115  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 7 items.                            ",GOSSIP_SENDER_MAIN,116  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 8 items.                            ",GOSSIP_SENDER_MAIN,117  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 9 items.                            ",GOSSIP_SENDER_MAIN,118  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Insure 10 items.                           ",GOSSIP_SENDER_MAIN,119  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Refresh insurance charges (50 gold/item)   ",GOSSIP_SENDER_MAIN,120  ,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Back to Main Menu                          ",GOSSIP_SENDER_MAIN,1  ,"",0);
        pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    }
    else if (action == 111 || action == 112 || action == 113 || action == 114 || action == 115 || action == 116 || action == 117 || action == 118 || action == 119)
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        int32 setinsurance = 0;
        switch(action)
        {
            case 111: setinsurance = 2; break;
            case 112: setinsurance = 3; break;
            case 113: setinsurance = 4; break;
            case 114: setinsurance = 5; break;
            case 115: setinsurance = 6; break;
            case 116: setinsurance = 7; break;
            case 117: setinsurance = 8; break;
            case 118: setinsurance = 9; break;
            case 119: setinsurance = 10; break;
            default: setinsurance = 0; break;
        }
        if (pPlayer->BuyEnabled && (pPlayer->GetMoney() >= uint32(setinsurance*1000000)))
        {
            pPlayer->ItemInsurance = setinsurance;
            pPlayer->ItemInsuranceCharges = 50;
            pPlayer->ModifyMoney(-setinsurance*1000000);
            ChatHandler(pPlayer).PSendSysMessage("%s[Vendor System]%s You bought 50 death charges of %u item insurance",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,setinsurance);
        }
        else
            ChatHandler(pPlayer).PSendSysMessage("%s[Vendor System]%s please %s.togglebuy%s to buy insurance. Or you cannot afford buying this insurance.",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,MSG_COLOR_RED,MSG_COLOR_WHITE);
    }
    else if (action == 120)
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        int32 cost = pPlayer->ItemInsurance*500000;
        if (pPlayer->BuyEnabled && pPlayer->GetMoney() >= uint32(cost))
        {
            pPlayer->ModifyMoney(-cost);
            pPlayer->ItemInsuranceCharges = 50;
            ChatHandler(pPlayer).PSendSysMessage("%s[Vendor System]%s Refreshed your item insurance charges for %u gold.",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,pPlayer->ItemInsurance*50);
        }
        else
            ChatHandler(pPlayer).PSendSysMessage("%s[Vendor System]%s You must %s.togglebuy%s or you do not have enough money to refresh your insurance",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,MSG_COLOR_RED,MSG_COLOR_WHITE);
    }
/*    else if (action == 121)
    {
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"1  Gold            ",GOSSIP_SENDER_MAIN,122,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"5  Gold            ",GOSSIP_SENDER_MAIN,123,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"10 Gold            ",GOSSIP_SENDER_MAIN,124,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"25 Gold            ",GOSSIP_SENDER_MAIN,125,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"50 Gold            ",GOSSIP_SENDER_MAIN,126,"",0);
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"Back to Main Menu  ",GOSSIP_SENDER_MAIN,1  ,"",0);
        pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    }
    else if (action == 122 || action == 123 || action == 124 || action == 125 || action == 126)
    {
        uint32 addgold = 0;
        switch(action)
        {
            case 122:addgold=10000;break;
            case 123:addgold=50000;break;
            case 124:addgold=100000;break;
            case 125:addgold=250000;break;
            case 126:addgold=500000;break;
        }
        pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem(5,"",addgold,127,"Type in the victims name.",addgold,true);
        pPlayer->PlayerTalkClass->SendGossipMenu(1,pCreature->GetObjectGuid());
    }*/

    return true;
}

bool GossipSelectWithCode_telenpc(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction, const char* sCode)
{
    /*if (uiAction == 127)
    {
        std::string recvname = sCode;
        Player* pTarget = sObjectMgr.GetPlayer(sCode);
        std::string sendname = pTarget->GetName();
        strToLower(sendname);
        strToLower(recvname);

        if (sendname == recvname && pPlayer->GetMoney() > uiSender)
        {
            pPlayer->ModifyMoney(-int32(uiSender));
            pTarget->KillBounty += uiSender;
            ChatHandler(pPlayer).PSendGlobalSysMessage("%s[PvP System]%s %s set %u bounty on %s",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE,pPlayer->GetNameLink(),uiSender/10000,pTarget->GetNameLink());
        }
    }*/

    return false;
}

bool GossipHello_beast_master(Player *pPlayer, Creature *pCreature)
{
    if (pPlayer->isInCombat())
    {
        ChatHandler(pPlayer).PSendSysMessage("%s[Mr.Zoo]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return true;
    }
    else if (pPlayer->getClass() != CLASS_HUNTER)
    {
        ChatHandler(pPlayer).PSendSysMessage("%s[Mr.Zoo]%s You must be a hunter!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
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
        ChatHandler(pPlayer).PSendSysMessage("%s[Mr.Zoo]%s You are in combat!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return false;
    }
    else if (pPlayer->getClass() != CLASS_HUNTER)
    {
        ChatHandler(pPlayer).PSendSysMessage("%s[Mr.Zoo]%s You must be a hunter!",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
        return false;
    }

    if (action == 1) // Bat 16173
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(16173);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(27051); // Screech
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a bat",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 2) // Bear 23584
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(23584);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a bear",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 3) // Boar 21878
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(21878);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27685); // Charge
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->GetPet()->learnSpell(35298); // Gore
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a boar",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 4) // Carrion Bird 18707
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(18707);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(27051); // Screech
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a carrion bird",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 5) // Cat 20671
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20671);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->GetPet()->learnSpell(24453); // Prowl
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a cat",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 6) // Crab 18241
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(18241);
        pPlayer->GetPet()->learnSpell(27049); // Claw
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a crab",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 7) // Crocolisk 20773
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20773);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a crocolisk",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 8) // Dragonhawk 18155
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(18155);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(35323); // Fire Breath
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a dragonhawk",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 9) // Gorilla 6585
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(6585);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27063); // Thunderstomp
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a gorilla",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 10) // Hyena 8300
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(8300);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23110); // Dash
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a hyena",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 11) // Nether Ray 17731
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(17731);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a nether ray",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 12) // Raptor 20634
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20634);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(23110); // Dash
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a raptor",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
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
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a ravager",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 15) // Scorpid 21864
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(21864);
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(27060); // Scorpid Poision
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a scorpid",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 16) // Serpent 19784
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(19784);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(35392); // Poision Spit
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a serpent",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 17) // Spider 16170
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(16170);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(4167);  // Web
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a spider",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 18) // Tallstrider 22807
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(22807);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23110); // Dash
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a tallstrider",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 19) // Turtle 5431
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(5431);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(26064); // Spell Shield
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a turtle",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 10) // Warp Stalker 23219
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(23219);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(27049); // Claw
        pPlayer->GetPet()->learnSpell(35346); // Warp
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a warp stalker",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 11) // Wind Serpent 20673
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(20673);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23148); // Dive
        pPlayer->GetPet()->learnSpell(25012); // Lightning Breath
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a wind serpent",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 12) // Wolf 17280
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->CreatePet(17280);
        pPlayer->GetPet()->learnSpell(27050); // Bite
        pPlayer->GetPet()->learnSpell(23110); // Dash
        pPlayer->GetPet()->learnSpell(27685); // Charge
        pPlayer->GetPet()->learnSpell(24604); // Furious Howl
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s[Mr.Zoo]%s You have tamed a wolf",MSG_COLOR_MAGENTA,MSG_COLOR_WHITE);
    }
    else if (action == 13)
    {
        GossipHello_beast_master(pPlayer,pCreature);
    }
    return true;
}

std::map<uint64, std::map<uint32, Item*> > Items; // Items[GUID][DISPLAY] = item
char * GetSlotName(uint8 slot)
{
    switch(slot)
    {
    case EQUIPMENT_SLOT_HEAD      : return "Head";
    case EQUIPMENT_SLOT_SHOULDERS : return "Shoulders";
    case EQUIPMENT_SLOT_BODY      : return "Shirt";
    case EQUIPMENT_SLOT_CHEST     : return "Chest";
    case EQUIPMENT_SLOT_WAIST     : return "Waist";
    case EQUIPMENT_SLOT_LEGS      : return "Legs";
    case EQUIPMENT_SLOT_FEET      : return "Feet";
    case EQUIPMENT_SLOT_WRISTS    : return "Wrists";
    case EQUIPMENT_SLOT_HANDS     : return "Hands";
    case EQUIPMENT_SLOT_BACK      : return "Back";
    case EQUIPMENT_SLOT_MAINHAND  : return "Main hand";
    case EQUIPMENT_SLOT_OFFHAND   : return "Off hand";
    case EQUIPMENT_SLOT_RANGED    : return "Ranged";
    case EQUIPMENT_SLOT_TABARD    : return "Tabard";
    default: return NULL;
    }
}

bool GossipHello_transmog(Player* pPlayer, Creature* pUnit)
{
    for (uint8 Slot = EQUIPMENT_SLOT_START; Slot < EQUIPMENT_SLOT_TABARD; Slot++) // EQUIPMENT_SLOT_END
    {
        if (Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, Slot))
            if(pItem->HasGoodFakeQuality())
                if(const char* SlotName = GetSlotName(Slot))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, SlotName, EQUIPMENT_SLOT_END, Slot);
    }
    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Remove all transmogrifications", EQUIPMENT_SLOT_END+2, 0, "Remove transmogrifications from all equipped items?", 0, false);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Update Menu", EQUIPMENT_SLOT_END+1, 0);
    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pUnit->GetObjectGuid());
    return true;
}

bool GossipSelect_transmog(Player* pPlayer, Creature* pUnit, uint32 sender, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    switch(sender)
    {
    case EQUIPMENT_SLOT_END: // Show items you can use
        {
            if (Item* OLD = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, uiAction))
            {
                uint32 GUID = pPlayer->GetGUIDLow();
                Items[GUID].clear();
                uint32 limit = 0;
                for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
                {
                    if(limit > 30)
                        break;
                    if (Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    {
                        uint32 Display = pItem->GetProto()->DisplayInfoID;
                        if(pPlayer->SuitableForTransmogrification(OLD, pItem) == ERR_FAKE_OK)
                            if(Items[GUID].find(Display) == Items[GUID].end())
                            {
                                limit++;
                                Items[GUID][Display] = pItem;
                                pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, pItem->GetProto()->Name1, uiAction, Display, "Using this item for transmogrify will bind it to you and make it non-refundable and non-tradeable.nDo you wish to continue?", 0, false);
                            }
                    }
                }

                for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
                {
                    if (Bag* pBag = pPlayer->GetBagByPos(i))
                        for (uint32 j = 0; j < pBag->GetBagSize(); j++)
                        {
                            if(limit > 30)
                                break;
                            if (Item* pItem = pPlayer->GetItemByPos(i, j))
                            {
                                uint32 Display = pItem->GetProto()->DisplayInfoID;
                                if(pPlayer->SuitableForTransmogrification(OLD, pItem) == ERR_FAKE_OK)
                                    if(Items[GUID].find(Display) == Items[GUID].end())
                                    {
                                        limit++;
                                        Items[GUID][Display] = pItem;
                                        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, pItem->GetProto()->Name1, uiAction, Display, "Using this item for transmogrify will bind it to you and make it non-refundable and non-tradeable.nDo you wish to continue?", 0, false);
                                    }
                            }
                        }
                }

                pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Remove transmogrification", EQUIPMENT_SLOT_END+3, uiAction, "Remove transmogrification from "+(std::string)GetSlotName(uiAction)+"?", 0, false);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back..", EQUIPMENT_SLOT_END+1, 0);
                pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pUnit->GetObjectGuid());
            }
            else
                GossipHello_transmog(pPlayer, pUnit);
        } break;
    case EQUIPMENT_SLOT_END+1: // Back
        {
            GossipHello_transmog(pPlayer, pUnit);
        } break;
    case EQUIPMENT_SLOT_END+2: // Remove Transmogrifications
        {
            bool removed = false;
            for (uint8 Slot = EQUIPMENT_SLOT_START; Slot < EQUIPMENT_SLOT_END; Slot++)
                if (Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, Slot))
                    if(pItem->DeleteFakeEntry() && !removed)
                            removed = true;
            if(removed)
            {
                pPlayer->GetSession()->SendAreaTriggerMessage("Transmogrifications removed from equipped items");
                pPlayer->PlayDirectSound(3337);
            }
            else
                pPlayer->GetSession()->SendNotification("You have no transmogrified items equipped");
            GossipHello_transmog(pPlayer, pUnit);
        } break;
    case EQUIPMENT_SLOT_END+3: // Remove Transmogrification from single item
        {
            if (Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, uiAction))
            {
                if(pItem->DeleteFakeEntry())
                {
                    pPlayer->GetSession()->SendAreaTriggerMessage("%s transmogrification removed", GetSlotName(uiAction));
                    pPlayer->PlayDirectSound(3337);
                }
                else
                    pPlayer->GetSession()->SendNotification("No transmogrification on %s slot", GetSlotName(uiAction));
            }
            GossipSelect_transmog(pPlayer, pUnit, EQUIPMENT_SLOT_END, uiAction);
        } break;
    default: // Transmogrify
        {
            uint32 GUID = pPlayer->GetGUIDLow();
            if(Item* OLD = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, sender))
            {
                if(Items[GUID].find(uiAction) != Items[GUID].end() && Items[GUID][uiAction]->IsInWorld())
                {
                    Item* pItem = Items[GUID][uiAction];
                    if(pItem->GetOwnerGuid() == pPlayer->GetObjectGuid() && (pItem->IsInBag() || pItem->GetBagSlot() == INVENTORY_SLOT_BAG_0) && pPlayer->SuitableForTransmogrification(OLD, pItem) == ERR_FAKE_OK)
                    {
                        OLD->SetFakeEntry(pItem->GetEntry());
                        pItem->SetBinding(true);
                        pPlayer->PlayDirectSound(3337);
                        pPlayer->GetSession()->SendAreaTriggerMessage("%s transmogrified", GetSlotName(sender));
                    }
                    else
                        pPlayer->GetSession()->SendNotification("Selected items are not suitable");
                }
                else
                    pPlayer->GetSession()->SendNotification("Selected item does not exist");
            }
            else
                pPlayer->GetSession()->SendNotification("Equipment slot is empty");
            Items[GUID].clear();
            GossipSelect_transmog(pPlayer, pUnit, EQUIPMENT_SLOT_END, sender);
        } break;
    }
    return true;
}

typedef UNORDERED_MAP<ObjectGuid, uint32> AttackerMap;

struct MANGOS_DLL_DECL npc_training_dummyAI : public Scripted_NoMovementAI
{
    npc_training_dummyAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature){}

    AttackerMap m_AttackerMap;

    void Reset()
    {
        m_creature->SetStunned(true);
    }

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
    pNewScript->pGossipSelectWithCode = &GossipSelectWithCode_telenpc;
    pNewScript->RegisterSelf();

    pNewScript                  = new Script;
    pNewScript->Name            = "npc_beastmaster";
    pNewScript->pGossipHello    = &GossipHello_beast_master;
    pNewScript->pGossipSelect   = &GossipSelect_beast_master;
    pNewScript->RegisterSelf();

    pNewScript                  = new Script;
    pNewScript->Name            = "npc_transmog";
    pNewScript->pGossipHello    = &GossipHello_transmog;
    pNewScript->pGossipSelect   = &GossipSelect_transmog;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "training_dummy";
    pNewScript->GetAI = &GetAI_npc_training_dummy;
    pNewScript->RegisterSelf();
}