#pragma once

#include "common/keymap.h"
#include <functional>

namespace lr2skin::slider
{
std::function<void(double)> getSliderCallback(int type);

// ==================== may be used externally

void pitch(double p);
void updatePitchState(int val);
} // namespace lr2skin::slider
