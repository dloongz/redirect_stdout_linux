#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

void redirect_stdout(const char *file_name, int append);
void recover_stdout();

typedef struct _context_t
{
	int original_fd;
	int fd;
	int redirect_flag;
}Context;

static Context ctx = {-1, -1, 0};

void redirect_stdout(const char *file_name, int append)
{
	int flag = O_WRONLY | O_CREAT;		

	if (ctx.redirect_flag == 1)
	{
		printf("Redirected return\n");	
		return ;
	}
	
	ctx.original_fd = dup(STDOUT_FILENO);
	if (ctx.original_fd < 0)
	{
		printf("%s:dup stdout --> original\n", __func__);	
		return ;
	}

	if (1 == append)
	{
		flag |= O_APPEND;
	}

	ctx.fd = open(file_name, flag, 0666);
	if (ctx.fd < 0)
	{
		dup2(ctx.original_fd, STDOUT_FILENO);	
	//	close(ctx.original_fd);
		printf("%s: open %s error\n", __func__, file_name);
		return ;
	}
	
	if (dup2(ctx.fd, STDOUT_FILENO) < 0)
	{
		printf("%s : dup2 fd-->stdout error\n", __func__);
		dup2(ctx.original_fd, STDOUT_FILENO);	
	//	close(ctx.original_fd);
		return ;
	}
	ctx.redirect_flag = 1;
	printf("Now,stdout to file \"%s\" \n", file_name);
	return ;
}

void recover_stdout()
{
		
	if (dup2(ctx.original_fd, STDOUT_FILENO) < 0)
	{
		printf("%s : dup2 stdout recover\n", __func__);	
		return;
	}
	if (ctx.fd > 0)
	{
		close(ctx.fd);
	}
	if (ctx.original_fd > 0)
	{
		close(ctx.original_fd);
	}
	ctx.redirect_flag = 0;
	printf("Now,close stdout redirect! \n");
	return ;
}






