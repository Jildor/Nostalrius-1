/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Stormwind_City
SD%Complete: 100
SDComment: Quest support: 1640, 1447, 4185, 11223 (DB support required for spell 42711)
SDCategory: Stormwind City
EndScriptData */

/* ContentData
npc_archmage_malin
npc_bartleby
npc_dashel_stonefist
npc_lady_katrana_prestor
EndContentData */

#include "scriptPCH.h"
#include "./../../custom/PlayerStartMgr.h"
#include <list>

/*######
## npc_archmage_malin
######*/

#define GOSSIP_ITEM_MALIN "Can you send me to Theramore? I have an urgent message for Lady Jaina from Highlord Bolvar."

bool GossipHello_npc_archmage_malin(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(11223) == QUEST_STATUS_COMPLETE && !pPlayer->GetQuestRewardStatus(11223))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_archmage_malin(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 42711, true);
    }

    return true;
}

/*######
## npc_bartleby
######*/

enum
{
    FACTION_ENEMY       = 168,
    QUEST_BEAT          = 1640
};

struct npc_bartlebyAI : public ScriptedAI
{
    npc_bartlebyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormalFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormalFaction;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormalFaction)
            m_creature->setFaction(m_uiNormalFaction);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || ((m_creature->GetHealth() - uiDamage) * 100 / m_creature->GetMaxHealth() < 15))
        {
            uiDamage = 0;

            if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
                ((Player*)pDoneBy)->AreaExploredOrEventHappens(QUEST_BEAT);

            EnterEvadeMode();
        }
    }
};

bool QuestAccept_npc_bartleby(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BEAT)
    {
        pCreature->setFaction(FACTION_ENEMY);
        ((npc_bartlebyAI*)pCreature->AI())->AttackStart(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_bartleby(Creature* pCreature)
{
    return new npc_bartlebyAI(pCreature);
}

/*######
## npc_dashel_stonefist
######*/

enum
{
    QUEST_MISSING_DIPLO_PT8     = 1447,
    FACTION_HOSTILE             = 168
};

struct npc_dashel_stonefistAI : public ScriptedAI
{
    npc_dashel_stonefistAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormalFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormalFaction;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormalFaction)
            m_creature->setFaction(m_uiNormalFaction);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || ((m_creature->GetHealth() - uiDamage) * 100 / m_creature->GetMaxHealth() < 15))
        {
            uiDamage = 0;

            if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
                ((Player*)pDoneBy)->AreaExploredOrEventHappens(QUEST_MISSING_DIPLO_PT8);

            EnterEvadeMode();
        }
    }
};

