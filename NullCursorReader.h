#ifndef NULL_CURSOR_READER_H
#define NULL_CURSOR_READER_H

#include "ICursorReader.h"

class NullCursorReader : public ICursorReader {
public:
  void Read() override;
};

#endif
