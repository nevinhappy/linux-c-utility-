
#include "uti_file.h"

file_desc_t os_get_std_input()
{
	return 0;
}

file_desc_t os_get_std_output()
{
	return 1;
}

/* check file and if ok return file size */
long os_stat_file_size(file_desc_t fd)
{
    struct stat buf;

    if(fstat(fd, &buf)<0) {
        return -1;
    }

    return (long)buf.st_size;
}

/*-------------------------------------------------------------------------*/
int os_read_file(file_desc_t file, void *buff, int len)
{
	int n, l = len;
	U8 *p = (U8*)buff;

	while (l > 0)
	{
		n = read(file, p, l);
		if (n < 0)
		{
//			perror("read error ");
			if (errno == EINTR)
				n = 0;
			else
				return -1;
		}
		else if (n == 0)
			break;

		p += n;
		l -= n;
	}

	return len - l;
}

int os_write_file(file_desc_t file, const void *buff, int len)
{
	int n, l = len;
	U8 *p = (U8*)buff;

	while (l > 0)
	{
		n = write(file, p, l);
		if (n <= 0)
		{
//			perror("write error ");
			if (errno == EINTR)
				n = 0;
			else
				return -1;
		}

		p += n;
		l -= n;
	}

	return len;
}

int os_read_file_datagram(file_desc_t file, void *buff, int len)
{
	while (1)
	{
		int n = read(file, buff, len);
		if (n >= 0)
			return n;
		else if (errno != EINTR)
			return -1;
	}
	return -1;
}

int os_write_file_datagram(file_desc_t file, const void *buff, int len)
{
	while (1)
	{
		int n = write(file, buff, len);
		if (n > 0)
			return n;
		else if (errno != EINTR)
			return -1;
	}
	return -1;
}

file_desc_t os_duplicate_file(file_desc_t file)
{
	return dup(file);
}

int os_rm_file(const char* file_name)
{
    return unlink(file_name);
}


int os_close_file(file_desc_t file)
{
	close(file);
	return TRUE;
}

int os_read_file_line(file_desc_t file, void *buff, int len)
{
	int nLen = 0;
	char *p = (char*)buff;

	while (nLen < len)
	{
		if (1 == os_read_file(file, p, 1))
		{
			nLen ++;
			if ((*p) == '\n')
				return nLen;
		}
		else
			return -1;
		p ++;
	}
	return nLen;
}



