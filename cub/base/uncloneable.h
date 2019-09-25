#ifndef H671141B2_B247_440B_8E83_476558110FFE
#define H671141B2_B247_440B_8E83_476558110FFE

#define DISALLOW_COPY_AND_ASSIGN(xlass) \
  DISALLOW_ASSIGN(xlass)                \
  DISALLOW_COPY(xlass)

#define DISALLOW_ASSIGN(xlass) \
public:                        \
  xlass& operator=(const xlass&) = delete;

#define DISALLOW_COPY(xlass) \
public:                      \
  xlass(const xlass&) = delete;

#endif
