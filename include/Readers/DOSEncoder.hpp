#ifndef DOSENCODER_HPP
#define DOSENCODER_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"

class MsDOSEncoder
{
private:
    std::vector<int> oldChars = { 160, 130, 162, 163, 164 };
    RedatamDatabase* db;
    std::unordered_map<char, int> matches;
    int sumMatches;
    bool testing;

    void Process(std::string& value)
    {
        if (testing)
        {
            CountMsDOSChars(value);
        }
        else
        {
            value = ReplaceMsDOSChars(value);
        }
    }

    static std::string ReplaceMsDOSChars(const std::string& value)
    {
        std::vector<char> text(value.begin(), value.end());
        return std::string(text.begin(), text.end()); // Assuming encoding replacement here
    }

    void CountMsDOSChars(const std::string& value)
    {
        for (char c : value)
        {
            if (std::find(oldChars.begin(), oldChars.end(), static_cast<int>(c)) != oldChars.end())
            {
                matches[c]++;
                sumMatches++;
            }
        }
    }

    bool ProcessEntities(std::vector<std::shared_ptr<Entity>>& entities)
    {
        for (auto& entity : entities)
        {
            for (auto& variable : entity->Variables)
            {
                Process(variable->Label);
                for (auto& value : variable->ValueLabels)
                {
                    Process(value.Value);
                }
            }
            if (ProcessEntities(entity->Children))
            {
                if (testing)
                    return true;
            }
        }
        return (matches.size() >= 3 && sumMatches >= 6);
    }

public:
    MsDOSEncoder(RedatamDatabase* db) : db(db) {}

    void ReencodeLabels()
    {
        testing = false;
        ProcessEntities(db->Entities);
    }

    bool RequiresProcessing()
    {
        testing = true;
        matches.clear();
        sumMatches = 0;
        return ProcessEntities(db->Entities);
    }
};

#endif // DOSENCODER_HPP
