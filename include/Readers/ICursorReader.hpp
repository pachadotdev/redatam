#ifndef ICURSORREADER_HPP
#define ICURSORREADER_HPP

#include <string>

namespace RedatamLib {

class ICursorReader
{
public:
    virtual ~ICursorReader() = default;

    virtual void Close() = 0;
    virtual bool IsLastPos() const = 0;
    virtual void Open() = 0;
    virtual double ReadDouble() = 0;
    virtual uint32_t ReadInt16() = 0;
    virtual uint32_t ReadInt32() = 0;
    virtual uint32_t ReadLastInt32() = 0;
    virtual uint64_t ReadNumber() = 0;
    virtual std::string ReadString() = 0;

    virtual int64_t Length() const = 0;
    virtual uint32_t ReadInt32At(int64_t pos) = 0;
};

}

#endif // ICURSORREADER_HPP