bool QuestAccept_npc_dashel_stonefist(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_DIPLO_PT8)
    {
        pCreature->setFaction(FACTION_HOSTILE);
        ((npc_dashel_stonefistAI*)pCreature->AI())->AttackStart(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_dashel_stonefist(Creature* pCreature)
{
    return new npc_dashel_stonefistAI(pCreature);
}

/*######
## npc_lady_katrana_prestor
######*/

#define GOSSIP_ITEM_KAT_1 "Pardon the intrusion, Lady Prestor, but Highlord Bolvar suggested that I seek your advice."
#define GOSSIP_ITEM_KAT_2 "My apologies, Lady Prestor."
#define GOSSIP_ITEM_KAT_3 "Begging your pardon, Lady Prestor. That was not my intent."
#define GOSSIP_ITEM_KAT_4 "Thank you for your time, Lady Prestor."

bool GossipHello_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(4185) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(2693, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2694, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(2695, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(2696, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(4185);
            break;
    }
    return true;
}
/*######
## Quest #434 The Attack
######*/

enum QuestTheAttack
{
    QUEST_ITEMS_OF_SOME_CONSEQUENCE = 2746, // prequest for 434
    QUEST_THE_ATTACK                = 434,

    FACTION_ENEMYY                  = 14,
    FACTION_NORMAL_LESCOVAR         = 12,
    FACTION_NORMAL_MARZON           = 84,

    NPC_LORD_GREGOR_LESCOVAR        = 1754,
    NPC_MARZON_THE_SILENT_BLADE     = 1755,
    NPC_STORMWIND_ROYAL_GUARD       = 1756,
    NPC_PRIESTRESS                  = 7779,
    NPC_TYRION                      = 7766,
    NPC_TYRIONS_SPYBOT              = 8856,

    MODEL_TYRIANA                   = 6703,
    MODEL_SPYBOT                    = 1159,

    GOSSIP_TEXT_ID_EVENT_RUNNING    = 2394,

    SAY_SPYBOT                      = -1700000,
    SAY_TYRION_1                    = -1700001,
    SAY_TYRIONA_1                   = -1700002,
    SAY_ROYAL_GUARD_1               = -1700003,
    SAY_TYRIONA_2                   = -1700004,
    SAY_TYRIONA_3                   = -1700005,
    SAY_GREGOR_1                    = -1700006,
    SAY_TYRIONA_4                   = -1700007,
    SAY_GREGOR_2                    = -1700008,
    SAY_ROYAL_GUARD_2               = -1700009,
    SAY_GREGOR_3                    = -1700010,
    SAY_GREGOR_4                    = -1700011,
    SAY_MARZON_1                    = -1700012,
    SAY_GREGOR_5                    = -1700013,
    SAY_MARZON_2                    = -1700014,
    SAY_TYRION_2                    = -1700015,
    SAY_GREGOR_6                    = -1700016,
    SAY_MARZON_3                    = -1700017,

    SPELL_STEALTH                   = 8874, // for Marzon
};


/*######
## npc_tyrion
######*/
struct npc_tyrionAI : public ScriptedAI
{
    npc_tyrionAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_IsEventRunning;

    void GetAIInformation(ChatHandler& reader)
    {
        ScriptedAI::GetAIInformation(reader);
        reader.PSendSysMessage("TYRION: Event running: [%s]", m_IsEventRunning ? "YES" : "NO");
    }

    void Reset() override
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_IsEventRunning = false;
    }

    bool AreCreaturesRequiredForQuestPresent(float fMaxSearchRange = 40.0f)
    {
        // m_guidTyrion Spybot
        if (!GetClosestCreatureWithEntry(m_creature, NPC_TYRIONS_SPYBOT, VISIBLE_RANGE))
            return false;
        return true;
    }

};

CreatureAI* GetAI_npc_tyrion(Creature* pCreature)
{
    return new npc_tyrionAI(pCreature);
}


/*######
## npc_lord_gregor_lescovar
######*/
struct npc_lord_gregor_lescovarAI : public npc_escortAI
{
    npc_lord_gregor_lescovarAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    ObjectGuid m_guidGuard1;
    ObjectGuid m_guidGuard2;
    ObjectGuid m_guidMarzon;
    ObjectGuid m_guidTyrion;
    ObjectGuid m_guidPriestress;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiEventTimer = 0;
        m_uiEventPhase = 0;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_creature->getFaction() == FACTION_ENEMYY)
            m_creature->setFaction(FACTION_NORMAL_LESCOVAR);

        if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
            if (!pMarzon->isAlive())
            {
                if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                {
                    pGuard1->ForcedDespawn(0);
                    pGuard1->Respawn();
                }
                if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
                {
                    pGuard2->ForcedDespawn(0);
                    pGuard2->Respawn();
                }
                if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                    pPriestress->Respawn();

                if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                {
                    pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                        ptyrionAI->Reset();
                }
            }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() != NPC_MARZON_THE_SILENT_BLADE)
            return;

        if (!m_creature->isAlive())
        {
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
            {
                pGuard1->ForcedDespawn(0);
                pGuard1->Respawn();
            }
            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
            {
                pGuard2->ForcedDespawn(0);
                pGuard2->Respawn();
            }
            if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                pPriestress->Respawn();

            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
            {
                pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                    ptyrionAI->m_IsEventRunning = false;
            }
        }
    }

    void Aggro(Unit* pAttacker)
    {
        if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
        {
            DoScriptText(SAY_MARZON_3, pMarzon);
            DoScriptText(SAY_GREGOR_6, m_creature);
            pMarzon->AI()->AttackStart(pAttacker);
        }
        m_uiEventPhase = 13;
    }

    void SummonedCreatureDespawn(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() != NPC_MARZON_THE_SILENT_BLADE)
            return;

        if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
        {
            pGuard1->ForcedDespawn(0);
            pGuard1->Respawn();
        }

        if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
        {
            pGuard2->ForcedDespawn(0);
            pGuard2->Respawn();
        }

        if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
            pPriestress->Respawn();

        if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
        {
            pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                ptyrionAI->m_IsEventRunning = false;
        }

    }

    void WaypointReached(uint32 uiPoint)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch (uiPoint)
        {
        case 13:
            SetEscortPaused(false);
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
              if (pGuard1->isAlive())
                  pGuard1->SetFacingToObject(m_creature);
            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
              if (pGuard2->isAlive())
                  pGuard2->SetFacingToObject(m_creature);
            DoScriptText(SAY_GREGOR_2, m_creature);
            m_uiEventPhase = 1;
            m_uiEventTimer = 3500;
            SetEscortPaused(true);
            break;
        case 17:
            m_uiEventPhase = 4;
            m_uiEventTimer = 1500;
            SetEscortPaused(true);
            break;
        case 20:
            m_creature->setFaction(FACTION_NORMAL_LESCOVAR);
            if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
            {
                pMarzon->CastSpell(pMarzon, SPELL_STEALTH, true);
                pMarzon->ForcedDespawn(0);
            }
            break;
        case 21:
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
            {
                pGuard1->ForcedDespawn(0);
                pGuard1->Respawn();
            }
            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
            {
                pGuard2->ForcedDespawn(0);
                pGuard2->Respawn();
            }
            if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                pPriestress->Respawn();

            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
            {
                pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                    ptyrionAI->m_IsEventRunning = false;
            }

            if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                if (!pMarzon->isInCombat())
                    pMarzon->ForcedDespawn(0);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventPhase)
                {
                case 1:
                    if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                        if (pGuard1->isAlive() && !pGuard1->getVictim())
                        {
                            DoScriptText(SAY_ROYAL_GUARD_2, pGuard1);
                            pGuard1->GetMotionMaster()->MovePoint(0, -8364.07f, 406.775f, 122.274f, MOVE_PATHFINDING);
                        }
                    if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
                        if (pGuard2->isAlive() && !pGuard2->getVictim())
                        {
                            DoScriptText(SAY_ROYAL_GUARD_2, pGuard2);
                            pGuard2->GetMotionMaster()->MovePoint(0, -8353.91f, 415.318f, 122.274f, MOVE_PATHFINDING);
                        }
                    ++m_uiEventPhase;
                    m_uiEventTimer = 3000;
                    break;
                case 2:
                    ++m_uiEventPhase;
                    SetEscortPaused(false);
                    break;
                case 4:
                    if (Creature* pMarzon = m_creature->SummonCreature(NPC_MARZON_THE_SILENT_BLADE, -8407.71f, 482.117f, 123.76f, 4.79f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1 * MINUTE*IN_MILLISECONDS))
                    {
                        pMarzon->CastSpell(pMarzon, SPELL_STEALTH, true);
                        pMarzon->GetMotionMaster()->MovePoint(0, -8406.00f, 470.00f, 123.76f, MOVE_PATHFINDING);
                        m_guidMarzon = pMarzon->GetObjectGuid();
                    }
                    m_uiEventTimer = 10000;
                    ++m_uiEventPhase;
                    SetEscortPaused(true);
                    break;
                case 5:
                    DoScriptText(SAY_GREGOR_3, m_creature);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 6:

                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                    {
                        pMarzon->RemoveAura(SPELL_STEALTH, EFFECT_INDEX_0);
                        m_creature->SetFacingToObject(pMarzon);
                    }
                    DoScriptText(SAY_GREGOR_4, m_creature);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 7:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                        DoScriptText(SAY_MARZON_1, pMarzon);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 8:
                    DoScriptText(SAY_GREGOR_5, m_creature);
                    m_uiEventTimer = 4000;
                    ++m_uiEventPhase;
                    break;
                case 9:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                        DoScriptText(SAY_MARZON_2, pMarzon);
                    m_uiEventTimer = 3000;
                    ++m_uiEventPhase;
                    break;
                case 10:
                    if (Player* pPlayer = GetPlayerForEscort())
                        pPlayer->GroupEventHappens(QUEST_THE_ATTACK, m_creature);
                    if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                        DoScriptText(SAY_TYRION_2, pTyrion);
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                        pMarzon->setFaction(FACTION_ENEMYY);
                    m_creature->setFaction(FACTION_ENEMYY);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 11:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                    {
                        if (!m_creature->isInCombat())
                            pMarzon->GetMotionMaster()->MovePoint(0, -8419.00f, 486.26f, 123.75f, MOVE_PATHFINDING);
                        else if (m_creature->getVictim())
                            pMarzon->AI()->AttackStart(m_creature->getVictim());
                    }
                    ++m_uiEventPhase;
                    SetEscortPaused(false);
                    break;
                case 12:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                      if (pMarzon->isInCombat() && pMarzon->getVictim() && !m_creature->isInCombat())
                          m_creature->AI()->AttackStart(pMarzon->getVictim());
                    break;
                case 13:
                    if (Player* pPlayer = GetPlayerForEscort())
                        if (pPlayer->isDead())
                        {
                            SetEscortPaused(false);
                            m_creature->setFaction(FACTION_NORMAL_LESCOVAR);
                            if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                                if (!pMarzon->isDead() && pMarzon->getFaction() == FACTION_ENEMYY)
                                    pMarzon->setFaction(FACTION_NORMAL_MARZON);
                        }
                    if (!m_creature->isInCombat())
                        SetEscortPaused(false);
                    break;
                }
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
          if (pMarzon && pMarzon->isDead() && pMarzon->getFaction() == FACTION_ENEMYY)
              pMarzon->setFaction(FACTION_NORMAL_MARZON);

        npc_escortAI::UpdateAI(uiDiff);

        if (m_creature->SelectHostileTarget() || m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_npc_lord_gregor_lescovar(Creature* pCreature)
{
    return new npc_lord_gregor_lescovarAI(pCreature);
}


/*######
## npc_tyrion_spybot
######*/

struct npc_tyrion_spybotAI : public npc_escortAI
{
    npc_tyrion_spybotAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_guidGuard1;
    ObjectGuid m_guidGuard2;
    ObjectGuid m_guidLordGregor;
    ObjectGuid m_guidTyrion;
    ObjectGuid m_guidMarzon;
    ObjectGuid m_guidPriestress;
    uint8 m_uiGardenGuardsCounter;

    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    float m_fDefaultScaleSize;

    void Reset()
    {

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_fDefaultScaleSize = 0.30f;
        m_uiEventTimer = 0;
        m_uiEventPhase = 0;
        m_uiGardenGuardsCounter = 0;
        m_creature->SetDisplayId(MODEL_SPYBOT);

        if (Creature* tyrion = GetClosestCreatureWithEntry(m_creature, NPC_TYRION, VISIBLE_RANGE))
        {
            if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(tyrion->AI()))
            {
                ptyrionAI->m_IsEventRunning = false;
                tyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            }
        }
    }

    void Aggro(Unit* pAttacker) {}


    bool AreCreaturesRequiredForQuestPresent(float fMaxSearchRange = 40.0f)
    {
        if (m_guidTyrion && m_guidLordGregor && m_guidGuard1 && m_guidGuard2)
            return true;

        // m_guidPriestress
        if (!m_guidPriestress)
            if (Creature* pPriestress = GetClosestCreatureWithEntry(m_creature, NPC_PRIESTRESS, VISIBLE_RANGE))
            {
                if (!pPriestress)
                    return false;
                else if (pPriestress->GetEntry() != NPC_PRIESTRESS)
                    return false;
                else
                    m_guidPriestress = pPriestress->GetObjectGuid();
            }

        // m_guidTyrion
        if (!m_guidTyrion)
            if (Creature* pTyrion = GetClosestCreatureWithEntry(m_creature, NPC_TYRION, VISIBLE_RANGE))
            {
                if (!pTyrion)
                    return false;
                else if (pTyrion->GetEntry() != NPC_TYRION)
                    return false;
                else
                    m_guidTyrion = pTyrion->GetObjectGuid();
            }

        // Lord Gregor
        if (!m_guidLordGregor)
            if (Creature* pGregor = GetClosestCreatureWithEntry(m_creature, NPC_LORD_GREGOR_LESCOVAR, VISIBLE_RANGE))
            {
                if (!pGregor)
                    return false;
                else if (!pGregor->isAlive())
                    pGregor->Respawn();
                m_guidLordGregor = pGregor->GetObjectGuid();
            }
        // Stormwind Royal Guards (Garden)
        if (!m_guidGuard1 || !m_guidGuard2)
        {
            std::list<Creature*> lGardenStormwindRoyalGuards;
            GetCreatureListWithEntryInGrid(lGardenStormwindRoyalGuards, m_creature, NPC_STORMWIND_ROYAL_GUARD, fMaxSearchRange);

            if (lGardenStormwindRoyalGuards.empty())
                return true;

            for (std::list<Creature*>::iterator iter = lGardenStormwindRoyalGuards.begin(); iter != lGardenStormwindRoyalGuards.end(); ++iter)
            {
                Creature* GardenStormwindRoyalGuard = (*iter);
                if (GardenStormwindRoyalGuard->isDead() || !GardenStormwindRoyalGuard->isAlive())
                    GardenStormwindRoyalGuard->Respawn();

                if (m_uiGardenGuardsCounter == 0)
                {
                    m_guidGuard1 = GardenStormwindRoyalGuard->GetObjectGuid();
                    m_uiGardenGuardsCounter++;
                    continue;
                }
                else if (m_uiGardenGuardsCounter == 1)
                {
                    m_guidGuard2 = GardenStormwindRoyalGuard->GetObjectGuid();
                    m_uiGardenGuardsCounter++;
                    continue;
                }
            }
            return true;
        }
        return true;
    }

    void WaypointReached(uint32 uiPoint)
    {
        if (!AreCreaturesRequiredForQuestPresent())
            return;

        switch (uiPoint)
        {
        case 1:
            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
            {
                pTyrion->SetFacingToObject(m_creature);
                pTyrion->HandleEmote(EMOTE_STATE_USESTANDING);
            }
            m_fDefaultScaleSize = m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
            m_creature->SetDisplayId(MODEL_TYRIANA);
            m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.00f);

            SetEscortPaused(true);
            m_uiEventTimer = 5000;
            m_uiEventPhase = 1;
            break;
        case 2:
            if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                pPriestress->ForcedDespawn(0);
            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                DoScriptText(SAY_TYRION_1, pTyrion, GetPlayerForEscort());
            break;
        case 6:
            DoScriptText(SAY_TYRIONA_1, m_creature);
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
            {
                if (pGuard1 && pGuard1->isAlive() && !pGuard1->getVictim())
                    pGuard1->SetFacingToObject(m_creature);
            }

            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
            {
                if (pGuard2 && pGuard2->isAlive() && !pGuard2->getVictim())
                    pGuard2->SetFacingToObject(m_creature);
            }

            m_uiEventPhase = 2;
            m_uiEventTimer = 5000;
            SetEscortPaused(true);
            break;
        case 18:
            DoScriptText(SAY_TYRIONA_3, m_creature);
            if (Creature* gregor = m_creature->GetMap()->GetCreature(m_guidLordGregor))
                gregor->SetFacingToObject(m_creature);
            m_uiEventPhase = 5;
            m_uiEventTimer = 5000;
            SetEscortPaused(true);
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventPhase)
                {
                case 1:
                    if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                        pTyrion->HandleEmote(EMOTE_ONESHOT_NONE);
                    SetEscortPaused(false);
                    break;
                case 2:
                    if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                    {
                        DoScriptText(SAY_ROYAL_GUARD_1, pGuard1);
                        m_creature->SetFacingToObject(pGuard1);
                    }
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 3:
                    DoScriptText(SAY_TYRIONA_2, m_creature);
                    if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                        pGuard1->HandleEmote(EMOTE_ONESHOT_KNEEL);
                    if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
                        pGuard2->HandleEmote(EMOTE_ONESHOT_KNEEL);
                    ++m_uiEventPhase; // 4 = nothing (It's OK)
                    SetEscortPaused(false);
                    break;
                case 5:
                    if (Creature* gregor = m_creature->GetMap()->GetCreature(m_guidLordGregor))
                        DoScriptText(SAY_GREGOR_1, gregor);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 6:
                    DoScriptText(SAY_TYRIONA_4, m_creature);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 7:
                    if (Player* pPlayer = GetPlayerForEscort())
                        if (Creature* gregor = m_creature->GetMap()->GetCreature(m_guidLordGregor))
                            if (npc_lord_gregor_lescovarAI* pGregorEscortAI = dynamic_cast<npc_lord_gregor_lescovarAI*>(gregor->AI()))
                            {
                                pGregorEscortAI->SetMaxPlayerDistance(200.0f);
                                pGregorEscortAI->Start(false, false, pPlayer->GetGUID());
                                pGregorEscortAI->m_guidGuard1 = m_guidGuard1;
                                pGregorEscortAI->m_guidGuard2 = m_guidGuard2;
                                pGregorEscortAI->m_guidTyrion = m_guidTyrion;
                                pGregorEscortAI->m_guidPriestress = m_guidPriestress;
                            }
                    m_creature->SetDisplayId(MODEL_SPYBOT);
                    m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, m_fDefaultScaleSize);
                    ++m_uiEventPhase;
                    m_uiEventTimer = 1000;
                    break;
                case 8:
                    SetEscortPaused(false);
                    break;
                }
            }
            else m_uiEventTimer -= uiDiff;
        }

        npc_escortAI::UpdateAI(uiDiff);
    }
};

