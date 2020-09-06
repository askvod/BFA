/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITYCORE_WORDFILTERMGR_H
#define TRINITYCORE_WORDFILTERMGR_H

#include <string>
#include <map>

class WordFilterMgr
{
    private:
       // friend class ACE_Singleton<WordFilterMgr, ACE_Null_Mutex>;

        WordFilterMgr();
        ~WordFilterMgr();

    public:
    static WordFilterMgr* instance()
        {
          static WordFilterMgr* instance = new WordFilterMgr();
          return instance;
        }
        /// Note for 0.6v : Will used std::map instead of UNORDERED_MAP, because of problems with cross-platform compilation.
        // [letter][analogs] 
        typedef std::map<char, std::string> LetterAnalogMap; 
        // [converted][original]
        typedef std::map<std::string, std::string> BadWordMap; 

        void LoadLetterAnalogs();
        void LoadBadWords();	
		
        inline void ConvertLettersToAnalogs(std::string& text);
        std::string FindBadWord(const std::string& text);	
		
        // manipulations with container 
        bool AddBadWord(const std::string& badWord, bool toDB = false);
        bool RemoveBadWord(const std::string& badWord, bool fromDB = false);

		// element (const) accessor 
        BadWordMap GetBadWords() const { return m_badWords; }

    private:
        LetterAnalogMap m_letterAnalogs;
        BadWordMap m_badWords; 
};

#define sWordFilterMgr WordFilterMgr::instance()

#endif