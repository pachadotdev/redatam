#ifndef CURSORREADER_HPP
#define CURSORREADER_HPP

#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include "ICursorReader.hpp"

class CursorReader : public ICursorReader
{
public:
    CursorReader(const std::string& file, int size)
        : CursorReader(file, false, false, size) {}

    CursorReader(const std::string& file, bool isString, bool isBin, int size)
        : BlockSize(size), Filename(file), IsString(isString), IsBinaryDataSet(isBin) {}

    void Open()
    {
        fileSize = std::filesystem::file_size(Filename);
        stream.open(Filename, std::ios::binary);
        if (!stream)
            throw std::runtime_error("Cannot open file: " + Filename);
        bytesPos = 0;
    }

    std::string ReadString()
    {
        std::vector<char> bytes(BlockSize);
        stream.read(bytes.data(), BlockSize);
        return std::string(bytes.begin(), bytes.end());
    }

    double ReadDouble()
    {
        std::vector<char> bytes(8);
        for (int i = 0; i < 8; i++)
            bytes[i] = static_cast<char>(ReadByte());
        double ret;
        std::memcpy(&ret, bytes.data(), sizeof(ret));
        return ret;
    }

    uint64_t ReadNumber()
    {
        int bitsRead = 0;
        uint64_t data = 0;
        data = feedBits(data, bitsRead);
        while (bitsRead < BlockSize)
        {
            if (IsBinaryDataSet)
                trailingBits = this->Read4Bytes();
            else
                trailingBits = this->ReadInt32();
            bytesPos += 4;
            trailingBitsCount = 32;
            data = feedBits(data, bitsRead);
        }
        return data;
    }

    void Close()
    {
        stream.close();
    }

    bool IsLastPos() const
    {
        return bytesPos >= fileSize;
    }

    int64_t Length() const
    {
        return stream.tellg();
    }

    uint32_t ReadInt32At(int64_t pos)
    {
        auto keepPos = stream.tellg();
        stream.seekg(pos * 4, std::ios::beg);
        uint32_t ret = this->ReadInt32();
        stream.seekg(keepPos);
        return ret;
    }

    uint32_t ReadLastInt32()
    {
        auto keepPos = stream.tellg();
        stream.seekg(-4, std::ios::end);
        uint32_t ret = this->ReadInt32();
        stream.seekg(keepPos);
        return ret;
    }

private:
    int BlockSize;
    std::string Filename;
    bool IsString;
    bool IsBinaryDataSet;
    std::ifstream stream;
    int64_t fileSize;
    uint32_t trailingBits = 0;
    int trailingBitsCount = 0;
    int bytesPos = 0;

    uint32_t ReadInt32()
    {
        return ReadInt16() + ReadInt16() * 0x10000;
    }

    uint32_t ReadInt16()
    {
        return ReadByte() + ReadByte() * 0x100;
    }

    uint32_t Read4Bytes()
    {
        return Read2Bytes() * 0x10000 + Read2Bytes();
    }

    uint32_t Read2Bytes()
    {
        return ReadByte() * 0x100 + ReadByte();
    }

    uint64_t feedBits(uint64_t data, int& bitsRead)
    {
        while (bitsRead < BlockSize && trailingBitsCount > 0)
        {
            data <<= 1;
            uint32_t bit = (trailingBits >> 31) & 1;
            data += bit;
            trailingBits <<= 1;
            trailingBitsCount--;
            bitsRead++;
        }
        return data;
    }

    uint32_t ReadByte()
    {
        char byte;
        stream.read(&byte, 1);
        if (stream.eof())
            return 0;
        if (!stream)
            throw std::runtime_error("Unable to read byte from stream.");
        return static_cast<uint32_t>(static_cast<unsigned char>(byte));
    }
};

#endif // CURSORREADER_HPP
