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
#include "SpellScript.h"

/*######
## Quest 25134: Lazy Peons
## npc_lazy_peon
######*/

enum LazyPeonYells
{
    SAY_SPELL_HIT = 0
};

enum LazyPeon
{
    QUEST_LAZY_PEONS    = 25134,
    GO_LUMBERPILE       = 175784,
    SPELL_BUFF_SLEEP    = 17743,
    SPELL_AWAKEN_PEON   = 19938
};

class npc_lazy_peon : public CreatureScript
{
public:
    npc_lazy_peon() : CreatureScript("npc_lazy_peon") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lazy_peonAI(creature);
    }

    struct npc_lazy_peonAI : public ScriptedAI
    {
        npc_lazy_peonAI(Creature* creature) : ScriptedAI(creature) {}

        ObjectGuid PlayerGUID;

        uint32 RebuffTimer;
        bool work;

        void Reset() override
        {
            PlayerGUID.Clear();
            RebuffTimer = 0;
            work = false;
        }

        void MovementInform(uint32 /*type*/, uint32 id) override
        {
            if (id == 1)
                work = true;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_AWAKEN_PEON && caster->IsPlayer() && CAST_PLR(caster)->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
            {
                caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                Talk(SAY_SPELL_HIT, caster->GetGUID());
                me->RemoveAllAuras();

                if (GameObject* Lumberpile = me->FindNearestGameObject(GO_LUMBERPILE, 20))
                    me->GetMotionMaster()->MovePoint(1, Lumberpile->GetPositionX()-1, Lumberpile->GetPositionY(), Lumberpile->GetPositionZ());
            }
        }

        void UpdateAI(uint32 Diff) override
        {
            if (work == true)
                me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);

            if (RebuffTimer <= Diff)
            {
                DoCast(me, SPELL_BUFF_SLEEP);
                RebuffTimer = 300000; //Rebuff agian in 5 minutes
            }
            else
                RebuffTimer -= Diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

enum VoodooSpells
{
    SPELL_BREW      = 16712, // Special Brew
    SPELL_GHOSTLY   = 16713, // Ghostly
    SPELL_HEX1      = 16707, // Hex
    SPELL_HEX2      = 16708, // Hex
    SPELL_HEX3      = 16709, // Hex
    SPELL_GROW      = 16711, // Grow
    SPELL_LAUNCH    = 16716, // Launch (Whee!)
};

// 17009
class spell_voodoo : public SpellScriptLoader
{
    public:
        spell_voodoo() : SpellScriptLoader("spell_voodoo") {}

        class spell_voodoo_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_voodoo_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BREW) || !sSpellMgr->GetSpellInfo(SPELL_GHOSTLY) ||
                    !sSpellMgr->GetSpellInfo(SPELL_HEX1) || !sSpellMgr->GetSpellInfo(SPELL_HEX2) ||
                    !sSpellMgr->GetSpellInfo(SPELL_HEX3) || !sSpellMgr->GetSpellInfo(SPELL_GROW) ||
                    !sSpellMgr->GetSpellInfo(SPELL_LAUNCH))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 spellid = RAND(SPELL_BREW, SPELL_GHOSTLY, RAND(SPELL_HEX1, SPELL_HEX2, SPELL_HEX3), SPELL_GROW, SPELL_LAUNCH);
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, spellid, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_voodoo_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_voodoo_SpellScript();
        }
};

class npc_100874_illidari_enforcer_dh_questline : public CreatureScript
{
public:
    npc_100874_illidari_enforcer_dh_questline() : CreatureScript("npc_100874_illidari_enforcer_dh_questline") { }

    struct npc_100874_illidari_enforcer_dh_questlineAI : ScriptedAI
    {
        npc_100874_illidari_enforcer_dh_questlineAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void Reset() override
        {
            events.RescheduleEvent(EVENT_1, 2000);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                    events.ScheduleEvent(EVENT_1, 2000);

                    std::list<Player*> list;
                    me->GetPlayerListInGrid(list, 20.0f);
                    for (Player* player : list)
                        if (player->HasAura(188501) && player->GetQuestStatus(40982) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(102563);

                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_100874_illidari_enforcer_dh_questlineAI(creature);
    }
};

void AddSC_durotar()
{
    new npc_lazy_peon();
    new spell_voodoo();
    new npc_100874_illidari_enforcer_dh_questline();
}
