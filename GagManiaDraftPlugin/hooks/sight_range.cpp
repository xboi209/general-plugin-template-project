﻿#include "sight_range.h"
#include <SCBW/enumerations.h>
#include <SCBW/scbwdata.h>
#include <SCBW/api.h>

namespace {
/// Helper function: checks if the unit is a building morphing into another building.
/// Logically equivalent to the SCBW function @ 0x0045CD00
bool isRemorphingBuilding(const CUnit *unit);
} //unnamed namespace

namespace hooks {

/// Returns the modified sight range of the unit, measured in matrices.
/// StarCraft passes 1 for isForSpellCasting when using Feedback, Mind Control,
/// and Hallucination (but not when launching Nukes).
/// Note: sight ranges cannot exceed 11, unless extended.
u32 getSightRangeHook(const CUnit *unit, bool isForSpellCasting) {

  using scbw::getUpgradeLevel;

  //Check if the unit is a constructing building (exclude remorphing buildings)
  if (unit->status & UnitStatus::GroundedBuilding
      && !(unit->status & UnitStatus::Completed)
      && !isRemorphingBuilding(unit))
    return 4;

  //Check if the unit is blinded (don't bother if this is for spellcasting)
  if (!isForSpellCasting && unit->isBlind)
    return 2;

  //Sight range upgrades
  switch (unit->id) {
    case UnitId::ghost:
      if (getUpgradeLevel(unit->playerId, UpgradeId::OcularImplants))
        return 11;
      break;
	case UnitId::science_vessel:  //사이언스 베슬 시야업 적용
		if (getUpgradeLevel(unit->playerId, UPGRADE_SCIENCE_VESSEL_SIGHT))
			return 11;
		break;
    case UnitId::overlord:
      if (getUpgradeLevel(unit->playerId, UpgradeId::Antennae))
        return 11;
      break;
    case UnitId::observer:
      if (getUpgradeLevel(unit->playerId, UpgradeId::SensorArray))
        return 11;
      break;
    case UnitId::scout:
      if (getUpgradeLevel(unit->playerId, UpgradeId::ApialSensors))
        return 11;
      break;
	case UnitId::lurker: //러커 시야업 적용(버러우시에만 효과 있게)
		if(getUpgradeLevel(unit->playerId,UpgradeId::UnusedUpgrade56)&&unit->status & UnitStatus::Burrowed)
			return 11;
		break;
  }

  //Default
  return Unit::SightRange[unit->id];
}

} //hooks

namespace {
/**** Definitions of helper functions. Do not edit anything below this! ****/

bool isRemorphingBuilding(const CUnit *unit) {
  if (unit->status & UnitStatus::Completed)
    return false;
  u16 firstQueuedUnit = unit->buildQueue[unit->buildQueueSlot];
  return firstQueuedUnit == UnitId::lair
         || firstQueuedUnit == UnitId::hive
         || firstQueuedUnit == UnitId::greater_spire
         || firstQueuedUnit == UnitId::spore_colony
         || firstQueuedUnit == UnitId::sunken_colony;
}

} //unnamed namespace

