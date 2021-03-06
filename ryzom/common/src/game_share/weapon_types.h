// Ryzom - MMORPG Framework <http://dev.ryzom.com/projects/ryzom/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.



#ifndef RY_WEAPON_TYPES_H
#define RY_WEAPON_TYPES_H

#include "nel/misc/types_nl.h"

namespace WEAPONTYPE
{
	// Mode
	enum EWeaponType
	{
		UNKNOWN = 0,

		HANDS,

		HEAVY,
		MEDIUM,
		LIGHT,

		LIGHT_GUN,
		MEDIUM_GUN,
		HEAVY_GUN,

	};


	/**
	 * get the right weapon type from the input string
	 * \param str the input string
	 * \return the EWeaponType associated to this string (UNKNOWN if the string cannot be interpreted)
	 */
	EWeaponType stringToWeaponType(const std::string &str);

	std::string toString(EWeaponType type);

}; // WEAPONTYPE

#endif // RY_WEAPON_TYPES_H
/* End of weapon_types.h */
