#include "stim_packs.h"
#include "../SCBW/api.h"

const s32 getStimPacksHpCost(const CUnit *unit) {
  if (unit->id == UnitId::firebat || unit->id == UnitId::gui_montag)
    return 20 * 256;
  else
    return 10 * 256;
}

namespace hooks {

void useStimPacksHook(CUnit *unit) {
  const s32 hpCost = getStimPacksHpCost(unit);

  if (unit->hitPoints > hpCost) {
    scbw::playSound(scbw::randBetween(278, 279), unit);
    unit->damageHp(hpCost);
    if (unit->stimTimer < 37) {
      unit->stimTimer = 37;
      unit->updateSpeed();
    }
  }
}

bool canUseStimPacksHook(const CUnit *unit) {
  //Default StarCraft behavior
  return unit->hitPoints > getStimPacksHpCost(unit);
}

} //hooks