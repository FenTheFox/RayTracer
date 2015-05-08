#include "stdafx.h"

#include "Material.h"

using namespace std;

vector<Material> Material::parseMaterials (const string& f)
{
	vector<Material> mats;
	ifstream ifs (f);
	string line;
	while (getline (ifs, line, 'n'))
		mats.emplace_back (ifs);

	for (size_t i = 0; i < mats.size (); i++)
		mats[i].id = i;

	return mats;
}

Material::Material (ifstream &f)
{
	size_t splitIdx;
	string line, fieldName, field;
	/*getline (f, line);
	if ((line = trim (line)) == "") return;
	splitIdx = line.find_first_of (' ');
	name = line.substr (splitIdx, line.length() - 1);*/
	while (getline (f, line)) {
		if ((line = trim (line)) == "") break;
		splitIdx = line.find_first_of (' ');
		fieldName = line.substr (0, splitIdx);
		field = line.substr (splitIdx + 1, line.length () - 1);
		if (fieldName == "ewmtl")
			name = field;
		else if (fieldName == "Ka")
			Ka = Point3f (field);
		else if (fieldName == "Kd")
			Kd = Point3f (field);
		else if (fieldName == "Ks")
			Ks = Point3f (field);
		else if (fieldName == "Ke")
			Ke = Point3f (field);
		else if (fieldName == "Tf")
			Tf = Point3f (field);
		else if (fieldName == "illum")
			illum = (illum_model)atoi (field.c_str ());
		else if (fieldName == "Ns")
			Ns = atof (field.c_str ()) / 1000;
		else if (fieldName == "Ni")
			Ni = atof (field.c_str ());
		else if (fieldName == "d")
			d = atof (field.c_str ());
		else if (fieldName == "Tr")
			Tr = atof (field.c_str ());
	}
}


Material::Material ()
{}
Material::~Material ()
{}
