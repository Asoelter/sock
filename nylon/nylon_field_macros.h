#ifndef NYLON_FIELD_MACROS_H
#define NYLON_FIELD_MACROS_H

#define FIELD(archetype, fieldName)                             \
    struct fieldName##Field : archetype<fieldName##Field>       \
    {                                                           \
        static constexpr const char * name() noexcept           \
        {                                                       \
            return #fieldName;                                  \
        }                                                       \
                                                                \
        ValueType const & value() const                         \
        {                                                       \
            return fieldName;                                   \
        }                                                       \
                                                                \
        ValueType & value()                                     \
        {                                                       \
            return fieldName;                                   \
        }                                                       \
                                                                \
        ValueType fieldName;                                    \
    }

#define CHAR_FIELD(name)   FIELD(CharField,   name)
#define STRING_FIELD(name) FIELD(StringField, name)
#define UINT8_FIELD(name)  FIELD(UInt8Field,  name)

#endif // NYLON_FIELD_MACROS_H
