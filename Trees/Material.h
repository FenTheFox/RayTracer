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
	size_t id;
	std::string name;
	Point3f Ka, Kd, Ks, Ke, Tf;
	double Ns, Ni, d, Tr;
	illum_model illum;

	operator Mat_Struct() const
	{
		return { Ka, Kd, Ks, Ns };
	}
};