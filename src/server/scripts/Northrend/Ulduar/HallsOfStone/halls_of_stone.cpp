/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "halls_of_stone.h"

enum Texts
{
    SAY_KILL_1                          = -1599016,
    SAY_KILL_2                          = -1599017,
    SAY_KILL_3                          = -1599018,
    SAY_LOW_HEALTH                      = -1599019,
    SAY_DEATH                           = -1599020,
    SAY_PLAYER_DEATH_1                  = -1599021,
    SAY_PLAYER_DEATH_2                  = -1599022,
    SAY_PLAYER_DEATH_3                  = -1599023,
    SAY_ESCORT_START                    = -1599024,

    SAY_SPAWN_DWARF                     = -1599025,
    SAY_SPAWN_TROGG                     = -1599026,
    SAY_SPAWN_OOZE                      = -1599027,
    SAY_SPAWN_EARTHEN                   = -1599028,

    SAY_EVENT_INTRO_1                   = -1599029,
    SAY_EVENT_INTRO_2                   = -1599030,
    SAY_EVENT_INTRO_3_ABED              = -1599031,

    SAY_EVENT_A_1                       = -1599032,
    SAY_EVENT_A_2_KADD                  = -1599033,
    SAY_EVENT_A_3                       = -1599034,

    SAY_EVENT_B_1                       = -1599035,
    SAY_EVENT_B_2_MARN                  = -1599036,
    SAY_EVENT_B_3                       = -1599037,

    SAY_EVENT_C_1                       = -1599038,
    SAY_EVENT_C_2_ABED                  = -1599039,
    SAY_EVENT_C_3                       = -1599040,

    SAY_EVENT_D_1                       = -1599041,
    SAY_EVENT_D_2_ABED                  = -1599042,
    SAY_EVENT_D_3                       = -1599043,
    SAY_EVENT_D_4_ABED                  = -1599044,

    SAY_EVENT_END_01                    = -1599045,
    SAY_EVENT_END_02                    = -1599046,
    SAY_EVENT_END_03_ABED               = -1599047,
    SAY_EVENT_END_04                    = -1599048,
    SAY_EVENT_END_05_ABED               = -1599049,
    SAY_EVENT_END_06                    = -1599050,
    SAY_EVENT_END_07_ABED               = -1599051,
    SAY_EVENT_END_08                    = -1599052,
    SAY_EVENT_END_09_KADD               = -1599053,
    SAY_EVENT_END_10                    = -1599054,
    SAY_EVENT_END_11_KADD               = -1599055,
    SAY_EVENT_END_12                    = -1599056,
    SAY_EVENT_END_13_KADD               = -1599057,
    SAY_EVENT_END_14                    = -1599058,
    SAY_EVENT_END_15_MARN               = -1599059,
    SAY_EVENT_END_16                    = -1599060,
    SAY_EVENT_END_17_MARN               = -1599061,
    SAY_EVENT_END_18                    = -1599062,
    SAY_EVENT_END_19_MARN               = -1599063,
    SAY_EVENT_END_20                    = -1599064,
    SAY_EVENT_END_21_ABED               = -1599065,

    SAY_VICTORY_SJONNIR_1               = -1599066,
    SAY_VICTORY_SJONNIR_2               = -1599067,

    SAY_ENTRANCE_MEET                   = -1599068,

    TEXT_ID_START                       = 13100,
    TEXT_ID_PROGRESS                    = 13101
};

enum BrannCreatures
{
    CREATURE_TRIBUNAL_OF_THE_AGES       = 28234,
    CREATURE_BRANN_BRONZEBEARD          = 28070,
    CREATURE_DARK_MATTER_TARGET         = 28237,
    CREATURE_SEARING_GAZE_TARGET        = 28265,
    CREATURE_DARK_RUNE_PROTECTOR        = 27983,
    CREATURE_DARK_RUNE_STORMCALLER      = 27984,
    CREATURE_IRON_GOLEM_CUSTODIAN       = 27985,
};

