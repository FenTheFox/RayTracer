#pragma once

#include "Structs.h"

class Material
{
public:
	static std::vector<Material> parseMaterials (const std::string&);

	Material (std::ifstream&);
	Material ();
	~Material ();

	enum illum_model
	{
		ColorOnAmbientOff, ColorAndAmbientOn, HighlightOn
	};
	int id;
	std::string name;
	Point3d Ka, Kd, Ks, Ke, Tf;
	double Ns, Ni, d, Tr;
	illum_model illum;
};