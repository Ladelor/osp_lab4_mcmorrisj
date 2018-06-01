SRCS = file_search.c

OPTS = -Wall -O -pthread

clean: file_search file_search_threaded
	rm file_search
	rm file_search_threaded

file_search: $(SRCS)
	gcc $(OPTS) -o file_search $(SRCS)

file_search_threaded: file_search_threaded.c
	gcc $(OPTS) -o file_search_threaded file_search_threaded.c