enum Spells
{
    SPELL_STEALTH                       = 58506,
    //Kadrak
    SPELL_GLARE_OF_THE_TRIBUNAL         = 50988,
    H_SPELL_GLARE_OF_THE_TRIBUNAL       = 59868,
    //Marnak
    SPELL_DARK_MATTER                   = 51012,
    H_SPELL_DARK_MATTER                 = 59868,
    //Abedneum
    SPELL_SEARING_GAZE                  = 51136,
    H_SPELL_SEARING_GAZE                = 59867,

    SPELL_REWARD_ACHIEVEMENT            = 59046,
};

enum Quests
{
    QUEST_HALLS_OF_STONE                = 13207
};

#define GOSSIP_ITEM_START               "Brann, it would be our honor!"
#define GOSSIP_ITEM_PROGRESS            "Let's move Brann, enough of the history lessons!"
#define DATA_BRANN_SPARKLIN_NEWS        1

static Position SpawnLocations[]=
{
    {946.992f, 397.016f, 208.374f, 0.0f},
    {960.748f, 382.944f, 208.374f, 0.0f},
};

class mob_tribuna_controller : public CreatureScript
{
public:
    mob_tribuna_controller() : CreatureScript("mob_tribuna_controller") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_tribuna_controllerAI(creature);
    }

    struct mob_tribuna_controllerAI : public ScriptedAI
    {
        mob_tribuna_controllerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            SetCombatMovement(false);
        }

        InstanceScript* instance;

        uint32 uiKaddrakEncounterTimer;
        uint32 uiMarnakEncounterTimer;
        uint32 uiAbedneumEncounterTimer;

        bool bKaddrakActivated;
        bool bMarnakActivated;
        bool bAbedneumActivated;

        GuidList KaddrakGUIDList;

        void Reset() override
        {
            uiKaddrakEncounterTimer = 1500;
            uiMarnakEncounterTimer = 10000;
            uiAbedneumEncounterTimer = 10000;

            bKaddrakActivated = false;
            bMarnakActivated = false;
            bAbedneumActivated = false;

            if (instance)
            {
                instance->HandleGameObject(instance->GetGuidData(DATA_GO_KADDRAK), false);
                instance->HandleGameObject(instance->GetGuidData(DATA_GO_MARNAK), false);
                instance->HandleGameObject(instance->GetGuidData(DATA_GO_ABEDNEUM), false);
                instance->HandleGameObject(instance->GetGuidData(DATA_GO_SKY_FLOOR), false);
            }

            KaddrakGUIDList.clear();
        }

        void UpdateFacesList()
        {
            /*GetCreatureListWithEntryInGrid(lKaddrakGUIDList, me, CREATURE_KADDRAK, 50.0f);
            if (!lKaddrakGUIDList.empty())
            {
                uint32 uiPositionCounter = 0;
                for (std::list<Creature*>::const_iterator itr = lKaddrakGUIDList.begin(); itr != lKaddrakGUIDList.end(); ++itr)
                {
                    if ((*itr)->isAlive())
                    {
                        if (uiPositionCounter == 0)
                        {
                            (*itr)->GetMap()->CreatureRelocation((*itr), 927.265f, 333.200f, 218.780f, (*itr)->GetOrientation());
                            (*itr)->SendMonsterMove(927.265f, 333.200f, 218.780f, 0, (*itr)->GetMovementFlags(), 1);
                        }
                        else
                        {
                            (*itr)->GetMap()->CreatureRelocation((*itr), 921.745f, 328.076f, 218.780f, (*itr)->GetOrientation());
                            (*itr)->SendMonsterMove(921.745f, 328.076f, 218.780f, 0, (*itr)->GetMovementFlags(), 1);
                        }
                    }
                    ++uiPositionCounter;
                }
            }*/
        }

        void UpdateAI(uint32 diff) override
        {
            if (bKaddrakActivated)
            {
                if (uiKaddrakEncounterTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        if (!KaddrakGUIDList.empty())
                            for (GuidList::const_iterator itr = KaddrakGUIDList.begin(); itr != KaddrakGUIDList.end(); ++itr)
                            {
                                if (Creature* pKaddrak = Unit::GetCreature(*me, *itr))
                                {
                                    if (pKaddrak->IsAlive())
                                        pKaddrak->CastSpell(target, DUNGEON_MODE(SPELL_GLARE_OF_THE_TRIBUNAL, H_SPELL_GLARE_OF_THE_TRIBUNAL), true);
                                }
                            }
                    uiKaddrakEncounterTimer = 1500;
                } else uiKaddrakEncounterTimer -= diff;
            }
            if (bMarnakActivated)
            {
                if (uiMarnakEncounterTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    {
                        if (Creature* summon = me->SummonCreature(CREATURE_DARK_MATTER_TARGET, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                        {
                            summon->SetDisplayId(11686);
                            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            summon->CastSpell(target, DUNGEON_MODE(SPELL_DARK_MATTER, H_SPELL_DARK_MATTER), true);
                        }
                    }
                    uiMarnakEncounterTimer = urand(30000, 31000);
                } else uiMarnakEncounterTimer -= diff;
            }
            if (bAbedneumActivated)
            {
                if (uiAbedneumEncounterTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    {
                        if (Creature* summon = me->SummonCreature(CREATURE_SEARING_GAZE_TARGET, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                        {
                            summon->SetDisplayId(11686);
                            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            summon->CastSpell(target, DUNGEON_MODE(SPELL_SEARING_GAZE, H_SPELL_SEARING_GAZE), true);
                        }
                    }
                    uiAbedneumEncounterTimer = urand(30000, 31000);
                } else uiAbedneumEncounterTimer -= diff;
            }
        }
    };

};

class npc_brann_hos : public CreatureScript
{
public:
    npc_brann_hos() : CreatureScript("npc_brann_hos") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1 || action == GOSSIP_ACTION_INFO_DEF+2)
        {
            player->CLOSE_GOSSIP_MENU();
            CAST_AI(npc_brann_hos::npc_brann_hosAI, creature->AI())->StartWP();
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM(GossipOptionNpc::None, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(TEXT_ID_START, creature->GetGUID());

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_brann_hosAI(creature);
    }

    struct npc_brann_hosAI : public npc_escortAI
    {
        npc_brann_hosAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 uiStep;
        uint32 uiPhaseTimer;

        ObjectGuid uiControllerGUID;
        GuidList lDwarfGUIDList;

        InstanceScript* instance;

        bool bIsBattle;
        bool bIsLowHP;
        bool brannSparklinNews;

        void Reset() override
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
            {
                bIsLowHP = false;
                bIsBattle = false;
                uiStep = 0;
                uiPhaseTimer = 0;
                uiControllerGUID.Clear();
                brannSparklinNews = true;

                DespawnDwarf();

                if (instance)
                    instance->SetData(DATA_BRANN_EVENT, NOT_STARTED);
            }
        }

        void DespawnDwarf()
        {
            if (lDwarfGUIDList.empty())
                return;
            for (GuidList::const_iterator itr = lDwarfGUIDList.begin(); itr != lDwarfGUIDList.end(); ++itr)
            {
                Creature* temp = Unit::GetCreature(*me, instance ? (*itr) : ObjectGuid::Empty);
                if (temp && temp->IsAlive())
                    temp->DespawnOrUnsummon();
            }
            lDwarfGUIDList.clear();
        }

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
                case 7:
                    if (Creature* creature = GetClosestCreatureWithEntry(me, CREATURE_TRIBUNAL_OF_THE_AGES, 100.0f))
                    {
                        if (!creature->IsAlive())
                            creature->Respawn();
                        CAST_AI(mob_tribuna_controller::mob_tribuna_controllerAI, creature->AI())->UpdateFacesList();
                        uiControllerGUID = creature->GetGUID();
                    }
                    break;
                case 13:
                    DoScriptText(SAY_EVENT_INTRO_1, me);
                    SetEscortPaused(true);
                    JumpToNextStep(20000);
                    break;
                case 17:
                    DoScriptText(SAY_EVENT_INTRO_2, me);
                    if (instance)
                        instance->HandleGameObject(instance->GetGuidData(DATA_GO_TRIBUNAL_CONSOLE), true);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    SetEscortPaused(true);
                    JumpToNextStep(8500);
                    break;
                case 18:
                    SetEscortPaused(true);
                    break;
            }
         }

         void SpawnDwarf(uint32 uiType)
         {
           switch (uiType)
           {
               case 1:
               {
                   uint32 uiSpawnNumber = DUNGEON_MODE(2, 3);
                   for (uint8 i = 0; i < uiSpawnNumber; ++i)
                       me->SummonCreature(CREATURE_DARK_RUNE_PROTECTOR, SpawnLocations[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                   me->SummonCreature(CREATURE_DARK_RUNE_STORMCALLER, SpawnLocations[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                   break;
               }
               case 2:
                   for (uint8 i = 0; i < 2; ++i)
                       me->SummonCreature(CREATURE_DARK_RUNE_STORMCALLER, SpawnLocations[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                   break;
               case 3:
                   me->SummonCreature(CREATURE_IRON_GOLEM_CUSTODIAN, SpawnLocations[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                   break;
           }
         }

        void JustSummoned(Creature* summoned) override
        {
            lDwarfGUIDList.push_back(summoned->GetGUID());
            summoned->AddThreat(me, 0.0f);
            summoned->AI()->AttackStart(me);
        }

        void JumpToNextStep(uint32 uiTimer)
        {
          uiPhaseTimer = uiTimer;
          ++uiStep;
        }

        void StartWP()
        {
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            SetEscortPaused(false);
            uiStep = 1;
            Start();
        }

        void DamageTaken(Unit* /*done_by*/, uint32 & /*damage*/, DamageEffectType dmgType) override
        {
            if (brannSparklinNews)
                brannSparklinNews = false;
        }

        uint32 GetData(uint32 type) const override
        {
            if (type == DATA_BRANN_SPARKLIN_NEWS)
                return brannSparklinNews ? 1 : 0;

            return 0;
        }

        void UpdateEscortAI(const uint32 uiDiff) override
        {
            if (uiPhaseTimer <= uiDiff)
            {
                switch (uiStep)
                {
                    case 1:
                        if (instance)
                        {
                            if (instance->GetData(DATA_BRANN_EVENT) != NOT_STARTED)
                                return;
                            instance->SetData(DATA_BRANN_EVENT, IN_PROGRESS);
                        }
                        bIsBattle = false;
                        DoScriptText(SAY_ESCORT_START, me);
                        SetRun(true);
                        JumpToNextStep(0);
                        break;
                    case 3:
                        SetEscortPaused(false);
                        JumpToNextStep(0);
                        break;
                    case 5:
                        if (instance)
                            if (Creature* temp = (Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM))))
                                DoScriptText(SAY_EVENT_INTRO_3_ABED, temp);
                            JumpToNextStep(8500);
                        break;
                    case 6:
                        DoScriptText(SAY_EVENT_A_1, me);
                        JumpToNextStep(6500);
                        break;
                    case 7:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_KADDRAK)))
                                DoScriptText(SAY_EVENT_A_2_KADD, temp);
                            JumpToNextStep(12500);
                        break;
                    case 8:
                        DoScriptText(SAY_EVENT_A_3, me);
                        if (instance)
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_KADDRAK), true);
                        if (Creature* temp = Unit::GetCreature(*me, uiControllerGUID))
                            CAST_AI(mob_tribuna_controller::mob_tribuna_controllerAI, temp->AI())->bKaddrakActivated = true;
                        JumpToNextStep(5000);
                        break;
                    case 9:
                        me->SetReactState(REACT_PASSIVE);
                        SpawnDwarf(1);
                        JumpToNextStep(20000);
                        break;
                    case 10:
                        DoScriptText(SAY_EVENT_B_1, me);
                        JumpToNextStep(6000);
                        break;
                    case 11:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MARNAK)))
                                DoScriptText(SAY_EVENT_B_2_MARN, temp);
                        SpawnDwarf(1);
                        JumpToNextStep(20000);
                        break;
                    case 12:
                        DoScriptText(SAY_EVENT_B_3, me);
                        if (instance)
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_MARNAK), true);
                        if (Creature* temp = Unit::GetCreature(*me, uiControllerGUID))
                            CAST_AI(mob_tribuna_controller::mob_tribuna_controllerAI, temp->AI())->bMarnakActivated = true;
                        JumpToNextStep(10000);
                        break;
                    case 13:
                        SpawnDwarf(1);
                        JumpToNextStep(10000);
                        break;
                    case 14:
                        SpawnDwarf(2);
                        JumpToNextStep(20000);
                        break;
                    case 15:
                        DoScriptText(SAY_EVENT_C_1, me);
                        SpawnDwarf(1);
                        JumpToNextStep(10000);
                        break;
                    case 16:
                        SpawnDwarf(2);
                        JumpToNextStep(20000);
                        break;
                    case 17:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM)))
                                DoScriptText(SAY_EVENT_C_2_ABED, temp);
                            SpawnDwarf(1);
                        JumpToNextStep(20000);
                        break;
                    case 18:
                        DoScriptText(SAY_EVENT_C_3, me);
                        if (instance)
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_ABEDNEUM), true);
                        if (Creature* temp = Unit::GetCreature(*me, uiControllerGUID))
                            CAST_AI(mob_tribuna_controller::mob_tribuna_controllerAI, temp->AI())->bAbedneumActivated = true;
                        JumpToNextStep(5000);
                        break;
                    case 19:
                        SpawnDwarf(2);
                        JumpToNextStep(10000);
                        break;
                    case 20:
                        SpawnDwarf(1);
                        JumpToNextStep(15000);
                        break;
                    case 21:
                        DoScriptText(SAY_EVENT_D_1, me);
                        SpawnDwarf(3);
                        JumpToNextStep(20000);
                        break;
                    case 22:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM)))
                                DoScriptText(SAY_EVENT_D_2_ABED, temp);
                        SpawnDwarf(1);
                        JumpToNextStep(5000);
                        break;
                    case 23:
                        SpawnDwarf(2);
                        JumpToNextStep(15000);
                        break;
                    case 24:
                        DoScriptText(SAY_EVENT_D_3, me);
                        SpawnDwarf(3);
                        JumpToNextStep(5000);
                        break;
                    case 25:
                        SpawnDwarf(1);
                        JumpToNextStep(5000);
                        break;
                    case 26:
                        SpawnDwarf(2);
                        JumpToNextStep(10000);
                        break;
                    case 27:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM)))
                                DoScriptText(SAY_EVENT_D_4_ABED, temp);
                        SpawnDwarf(1);
                        JumpToNextStep(10000);
                        break;
                    case 28:
                        me->SetReactState(REACT_DEFENSIVE);
                        DoScriptText(SAY_EVENT_END_01, me);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        if (instance)
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_SKY_FLOOR), true);
                        if (Creature* temp = Unit::GetCreature(*me, uiControllerGUID))
                            temp->DealDamage(temp, temp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        bIsBattle = true;
                        SetEscortPaused(false);
                        JumpToNextStep(6500);
                        break;
                    case 29:
                        DoScriptText(SAY_EVENT_END_02, me);
                        instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_REWARD_ACHIEVEMENT, 0, 0, me);
                        if (instance)
                            instance->SetData(DATA_BRANN_EVENT, DONE);
                        JumpToNextStep(5500);
                        break;
                    case 30:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM)))
                                DoScriptText(SAY_EVENT_END_03_ABED, temp);
                        JumpToNextStep(8500);
                        break;
                    case 31:
                        DoScriptText(SAY_EVENT_END_04, me);
                        JumpToNextStep(11500);
                        break;
                    case 32:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM)))
                                DoScriptText(SAY_EVENT_END_05_ABED, temp);
                            JumpToNextStep(11500);
                        break;
                    case 33:
                        DoScriptText(SAY_EVENT_END_06, me);
                        JumpToNextStep(4500);
                        break;
                    case 34:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM)))
                                DoScriptText(SAY_EVENT_END_07_ABED, temp);
                            JumpToNextStep(22500);
                        break;
                    case 35:
                        DoScriptText(SAY_EVENT_END_08, me);
                        JumpToNextStep(7500);
                        break;
                    case 36:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_KADDRAK)))
                                DoScriptText(SAY_EVENT_END_09_KADD, temp);
                        JumpToNextStep(18500);
                        break;
                    case 37:
                        DoScriptText(SAY_EVENT_END_10, me);
                        JumpToNextStep(5500);
                        break;
                    case 38:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_KADDRAK)))
                                DoScriptText(SAY_EVENT_END_11_KADD, temp);
                            JumpToNextStep(20500);
                        break;
                    case 39:
                        DoScriptText(SAY_EVENT_END_12, me);
                        JumpToNextStep(2500);
                        break;
                    case 40:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_KADDRAK)))
                                DoScriptText(SAY_EVENT_END_13_KADD, temp);
                        JumpToNextStep(19500);
                        break;
                    case 41:
                        DoScriptText(SAY_EVENT_END_14, me);
                        JumpToNextStep(10500);
                        break;
                    case 42:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MARNAK)))
                                DoScriptText(SAY_EVENT_END_15_MARN, temp);
                            JumpToNextStep(6500);
                        break;
                    case 43:
                        DoScriptText(SAY_EVENT_END_16, me);
                        JumpToNextStep(6500);
                        break;
                    case 44:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MARNAK)))
                                DoScriptText(SAY_EVENT_END_17_MARN, temp);
                            JumpToNextStep(25500);
                        break;
                    case 45:
                        DoScriptText(SAY_EVENT_END_18, me);
                        JumpToNextStep(23500);
                        break;
                    case 46:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MARNAK)))
                                DoScriptText(SAY_EVENT_END_19_MARN, temp);
                            JumpToNextStep(3500);
                        break;
                    case 47:
                        DoScriptText(SAY_EVENT_END_20, me);
                        JumpToNextStep(8500);
                        break;
                    case 48:
                        if (instance)
                            if (Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_ABEDNEUM)))
                                DoScriptText(SAY_EVENT_END_21_ABED, temp);
                            JumpToNextStep(5500);
                        break;
                    case 49:
                    {
                        if (instance)
                        {
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_KADDRAK), false);
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_MARNAK), false);
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_ABEDNEUM), false);
                            instance->HandleGameObject(instance->GetGuidData(DATA_GO_SKY_FLOOR), false);
                        }
                        Player* player = GetPlayerForEscort();
                        if (player)
                            player->GroupEventHappens(QUEST_HALLS_OF_STONE, me);
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        JumpToNextStep(180000);
                        break;
                    }
                    case 50:
                        SetEscortPaused(false);
                        break;
                }
            } else uiPhaseTimer -= uiDiff;

            if (!bIsLowHP && HealthBelowPct(30))
            {
                DoScriptText(SAY_LOW_HEALTH, me);
                bIsLowHP = true;
            }
            else if (bIsLowHP && !HealthBelowPct(30))
                bIsLowHP = false;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

};

class achievement_brann_spankin_new : public AchievementCriteriaScript
{
    public:
        achievement_brann_spankin_new() : AchievementCriteriaScript("achievement_brann_spankin_new")
        {
        }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature* Brann = target->ToCreature())
                if (Brann->AI()->GetData(DATA_BRANN_SPARKLIN_NEWS))
                    return true;

            return false;
        }
};

void AddSC_halls_of_stone()
{
    new npc_brann_hos();
    new mob_tribuna_controller();
    new achievement_brann_spankin_new();
}
