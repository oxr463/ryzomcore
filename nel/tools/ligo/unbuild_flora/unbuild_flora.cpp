// NeL - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2019  Jan BOON (Kaetemi) <jan.boon@kaetemi.be>
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
//
// This utility is intended to rescue a lost flora .primitive file.
// It only recovers the generated flora positions, it cannot recover
// the flora zones.
//
// Author: Jan BOON (Kaetemi) <jan.boon@kaetemi.be>

// #include "../../3d/zone_lib/zone_utility.h"

#include <iostream>

#include <nel/misc/types_nl.h>
#include <nel/misc/file.h>
//#include <nel/misc/o_xml.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
//#include <nel/misc/bitmap.h>
#include <nel/misc/string_view.h>

#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/georges/u_form_loader.h>

//#include <nel/3d/quad_tree.h>
//#include <nel/3d/zone.h>
//#include <nel/3d/landscape.h>
//#include <nel/3d/zone_smoother.h>
//#include <nel/3d/zone_tgt_smoother.h>
//#include <nel/3d/zone_corner_smoother.h>
#include <nel/3d/scene_group.h>

//#include <nel/ligo/zone_region.h>

#include <vector>
//#include <set>
#include <map>

using namespace std;
using namespace NLMISC;
using namespace NL3D;
using namespace NLGEORGES;
//using namespace NLLIGO;

namespace /* anonymous */
{

/*

Process:
- Load all .plant sheets, map from .shape to .plant and bounding radius (Reference: prim_export, main.cpp)
- Load all source igs (Reference: ig_info, ig_info.cpp)
- Load all reference igs, remove matching entries from source igs
- Generate primitives

*/

std::string s_DfnDir; /* R:\leveldesign\DFN */
std::string s_LeveldesignDir; /* R:\leveldesign\game_elem\plant\ecosystem */
std::string s_SourceDir; /* R:\reference\2008_july\data\r2_desert2 */
std::string s_ReferenceDir; /* R:\pipeline\export\continents\r2_desert\zone_lighted_ig_land */
std::string s_PrimitiveFile; /* R:\graphics\primitive\r2_desert\r2_desert.primitive */

/*

Debug arguments:

"R:\leveldesign\DFN" "R:\leveldesign\game_elem\plant\ecosystem" "R:\reference\2008_july\data\r2_desert2" "R:\pipeline\export\continents\r2_desert\zone_lighted_ig_land" "R:\graphics\primitive\r2_desert\r2_desert.primitive"

*/

struct CPoint
{
	std::string ZoneLwr;

	CVector Pos; /* Position, height not necessarily specified (X="26218.738281" Y="-1092.078979" Z="0.000000") */
	float Angle; /* (2.827213) */
	float Scale; /* Scale (0.643217) */

