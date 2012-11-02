
#ifndef __UTI_FILE_H__
#define __UTI_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "platform.h"

typedef int file_desc_t;

#define INVALID_FILE_DESC   -1
#define FILE_OP_ERROR       -1


file_desc_t OSGetStdInput();
file_desc_t OSGetStdOutput();

long os_stat_file_size(file_desc_t fd);
// file open ---> open()
//
int os_read_file(file_desc_t file, void *buff, int len);
//
int os_write_file(file_desc_t file, const void *buff, int len);
//
int os_read_file_datagram(file_desc_t file, void *buff, int len);
//
int os_write_file_datagram(file_desc_t file, const void *buff, int len);
//
int os_close_file(file_desc_t file);

//
int os_read_file_line(file_desc_t file, void *buff, int len);
//
file_desc_t os_duplicate_file(file_desc_t file);

int os_rm_file(const char* file_name);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif /*__UTI_FILE_H__*/

