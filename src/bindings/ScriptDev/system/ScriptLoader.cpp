/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

extern void AddSC_battleground();
extern void AddSC_npc_scripts();
extern void AddSC_boss_doomwalker();
extern void AddSC_boss_doomlordkazzak();

void AddScripts()
{
    AddSC_battleground();
	AddSC_npc_scripts();
    AddSC_boss_doomwalker();
    AddSC_boss_doomlordkazzak();
}