	bool Plant;
	std::string Form; /* (FY_S2_savantree_B) */
	std::string Name; /* Generated unique name (ilot_008_savantree 13) */
	float Radius; /* Bounding radius (calculated from plant sheet and scale) (0.450252) */

};

/*
<CHILD TYPE="CPrimPoint">
	<PT X="26060.041016" Y="-1033.684692" Z="0.000000"/>
	<ANGLE VALUE="4.806300"/>
	<PROPERTY TYPE="string">
		<NAME>class</NAME>
		<STRING>prim</STRING>
	</PROPERTY>
	<PROPERTY TYPE="string">
		<NAME>form</NAME>
		<STRING>FY_S2_savantree_B</STRING>
	</PROPERTY>
	<PROPERTY TYPE="string">
		<NAME>layer</NAME>
		<STRING>0</STRING>
	</PROPERTY>
	<PROPERTY TYPE="string">
		<NAME>name</NAME>
		<STRING>ilot_008_savantree 15</STRING>
	</PROPERTY>
	<PROPERTY TYPE="string">
		<NAME>radius</NAME>
		<STRING>0.784730</STRING>
	</PROPERTY>
	<PROPERTY TYPE="string">
		<NAME>scale</NAME>
		<STRING>1.121043</STRING>
	</PROPERTY>
</CHILD>
*/

/*
* <?xml version="1.0"?>
<PRIMITIVES VERSION="1">
	<ROOT_PRIMITIVE TYPE="CPrimNode">
		<ALIAS LAST_GENERATED="0"/>
		<CHILD TYPE="CPrimNode">
			<PROPERTY TYPE="string">
				<NAME>class</NAME>
				<STRING>flora</STRING>
			</PROPERTY>
			<PROPERTY TYPE="string">
				<NAME>name</NAME>
				<STRING>R2 flora 1</STRING>
			</PROPERTY>
			<CHILD ... />
			<CHILD ... />
			<CHILD ... />
			...
		</CHILD>
	</ROOT_PRIMITIVE>
</PRIMITIVES>
*/

/*
instance fy_s2_savantree_c.shape : x = 23031.2, y = -1269.2, z = 75.8, sx = 0.5, sy = 0.5, sz = 0.5
instance fy_s2_savantree_c.shape : x = 22906.6, y = -1148.1, z = 79.6, sx = 0.6, sy = 0.6, sz = 0.6
*/

struct CPlant
{
	std::string Form;
	std::string Shape;
	float Radius;
};

std::map<std::string, CPlant> s_ShapeToForm;
std::set<CPoint> s_Instances;

bool loadLeveldesign()
{
	UFormLoader *formLoader = UFormLoader::createLoader();
	struct CRel0 { CRel0(UFormLoader *v) : m(v) {} ~CRel0() { UFormLoader::releaseLoader(m); } UFormLoader *m; } rel0(formLoader);

	std::vector<std::string> plants;
	CPath::getFileList("plant", plants);

	for (std::vector<std::string>::iterator it(plants.begin()), end(plants.end()); it != end; ++it)
	{
		printf("%s\n", (*it).c_str());
		CSmartPtr <UForm> form = formLoader->loadForm(*it);
		if (!form)
			continue;
		CPlant plant;
		plant.Form = toLowerAscii(*it);
		if (!form->getRootNode().getValueByName(plant.Shape, "3D.Shape"))
			continue;
		if (plant.Shape.empty())
		{
			if (!form->getRootNode().getValueByName(plant.Shape, "3D.SpringFX.FXName"))
				continue;
		}
		if (plant.Shape.empty())
			continue;
		plant.Shape.c_str();
		(void)toLowerAscii(&plant.Shape[0]);
		if (!form->getRootNode().getValueByName(plant.Radius, "3D.Bounding Radius"))
			continue;
		printf(" = '%s', %f\n", plant.Shape.c_str(), plant.Radius);
		s_ShapeToForm[plant.Shape] = plant;
	}

	return true;
}

bool loadInstances()
{
	std::vector<std::string> igs;
	CPath::getPathContent(s_SourceDir, true, false, true, igs);

	for (std::vector<std::string>::iterator it(igs.begin()), end(igs.end()); it != end; ++it)
	{
		if (CFile::getExtension(*it) != nlstr("ig"))
			continue;
		printf("%s\n", (*it).c_str());
		CInstanceGroup ig;
		CIFile inputStream;
		if (!inputStream.open(*it))
		{
			nlwarning("Unable to open %s\n", (*it).c_str());
			return false;
		}
		ig.serial(inputStream);
		CVector gpos = ig.getGlobalPos();
		if (gpos.x != 0.0f || gpos.y != 0.0f || gpos.z != 0.0f)
		{
			nlwarning("Invalid global pos: %f, %f, %f", gpos.x, gpos.y, gpos.z);
			return false;
		}
		string zoneLwr = toLowerAscii(CFile::getFilenameWithoutExtension(*it));
		for (ptrdiff_t i = 0; i < (ptrdiff_t)ig._InstancesInfos.size(); ++i)
		{
			CInstanceGroup::CInstance &info = ig._InstancesInfos[i];
			CPoint instance;
			instance.Pos = info.Pos;
			instance.Angle = info.Rot.getAngle();
			instance.Scale = info.Scale.z;
			string shape = toLowerAscii(info.Name);
			printf("%s\n", shape.c_str());
			std::map<std::string, CPlant>::iterator formIt = s_ShapeToForm.find(shape);
			if (formIt != s_ShapeToForm.end())
			{
				instance.Form = formIt->second.Form;
				instance.Name = CFile::getFilenameWithoutExtension(instance.Form) + nlstr("_") + zoneLwr + nlstr("_") + toString(i);
				instance.Radius = instance.Scale * formIt->second.Radius;
				printf(" = %f, %f, %f, %f, %f, '%s', '%s', %f\n",  instance.Pos.x, instance.Pos.y, instance.Pos.z, instance.Angle, instance.Scale, instance.Form .c_str(), instance.Name.c_str(), instance.Radius);
				instance.Plant = true;
			}
			else
			{
				instance.Plant = false;
			}
		}
	}

	return true;
}

bool unbuildFlora()
{
	CPath::addSearchPath(s_DfnDir, true, false);
	CPath::addSearchPath(s_LeveldesignDir, true, false);

	return loadLeveldesign()
		&& loadInstances();
}

bool unbuildFlora(NLMISC::CCmdArgs &args)
{
	s_DfnDir = args.getAdditionalArg("dfn")[0];
	s_LeveldesignDir = args.getAdditionalArg("leveldesign")[0];
	s_SourceDir = args.getAdditionalArg("source")[0];
	s_ReferenceDir = args.getAdditionalArg("reference")[0];
	s_PrimitiveFile = args.getAdditionalArg("primitive")[0];

	return unbuildFlora();
}

} /* anonymous namespace */

int main(int argc, char **argv)
{
	NLMISC::CApplicationContext myApplicationContext;

	NLMISC::CCmdArgs args;

	args.addAdditionalArg("dfn", "Input folder with DFN");
	args.addAdditionalArg("leveldesign", "Input folder with plant sheets");
	args.addAdditionalArg("source", "Input folder with IGs containing flora");
	args.addAdditionalArg("reference", "Input folder with IGs missing flora");
	args.addAdditionalArg("primitive", "Output flora primitive file");

	if (!args.parse(argc, argv))
	{
		return EXIT_FAILURE;
	}

	if (!unbuildFlora(args))
	{
		args.displayHelp();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/* end of file */
