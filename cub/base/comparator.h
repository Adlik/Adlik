#ifndef H41C78ACD_06D8_4B41_A442_24F761B9DBE9
#define H41C78ACD_06D8_4B41_A442_24F761B9DBE9

////////////////////////////////////////////////////////////////////////////////
#define DECL_EQUALS(cls)                 \
  bool operator!=(const cls& rhs) const; \
  bool operator==(const cls& rhs) const

////////////////////////////////////////////////////////////////////////////////
#define FIELD_EQ(name) this->name == rhs.name
#define FIELD_LT(name) this->name < rhs.name

////////////////////////////////////////////////////////////////////////////////
#define SUPER_EQ(super) static_cast<const super&>(*this) == rhs
#define SUPER_LT(super) static_cast<const super&>(*this) < rhs

//////////////////////////////////////////////////////////////
#define DEF_EQUALS(cls)                        \
  bool cls::operator!=(const cls& rhs) const { \
    return !(*this == rhs);                    \
  }                                            \
  bool cls::operator==(const cls& rhs) const

////////////////////////////////////////////////////////////////////////////////
#define INLINE_EQUALS(cls)                \
  bool operator!=(const cls& rhs) const { \
    return !(*this == rhs);               \
  }                                       \
  bool operator==(const cls& rhs) const

////////////////////////////////////////////////////////////////////////////////
#define DECL_LESS(cls)               \
  bool operator<(const cls&) const;  \
  bool operator>(const cls&) const;  \
  bool operator<=(const cls&) const; \
  bool operator>=(const cls&) const

////////////////////////////////////////////////////////////////////////////////
#define DEF_LESS(cls)                          \
  bool cls::operator>(const cls& rhs) const {  \
    return !(*this <= rhs);                    \
  }                                            \
  bool cls::operator>=(const cls& rhs) const { \
    return !(*this < rhs);                     \
  }                                            \
  bool cls::operator<=(const cls& rhs) const { \
    return (*this < rhs) || (*this == rhs);    \
  }                                            \
  bool cls::operator<(const cls& rhs) const

////////////////////////////////////////////////////////////////////////////////
#define INLINE_LESS(cls)                    \
  bool operator>(const cls& rhs) const {    \
    return !(*this <= rhs);                 \
  }                                         \
  bool operator>=(const cls& rhs) const {   \
    return !(*this < rhs);                  \
  }                                         \
  bool operator<=(const cls& rhs) const {   \
    return (*this < rhs) || (*this == rhs); \
  }                                         \
  bool operator<(const cls& rhs) const

#endif
