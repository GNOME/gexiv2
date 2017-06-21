/*
 *  File Copied from Moonlight. See Moonlight for license
 */


#ifndef GEXIV2_MANAGED_STREAM
#define GEXIV2_MANAGED_STREAM

#include <glib-object.h>

/**
 * WrapperSeekOrigin:
 * @Begin: Seeking offset is relative to the beginning of the stream
 * @Current: Seeking offset is relative to the current offset in the stream
 * @End: Seeking offset is relative to the end of the stream
 *
 * Seek positions for #Stream_Seek
 *
 */
typedef enum {
    Begin = 0,
    Current = 1,
    End = 2
} WrapperSeekOrigin;

/**
 * Stream_CanSeek:
 * @handle: Opaque storage for the native handle this function will operate on
 *
 * Function that returns the seekability of the stream
 *
 * Returns: %TRUE if the stream object can be seeked
 */
typedef gboolean (*Stream_CanSeek)  (void *handle);

/**
 * Stream_CanRead:
 * @handle: Opaque storage for the native handle this function will operate on
 *
 * Function that returns the readability of the stream
 *
 * Returns: %TRUE if the stream object can be read from.
 */
typedef gboolean (*Stream_CanRead)  (void *handle);

/**
 * Stream_CanWrite:
 * @handle: Opaque storage for the native handle this function will operate on
 *
 * Function that returns the readability of the stream
 *
 * Returns: %TRUE if the stream object can be written to.
 */
typedef gboolean (*Stream_CanWrite) (void *handle);

/**
 * Stream_Length:
 * @handle: Opaque storage for the native handle this function will operate on
 *
 * Function to query the length of the stream
 *
 * Returns: The length of the stream
 */
typedef gint64   (*Stream_Length)   (void *handle);

/**
 * Stream_Position:
 * @handle: Opaque storage for the native handle this function will operate on
 *
 * Function to query the current position in the stream
 *
 * Returns: The current position
 */
typedef gint64   (*Stream_Position) (void *handle);

/**
 * Stream_Read:
 * @handle: Opaque storage for the native handle this function will operate on
 * @buffer: Destination data to read into
 * @offset: Offset in @buffer where data should be written to
 * @count: Number of bytes to read
 *
 * Function to read data from the stream
 *
 * Returns: The number of bytes read, 0 on EOF or -1 on error.
 */
typedef gint32   (*Stream_Read)     (void *handle, void *buffer, gint32 offset, gint32 count);

/**
 * Stream_Write:
 * @handle: Opaque storage for the native handle this function will operate on
 * @buffer: Source data to put into the stream
 * @offset: Offset in @buffer where data should be read from
 * @count: Number of bytes to write
 *
 * Function to write data to the stream
 *
 * Returns: The number of bytes written or -1 on error.
 */
typedef void     (*Stream_Write)    (void *handle, void *buffer, gint32 offset, gint32 count);

/**
 * Stream_Seek:
 * @handle: Opaque storage for the native handle this function will operate on
 * @offset: Position in the stream, relative to @origin
 * @origin: Determines meaning of offset, being relative to current position, start or end of the stream.
 *
 * Change the read or write position in the current stream
 */
typedef void     (*Stream_Seek)     (void *handle, gint64 offset, WrapperSeekOrigin origin);

/**
 * Stream_Flush:
 * @handle: Opaque storage for the native handle this function will operate on
 *
 * Schedule writing buffered data to the stream's real storage.
 */
typedef void     (*Stream_Flush)    (void *handle);

/**
 * ManagedStreamCallbacks:
 * @handle: Storage for the native handle to be used with this stream
 * @CanSeek: Pointer to a function that describes the seekability of @handle
 * @CanRead: Pointer to a function that describes the readability of @handle
 * @CanWrite: Pointer to a function that describes the writability of @handle
 * @Length: Pointer to a function that gets the length of @handle
 * @Position: Pointer to a function that gives the current location inside @handle
 * @Read: Read bytes from the stream
 * @Write: (nullable): Function pointer to write to the stream. Can be #NULL if @CanWrite returns #FALSE
 * @Seek: Function pointer to seek in the stream.
 * @Flush: (nullable): Function pointer schedule writing of all cached data to disk. Can
 * be #NULL if @CanWrite returns #FALSE
 *
 * #ManagedStreamCallbacks is a set of callbacks that describe a stream that
 * can be passed to gexiv2_metadata_open_stream() to read image meta-data from
 * arbitrary data sources that can be mapped to this set of callbacks.
 *
 * At least @CanSeek and @CanRead need to return #TRUE and the relevant
 * call-backs must not be #NULL to be any useful.
 */
struct _ManagedStreamCallbacks {
    void *handle;
    Stream_CanSeek CanSeek;
    Stream_CanRead CanRead;
    Stream_CanWrite CanWrite;
    Stream_Length Length;
    Stream_Position Position;
    Stream_Read Read;
    Stream_Write Write;
    Stream_Seek Seek;
    Stream_Flush Flush;
};

typedef struct _ManagedStreamCallbacks ManagedStreamCallbacks;

#endif /* GEXIV2_MANAGED_STREAM */

