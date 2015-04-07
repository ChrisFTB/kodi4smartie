/*
Kodi4Smartie - A DLL to display information from Kodi on a LCD character display

Copyright (C) 2015  Chris Vavruska

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <Psapi.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <regex>
#include <cpprest/asyncrt_utils.h>
#include <map>

#include "config.hpp"
#include "logging.hpp"

using namespace std;
using namespace utility;

typedef map<std::regex *, string> regex_map_t;
regex_map_t regex_map;

config_t configs[] = {
	{ "logging",	MINT, 0,	"" },
	{ "newfile",	MINT, 1,	"" },
	{ "host",		MSTR, 0,	"LOCALHOST" },
	{ "port",		MSTR, 0,	"9090" },
	{ "lcd_width",	MINT, 20,	"" },
	{ "use_bars",	MSTR, 1,	"" },
	{ "reset_delay",MINT, 3,	"" },
	{ "kodiexe",	MSTR, 0,	"Kodi.exe"},	
	{ "connect_delay", MINT, 1, "" },
	{ "time_format", MSTR, 0, "mm/dd/yy hh:nn" },

	//strings. override these in the config file to localize
	{ "welcome", MSTR, 0, "Kodi Media Center" },
	{ "stop_str", MSTR, 0, "Stop" },
	{ "track_str",	MSTR, 0,	"Track" },
	{ "channel_str",MSTR, 0,	"Channel" },	
	{ "ff_str",		MSTR, 0,	"Fast-Forward" },
	{ "rewind_str",	MSTR, 0,	"Rewind" },
	{ "volume_str", MSTR, 0,    "Volume"},	

};

unsigned int get_config(configs_e index)
{
	if ((index >= 0) && (index < cMAX))
	{
		return configs[index].val;
	}
	else
	{
		return -1;
	}
}

const char *get_config_str(configs_e index)
{
	if ((index >= 0) && (index < cMAX))
	{
		return configs[index].str;
	}
	else
	{
		return NULL;
	}
}

void set_config(char *key, char *data)
{

	for (int loop = 0; loop < (configs_e)cMAX; loop++)
	{
		if (_stricmp(key, configs[loop].key) == 0)
		{
			if (configs[loop].datatype == MINT)
			{
				configs[loop].val = atoi(data);
			} 
			else if (configs[loop].datatype == MSTR)
			{
				strncpy_s(configs[loop].str, data, sizeof(configs[loop].str));
			}
		}
	}
}

void set_regex(char *data)
{
	char *comma; 
	
	do //find a comma that is not escaped
	{
		comma = strchr(data, ',');
	} while (comma && (*(comma - 1) == '\\'));

	if (comma)
	{
		*comma = 0;
		comma++;
	}

	try
	{
		regex *reg = new regex(string(data));
		regex_map.insert(pair<regex *, string>(reg, string(comma)));
	}
	catch (...)
	{
		OutputDebugStringA("Bad RegEx found");
		OutputDebugStringA(data);
	}
}

void init_config()
{
	FILE *fd;
	char line[128];
	char *equal;
	char *data;

	if (fopen_s(&fd, "C:\\Users\\cv\\Documents\\Visual Studio 2013\\Projects\\kodi_smartie\\Debug\\kodi4smartie.cfg", "r") == 0)
	{
		while (fgets(line, sizeof(line), fd))
		{
			if (line[0] == '/' && line[1] == '/')
			{
				continue;
			}

			if ((equal = strchr(line, '=')) != NULL)
			{
				*equal = 0;
				data = equal + 1;

				//remove the trailing newline
				if (data[strlen(data)] == '\n')
				{
					data[strlen(data)] = 0;
				}
				if (_stricmp(line, "regex") == 0)
				{
					set_regex(data);
				}
				else
				{
					set_config(line, data);
				}
			}
		}
		fclose(fd);
	}
}
