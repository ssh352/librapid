#include <uuid/uuid.h>

#include <rapid/core/exception.h>
#include <rapid/core/uuid.h>

namespace rapid { namespace core {

static uint8_t to_char(const char digital) {
    if (digital > 47 && digital < 58)
        return digital - 48;

    if (digital > 96 && digital < 103)
        return digital - 87;

    if (digital > 64 && digital < 71)
        return digital - 55;

    throw UuidErrorException("invalid digital");
}

static uint8_t make_hex(char a, char b) {
    return to_char(a) * 16 + to_char(b);
}

static std::array<uint8_t, 16> parse_uuid_string(const std::string & from_string) {
    if (from_string.length() != 36) {
        throw UuidErrorException("invalid UUID");
    }

    if (from_string[8] != '-'
        || from_string[13] != '-'
        || from_string[18] != '-'
        || from_string[23] != '-') {
        throw UuidErrorException("invalid UUID");
    }

    std::array<uint8_t, 16> uuid{};

    for (size_t i = 0, j = 0; i < from_string.length(); ++i) {
        if (from_string[i] == '-')
            continue;
        uuid[j] = make_hex(from_string[i], from_string[i + 1]);
        ++j;
        ++i;
    }
    return uuid;
}

uuid::uuid() {
    bytes_.fill(0);
}

uuid uuid::fromString(std::string_view str) {
   return uuid(parse_uuid_string(str.data()));
}

uuid::uuid(const std::array<uint8_t, 16> &bytes)
    : bytes_(bytes) {
}

uuid::uuid(const uuid &other) {
    *this = other;
}

uuid & uuid::operator=(const uuid & other) {
    if (this != &other) {
        bytes_ = other.bytes_;
    }
    return *this;
}

size_t uuid::hash() const {
    size_t seed = 0;

    for (auto const byte : bytes_) {
        seed ^= static_cast<size_t>(byte) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

std::ostream & operator<<(std::ostream &s, const uuid & id) {
    return s << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<int32_t>(id.bytes_[0])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[1])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[2])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[3])
        << "-"
        << std::setw(2) << static_cast<int32_t>(id.bytes_[4])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[5])
        << "-"
        << std::setw(2) << static_cast<int32_t>(id.bytes_[6])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[7])
        << "-"
        << std::setw(2) << static_cast<int32_t>(id.bytes_[8])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[9])
        << "-"
        << std::setw(2) << static_cast<int32_t>(id.bytes_[10])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[11])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[12])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[13])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[14])
        << std::setw(2) << static_cast<int32_t>(id.bytes_[15]);
}

uuid uuid::newUUID() {
#if defined(__linux__) || defined(__unix__)
    uuid_t id;
    uuid_generate(id);

    const std::array<uint8_t, 16> bytes =
             { {
                   id[0],
                   id[1],
                   id[2],
                   id[3],
                   id[4],
                   id[5],
                   id[6],
                   id[7],
                   id[8],
                   id[9],
                   id[10],
                   id[11],
                   id[12],
                   id[13],
                   id[14],
                   id[15]
                } };

    return uuid(bytes);
#endif
}

} }
