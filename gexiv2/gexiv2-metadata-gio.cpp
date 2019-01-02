#include "gexiv2-metadata-gio.h"

namespace GExiv2 {
namespace Internal {
GioIo::GioIo(GInputStream *is)
    : BasicIo ()
    , _is (G_INPUT_STREAM (g_object_ref (is)))
    , _seekable(G_IS_SEEKABLE(_is) ? G_SEEKABLE (_is) : nullptr)
    , _error{nullptr}
    , _eof{false}
{}

GioIo::~GioIo()
{
    g_clear_object (&_is);
    g_clear_error (&_error);
    _seekable = nullptr;
}

int GioIo::open()
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    return this->seek(0, Exiv2::BasicIo::beg);
}

Exiv2::DataBuf GioIo::read(long rcount)
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    Exiv2::DataBuf b{rcount};

    long bytes_read = this->read(b.pData_, rcount);
    b.size_ = bytes_read;

    return b;
}

long GioIo::read(Exiv2::byte *buf, long rcount)
{
    g_debug ("%s %ld", __PRETTY_FUNCTION__, rcount);
    GError *error = NULL;
    gssize result = 0;

    result = g_input_stream_read(_is, reinterpret_cast<void *>(buf), rcount, NULL, &error);
    if (error != NULL) {
        g_critical ("Error reading from stream: %d %s", error->code, error->message);
        g_clear_error (&_error);
        _error = error;

#if EXIV2_TEST_VERSION(0,27,0)
        throw Exiv2::Error(Exiv2::ErrorCode::kerFailedToReadImageData);
#else
        throw Exiv2::Error(2);
#endif
        return 0;
    }

    if (result == 0) {
        _eof = true;
    } else {
        _eof = false;
    }

    return result;
}

int GioIo::getb()
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    Exiv2::byte b;
    return this->read (&b, 1) == 1 ? b : EOF;
}

int GioIo::seek(seek_offset_t offset, Exiv2::BasicIo::Position position)
{
    g_debug ("%s %ld %d", __PRETTY_FUNCTION__, offset, (int)position);
    if (_seekable != NULL && g_seekable_can_seek (_seekable)) {
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
                g_assert_not_reached ();
                break;
        }

        GError *error = NULL;
        g_seekable_seek (_seekable, offset, t, NULL, &error);
        if (error != NULL) {
            g_clear_error(&_error);
            g_critical ("Failed to seek: %s", error->message);
            _error = error;

            return -1;
        }

        return 0;
    } else {
        // Can only seek forward here...
        if (position != Exiv2::BasicIo::cur) {
            return -1;
        }

        GError *error = NULL;
        g_input_stream_skip (_is, offset, NULL, &error);
        if (error != NULL) {
            g_clear_error(&_error);
            _error = error;
            g_critical("Failed to seek forward: %s", error->message);

            return -1;
        }

        return 0;
    }
}

long GioIo::tell() const
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    if (_seekable != NULL && g_seekable_can_seek (_seekable)) {
        return static_cast<long>(g_seekable_tell (_seekable));
    } else {
        return -1;
    }
}

GioRwIo::GioRwIo(GIOStream *is)
    : BasicIo ()
    , _is (G_IO_STREAM (g_object_ref (is)))
    , _seekable(G_IS_SEEKABLE(_is) ? G_SEEKABLE (_is) : nullptr)
    , _error{nullptr}
    , _eof{false}
{
    if (not _seekable) {
        _seekable = G_SEEKABLE (g_io_stream_get_input_stream (_is));
    }
    g_debug ("%s %p", __PRETTY_FUNCTION__, _seekable);
}

GioRwIo::~GioRwIo()
{
    g_clear_object (&_is);
    g_clear_error (&_error);
    _seekable = nullptr;
}

int GioRwIo::open()
{
    g_output_stream_flush (g_io_stream_get_output_stream (_is), nullptr, nullptr);
    return this->seek(0, Exiv2::BasicIo::beg);
}

int GioRwIo::close()
{
    g_output_stream_flush (g_io_stream_get_output_stream (_is), nullptr, nullptr);
    return 0;
}


