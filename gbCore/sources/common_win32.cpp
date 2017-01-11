#if defined(__WIN32__)

#include "common.h"

static std::set<std::string> g_custom_pathes;

std::string bundlepath()
{
	return "output/debug/resources/";
};

std::string executablepath()
{
	return "";
};

void add_custom_path(const std::string& path)
{
	g_custom_pathes.insert(path);
};

void remove_custom_path(const std::string& path)
{
	g_custom_pathes.erase(path);
};

std::set<std::string> custom_pathes()
{
	return g_custom_pathes;
};

#endif