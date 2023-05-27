all:file_client	file_manager
file_client:	file_client.o
	gcc	file_client.c -o file_client
file_manager:	file_manager.o
	gcc	file_manager.c -o file_manager
clean:
	del	*.o