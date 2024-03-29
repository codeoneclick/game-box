#if defined(__WINOS__)

#include "common.h"

static std::set<std::string> g_custom_pathes;

std::string bundlepath()
{
	return "../gbWin32SharedResources/";
};

std::string executablepath()
{
	return "";
};

std::string documentspath()
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