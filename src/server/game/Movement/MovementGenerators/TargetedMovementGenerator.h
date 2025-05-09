/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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

#ifndef TRINITY_TARGETEDMOVEMENTGENERATOR_H
#define TRINITY_TARGETEDMOVEMENTGENERATOR_H

#include "FollowerReference.h"
#include "MovementGenerator.h"
#include "Timer.h"

class TargetedMovementGeneratorBase
{
    public:
        TargetedMovementGeneratorBase(Unit& target) { _target.link(&target, this); }

        bool IsTargetValid() const { return _target.isValid(); }
        Unit* GetTarget() const { return _target.getTarget(); }
        void stopFollowing() { }

    private:
        FollowerReference _target;
};

template<class T, typename D>
class TargetedMovementGenerator : public MovementGeneratorMedium<T, D>, public TargetedMovementGeneratorBase
{
    public:
        explicit TargetedMovementGenerator(Unit& target, float offset, float angle) : TargetedMovementGeneratorBase(target), _path(nullptr), _timer(0), _offset(offset), _angle(angle), _recalculateTravel(false), _targetReached(false), _interrupt(false) { }
        ~TargetedMovementGenerator();

        virtual bool DoUpdate(T&, uint32);

        void UnitSpeedChanged() override { _recalculateTravel = true; }

        virtual void ClearUnitStateMove(T&) { }
        virtual void AddUnitStateMove(T&) { }
        virtual bool HasLostTarget(T&) const { return false; }
        virtual void ReachTarget(T&) { }
        virtual bool EnableWalking() const { return false; }
        virtual void MovementInform(T&) { }

        bool IsReachable() const;

        void SetTargetLocation(T &owner, bool updateDestination);

    private:
        PathGenerator* _path;
        TimeTrackerSmall _timer;
        float _offset;
        float _angle;
        bool _recalculateTravel;
        bool _targetReached;
        bool _interrupt;
};

template<class T>
class ChaseMovementGenerator : public TargetedMovementGenerator<T, ChaseMovementGenerator<T>>
{
    public:
        explicit ChaseMovementGenerator(Unit& target, float offset, float angle) : TargetedMovementGenerator<T, ChaseMovementGenerator<T> >(target, offset, angle) { }

        MovementGeneratorType GetMovementGeneratorType() const override { return CHASE_MOTION_TYPE; }

        void DoInitialize(T&);
        void DoFinalize(T&);
        void DoReset(T&);

        void ClearUnitStateMove(T&) override;
        void AddUnitStateMove(T&) override;
        bool HasLostTarget(T&) const override;
        void ReachTarget(T&) override;
        void MovementInform(T&) override;
};

template<class T>
class FetchMovementGenerator : public TargetedMovementGenerator<T, FetchMovementGenerator<T>>
{
    public:
        explicit FetchMovementGenerator(Unit &target, float offset, float angle) : TargetedMovementGenerator<T, FetchMovementGenerator<T> >(target, offset, angle) { }

        MovementGeneratorType GetMovementGeneratorType() const override { return FOLLOW_MOTION_TYPE; }

        void DoInitialize(T&);
        void DoFinalize(T&);
        void DoReset(T&);

        void ClearUnitStateMove(T&) override;
        void AddUnitStateMove(T&) override;
        void MovementInform(T&) override;
};

template<class T>
class FollowMovementGenerator : public TargetedMovementGenerator<T, FollowMovementGenerator<T>>
{
    public:
        explicit FollowMovementGenerator(Unit &target, float offset, float angle) : TargetedMovementGenerator<T, FollowMovementGenerator<T> >(target, offset, angle), _path(nullptr), _recheckPredictedDistanceTimer(0), _recheckPredictedDistance(false), _range(offset), _angle(angle), _inheritWalkState(false) { }
        ~FollowMovementGenerator() = default;

        MovementGeneratorType GetMovementGeneratorType() const override { return FOLLOW_MOTION_TYPE; }

        void DoInitialize(T&);
        void DoFinalize(T&);
        void DoReset(T&);
        bool DoUpdate(T&, uint32) override;

        void ClearUnitStateMove(T&) override;
        void AddUnitStateMove(T&) override;
        bool HasLostTarget(T&) const override { return false; };
        void ReachTarget(T&) override;
        bool EnableWalking() const override;
        void MovementInform(T&) override;

    private:
        void UpdateSpeed(T& owner);

        bool PositionOkay(Unit* target, bool isPlayerPet, bool& targetIsMoving, uint32 diff);

        std::unique_ptr<PathGenerator> _path;
        TimeTrackerSmall _recheckPredictedDistanceTimer;
        bool _recheckPredictedDistance;

        Optional<Position> _lastTargetPosition;
        Optional<Position> _lastPredictedPosition;
        float _range;
        float _angle;
        bool _inheritWalkState;
};

#endif

