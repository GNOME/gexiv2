#ifndef GEXIV2_METADATA_GIO
#define GEXIV2_METADATA_GIO

#include <gio/gio.h>
#include <exiv2/exiv2.hpp>

namespace GExiv2 {
namespace Internal {
class GioIo : public Exiv2::BasicIo {
public:
    GioIo (GInputStream *is);
    ~GioIo();

#if defined(_MSC_VER)
    using seek_offset_t = int64_t;
#else
    using seek_offset_t = long;
#endif

#if EXIV2_TEST_VERSION(0,27,99)
    using ptr_type = Exiv2::BasicIo::UniquePtr;
#else
    using ptr_type = Exiv2::BasicIo::AutoPtr;
#endif

    int open() override;
    int close() override { return 0; }

    // Writing is not supported
    long write(const Exiv2::byte *data, long wcount) override { return 0; }
    long write(BasicIo &src) override { return 0; }
    int putb(Exiv2::byte data) override { return EOF; }

    Exiv2::DataBuf read(long rcount) override;
    long read(Exiv2::byte *buf, long rcount) override ;
    int getb() override;

    // Does not seem necessary for Read-only support
    void transfer(Exiv2::BasicIo &src) override { }

    int seek(seek_offset_t offset, Exiv2::BasicIo::Position position) override;


    // MMapping not supported
    Exiv2::byte *mmap(bool writable) override { return nullptr; }
    int munmap() override { return 0; }

    long tell() const override;
    // Cannot tell the size from a random stream - also it does not
    // seem to be necessary for what we are doing
    size_t size() const override { return -1; }

    bool isopen() const override { return true; }

    int error() const override { return _error == nullptr ? 0 : 1; }

    bool eof() const override { return _eof; }

    std::string path() const { return "GIO Wrapper"; }

#if EXIV2_TEST_VERSION(0,27,99)
    Exiv2::BasicIo::UniquePtr temporary() const {
        return Exiv2::BasicIo::UniquePtr(nullptr);
    }
#else
    Exiv2::BasicIo::AutoPtr temporary() const {
        return Exiv2::BasicIo::AutoPtr(nullptr);
    }
#endif


private:
    GInputStream *_is;
    GSeekable *_seekable;
    GError *_error;
    bool _eof;
}; // class GioIo

class GioRwIo : public Exiv2::BasicIo {
public:
    GioRwIo (GIOStream *ios);
    ~GioRwIo();

#if defined(_MSC_VER)
    using seek_offset_t = int64_t;
#else
    using seek_offset_t = long;
#endif

#if EXIV2_TEST_VERSION(0,27,99)
    using ptr_type = Exiv2::BasicIo::UniquePtr;
#else
    using ptr_type = Exiv2::BasicIo::AutoPtr;
#endif

    int open() override;
    int close() override;

    long write(const Exiv2::byte *data, long wcount) override;
    long write(BasicIo &src) override;
    int putb(Exiv2::byte data) override;

    Exiv2::DataBuf read(long rcount) override;
    long read(Exiv2::byte *buf, long rcount) override ;
    int getb() override;

    // Does not seem necessary for Read-only support
    void transfer(Exiv2::BasicIo &src) override;

    int seek(seek_offset_t offset, Exiv2::BasicIo::Position position) override;


    // MMapping not supported
    Exiv2::byte *mmap(bool writable) override {
        g_debug(__PRETTY_FUNCTION__);
        return nullptr; }
    int munmap() override { return 0; }

    long tell() const override;
    // Cannot tell the size from a random stream - also it does not
    // seem to be necessary for what we are doing
    size_t size() const override {
        g_debug(__PRETTY_FUNCTION__);
        return -1; }

    bool isopen() const override {
        g_debug(__PRETTY_FUNCTION__);
        return true; }

    int error() const override {
        g_debug(__PRETTY_FUNCTION__);
        return _error == nullptr ? 0 : 1; }

    bool eof() const override {
        g_debug(__PRETTY_FUNCTION__);
        return _eof; }

    std::string path() const {
        g_debug(__PRETTY_FUNCTION__);
        return "GIO Wrapper"; }

#if EXIV2_TEST_VERSION(0,27,99)
    Exiv2::BasicIo::UniquePtr temporary() const {
        g_debug(__PRETTY_FUNCTION__);
        return Exiv2::BasicIo::UniquePtr(nullptr);
    }
#else
    Exiv2::BasicIo::AutoPtr temporary() const {
        g_debug(__PRETTY_FUNCTION__);
        return Exiv2::BasicIo::AutoPtr(nullptr);
    }
#endif


private:
    GIOStream *_is;
    GSeekable *_seekable;
    GError *_error;
    bool _eof;
}; // class GioIo

} // namespace Internal
} // namespace GExiv2

#endif // GEXIV2_METADATA_GIO
