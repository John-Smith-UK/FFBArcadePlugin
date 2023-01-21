/*This file is part of FFB Arcade Plugin.
FFB Arcade Plugin is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
FFB Arcade Plugin is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with FFB Arcade Plugin.If not, see < https://www.gnu.org/licenses/>.
*/

#include <string>
#include "TokyoCop.h"
static EffectTriggers* myTriggers;
static EffectConstants* myConstants;
static Helpers* myHelpers;
extern int EnableDamper;
extern int DamperStrength;
static bool init;

static int __stdcall SetMotor(DWORD* a1, float a2)
{
	if (a2 > 0)
	{
		double percentForce = a2;
		double percentLength = 100;

		if (percentForce > 1.0)
			percentForce = 1.0;

		myTriggers->Rumble(percentForce, 0, percentLength);
		myTriggers->Constant(myConstants->DIRECTION_FROM_LEFT, percentForce);
	}
	else if (a2 < 0)
	{
		double percentForce = -a2;
		double percentLength = 100;

		if (percentForce > 1.0)
			percentForce = 1.0;

		myTriggers->Rumble(0, percentForce, percentLength);
		myTriggers->Constant(myConstants->DIRECTION_FROM_RIGHT, percentForce);
	}
	return 0;
}

static bool Hook(void* toHook, void* ourFunct, int len) {
	if (len < 5) {
		return false;
	}

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len);

	DWORD relativeAddress = ((DWORD)ourFunct - (DWORD)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}

static DWORD jmpBackAddy;

void TokyoCop::FFBLoop(EffectConstants* constants, Helpers* helpers, EffectTriggers* triggers) {
	if (!init)
	{
		int hookLength = 6;
		DWORD hookAddress = 0x80E6FA4;
		if (hookAddress)
		{
			jmpBackAddy = hookAddress + hookLength;
			Hook((void*)hookAddress, SetMotor, hookLength);
			init = true;
		}
	}

	if (EnableDamper == 1)
		triggers->Damper(DamperStrength / 100.0);

	myTriggers = triggers;
	myConstants = constants;
	myHelpers = helpers;
}