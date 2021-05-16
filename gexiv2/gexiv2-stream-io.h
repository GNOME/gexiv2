/*
 * gio-stream-io.h
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    using size_type = long;
#else
    using ptr_type = Exiv2::BasicIo::AutoPtr;
    using size_type = long;
#endif

	StreamIo (ManagedStreamCallbacks* cb);

	~StreamIo ();
	int open () override;
	int close () override;
	size_type write (const Exiv2::byte* data, size_type wcount) override;
	size_type write (Exiv2::BasicIo& src) override;
	int putb (Exiv2::byte data) override;
    Exiv2::DataBuf read (size_type rcount) override;
    size_type read (Exiv2::byte* buf, size_type rcount) override;
	int getb () override;
	void transfer (Exiv2::BasicIo& src) override;
	int seek (long offset, Position pos) override;
	Exiv2::byte* mmap (bool isWriteable = false) override;
	int munmap () override;
	long tell () const override;
	size_t size () const override;
	bool isopen () const override;
	int error () const override;
	bool eof () const override;
	std::string path () const override;
#ifdef EXV_UNICODE_PATH
	std::wstring wpath () const override;
#endif
	ptr_type temporary () const;

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

