/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AbstractFollower.h"
#include "TestCreature.h"
#include "TestMap.h"
#include "WorldMock.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

namespace
{
// ============================================================================
// Regression test for a follower registered on a unit already queued for removal.
//
// Map::AddObjectToRemoveList runs RemoveFromWorld (and RemoveAllFollowers) at
// once, but deletes the unit only at the end of the tick. A MoveFollow issued in
// between (Brewfest guzzlers searching kegs) registered a follower that was never
// detached, so the follow generator read the freed unit on the next update.
// ============================================================================
class FollowerTargetDeleteTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _previousWorld = std::move(sWorld);
        _worldMock = new NiceMock<WorldMock>();
        ON_CALL(*_worldMock, getIntConfig(_)).WillByDefault(Return(0));
        ON_CALL(*_worldMock, getFloatConfig(_)).WillByDefault(Return(1.0f));
        ON_CALL(*_worldMock, getBoolConfig(_)).WillByDefault(Return(false));
        static std::string emptyString;
        ON_CALL(*_worldMock, GetDataPath()).WillByDefault(ReturnRef(emptyString));
        sWorld.reset(_worldMock);

        TestMap::EnsureDBC();
        _map = new TestMap();
        _target = new TestCreature();
        _target->SetupForCombatTest(_map, 1, 24373);
    }

    void TearDown() override
    {
        delete _map;
        sWorld = std::move(_previousWorld);
    }

    struct Follower : AbstractFollower
    {
        using AbstractFollower::AbstractFollower;
    };

    std::unique_ptr<IWorld> _previousWorld;
    NiceMock<WorldMock>* _worldMock = nullptr;
    TestMap* _map = nullptr;
    TestCreature* _target = nullptr;
};

// cppcheck-suppress syntaxError
TEST_F(FollowerTargetDeleteTest, DeletedTargetDetachesLateFollower)
{
    _target->RemoveAllFollowers(); // removal cleanup already ran
    Follower* follower = new Follower(_target); // late MoveFollow before deletion

    _target->CleanupCombatState();
    delete _target;

    // A dangling target would be read again by the follower destructor.
    ASSERT_EQ(follower->GetTarget(), nullptr);
    delete follower;
}
}