CreatureAI* GetAI_npc_tyrion_spybot(Creature* pCreature)
{
    return new npc_tyrion_spybotAI(pCreature);
}

/*######
## npc_tyrion
######*/

bool GossipHello_npc_tyrion(Player* pPlayer, Creature* pCreature)
{
    bool bIsEventReady = false;

    if (npc_tyrionAI* pTyrion = dynamic_cast<npc_tyrionAI*>(pCreature->AI()))
        bIsEventReady =  (!pTyrion->m_IsEventRunning && pTyrion->AreCreaturesRequiredForQuestPresent());

    // Check if event is possible and also check the status of the quests
    if (!bIsEventReady)
    {
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_EVENT_RUNNING, pCreature->GetObjectGuid());
    }
    else
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

        pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetObjectGuid());
    }

    return true;
}


bool QuestAccept_npc_tyrion(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pCreature->AI()))
        if (ptyrionAI->m_IsEventRunning)
            return false;

    if (pQuest->GetQuestId() == QUEST_THE_ATTACK)
    {
        if (Creature* TyrionSpyBot = GetClosestCreatureWithEntry(pCreature, NPC_TYRIONS_SPYBOT, VISIBLE_RANGE))
        {
            if (npc_tyrion_spybotAI* pSpybotEscortAI = dynamic_cast<npc_tyrion_spybotAI*>(TyrionSpyBot->AI()))
            {
                DoScriptText(SAY_SPYBOT, TyrionSpyBot);
                pSpybotEscortAI->SetMaxPlayerDistance(200.0f);
                pSpybotEscortAI->Start(false, false, pPlayer->GetGUID(), NULL, false, false);
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pCreature->AI()))
                    ptyrionAI->m_IsEventRunning = true;
            }
        }
    }
    return true;
}
// Elysium

