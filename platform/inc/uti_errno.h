
#ifndef __UTI_ERRNO_H__
#define __UTI_ERRNO_H__

#include <errno.h>

typedef int     uti_err_t;

#define UTI_EPERM         EPERM
#define UTI_ENOENT        ENOENT
#define UTI_ENOPATH       ENOENT
#define UTI_ESRCH         ESRCH
#define UTI_EINTR         EINTR
#define UTI_ECHILD        ECHILD
#define UTI_ENOMEM        ENOMEM
#define UTI_EACCES        EACCES
#define UTI_EBUSY         EBUSY
#define UTI_EEXIST        EEXIST
#define UTI_EXDEV         EXDEV
#define UTI_ENOTDIR       ENOTDIR
#define UTI_EISDIR        EISDIR
#define UTI_EINVAL        EINVAL
#define UTI_ENFILE        ENFILE
#define UTI_EMFILE        EMFILE
#define UTI_ENOSPC        ENOSPC
#define UTI_EPIPE         EPIPE
#define UTI_EINPROGRESS   EINPROGRESS
#define UTI_EADDRINUSE    EADDRINUSE
#define UTI_ECONNABORTED  ECONNABORTED
#define UTI_ECONNRESET    ECONNRESET
#define UTI_ENOTCONN      ENOTCONN
#define UTI_ETIMEDOUT     ETIMEDOUT
#define UTI_ECONNREFUSED  ECONNREFUSED
#define UTI_ENAMETOOLONG  ENAMETOOLONG
#define UTI_ENETDOWN      ENETDOWN
#define UTI_ENETUNREACH   ENETUNREACH
#define UTI_EHOSTDOWN     EHOSTDOWN
#define UTI_EHOSTUNREACH  EHOSTUNREACH
#define UTI_ENOSYS        ENOSYS
#define UTI_ECANCELED     ECANCELED
#define UTI_EILSEQ        EILSEQ
#define UTI_ENOMOREFILES  0

#if (UTI_HAVE_OPENAT)
#define UTI_EMLINK        EMLINK
#define UTI_ELOOP         ELOOP
#endif

#if (__hpux__)
#define UTI_EAGAIN        EWOULDBLOCK
#else
#define UTI_EAGAIN        EAGAIN
#endif


#define uti_errno                  errno



#endif /*__UTI_ERRNO_H__*/

