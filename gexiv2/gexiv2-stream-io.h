/*
 * gio-stream-io.h
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 *
 * This is free software. See COPYING for details.
 */

/*
 * The class StreamIo is derived from BasicIo to provide usage of a
 * ManagedStream to exiv2.
 */

#ifndef GEXIV2_STREAM_IO_H
#define GEXIV2_STREAM_IO_H

#include <gexiv2/gexiv2-managed-stream.h>
#include <exiv2/exiv2.hpp>
#include <gio/gio.h>


class StreamIo : public Exiv2::BasicIo {
public:
#if EXIV2_TEST_VERSION(0,27,99)
    using ptr_type = Exiv2::BasicIo::UniquePtr;
    using size_type = size_t;
#else
    using ptr_type = Exiv2::BasicIo::AutoPtr;
    using size_type = long;
#endif

	StreamIo (ManagedStreamCallbacks* cb);

	virtual ~StreamIo ();
	virtual int open ();
	virtual int close ();
	virtual size_type write (const Exiv2::byte* data, size_type wcount);
	virtual size_type write (Exiv2::BasicIo& src);
	virtual int putb (Exiv2::byte data);
	virtual Exiv2::DataBuf read (long rcount);
	Exiv2::DataBuf read (size_t rcount);
	virtual size_type read (Exiv2::byte* buf, size_type rcount);
	virtual int getb ();
	virtual void transfer (Exiv2::BasicIo& src);
	virtual int seek (long offset, Position pos);
	virtual Exiv2::byte* mmap (bool isWriteable = false);
	virtual int munmap ();
	virtual long tell () const;
	virtual size_t size () const;
	virtual bool isopen () const;
	virtual int error () const;
	virtual bool eof () const;
	virtual std::string path () const;
#ifdef EXV_UNICODE_PATH
	virtual std::wstring wpath () const;
#endif
	virtual ptr_type temporary () const;

private:
	/* stream callbacks */
	ManagedStreamCallbacks* cb;
	
	/* used for mmap and  munmap */
	ptr_type memio;

	/* closing does not mean closing the stream, because this would
	   destroy it. So just keep track about current state and let stream
	   open. */
	gboolean is_open;
	
	/* stream can be used for writing */
	gboolean can_write;
};


#endif /* GEXIV2_STREAM_IO_H */

