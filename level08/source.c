#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

void log_wrapper(FILE *backup_file, const char *command, const char *file_name)
{
	char dest[264];

	strcpy(dest, command);
	snprintf(&dest[strlen(dest)], 254 - strlen(dest), file_name);
	dest[strcspn(dest, "\n")] = 0;
	fprintf(backup_file, "LOG: %s\n", dest);
}

int main(int argc, const char **argv, const char **envp)
{
	FILE *backups_log;
	FILE *stream;
	int fd;
	char buf;
	char dest[104];

	buf = -1;
	if (argc != 2)
		printf("Usage: %s filename\n", *argv);
	backups_log = fopen("./backups/.log", "w");
	if (!backups_log)
	{
		printf("ERROR: Failed to open %s\n", "./backups/.log");
		exit(1);
	}
	log_wrapper(backups_log, "Starting back up: ", argv[1]);
	stream = fopen(argv[1], "r");
	if (!stream)
	{
		printf("ERROR: Failed to open %s\n", argv[1]);
		exit(1);
	}
	strcpy(dest, "./backups/");
	strncat(dest, argv[1], 99 - strlen(dest));
	/*
		Flags: O_WRONLY, O_CREATE, O_TRUNC:
			Opens in read only, creates the file if doesn't exist, truncate the
			file to 0 length if already exists
		Mode: S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP
			Read and write permission for the file owner and group (perm 0660: rw-rw----)
	*/
	fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP); 
	if (fd < 0)
	{
		printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
		exit(1);
	}
	while (1)
	{
		buf = fgetc(stream);
		if (buf == -1)
			break;
		write(fd, &buf, 1);
	}
	log_wrapper(backups_log, "Finished back up ", argv[1]);
	fclose(stream);
	close(fd);
	return 0;
}
