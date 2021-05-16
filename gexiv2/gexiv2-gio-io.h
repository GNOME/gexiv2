// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright Jens Georg <mail@jensge.org>
#pragma once
#include <exiv2/exiv2.hpp>
#include <gio/gio.h>
#include <glib-object.h>

namespace GExiv2 {

class GioIo : public Exiv2::BasicIo {
  public:
    GioIo(GInputStream* is)
      : BasicIo()
      , _is(G_INPUT_STREAM(g_object_ref(is)))
      , _seekable(G_SEEKABLE(_is)) {

        auto position = tell();
        seek(0, Exiv2::BasicIo::end);
        _size = tell();
        g_debug("GioIo has size of %zu", _size);
        seek(position, Exiv2::BasicIo::beg);
    }

    using size_type = size_t;

    size_type _size{0};

    ~GioIo() override {
        g_clear_object(&_is);
        g_clear_error(&_error);
        _seekable = nullptr;

        if (_mmap_stream != nullptr) {
            g_object_unref(G_OBJECT(_mmap_stream));
            if (_mmap_stream != nullptr) {
                g_object_remove_weak_pointer(G_OBJECT(_mmap_stream), reinterpret_cast<gpointer*>(_mmap_stream));
                g_critical("Mismatching mmap/munmap calls, expect memory leak");
            }
        }
    }
    typedef int64_t seek_offset_t;

    using ptr_type = Exiv2::BasicIo::UniquePtr;

    void populateFakeData() override{};
    int open() override { return 0; }

    int close() override { return 0; }

    // Writing is not supported
    size_type write(const Exiv2::byte* /*data*/, size_type /*wcount*/) override { return 0; }
    size_type write(BasicIo& /*src*/) override { return 0; }
    int putb(Exiv2::byte /*data*/) override { return EOF; }

    Exiv2::DataBuf read(size_type rcount) override {
        Exiv2::DataBuf b{rcount};

        auto bytes_read = this->read(b.data(), rcount);
        if (bytes_read > 0 && bytes_read != rcount) {
            b = Exiv2::DataBuf{b};
        }

        return b;
    }

    size_type read(Exiv2::byte* buf, size_type rcount) override {
        GError* error = NULL;
        gssize result = 0;

        result = g_input_stream_read(_is, reinterpret_cast<void*>(buf), rcount, NULL, &error);
        if (error != NULL) {
            g_critical("Error reading from stream: %d %s", error->code, error->message);
            g_clear_error(&_error);
            _error = error;

            throw Exiv2::Error(Exiv2::ErrorCode::kerFailedToReadImageData);
            return 0;
        }

        if (result == 0) {
            _eof = true;
        } else {
            _eof = false;
        }

        return result;
    }

    int getb() override {
        Exiv2::byte b;
        return this->read(&b, 1) == 1 ? b : EOF;
    }

    void transfer(Exiv2::BasicIo& /*src*/) override {
        // Does not seem necessary for Read-only support
    }

    int seek(seek_offset_t offset, Exiv2::BasicIo::Position position) override {
        GSeekType t = G_SEEK_SET;
        switch (position) {
            case Exiv2::BasicIo::cur:
                t = G_SEEK_CUR;
                break;
            case Exiv2::BasicIo::beg:
                t = G_SEEK_SET;
                break;
            case Exiv2::BasicIo::end:
                t = G_SEEK_END;
                break;
            default:
                g_assert_not_reached();
                break;
        }

        GError* error = NULL;
        g_seekable_seek(_seekable, offset, t, NULL, &error);
        if (error != NULL) {
            g_clear_error(&_error);
            g_critical("Failed to seek: %s", error->message);
            _error = error;

            return -1;
        }

        return 0;
    }

    Exiv2::byte* mmap(bool /*writable*/) override {
        if (_mmap_stream != nullptr) {
            g_debug("mmap called previously, will just output old stream");
            g_object_ref(G_OBJECT(_mmap_stream));

            return reinterpret_cast<Exiv2::byte*>(
                g_memory_output_stream_get_data(G_MEMORY_OUTPUT_STREAM(_mmap_stream)));
        }
        goffset old_position;
        g_autoptr(GError) error = nullptr;

        old_position = g_seekable_tell(_seekable);
        g_seekable_seek(_seekable, 0, G_SEEK_SET, nullptr, &error);
        if (error != nullptr) {
            throw Exiv2::Error(Exiv2::ErrorCode::kerCallFailed, error->message);
        }

        _mmap_stream = g_memory_output_stream_new_resizable();
        g_object_add_weak_pointer(G_OBJECT(_mmap_stream), reinterpret_cast<gpointer*>(&_mmap_stream));
        g_output_stream_splice(_mmap_stream, _is, G_OUTPUT_STREAM_SPLICE_NONE, nullptr, &error);
        if (error != nullptr) {
            throw Exiv2::Error(Exiv2::ErrorCode::kerCallFailed, error->message);
        }

        g_seekable_seek(_seekable, old_position, G_SEEK_SET, nullptr, &error);
        if (error != nullptr) {
            throw Exiv2::Error(Exiv2::ErrorCode::kerCallFailed, error->message);
        }

        return reinterpret_cast<Exiv2::byte*>(g_memory_output_stream_get_data(G_MEMORY_OUTPUT_STREAM(_mmap_stream)));
    }

    int munmap() override {
        g_object_unref(G_OBJECT(_mmap_stream));
        return 0;
    }

    size_type tell() const override { return static_cast<long>(g_seekable_tell(_seekable)); }

    size_t size() const override { return static_cast<size_t>(_size); }

    bool isopen() const override { return true; }

    int error() const override { return _error == nullptr ? 0 : 1; }

    bool eof() const override { return _eof; }

    const std::string& path() const noexcept override {
        static std::string info{"GIO Wrapper"};
        return info;
    }

    Exiv2::BasicIo::UniquePtr temporary() const { return Exiv2::BasicIo::UniquePtr(nullptr); }

  private:
    GInputStream* _is{nullptr};
    GOutputStream* _mmap_stream{nullptr};
    GSeekable* _seekable{nullptr};
    GError* _error{nullptr};
    bool _eof{false};
}; // class GioIo
} // Anonymous namespace