// Ecuyer Rowe

struct RoweMove
{
    float X;
    float Y;
    float Z;
    float O;
};

static RoweMove RoweDeplacement[] =
{
    { -9058.072266f, 441.321320f, 93.056519f, 3.836762f},
    { -9084.878906f, 419.230377f, 92.421455f, 3.824980f}
};

struct WindsorMove
{
    float X;
    float Y;
    float Z;
    float O;
};

static WindsorMove WindsorDeplacement[] =
{
    { -9050.406250f, 443.974792f, 93.056458f, 0.659825f},   // Descend de cheval
    { -8968.008789f, 509.771759f, 96.350754f, 0.679460f},   // S'arrete et parle avec le general
    { -8954.638672f, 519.920410f, 96.355453f, 0.680187f},
    { -8933.738281f, 500.683533f, 93.842247f, 5.941573f},
    { -8923.248047f, 496.464294f, 93.858475f, 0.688045f},
    { -8907.830078f, 509.035645f, 93.842529f, 2.163023f},
    { -8927.302734f, 542.173523f, 94.291695f, 0.606364f},
    { -8825.773438f, 623.565918f, 93.838066f, 5.484471f},
    { -8795.209961f, 590.400269f, 97.495560f, 0.644063f},
    { -8769.717773f, 608.193298f, 97.130692f, 5.507248f},
    { -8736.326172f, 574.955811f, 97.385048f, 3.886185f},
    { -8749.043945f, 560.525330f, 97.400307f, 5.502535f},
    { -8730.701172f, 540.466370f, 101.105370f, 5.431850f},
    { -8713.182617f, 519.765442f, 97.185402f, 0.707678f},
    { -8673.321289f, 554.135986f, 97.267708f, 6.174048f},
    { -8651.262695f, 551.696045f, 97.002983f, 5.525308f},
    { -8618.138672f, 518.573425f, 103.123642f, 5.497819f},
    { -8566.013672f, 465.536804f, 104.597160f, 5.481625f},
    { -8548.403320f, 466.680695f, 104.533554f, 5.387382f},  // S'arrete en bas du chateau de SW, gossip pour continuer l'escorte
    { -8529.294922f, 443.376495f, 104.917046f, 5.399162f},
    { -8507.087891f, 415.847321f, 108.385857f, 5.371674f},
    { -8486.496094f, 389.750427f, 108.590248f, 5.391308f},
    { -8455.687500f, 351.054321f, 120.885910f, 5.391308f},
    { -8446.392578f, 339.602783f, 121.329506f, 5.359892f}
};

static WindsorMove WindsorEventMove[] =
{
    { -8964.973633f, 512.194519f, 96.355247f, 3.835189f},   // Position du general
    { -8963.196289f, 510.056549f, 96.355240f, 3.835189f},   // Add1 du général
    { -8961.235352f, 507.696808f, 96.595337f, 3.835189f},   // Add2 du général
    { -8959.596680f, 505.725403f, 96.595490f, 3.835189f},   // Add3 du général
    { -8967.410156f, 515.123535f, 96.354881f, 3.835189f},   // Add4 du général
    { -8968.840820f, 516.844482f, 96.595253f, 3.835189f},   // Add5 du général
    { -8970.687500f, 519.065796f, 96.595245f, 3.835189f},   // Add6 du général
    { -8958.585938f, 506.907959f, 96.595634f, 2.294317f},   // Add1 à genoux
    { -8960.827148f, 505.079407f, 96.593971f, 2.255047f},   // Add2 à genoux
    { -8962.866211f, 503.415009f, 96.591331f, 2.255047f},   // Add3 à genoux
    { -8969.562500f, 520.014587f, 96.595673f, 5.388790f},   // Add4 à genoux
    { -8971.773438f, 518.239868f, 96.594200f, 5.388790f},   // Add5 à genoux
    { -8973.923828f, 516.513611f, 96.590904f, 5.475183f},   // Add6 à genoux
    { -8976.549805f, 514.405701f, 96.590057f, 5.388790f},   // General à genoux
    { -8506.340820f, 338.364441f, 120.885841f, 6.219385f},  // Roi dans la salle des coffres
    { -8449.006836f, 337.693451f, 121.329552f, 5.826686f}   // Bolvar a coté de Windsor
};

uint32 ParlotteWindsor[] =
{
    10,//"Emparez vous de lui! Emparez vous de ce vaurien de hors la loi et de ses alliés!",
    11,//"Reginald, tu sais que je ne peux pas te laisser passer.",
    12,//"Fais ce quil te semble juste Marcus. Nous avons servi ensemble sous les ordres de Turalyon. Il a fait de nous les hommes que nous sommes aujourdhui. Penses-tu quil se soit fourvoyé à mon sujet? "
    //    "Crois-tu vraiment que mes intentions soient dagir contre les intérêts de l'alliance? Crois-tu que je deshonorerais nos héros?",
    13,//"Me retenir nest pas la bonne décision, Marcus.",
    14,//"Jai honte, mon vieil ami. Je ne sais plus quoi faire. Je sais bien que ce nest pas toi qui taviserais à jeter le deshonneur sur les héros legendaires... cest moi. "
    //    "Cest moi et tout ces politiciens corrompus. Ils emplissent nos vies de vaines promesses, et de mensonges sans fin.",
    15,//"Nous deshonorons nos ancetres. Nous deshonorons leurs sacrifices.. pardonnes moi, Reginald.",
    16,//"Mon cher ami, tu les honores de ton estime. Tu es loyal à ton serment d'allégeance. "
    //    "Je ne doute pas un seul instant que pour ton peuple tu serais prêt à un aussi grand sacrifice que les héros sous lesquels tu te tiens.",
    17,//"Il est temps maintenant de mettre fin à son règne, Marcus. Insurges toi, mon ami.",
    18,//"Vous lavez entendu! Ne reconnaissez vous pas un héros lorsque vous en voyez un?",
    19,//"Ecartez vous! Laissez les passer!",
    20,//"Aucun mal ne doit etre fait à Reginald Windsor! Il doit traverser la ville sain et sauf!",
    21,//"Va, Reginald. Que la lumière guide tes pas!",
    22,//"Merci, mon vieil ami. Tu as fait la bonne chose.",
    23,//"Suivez-moi, mes amis. Au donjon de Stormwind!",
    24,//"Soyez brave, mes amis. Le reptile se debattra sauvagement. Cest un geste de désespoir. Lorsque vous serez prêts, dites le moi.",
    25,//"Allons-y!",
    26,//"Votre Majesté, fuyez tant quil en est encore temps. Elle nest pas celle que vous pensez...",
    27,//"Allez vous mettre en sureté dans le hall, votre Majesté.",
    28,//"La mascarade est terminée, Dame Prestor. Ou devrais-je vous appeler par votre vrai nom... Onyxia...",
    29,//"Vous serez emprisonné et jugé pour trahison, Windsor. Je jubilerai lorsque vous serez declaré coupable et condamné à mort par pendaison...",
    30,//"Et quand votre corps inerte se balancera au bout dune corde, je me delecterai de la mort de cet homme insensé... "
    //    "Après tout, quelles preuves avez vous? Pensiez vous vraiment pouvoir faire irruption ici en accusant un membre de la famille royal et vous en retourner indemne?",
    31,//"Vous n'echapperez pas à votre destin, Onyxia. Il a été prophetisé - une vision qui résonne depuis les grandes salles de Karazhan. Il sachève maintenant...",
    32,//"Les Dark Irons pensaient que ces tablettes étaient codées. Cela na rien dun code, cest la langue des anciens dragons.",
    33,//"Ecoutes, dragon. Laisse la verité résonner entre ces murs.",
    34,//"Curieux... Windsor, dans cette vision, surviviez vous? Je demande simplement parce que la seule chose dont je sois certaine, c'est de votre mort. Ici et maintenant.",
    35,//"Vil dragon! Gardes! Gardes! Emparez vous de ce monstre!",
    36,//"NE LA LAISSEZ PAS S'ECHAPPER!",
    37,//"Reginald... Je... Je suis desolé.",
    38//"Bol... Bolvar... le medaillon... utilisez... "
};

