#include <unistd.h>
 
#include <sys/inotify.h>
//#include <libnotify/notify.h> 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define USAGE_ERR "Usage: ./Deamon FilePath/Filename\n"

int IeventQueue = -1;
int IeventStatus = -1;

void signalHandler(int signal)
{
    int closeStatus = -1;
    printf("\nSignal Received, Cleaning Up...\n");

    if (inotify_rm_watch(IeventQueue, IeventStatus) == -1) fprintf(stderr, "Error in remove an existing watch from an inotify instance\n");
    close(IeventQueue);
    exit(0);
}

int main ( int ac, char **av)
{
	char *basePath = NULL;
	char *token = NULL;
	char *readPointer = NULL;
	char *notificationMessage = NULL;

	const struct inotify_event* watchEvent;
	uint32_t watchMask = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE | IN_MODIFY | IN_MOVE_SELF;
	
	char buffer[4096];
	int readLenght;

	// cheking if there is an typ
	if (ac != 2) return (write(2, USAGE_ERR, strlen(USAGE_ERR) + 1));
	
	basePath = malloc(sizeof(char *) + strlen(av[1]) + 1);
	if (!basePath) return 1;
	strcpy(basePath, av[1]);
	
	token = strtok(basePath, "/");
	while (token != NULL)
	{
		basePath = token;
		token = strtok(NULL, "/");
	}

	if (basePath == NULL) return (fprintf(stderr, "Error: basePath == NULL\n"));

	printf("token (strtok) : %s\n", token);
	printf("BasePath = %s\n", basePath);
	
	// Init the instance of this inotify api
	IeventQueue = inotify_init();
	if (IeventQueue == -1) return (fprintf(stderr, "Error Initialising inotify instance!\n"));
	
	// adds a watch for a specified events on a file or directory
	IeventStatus = inotify_add_watch(IeventQueue, av[1], watchMask);
	if (IeventStatus < 0) return (fprintf(stderr, "Error adding file to watch instance!\n"));

    signal(SIGABRT, signalHandler);
    signal(SIGINT,  signalHandler);
    signal(SIGTERM, signalHandler);

	while (1337)
	{
		printf("Waiting for Ievent...\n");

		readLenght = read(IeventQueue, buffer, sizeof(buffer));
		if (readLenght < 0) return (fprintf(stderr, "Error reading from inotify instance!\n"));

		for (char *bufferPointer = buffer; bufferPointer < buffer + readLenght; bufferPointer += sizeof(struct inotify_event) + watchEvent->len) 
		{
			notificationMessage = NULL;
			watchEvent = (const struct inotify_event *)bufferPointer;
			
			// Print the event mask for debugging
        		printf("Event mask:  %u\n", watchEvent->mask);

			if (watchEvent->mask & IN_CREATE) notificationMessage = "File Created.\n";
			if (watchEvent->mask & IN_DELETE) notificationMessage = "File Removed.\n";
            if (watchEvent->mask & IN_ACCESS) notificationMessage = "Access Detected\n";
			if (watchEvent->mask & IN_CLOSE_WRITE) notificationMessage = "Write Closed\n";
			if (watchEvent->mask & IN_MODIFY) notificationMessage = "File Modified\n";
			if (watchEvent->mask & IN_MOVE_SELF) notificationMessage = "Self Moved\n";

			if (notificationMessage == NULL) continue;
			printf("%s\n", notificationMessage);
		}
	}
}