Exiv2::DataBuf GioRwIo::read(long rcount)
{
    g_debug ("%s %ld", __PRETTY_FUNCTION__, rcount);
    Exiv2::DataBuf b{rcount};

    long bytes_read = this->read(b.pData_, rcount);
    b.size_ = bytes_read;

    return b;
}

long GioRwIo::read(Exiv2::byte *buf, long rcount)
{
    g_debug ("%s %ld", __PRETTY_FUNCTION__, rcount);
    GError *error = NULL;
    gssize result = 0;
    GInputStream *is = g_io_stream_get_input_stream (_is);

    result = g_input_stream_read(is, reinterpret_cast<void *>(buf), rcount, NULL, &error);
    if (error != NULL) {
        g_critical ("Error reading from stream: %d %s", error->code, error->message);
        g_clear_error (&_error);
        _error = error;

#if EXIV2_TEST_VERSION(0,27,0)
        throw Exiv2::Error(Exiv2::ErrorCode::kerFailedToReadImageData);
#else
        throw Exiv2::Error(2);
#endif
        return 0;
    }

    if (result == 0) {
        _eof = true;
    } else {
        _eof = false;
    }

    return result;
}

int GioRwIo::getb()
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    Exiv2::byte b;
    return this->read (&b, 1) == 1 ? b : EOF;
}

int GioRwIo::seek(seek_offset_t offset, Exiv2::BasicIo::Position position)
{
    g_debug ("%s %ld %d", __PRETTY_FUNCTION__, offset, (int)position);
    if (_seekable != NULL && g_seekable_can_seek (_seekable)) {
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
                g_assert_not_reached ();
                break;
        }

        GError *error = NULL;
        auto r = g_seekable_seek (_seekable, offset, t, NULL, &error);
        g_debug ("Seeking success: %d", r);
        if (error != NULL) {
            g_clear_error(&_error);
            g_critical ("Failed to seek: %s", error->message);
            _error = error;

            return -1;
        }

        return 0;
    } else {
        g_warning ("No seekable!");
        return -1;
    }
}

long GioRwIo::tell() const
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    if (_seekable != NULL && g_seekable_can_seek (_seekable)) {
        return static_cast<long>(g_seekable_tell (_seekable));
    } else {
        return -1;
    }
}

long GioRwIo::write(const Exiv2::byte *data, long wcount)
{
    g_debug ("%s: %ld", __PRETTY_FUNCTION__, wcount);
    return EOF; 
}

long GioRwIo::write(BasicIo &src)
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    return 0;
}

int GioRwIo::putb(Exiv2::byte data)
{
    g_debug ("%s", __PRETTY_FUNCTION__);
    return this->write(&data, 1);
}

void GioRwIo::transfer(Exiv2::BasicIo &src)
{
    g_debug ("%s %s %ld %ld ", __PRETTY_FUNCTION__,
             typeid(src).name(),
             src.size(),
             src.tell());
    GioRwIo *gio = dynamic_cast<GioRwIo *>(&src);
    GError *error = nullptr;

    // rewind everything
    src.open();
    this->open();
    g_input_stream_close (g_io_stream_get_input_stream (_is), nullptr, &error);

    if (gio != nullptr) {
        // Shortcut, use GIO's stream splicing for this
        g_output_stream_splice (g_io_stream_get_output_stream (_is),
                g_io_stream_get_input_stream (gio->_is),
                G_OUTPUT_STREAM_SPLICE_NONE,
                nullptr,
                &error);

        if (error != nullptr) {
            g_warning ("Failed to splice: %s", error->message);
            g_clear_error (&_error);
            _error = error;
        }
    } else {
        auto os = g_io_stream_get_output_stream (_is);
        while (not src.eof()) {
            auto buf = src.read(4096);
            if (buf.size_ > 0) {
                g_debug ("Got %ld bytes, writing", buf.size_);
                g_output_stream_write_all (os, buf.pData_, buf.size_, nullptr, nullptr, &error);
                if (error != nullptr) {
                    g_warning ("Failed to write: %s", error->message);
                    g_clear_error (&_error);
                    _error = error;
                    break;
                }
            } else {
                g_debug ("No more data");
                break;
            }
        }
    }
    g_output_stream_flush (g_io_stream_get_output_stream (_is), nullptr, nullptr);
}



} // namespace Internal
} // namespace GExiv2
