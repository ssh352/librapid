//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_UUID_H
#define RAPID_CORE_UUID_H

#include <array>
#include <iomanip>
#include <cstdint>
#include <string_view>
#include <cstring>

namespace rapid { namespace core {

class uuid {
public:
    static uuid newUUID();

    static uuid fromString(std::string_view str);

    uuid();

    explicit uuid(const std::array<uint8_t, 16> &bytes);

    uuid(const uuid &other);

    uuid& operator=(const uuid &other);

    bool operator==(const uuid &other) const;

    bool operator!=(const uuid &other) const;

    size_t hash() const;

    operator std::string() const;

private:
    friend std::ostream & operator<<(std::ostream &s, const uuid & id);

    std::array<uint8_t, 16> bytes_;
};

inline bool uuid::operator==(const uuid & other) const {
    return memcmp(bytes_.data(), other.bytes_.data(), bytes_.size()) == 0;
}

inline bool uuid::operator!=(const uuid & other) const {
    return !(memcmp(bytes_.data(), other.bytes_.data(), bytes_.size()) == 0);
}

} }

namespace std {

template <>
struct hash<rapid::core::uuid> {
    typedef size_t result_type;
    typedef rapid::core::uuid argument_type;

    result_type operator()(const argument_type & id) const {
        return id.hash();
    }
};

}

#endif // RAPID_CORE_UUID_H