struct npc_RoweAI : public ScriptedAI
{
    npc_RoweAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 Timer;
    uint32 Tick;
    bool EventStart;

    void Reset()
    {
        Timer = 2000;
        Tick = 0;
        EventStart = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (EventStart == false)
            return;

        if (Timer < uiDiff)
        {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            float o = 0.0f;
            float X = 0.0f;
            float Y = 0.0f;
            switch (Tick)
            {
                case 0:
                    m_creature->SetSpeedRate(MOVE_WALK, 2.0f, true);
                    m_creature->GetMotionMaster()->MovePoint(0, RoweDeplacement[0].X, RoweDeplacement[0].Y, RoweDeplacement[0].Z);
                    X = m_creature->GetPositionX() - RoweDeplacement[0].X;
                    Y = m_creature->GetPositionY() - RoweDeplacement[0].Y;
                    Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    break;
                case 1:
                    m_creature->GetMotionMaster()->MovePoint(0, RoweDeplacement[1].X, RoweDeplacement[1].Y, RoweDeplacement[1].Z);
                    X = m_creature->GetPositionX() - RoweDeplacement[1].X;
                    Y = m_creature->GetPositionY() - RoweDeplacement[1].Y;
                    Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    break;
                case 2:
                    m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                    Timer = 5000;
                    break;
                case 3:
                    m_creature->GetMotionMaster()->MovePoint(0, RoweDeplacement[1].X, RoweDeplacement[1].Y, RoweDeplacement[1].Z);
                    X = m_creature->GetPositionX() - RoweDeplacement[1].X;
                    Y = m_creature->GetPositionY() - RoweDeplacement[1].Y;
                    Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    break;
                case 4:
                    m_creature->GetMotionMaster()->MovePoint(0, RoweDeplacement[0].X, RoweDeplacement[0].Y, RoweDeplacement[0].Z);
                    X = m_creature->GetPositionX() - RoweDeplacement[0].X;
                    Y = m_creature->GetPositionY() - RoweDeplacement[0].Y;
                    Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    break;
                case 5:
                    if (Creature* Windsor = m_creature->SummonCreature(12580, -9148.395508f, 371.322174f, 90.543655f, 0.700131f, TEMPSUMMON_MANUAL_DESPAWN, 1.5 * HOUR * IN_MILLISECONDS))
                    {
                        Windsor->Mount(2410);
                        Windsor->SetSpeedRate(MOVE_WALK, 2.5f, true);
                        Windsor->GetMotionMaster()->MovePoint(0, WindsorDeplacement[0].X, WindsorDeplacement[0].Y, WindsorDeplacement[0].Z);
                        Windsor->SetRespawnDelay(100000000);
                        if (Windsor->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
                            Windsor->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    }
                    break;
                case 6:
                    m_creature->GetRespawnCoord(x, y, z);
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    X = m_creature->GetPositionX() - x;
                    Y = m_creature->GetPositionY() - y;
                    Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    break;
                case 7:
                    m_creature->GetRespawnCoord(x, y, z, &o);
                    m_creature->SetFacingTo(o);
                    Reset();
                    break;
            }
            Tick++;
        }
        else Timer -= uiDiff;
    }
};

bool GossipHello_npc_Ecuyer_Rowe(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(6402) == QUEST_STATUS_COMPLETE)
    {
        Creature* windsor = pCreature->FindNearestCreature(12580, 150.0f);
        if (!windsor || !windsor->isAlive())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Announce me to Windsor", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    }

    pPlayer->SEND_GOSSIP_MENU(9063, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_Ecuyer_Rowe(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            if (pPlayer->GetQuestStatus(6402) == QUEST_STATUS_COMPLETE)
                if (npc_RoweAI* pRoweEventAI = dynamic_cast<npc_RoweAI*>(pCreature->AI()))
                    pRoweEventAI->EventStart = true;
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
    }
    return true;
}

CreatureAI* GetAI_npc_Rowe(Creature* pCreature)
{
    return new npc_RoweAI(pCreature);
}

// Windsor

struct npc_WindsorAI : public ScriptedAI
{
    explicit npc_WindsorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    // Attention, on ne stocke pas de pointeurs ! Seulement les GUIDs !
    // Le test "if (ptr)" n'indique pas que le pointeur est valide.

    uint32 Timer;
    uint32 Tick;
    uint32 IDSpeech;
    uint64 GardesGUIDs[30];
    uint64 DragsGUIDs[10];
    uint64 JoueurGUID;
    bool Begin;
    bool BeginQuete;
    bool NeedCheck;
    bool GardeNeed[6];
    bool PhaseFinale;
    bool TheEnd;
    uint32 FinalTimer;
    uint32 GardeTimer[6];

    Player* GetPlayer()
    {
        return me->GetMap()->GetPlayer(JoueurGUID);
    }

    Creature* GetGarde(uint8 num)
    {
        return me->GetMap()->GetCreature(GardesGUIDs[num]);
    }

    void Reset()
    {
        Timer = 2000;
        Tick = 0;
        IDSpeech = 0;
        JoueurGUID = 0;
        BeginQuete = false;
        Begin = true;
        NeedCheck = false;
        PhaseFinale = false;
        TheEnd = false;
        for (int i = 0; i < 6; i++)
        {
            GardeTimer[i] = 0;
            GardeNeed[i] = false;
        }
        for (int i = 0; i < 30; i++)
            GardesGUIDs[i] = 0;
        for (int i = 0; i < 10; i++)
            DragsGUIDs[i] = 0;
    }

    void Parlotte(Unit* Parleur, bool Crier, Unit* Target = NULL)
    {
        (Crier == true) ? Parleur->MonsterYell(ELYSIUM_TEXT(ParlotteWindsor[IDSpeech]), 0, Target) : Parleur->MonsterSay(ELYSIUM_TEXT(ParlotteWindsor[IDSpeech]), 0, Target);
        IDSpeech++;
    }

    void SituationFinale()
    {
        Player* pPlayer = GetPlayer();
        std::list<Creature*> MobListe;

        GetCreatureListWithEntryInGrid(MobListe, m_creature, 12739, 150.0f);
        for (std::list<Creature*>::iterator itr = MobListe.begin(); itr != MobListe.end(); ++itr)
            (*itr)->Respawn();

        if (Creature* Bolvar = m_creature->FindNearestCreature(1748, 150.0f))
        {
            float X = 0.0f;
            float Y = 0.0f;
            float Z = 0.0f;
            Bolvar->GetRespawnCoord(X, Y, Z);
            Bolvar->GetMotionMaster()->MovePoint(0, X, Y, Z);
            Parlotte(Bolvar, false);
        }
        if (Creature* Anduin = m_creature->FindNearestCreature(1747, 150.0f))
        {
            float X = 0.0f;
            float Y = 0.0f;
            float Z = 0.0f;
            Anduin->GetRespawnCoord(X, Y, Z);
            Anduin->GetMotionMaster()->MovePoint(0, X, Y, Z);
            float x = Anduin->GetPositionX() - X;
            float y = Anduin->GetPositionY() - Y;
            FinalTimer = 1000 + sqrt((x * x) + (y * y)) / (Anduin->GetSpeed(MOVE_WALK) * 0.001f);
            PhaseFinale = true;
        }
        if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
        {
            Onyxia->RemoveAurasDueToSpell(16380);
            Onyxia->CastSpell(Onyxia, 23452, true);
        }
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        Parlotte(m_creature, false);
        if (pPlayer)
        {
            if (Group* jGroup = pPlayer->GetGroup())
            {
                for (GroupReference* pRef = jGroup->GetFirstMember(); pRef != NULL; pRef = pRef->next())
                {
                    if (pRef)
                        if (pRef->getSource()->GetQuestStatus(6403) == QUEST_STATUS_INCOMPLETE)
                            pRef->getSource()->CompleteQuest(6403);
                }
            }
            else
            {
                if (pPlayer->GetQuestStatus(6403) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->CompleteQuest(6403);
            }
        }
    }

    void UpdateAI_corpse(const uint32 uiDiff)
    {
        if (PhaseFinale == false)
            return;

        if (FinalTimer < uiDiff)
        {
            float X = 0.0f;
            float Y = 0.0f;
            float Z = 0.0f;
            float O = 0.0f;
            if (Creature* Bolvar = m_creature->FindNearestCreature(1748, 150.0f))
            {
                Bolvar->GetRespawnCoord(X, Y, Z, &O);
                Bolvar->SetFacingTo(O);
            }
            if (Creature* Anduin = m_creature->FindNearestCreature(1747, 150.0f))
            {
                Anduin->GetRespawnCoord(X, Y, Z, &O);
                Anduin->SetFacingTo(O);
            }
            if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
            {
                Onyxia->GetRespawnCoord(X, Y, Z, &O);
                Onyxia->SetFacingTo(O);
                Onyxia->SetEntry(1749);
                Onyxia->UpdateEntry(1749);
            }
            TheEnd = true;
            FinalTimer = 120000;
        }
        else FinalTimer -= uiDiff;

        if (TheEnd == true)
        {
            if (FinalTimer < uiDiff)
            {
                m_creature->DisappearAndDie();
                TheEnd = false;
            }
        }
    }

    void MoveInLineOfSight(Unit* Victim)
    {
        if (Victim && Victim->isAlive())
        {
            if (Victim->GetEntry() == 68 || Victim->GetEntry() == 1756 || Victim->GetEntry() == 1976)
            {
                if (Victim->GetDistance2d(m_creature) < 8.0f && NeedCheck == true)
                {
                    bool Continuer = true;
                    for (int i = 0; i < 30; i++)
                    {
                        if (Victim->GetGUID() == GardesGUIDs[i])
                            Continuer = false;
                    }
                    if (Continuer == true)
                    {
                        Victim->SetFacingToObject(m_creature);
                        Victim->HandleEmote(EMOTE_ONESHOT_SALUTE);
                        Victim->MonsterSay(ELYSIUM_TEXT(urand(40, 45)));
                        int Var = 0;
                        while (GardesGUIDs[Var] && Var < 29)
                            Var++;

                        GardesGUIDs[Var] = Victim->GetGUID();
                    }
                }
            }
        }
    }

    void SpellHit(Unit* Caster, const SpellEntry* SE)
    {
        if (SE->Id == 20465)
            m_creature->SetFeignDeath(true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        for (int i = 0; i < 6; i++)
        {
            if (GardeNeed[i] == true)
            {
                if (GardeTimer[i] < uiDiff)
                {
                    if (Creature* pGarde = GetGarde(i))
                    {
                        int Var = i + 7;
                        pGarde->HandleEmote(EMOTE_STATE_KNEEL);
                        pGarde->SetFacingTo(WindsorEventMove[Var].O);
                        GardeNeed[i] = false;
                    }
                }
                else GardeTimer[i] -= uiDiff;
            }
        }
        if (Begin == true)
        {
            if (m_creature->GetDistance2d(WindsorDeplacement[0].X, WindsorDeplacement[0].Y) < 2.0f)
            {
                Begin = false;
                m_creature->Unmount();
                m_creature->CastSpell(m_creature, 20000, true);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                if (Creature* Cheval = m_creature->FindNearestCreature(12581, 10.0f))
                {
                    Cheval->SetSpeedRate(MOVE_WALK, 2.5f, true);
                    Cheval->setFaction(m_creature->getFaction());
                    Cheval->GetMotionMaster()->MovePoint(0, -9148.395508f, 371.322174f, 90.543655f);
                }
                m_creature->SetSpeedRate(MOVE_WALK, 1.0f, true);
            }
        }

        if (BeginQuete == false)
            return;

        if (Timer < uiDiff)
        {
            std::list<Creature*> DragListe;
            float X = 0.0f;
            float Y = 0.0f;
            float Z = 0.0f;
            float O = 0.0f;
            uint32 eventGardId = 6;
            switch (Tick)
            {
                case 0:
                    m_creature->GetMotionMaster()->MovePoint(0, WindsorDeplacement[0].X, WindsorDeplacement[0].Y, WindsorDeplacement[0].Z);
                    X = m_creature->GetPositionX() - WindsorDeplacement[0].X;
                    Y = m_creature->GetPositionY() - WindsorDeplacement[0].Y;
                    Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    break;
                case 1:
                    for (int i = 0; i < 6; i++)
                    {
                        int Var = i + 1;
                        Creature* pSummon = m_creature->SummonCreature(68, WindsorEventMove[Var].X, WindsorEventMove[Var].Y, WindsorEventMove[Var].Z, WindsorEventMove[Var].O, TEMPSUMMON_TIMED_DESPAWN, 240 * IN_MILLISECONDS);
                        if (pSummon)
                            GardesGUIDs[i] = pSummon->GetGUID();
                    }
                    if (Creature* Onyxia = m_creature->SummonCreature(1749, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() - 2.0f, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 10 * IN_MILLISECONDS))
                    {
                        Onyxia->SetDisplayId(11686);        // Hack pour avoir le yell d'Onyxia
                        Onyxia->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Parlotte(Onyxia, true);
                    }
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        General->GetMotionMaster()->MovePoint(0, WindsorEventMove[0].X, WindsorEventMove[0].Y, WindsorEventMove[0].Z);
                        General->Unmount();
                    }
                    m_creature->GetMotionMaster()->MovePoint(0, WindsorDeplacement[1].X, WindsorDeplacement[1].Y, WindsorDeplacement[1].Z);
                    X = m_creature->GetPositionX() - WindsorDeplacement[1].X;
                    Y = m_creature->GetPositionY() - WindsorDeplacement[1].Y;
                    Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    break;
                case 2:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        Parlotte(General, false);
                        General->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    Timer = 10000;
                    break;
                case 3:
                    Parlotte(m_creature, false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                    Timer = 10000;
                    break;
                case 4:
                    Parlotte(m_creature, false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                    Timer = 10000;
                    break;
                case 5:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        Parlotte(General, false);
                        General->HandleEmote(EMOTE_ONESHOT_TALK);
                        General->MonsterTextEmote("General Marcus Jonathan appears lost in contemplation.", NULL);
                    }
                    Timer = 10000;
                    break;
                case 6:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        Parlotte(General, false);
                        General->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    Timer = 10000;
                    break;
                case 7:
                    Parlotte(m_creature, false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                    Timer = 10000;
                    break;
                case 8:
                    Parlotte(m_creature, false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                    Timer = 10000;
                    break;
                case 9:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        if (Creature* pGarde = GetGarde(0))
                            General->SetFacingToObject(pGarde);
                        General->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        Parlotte(General, false);
                    }
                    Timer = 10000;
                    break;
                case 10:
                    eventGardId = 0;
                    break;
                case 11:
                    eventGardId = 1;
                    break;
                case 12:
                    eventGardId = 2;
                    break;
                case 13:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        if (Creature* pGarde = GetGarde(3))
                            General->SetFacingToObject(pGarde);
                        General->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        Parlotte(General, false);
                    }
                    Timer = 5000;
                    break;
                case 14:
                    eventGardId = 3;
                    break;
                case 15:
                    eventGardId = 4;
                    break;
                case 16:
                    eventGardId = 5;
                    break;
                case 17:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        General->SetFacingToObject(m_creature);
                        Parlotte(General, true);
                        General->HandleEmote(EMOTE_ONESHOT_SHOUT);
                    }
                    Timer = 5000;
                    break;
                case 18:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                        General->HandleEmote(EMOTE_ONESHOT_SALUTE);
                    Timer = 5000;
                    break;
                case 19:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                        Parlotte(General, false);
                    Timer = 10000;
                    break;
                case 20:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        General->GetMotionMaster()->MovePoint(0, WindsorEventMove[13].X, WindsorEventMove[13].Y, WindsorEventMove[13].Z);
                        X = General->GetPositionX() - WindsorEventMove[13].X;
                        Y = General->GetPositionY() - WindsorEventMove[13].Y;
                        Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    }
                    else Timer = 1000;
                    break;
                case 21:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        General->HandleEmote(EMOTE_STATE_KNEEL);
                        General->SetFacingTo(WindsorEventMove[13].O);
                        m_creature->SetFacingToObject(General);
                        Parlotte(m_creature, false);
                    }
                    Timer = 10000;
                    break;
                case 22:
                    if (Creature* pGarde = GetGarde(0))
                        m_creature->SetFacingToObject(pGarde);
                    Parlotte(m_creature, false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                    Timer = 5000;
                    break;
                case 25:
                    NeedCheck = true;
                    break;
                case 26:
                    if (Creature* General = m_creature->FindNearestCreature(466, 150.0f))
                    {
                        General->DealDamage(General, General->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        General->Respawn();
                    }
                    break;
                case 40:
                    BeginQuete = false;
                    m_creature->SetUInt32Value(UNIT_NPC_FLAGS, 1);
                    Parlotte(m_creature, false);
                    break;
                case 47:
                    Parlotte(m_creature, false);
                    Timer = 10000;
                    break;
                case 48:
                    if (Creature* Bolvar = m_creature->FindNearestCreature(1748, 150.0f))
                        Parlotte(Bolvar, false);
                    Timer = 500;
                    break;
                case 49:
                    if (Creature* Anduin = m_creature->FindNearestCreature(1747, 150.0f))
                        Anduin->GetMotionMaster()->MovePoint(0, WindsorEventMove[14].X, WindsorEventMove[14].Y, WindsorEventMove[14].Z);
                    Timer = 5000;
                    break;
                case 50:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                    {
                        m_creature->SetFacingToObject(Onyxia);
                        Parlotte(m_creature, false);
                        m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                    }
                    Timer = 10000;
                    break;
                case 51:
                case 52:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                    {
                        Parlotte(Onyxia, false);
                        Onyxia->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    Timer = 10000;
                    break;
                case 53:
                case 54:
                    Parlotte(m_creature, false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                    Timer = 10000;
                    break;
                case 55:
                    Parlotte(m_creature, false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                    Timer = 4000;
                    break;
                case 56:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                        m_creature->CastSpell(Onyxia, 20358, false);
                    Timer = 10000;
                    break;
                case 57:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                    {
                        Onyxia->CastSpell(Onyxia, 17136, true);
                        Onyxia->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.5f);
                    }
                    Timer = 2000;
                    break;
                case 58:
                    if (Creature* Bolvar = m_creature->FindNearestCreature(1748, 150.0f))
                    {
                        Bolvar->GetMotionMaster()->MovePoint(0, WindsorEventMove[15].X, WindsorEventMove[15].Y, WindsorEventMove[15].Z);
                        X = Bolvar->GetPositionX() - WindsorEventMove[15].X;
                        Y = Bolvar->GetPositionY() - WindsorEventMove[15].Y;
                        Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                    }
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                    {
                        Parlotte(Onyxia, false);
                        Onyxia->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    else Timer = 5000;
                    break;
                case 59:
                    if (Creature* Bolvar = m_creature->FindNearestCreature(1748, 150.0f))
                    {
                        if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                            Bolvar->SetFacingToObject(Onyxia);
                        Parlotte(Bolvar, true);
                    }
                    Timer = 4000;
                    break;
                case 60:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                    {
                        int Var = 0;
                        GetCreatureListWithEntryInGrid(DragListe, Onyxia, 1756, 25.0f);
                        for (std::list<Creature*>::iterator itr = DragListe.begin(); itr != DragListe.end(); ++itr)
                        {
                            DragsGUIDs[Var] = (*itr)->GetGUID();
                            (*itr)->UpdateEntry(12739);
                            (*itr)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            Var++;
                        }
                    }
                    Timer = 5000;
                    break;
                case 61:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                        Onyxia->CastSpell(m_creature, 20465, true);
                    Timer = 1500;
                    break;
                case 62:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                    {
                        if (Creature* Bolvar = m_creature->FindNearestCreature(1748, 150.0f))
                        {
                            int Var = 0;
                            while (DragsGUIDs[Var] && Var < 9)
                            {
                                Creature* crea = me->GetMap()->GetCreature(DragsGUIDs[Var]);
                                crea->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                crea->getThreatManager().addThreatDirectly(Bolvar, 5000.0f);
                                crea->SetTargetGuid(Bolvar->GetGUID());
                                Bolvar->AddThreat(crea);
                                Bolvar->SetInCombatWith(crea);
                                crea->SetInCombatWith(Bolvar);
                                Var++;
                            }
                        }
                    }
                    Parlotte(m_creature, false);
                    Timer = 5000;
                    break;
                case 63:
                    if (Creature* Onyxia = m_creature->FindNearestCreature(1749, 150.0f))
                    {
                        Onyxia->CastSpell(Onyxia, 20466, true);
                        Onyxia->CastSpell(Onyxia, 16380, true);
                    }
                    PhaseFinale = true;
                    break;
            }
            if (eventGardId < 6)
            {
                if (Creature* pGarde = GetGarde(eventGardId))
                {
                    int Var = eventGardId + 7;
                    pGarde->GetMotionMaster()->MovePoint(0, WindsorEventMove[Var].X, WindsorEventMove[Var].Y, WindsorEventMove[Var].Z);
                    X = pGarde->GetPositionX() - WindsorEventMove[Var].X;
                    Y = pGarde->GetPositionY() - WindsorEventMove[Var].Y;
                }
                GardeTimer[eventGardId] = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
                GardeNeed[eventGardId] = true;
                Timer = 1000;
            }
            if (Tick > 23 && Tick < 40)
            {
                int Var = Tick - 21;
                m_creature->GetMotionMaster()->MovePoint(0, WindsorDeplacement[Var].X, WindsorDeplacement[Var].Y, WindsorDeplacement[Var].Z);
                X = m_creature->GetPositionX() - WindsorDeplacement[Var].X;
                Y = m_creature->GetPositionY() - WindsorDeplacement[Var].Y;
                Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
            }
            else if (Tick > 41 && Tick < 46)
            {
                int Var = Tick - 22;
                m_creature->GetMotionMaster()->MovePoint(0, WindsorDeplacement[Var].X, WindsorDeplacement[Var].Y, WindsorDeplacement[Var].Z);
                X = m_creature->GetPositionX() - WindsorDeplacement[Var].X;
                Y = m_creature->GetPositionY() - WindsorDeplacement[Var].Y;
                Timer = 1000 + sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_WALK) * 0.001f);
            }
            else if (PhaseFinale == true)
            {
                if (Creature* Bolvar = m_creature->FindNearestCreature(1748, 150.0f))
                {
                    if (!Bolvar->isInCombat())
                    {
                        SituationFinale();
                        BeginQuete = false;
                        m_creature->CastSpell(m_creature, 5, true);
                    }
                }
            }
            Tick++;
        }
        else Timer -= uiDiff;
    }
};

bool QuestAccept_npc_Windsor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == 6403)
    {
        if (npc_WindsorAI* pWindsorEventAI = dynamic_cast<npc_WindsorAI*>(pCreature->AI()))
        {
            pWindsorEventAI->BeginQuete = true;
            pWindsorEventAI->JoueurGUID = pPlayer->GetGUID();
            if (pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }
    }
    return true;
}

bool GossipHello_npc_Windsor(Player* pPlayer, Creature* pCreature)
{
    if (npc_WindsorAI* pWindsorEventAI = dynamic_cast<npc_WindsorAI*>(pCreature->AI()))
    {
        if (pPlayer == pWindsorEventAI->GetPlayer())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Me and my friends are ready. Let's stop this masquerade!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        else if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        pPlayer->SEND_GOSSIP_MENU(125800, pCreature->GetGUID());
    }


    return true;
}

bool GossipSelect_npc_Windsor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            if (npc_WindsorAI* pWindsorEventAI = dynamic_cast<npc_WindsorAI*>(pCreature->AI()))
            {
                pWindsorEventAI->BeginQuete = true;
                pWindsorEventAI->Parlotte(pCreature, false);
                pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            }
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
    }
    return true;
}

CreatureAI* GetAI_npc_Windsor(Creature* pCreature)
{
    return new npc_WindsorAI(pCreature);
}


/*######
## npc_bolvar_fordragon
######*/

bool GossipHello_npc_bolvar_fordragon(Player* pPlayer, Creature* pCreature)
{
    if ((pPlayer->GetQuestStatus(PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF)) == QUEST_STATUS_INCOMPLETE) && (PSM_CheckOptionForPlayer(pPlayer, OPTION_SET_55)))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NOTREADY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_NOTREADY);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_IAMREADY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_IAMREADY);
        pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_1), pCreature->GetGUID());
    }
    else
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_bolvar_fordragon(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_NOTREADY:
            pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_2), pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_IAMREADY:
            pPlayer->AreaExploredOrEventHappens(PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF));
            if (pCreature->isQuestGiver())
                pPlayer->PrepareQuestMenu(pCreature->GetGUID());
            pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_3), pCreature->GetGUID());
            break;
    }
    return true;
}

