#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#include <net/core/openssl.h>

namespace net { namespace core {

class SSLLibInit {
public:
    static SSLLibInit & get() {
        static SSLLibInit init;
        return init;
    }

    SSLLibInit(const SSLLibInit &) = delete;
    SSLLibInit& operator=(const SSLLibInit &) = delete;

    ~SSLLibInit() {
        CRYPTO_set_id_callback(0);
        CRYPTO_set_locking_callback(0);
        ERR_free_strings();
        ERR_remove_state(0);
        EVP_cleanup();
        CRYPTO_cleanup_all_ex_data();
        CONF_modules_unload(1);
        ENGINE_cleanup();
    }

private:
    SSLLibInit() {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
        mutexes_.resize(CRYPTO_num_locks());
        for (size_t i = 0; i < mutexes_.size(); ++i)
            mutexes_[i].reset(new std::mutex());
        CRYPTO_set_locking_callback(&SSLLibInit::locking);
        CRYPTO_set_id_callback(&SSLLibInit::genId);
    }

    static unsigned long genId() {
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    }

    static void locking(int mode, int n, const char* /*file*/, int /*line*/) {
        if (mode & CRYPTO_LOCK)
            SSLLibInit::get().mutexes_[n]->lock();
         else
            SSLLibInit::get().mutexes_[n]->unlock();
    }

    std::vector<std::unique_ptr<std::mutex>> mutexes_;
};

class SSLContext::SSLContextImpl {
public:
    static const int SSL_BUF_SIZE = 8192;

    SSLContextImpl();

    ~SSLContextImpl();

    void handshake(Channel &channel);

    int read(void *messsage, size_t len);

    int write(const void *messsage, size_t len);

    SSLContextImpl(const SSLContextImpl &) = delete;
    SSLContextImpl& operator=(const SSLContextImpl &) = delete;

private:
    BIO *ext_bio_;
    SSL_CTX *ctx_;
    SSL *ssl_;
};

SSLContext::SSLContextImpl::SSLContextImpl() {
    ctx_ = SSL_CTX_new(TLSv1_method());
    ssl_ = SSL_new(ctx_);
    SSL_set_mode(ssl_, SSL_MODE_ENABLE_PARTIAL_WRITE);
    SSL_set_mode(ssl_, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
    BIO* int_bio = nullptr;
    BIO_new_bio_pair(&int_bio, SSL_BUF_SIZE, &ext_bio_, SSL_BUF_SIZE);
    SSL_set_bio(ssl_, int_bio, int_bio);
}

SSLContext::SSLContextImpl::~SSLContextImpl() {
    BIO_free(ext_bio_);
    SSL_free(ssl_);
}

void SSLContext::SSLContextImpl::handshake(Channel &channel) {
    SSL_set_accept_state(ssl_);
    auto result = SSL_do_handshake(ssl_);

    auto error = SSL_get_error(ssl_, result);
    if (error == SSL_ERROR_WANT_WRITE) {
        channel.enableWrite();
    } else if (error == SSL_ERROR_WANT_READ) {
        channel.enableRead();
    }
}

int SSLContext::SSLContextImpl::read(void *messsage, size_t len) {
    auto result = SSL_read(ssl_, messsage, len);

    auto eror = SSL_get_error(ssl_, result);
    if (result < 0 && eror != SSL_ERROR_WANT_READ) {
        return 0;
    }

    if (result == 0) {
        if (eror == SSL_ERROR_ZERO_RETURN) {
            // shutdown!
            return 0;
        }
    }
    return result;
}

int SSLContext::SSLContextImpl::write(const void *messsage, size_t len) {
    auto result = SSL_write(ssl_, messsage, len);

    auto eror = SSL_get_error(ssl_, result);
    if (result < 0 && eror != SSL_ERROR_WANT_READ) {
        return 0;
    }

    if (result == 0) {
        if (eror == SSL_ERROR_ZERO_RETURN) {
            // shutdown!
            return 0;
        }
    }
    return result;
}

SSLContext::SSLContext()
    : impl_(std::make_unique<SSLContextImpl>()) {
}

SSLContext::~SSLContext() = default;

void SSLContext::handshake(Channel &channel) {
    impl_->handshake(channel);
}

int SSLContext::read(void *messsage, size_t len) {
    return impl_->read(messsage, len);
}

int SSLContext::write(const void *messsage, size_t len) {
    return impl_->write(messsage, len);
}

} }
