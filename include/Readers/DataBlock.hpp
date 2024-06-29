#ifndef DATABLOCK_HPP
#define DATABLOCK_HPP

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <stdexcept>

class DataBlock
{
public:
    std::vector<uint8_t> data;
    int n = 0;

    DataBlock(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot open file: " + path);

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
    }

    DataBlock(const std::vector<uint8_t>& bytes)
        : data(bytes)
    {
    }

    DataBlock getPart(int prevStart, int iStart)
    {
        std::vector<uint8_t> part(data.begin() + prevStart, data.begin() + iStart);
        return DataBlock(part);
    }

    bool eatPlausibleString(std::string& cad, bool filterByContent = true)
    {
        if (!PlausibleString(cad, filterByContent))
            return false;

        cad = eatShortString();
        return true;
    }

    bool PlausibleString(std::string& cad, bool filterByContent = true)
    {
        int keepN = n;
        cad.clear();
        if (n + 2 >= static_cast<int>(data.size())) {
            return false;
        int length = eat16int();
        if (length < 0 || length > 128 || n + length > data.size())
        {
            n = keepN;
            return false;
        }
        move(-2);
        cad = eatShortString();
        n = keepN;

        if (filterByContent && !IsText(cad))
            return false;
        return true;
    }

    std::vector<int> GetAllMatches(const std::vector<uint8_t>& block)
    {
        std::vector<int> ret;
        int keepN = n;
        while (moveTo(block))
        {
            ret.push_back(n);
            n++;
        }
        n = keepN;
        return ret;
    }

    bool IsText(const std::string& cad)
    {
        return std::all_of(cad.begin(), cad.end(), [](char c) {
            c = std::tolower(c);
            return (c >= 'a' && c <= 'z') || c == ' ' || c == '-' || c == '_' ||
                   (c >= '0' && c <= '9');
        });
    }

    int moveBackString(int maxLenght = 65536)
    {
        move(-2);
        int offset = 0;
        while (offset < n)
        {
            auto bytes = calcSize16(offset);
            if (matches(data.data(), bytes.data(), n - offset, bytes.size()))
            {
                n -= offset;
                return offset;
            }
            offset++;
            if (offset > maxLenght)
                break;
        }
        return -1;
    }

    std::vector<uint8_t> makeStringBlock(const std::string& entity)
    {
        auto intSize = calcSize16(entity);
        auto text = makeString(entity);
        auto block = addArrays(intSize, text);
        return block;
    }

private:
    static std::vector<uint8_t> makeString(const std::string& entity)
    {
        return std::vector<uint8_t>(entity.begin(), entity.end());
    }

    std::vector<uint8_t> calcSize(const std::string& cad)
    {
        int i = cad.size();
        return addArrays(calcSize16(i / 65536), calcSize16(i % 65536));
    }

    std::vector<uint8_t> calcSize16(const std::string& cad)
    {
        return calcSize16(cad.size());
    }

    std::string eatString()
    {
        int length = eat32intInv();
        return eatChars(length);
    }

public:
    std::string eatShortString()
    {
        int length = eat16int();
        if (length == 0xFFFF)
        {
            return eatString();
        }
        else
        {
            return eatChars(length);
        }
    }

    int eat32intInv()
    {
        return eat16int() + 0x10000 * eat16int();
    }

    int eat32int()
    {
        return 0x10000 * eat16int() + eat16int();
    }

    int eat16int()
    {
        return 0x1 * eatByte() + 0x100 * eatByte();
    }

    uint8_t eatByte()
    {
        return data[n++];
    }

    std::string eatChars(int length)
    {
        std::string cad(data.begin() + n, data.begin() + n + length);
        n += length;
        return cad;
    }

    std::vector<uint8_t> addArrays(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b, const std::vector<uint8_t>& c)
    {
        return addArrays(addArrays(a, b), c);
    }

    std::vector<uint8_t> addArrays(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b)
    {
        std::vector<uint8_t> ret(a.size() + b.size());
        std::copy(a.begin(), a.end(), ret.begin());
        std::copy(b.begin(), b.end(), ret.begin() + a.size());
        return ret;
    }

    std::vector<uint8_t> calcSize16(int n)
    {
        return { static_cast<uint8_t>(n % 256), static_cast<uint8_t>(n / 256) };
    }

    bool moveTo(const std::string& item)
    {
        return moveTo(makeString(item));
    }

    bool moveTo(const std::vector<uint8_t>& item)
    {
        int ret = SearchBytes(data.data(), item.data(), data.size(), item.size(), n);
        if (ret == -1)
            return false;
        n = ret;
        return true;
    }

    void move(int i)
    {
        n += i;
    }

    int SearchBytes(const uint8_t* haystack, const uint8_t* needle, size_t haystack_len, size_t needle_len, int start = 0)
    {
        for (size_t i = start; i <= haystack_len - needle_len; ++i)
        {
            if (matches(haystack, needle, i, needle_len))
                return i;
        }
        return -1;
    }

    bool matches(const uint8_t* haystack, const uint8_t* needle, int offset, size_t needle_len)
    {
        return std::equal(needle, needle + needle_len, haystack + offset);
    }
};

#endif // DATABLOCK_HPP
