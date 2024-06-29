#ifndef DATABLOCK_HPP
#define DATABLOCK_HPP

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <stdexcept>

class DataBlock {
public:
    std::vector<uint8_t> data;
    int n = 0;

    DataBlock(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot open file: " + path);

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
    }

    DataBlock(const std::vector<uint8_t>& bytes) : data(bytes) {}

    DataBlock getPart(int prevStart, int iStart) {
        std::vector<uint8_t> part(data.begin() + prevStart, data.begin() + iStart);
        return DataBlock(part);
    }

    bool eatPlausibleString(std::string& cad, bool filterByContent = true) {
        if (!PlausibleString(cad, filterByContent))
            return false;

        cad = eatShortString();
        return true;
    }

    bool PlausibleString(std::string& cad, bool filterByContent = true) {
        int keepN = n;
        cad.clear();
        if (n + 2 >= static_cast<int>(data.size())) {
            return false;
        }
        int length = eat16int();
        if (length < 0 || length > 128 || static_cast<size_t>(n) + static_cast<size_t>(length) > data.size()) {
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

    std::vector<int> GetAllMatches(const std::vector<uint8_t>& block) {
        std::vector<int> ret;
        int keepN = n;
        while (moveTo(block)) {
            ret.push_back(n);
            n++;
        }
        n = keepN;
        return ret;
    }

    bool IsText(const std::string& cad) {
        return std::all_of(cad.begin(), cad.end(), [](char c) { return std::isprint(c); });
    }

    std::string eatShortString() {
        int length = eat16int();
        return eatChars(length);
    }

    std::string makeString(const std::string& str) {
        std::string block;
        block.push_back(static_cast<char>(str.size() % 256));
        block.push_back(static_cast<char>(str.size() / 256));
        block.append(str);
        return block;
    }

    int eatInteger() {
        return 10000 * eat16int();
    }

    int eat32int() {
        return 0x10000 * eat16int() + eat16int();
    }

    int eat16int() {
        return 0x1 * eatByte() + 0x100 * eatByte();
    }

    uint8_t eatByte() {
        return data[n++];
    }

    std::string eatChars(int length) {
        std::string cad(data.begin() + n, data.begin() + n + length);
        n += length;
        return cad;
    }

    std::vector<uint8_t> addArrays(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b, const std::vector<uint8_t>& c) {
        return addArrays(addArrays(a, b), c);
    }

    std::vector<uint8_t> addArrays(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
        std::vector<uint8_t> ret(a.size() + b.size());
        std::copy(a.begin(), a.end(), ret.begin());
        std::copy(b.begin(), b.end(), ret.begin() + a.size());
        return ret;
    }

    std::vector<uint8_t> calcSize16(int n) {
        return { static_cast<uint8_t>(n % 256), static_cast<uint8_t>(n / 256) };
    }

    bool moveTo(const std::string& item) {
        return moveTo(makeString(item));
    }

    bool moveTo(const std::vector<uint8_t>& item) {
        int ret = SearchBytes(data.data(), item.data(), data.size(), item.size(), n);
        if (ret == -1)
            return false;
        n = ret;
        return true;
    }

    void move(int i) {
        n += i;
    }

    int SearchBytes(const uint8_t* haystack, const uint8_t* needle, size_t haystack_len, size_t needle_len, int start = 0) {
        for (size_t i = start; i <= haystack_len - needle_len; ++i) {
            if (matches(haystack, needle, i, needle_len))
                return i;
        }
        return -1;
    }

    bool matches(const uint8_t* haystack, const uint8_t* needle, int offset, size_t needle_len) {
        return std::equal(needle, needle + needle_len, haystack + offset);
    }
};

#endif // DATABLOCK_HPP
