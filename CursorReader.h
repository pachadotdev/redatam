#ifndef CURSOR_READER_H
#define CURSOR_READER_H

#include "ICursorReader.h"

class CursorReader : public ICursorReader {
public:
  void Read() override;
};

#endif
