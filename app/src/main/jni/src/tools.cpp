/* Pushover
 *
 * Pushover is the legal property of its developers, whose
 * names are listed in the COPYRIGHT file, which is included
 * within the source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA
 */

#include "tools.h"

#include <SDL.h>

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <lmcons.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <dirent.h>

#include <stdexcept>

#if defined(__AROS__)
#include <proto/locale.h>
#include <proto/dos.h>
#endif

uint64_t getTime(void)
{
#ifdef WIN32
  FILETIME currentTime;
  GetSystemTimeAsFileTime(&currentTime);
  return currentTime.dwLowDateTime;
#else
  return time(0);
#endif
}


void srandFromTime(void) {

  srand(getTime());
}

std::string getHome() {
  char* prefDir = SDL_GetPrefPath("it.skowron.zbyl", "pushover");
  std::string prefDirString(prefDir);
  SDL_free(prefDir);
  return prefDirString;
}

std::vector<std::string> directoryEntries(const std::string & path) {
  std::vector<std::string> entries;

  if (path == "pushover/levels") {
    entries.push_back("original");
  }

  if (path == "pushover/levels/original") {
    entries.push_back("001.level");
    entries.push_back("002.level");
    entries.push_back("index.level");
  }

  return entries;

#if 0
  DIR * dir = ::opendir(path.c_str());
  if (dir == NULL)
    throw std::runtime_error("Can't open directory: " + path);
  std::vector<std::string> entries;
  for (struct dirent * i = ::readdir(dir); i != NULL; i = ::readdir(dir))
    entries.push_back(i->d_name);
  if (::closedir(dir) != 0)
    throw std::runtime_error("Can't close directory: " + path);
  return entries;
#endif
}


void amigaLocale(void)
{

#if defined (__AROS__)
  struct LocaleConv
  {
    char *langstr;
    char *gmostr;
  };

  struct LocaleConv LocaleConvTab[] =
  {
    {"catal�", "ca"}, // Catalan
    {"czech", "cz"}, // French
    {"dansk", "da"}, // Danish
    {"deutsch", "de"}, // German
    {"espa�ol", "es"}, // Spanish
    {"esperanto", "eo"}, // Esperanto
    {"fran�ais", "fr"}, // French
    {"hrvatski", "hr"}, // Croatian
    {"ellinik�", "el"}, // Greek
    {"�slenska", "is"}, // Icelandic
    {"italiano", "it"}, // Italian
    {"magyar", "hu"}, // Hungarian <3
    {"malti", "mt"}, // Maltese
    {"nederlands", "nl"}, // Dutch
    {"norsk", "no"}, // Norwegian Nynorsk
    {"polski", "pl"}, // Polish
    {"portugu�s" , "pt"}, // Portuguese
    {"russian", "ru"}, // Russian
    {"shqipja", "sq"}, // Albanian
    {"svenska", "sv"}, // Swedish
    {"suomi", "fi"}, // Finnish
    {"thai", "th"}, // Thai
    {"t�rk�e", "tr"}, // Turkish
    {0, 0}
  };

  struct Locale *defaultLocale;
  char *locale = NULL;

  if ((defaultLocale = OpenLocale(NULL)))
  {
    int i = 0;
    while (!locale && (i < 10) && defaultLocale->loc_PrefLanguages[i])
    {
      struct LocaleConv *lcptr = LocaleConvTab;

      while (lcptr->langstr)
      {
        if (!StrnCmp(defaultLocale, lcptr->langstr, defaultLocale->loc_PrefLanguages[i], -1, SC_ASCII))
          break;

        lcptr++;
      }

      locale = lcptr->gmostr;

      i++;
    }
    CloseLocale(defaultLocale);
  }

  if (locale)
  {
    SetVar("LC_MESSAGES", locale, strlen(locale) + 1, GVF_LOCAL_ONLY);
  }
#endif
}

