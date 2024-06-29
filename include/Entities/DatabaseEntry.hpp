#ifndef DATABASEENTRY_HPP
#define DATABASEENTRY_HPP

#include <string>
#include <vector>
#include <utility> // for std::pair
#include <memory>

class RedatamDatabase; // Forward declaration

class DatabaseEntry
{
public:
    std::string DictFolder;
    std::string Caption;
    std::string Inl;
    std::shared_ptr<RedatamDatabase> Database;
    std::vector<std::pair<std::string, std::string>> FilesQueue;

    DatabaseEntry() = default;
    ~DatabaseEntry() = default;
};

#endif // DATABASEENTRY_HPP
