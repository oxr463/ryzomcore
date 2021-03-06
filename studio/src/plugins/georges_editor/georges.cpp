// Object Viewer Qt - Georges Editor Plugin - MMORPG Framework <http://dev.ryzom.com/projects/ryzom/>
// Copyright (C) 2011  Adrian JAEKEL <aj@elane2k.com>
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

#include "stdpch.h"
#include "georges.h"
#include "nel/misc/o_xml.h"

// STL includes

// NeL includes
#include <nel/georges/u_form_loader.h>
#include <nel/georges/u_form.h>
#include <nel/georges/u_type.h>

// Project includes

using namespace NLGEORGES;

namespace GeorgesQt 
{

	CGeorges::CGeorges(): FormLoader(0) 
	{
		FormLoader = UFormLoader::createLoader();
	}

	CGeorges::~CGeorges() 
	{
	}

	UForm *CGeorges::loadForm(std::string formName) 
	{
		UForm *form = FormLoader->loadForm(formName.c_str());

		return form;
	}

	UFormDfn *CGeorges::loadFormDfn(std::string formName) 
	{
		UFormDfn *formdfn = FormLoader->loadFormDfn(formName.c_str());

		return formdfn;
	}

	UType *CGeorges::loadFormType(std::string formName) 
	{
		UType *type = FormLoader->loadFormType(formName.c_str());

		return type;
	}

} /* namespace GeorgesQt */
