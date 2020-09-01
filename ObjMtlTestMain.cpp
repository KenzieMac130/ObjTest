#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.hpp"

#define SHOW_PBR true
#define SHOW_LEGACY true
#define SHOW_TEXTURE_OPTIONS true

#include <iostream>

/* -------------------------------------- Logging -------------------------------------- */

#define DEBUG_NEWLINE() putchar('\n');
#define DEBUG_LOG(_format, ...) printf(_format, __VA_ARGS__); DEBUG_NEWLINE();
#define DEBUG_SEPERATOR() DEBUG_LOG("-------------------------------------------------------------------------------------------------")

void logInteger(const char* name, int val)
{
	DEBUG_LOG("%s: %d", name, val);
}

void logBool(const char* name, bool val)
{
	if (val) {
		DEBUG_LOG("%s: true", name);
	}
	else {
		DEBUG_LOG("%s: false", name);
	}
}

void logChar(const char* name, char val)
{
	DEBUG_LOG("%s: %c", name, val);
}

void logReal(const char* name, tinyobj::real_t val)
{
	DEBUG_LOG("%s: %f", name, (float)val);
}

void logVector3(const char* name, tinyobj::real_t val[3])
{
	DEBUG_LOG("%s: (%f, %f, %f)", name, (float)val[0], (float)val[1], (float)val[2]);
}

void logCString(const char* name, const char* val)
{
	DEBUG_LOG("%s: %s", name, val);
}

void logStdString(const char* name, std::string val)
{
	logCString(name, val.c_str());
}

void logTexture(const char* name, std::string path, tinyobj::texture_option_t& options, bool logOptions = true)
{
	if (path.empty()) { logCString(name, "None"); return; }

	logStdString(name, path);

	if (!logOptions) { return; }

	char* texTypesStr[]{
		"None",
		"Sphere",
		"Cube Top",
		"Cube Bottom",
		"Cube Front",
		"Cube Back",
		"Cube Left",
		"Cube Right"
	};
	if (options.type < 8 && options.type >= 0) {
		logCString("\tType", texTypesStr[options.type]);
	}
	else {
		logCString("\tType", "ERROR!!!");
	}

	logReal("\tSharpness", options.sharpness);
	logReal("\tBrightness", options.brightness);
	logReal("\tContrast", options.contrast);
	logVector3("\tOrigin Offset", options.origin_offset);
	logVector3("\tScale", options.scale);
	logVector3("\tTurbulence", options.turbulence);
	logInteger("\tTexture Resolution", options.texture_resolution);
	logBool("\tClamp", options.clamp);
	logChar("\tImfchan", options.imfchan);
	logBool("\tBlendU", options.blendu);
	logBool("\tBlendV", options.blendv);
	logReal("\tBump Multiplier", options.bump_multiplier);
	logStdString("\tColor Space", options.colorspace);
}

#define MAT_LEGACY(_contents) if(enableLegacy){_contents;}
#define MAT_PBR(_contents) if(enablePBR){_contents;}

void logMaterial(tinyobj::material_t& mat, bool logOptions, bool enableLegacy, bool enablePBR)
{
	logStdString("Name", mat.name);
	DEBUG_NEWLINE();

	MAT_LEGACY(logVector3("Ambient", mat.ambient));
	logVector3("Color", mat.diffuse);
	logVector3("Specular", mat.specular);
	logVector3("Transmittance", mat.transmittance);
	logVector3("Emission", mat.emission);
	DEBUG_NEWLINE();

	MAT_LEGACY(logReal("Specular Exponent", mat.shininess));
	logReal("IOR", mat.ior);
	logReal("Opacity", mat.dissolve);
	DEBUG_NEWLINE();

	MAT_LEGACY(logInteger("Illumination Method", mat.illum));
	DEBUG_NEWLINE();

	MAT_PBR(logReal("Roughness", mat.roughness));
	MAT_PBR(logReal("Metallic", mat.metallic));
	MAT_PBR(logReal("Sheen", mat.sheen));
	MAT_PBR(logReal("Clearcoat", mat.clearcoat_thickness));
	MAT_PBR(logReal("Clearcoat Roughness", mat.clearcoat_roughness));
	MAT_PBR(logReal("Anisotropy", mat.anisotropy));
	MAT_PBR(logReal("Anisotropy Rotation", mat.anisotropy_rotation));
	DEBUG_NEWLINE();

	MAT_LEGACY(logTexture("Ambient Map", mat.ambient_texname, mat.ambient_texopt, logOptions));
	logTexture("Color Map", mat.diffuse_texname, mat.diffuse_texopt, logOptions);
	logTexture("Specular Map", mat.specular_texname, mat.specular_texopt, logOptions);
	MAT_LEGACY(logTexture("Specular Exponent Map", mat.specular_texname, mat.specular_texopt, logOptions));
	logTexture("Bump Map", mat.bump_texname, mat.bump_texopt, logOptions);
	logTexture("Displacement Map", mat.displacement_texname, mat.displacement_texopt, logOptions);
	logTexture("Opacity Map", mat.alpha_texname, mat.alpha_texopt, logOptions);
	MAT_LEGACY(logTexture("Reflection Map", mat.reflection_texname, mat.reflection_texopt, logOptions));
	logTexture("Emission Map", mat.emissive_texname, mat.emissive_texopt, logOptions);
	DEBUG_NEWLINE();

	MAT_PBR(logTexture("Roughness Map", mat.roughness_texname, mat.roughness_texopt, logOptions));
	MAT_PBR(logTexture("Metallic Map", mat.metallic_texname, mat.metallic_texopt, logOptions));
	MAT_PBR(logTexture("Sheen Map", mat.sheen_texname, mat.sheen_texopt, logOptions));
	MAT_PBR(logTexture("Normal Map", mat.normal_texname, mat.normal_texopt, logOptions));
}

/* -------------------------------------- Main -------------------------------------- */

/* Stuff copied from tiny_obj examples */

int main(int argc, char* argv[])
{
	std::string fileName;
	std::string fileBaseDir;
	if (argc < 2) {
		printf("Enter File Path: ");
		std::cin >> fileName;
	}
	else {
		fileName = argv[1];
	}
	fileBaseDir = fileName.substr(0, fileName.find_last_of("/\\"));


	/* Load Obj */
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName.c_str(), fileBaseDir.c_str());

	if (!warn.empty()) {
		printf("%s\n", warn.c_str());
	}

	if (!err.empty()) {
		printf("%s\n", err.c_str());
	}

	if (!ret) {
		exit(1);
	}

	/* Loop over Materials */
	DEBUG_SEPERATOR();
	logInteger("Material Count", (int)materials.size());
	DEBUG_SEPERATOR();
	for (size_t i = 0; i < materials.size(); i++)
	{
		logMaterial(materials[i], SHOW_TEXTURE_OPTIONS, SHOW_LEGACY, SHOW_PBR);
		DEBUG_SEPERATOR();
	}

	getchar();
	return 0;
}