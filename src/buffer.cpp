#include <rapid/core/buffer.h>

namespace rapid { namespace core {

Buffer::Buffer() {
    buffer_.reserve(4096);
}

void Buffer::copyFrom(const char *buffer, size_t len) {
    buffer_.append(buffer, len);
}

std::string_view Buffer::slice() const {
    return std::string_view(buffer_.data(), buffer_.length());
}

size_t Buffer::readable() const {
    return buffer_.length();
}

std::string Buffer::readToEnd() const {
    return buffer_;
}

void Buffer::clear() {
    buffer_.clear();
}

} }
