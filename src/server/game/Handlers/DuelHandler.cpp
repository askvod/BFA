/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Log.h"
#include "Player.h"
#include "Pet.h"

void WorldSession::HandleDuelAcceptedOpcode(WorldPacket& recvPacket)
{
    ObjectGuid guid;
    Player* player;
    Player* plTarget;

    recvPacket >> guid;

    if (!GetPlayer()->duel)                                  // ignore accept from duel-sender
        return;

    player = GetPlayer();
    plTarget = player->duel->opponent;

    if (player == player->duel->initiator || !plTarget || player == plTarget || player->duel->startTime != 0 || plTarget->duel->startTime != 0)
        return;

    //TC_LOG_DEBUG("network", "WORLD: Received CMSG_DUEL_ACCEPTED");
    TC_LOG_DEBUG("network", "Player 1 is: %u (%s)", player->GetGUID().GetCounter(), player->GetName().c_str());
    TC_LOG_DEBUG("network", "Player 2 is: %u (%s)", plTarget->GetGUID().GetCounter(), plTarget->GetName().c_str());

    time_t now = time(NULL);
    player->duel->startTimer = now;
    plTarget->duel->startTimer = now;
    
	if (player->GetZoneId() != 4080)
	{
		player->SetHealth(player->GetMaxHealth());
		player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
  	    plTarget->SetHealth(plTarget->GetMaxHealth());
		plTarget->SetPower(POWER_MANA, plTarget->GetMaxPower(POWER_MANA));
		if (player->getClass() == CLASS_DEATH_KNIGHT)
			player->SetPower(POWER_RUNIC_POWER, 0); // Reset DeathKnight Power
		if (plTarget->getClass() == CLASS_DEATH_KNIGHT)
			plTarget->SetPower(POWER_RUNIC_POWER, 0);  // Reset DeathKnight Power
		if (player->getClass() == CLASS_WARRIOR)
			player->SetPower(POWER_RAGE, 0);  // Reset Warrior Rage
		if (plTarget->getClass() == CLASS_WARRIOR)
			plTarget->SetPower(POWER_RAGE, 0); // Reset Warrior Rage
		if (player->getClass() == CLASS_ROGUE)
			player->SetPower(POWER_ENERGY, player->GetMaxPower(POWER_ENERGY));  // Reset Warrior Rage
		if (plTarget->getClass() == CLASS_ROGUE)
			plTarget->SetPower(POWER_ENERGY, plTarget->GetMaxPower(POWER_ENERGY)); // Reset Warrior Rage
            
        if (Pet* pet = player->GetPet())
        {
            pet->SetFullHealth();
            pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));
        }

        if (Pet* pet = plTarget->GetPet())
        {
            pet->SetFullHealth();
            pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));
        }

		player->RemoveArenaSpellCooldowns(true); // Pet & Player's spell cooldowns
		plTarget->RemoveArenaSpellCooldowns(true); // Pet & Player's spell cooldowns
		player->RemoveAura(57723);
		player->RemoveAura(57724);
		player->RemoveAura(25771);
		player->RemoveAura(41425);
		player->RemoveAura(61987);
		plTarget->RemoveAura(57723);
		plTarget->RemoveAura(57724);
		plTarget->RemoveAura(25771);
		plTarget->RemoveAura(41425);
		plTarget->RemoveAura(61987);
        player->RemoveAura(15007);
        plTarget->RemoveAura(15007);
		plTarget->ClearDiminishings();
		player->ClearDiminishings();
	}

    player->SendDuelCountdown(3000);
    plTarget->SendDuelCountdown(3000);
}

void WorldSession::HandleDuelCancelledOpcode(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_DUEL_CANCELLED");
    ObjectGuid guid;
    recvPacket >> guid;

    // no duel requested
    if (!GetPlayer()->duel)
        return;
        
    // player surrendered in a duel using /forfeit
    if (GetPlayer()->duel->startTime != 0)
    {
        GetPlayer()->CombatStopWithPets(true);
        if (GetPlayer()->duel->opponent)
            GetPlayer()->duel->opponent->CombatStopWithPets(true);

        GetPlayer()->CastSpell(GetPlayer(), 7267, true);    // beg
        GetPlayer()->DuelComplete(DUEL_WON);
        return;
    }

    GetPlayer()->DuelComplete(DUEL_INTERRUPTED);
}