bool QuestRewarded_npc_bolvar_fordragon(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF))
        PSM_AddSpells(pPlayer);
    else if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, GET_PEX_REWARD))
        PSM_PexReward(pPlayer);
    return true;
}

enum
{
    QUEST_LORD_OF_BLACKROCK             = 7782,
    SPELL_RALLYING_CRY_DRAGONSLAYER     = 22888,
};

bool QuestRewarded_npc_field_marshal_afrasiabi(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_LORD_OF_BLACKROCK)
    {
        const char* his_or_her = player->getGender() ? "her" : "his";
        creature->CastSpell(creature, SPELL_RALLYING_CRY_DRAGONSLAYER, false);
        creature->PMonsterYell("Citizens of the Alliance, the Lord of Blackrock is slain! Nefarian has been subdued by the combined might of %s and %s allies!",
            player->GetName(), his_or_her);
        creature->PMonsterYell("Let your spirits rise! Rally around your champion, bask in %s glory! Revel in the rallying cry of the dragon slayer!", his_or_her);
    }
    return false;
}

void AddSC_stormwind_city()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_archmage_malin";
    newscript->pGossipHello = &GossipHello_npc_archmage_malin;
    newscript->pGossipSelect = &GossipSelect_npc_archmage_malin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bartleby";
    newscript->GetAI = &GetAI_npc_bartleby;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_bartleby;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dashel_stonefist";
    newscript->GetAI = &GetAI_npc_dashel_stonefist;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_dashel_stonefist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lady_katrana_prestor";
    newscript->pGossipHello = &GossipHello_npc_lady_katrana_prestor;
    newscript->pGossipSelect = &GossipSelect_npc_lady_katrana_prestor;
    newscript->RegisterSelf();

    // The Attack quest
    newscript = new Script;
    newscript->Name = "npc_lord_gregor_lescovar";
    newscript->GetAI = &GetAI_npc_lord_gregor_lescovar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion_spybot";
    newscript->GetAI = &GetAI_npc_tyrion_spybot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion";
    newscript->GetAI = &GetAI_npc_tyrion;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_tyrion;
    newscript->pGossipHello = &GossipHello_npc_tyrion;
    newscript->RegisterSelf();

    // Elysium

    newscript = new Script;
    newscript->Name = "npc_ecuyer_rowe";
    newscript->GetAI = &GetAI_npc_Rowe;
    newscript->pGossipHello = &GossipHello_npc_Ecuyer_Rowe;
    newscript->pGossipSelect = &GossipSelect_npc_Ecuyer_Rowe;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_windsor";
    newscript->GetAI = &GetAI_npc_Windsor;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_Windsor;
    newscript->pGossipHello = &GossipHello_npc_Windsor;
    newscript->pGossipSelect = &GossipSelect_npc_Windsor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bolvar_fordragon";
    newscript->pGossipHello   = &GossipHello_npc_bolvar_fordragon;
    newscript->pGossipSelect  = &GossipSelect_npc_bolvar_fordragon;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_bolvar_fordragon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_field_marshal_afrasiabi";
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_field_marshal_afrasiabi;
    newscript->RegisterSelf();
}
