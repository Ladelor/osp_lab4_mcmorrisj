SRCS = file_search.c

OPTS = -Wall -O

clean: file_search
	rm file_search

file_search: $(SRCS)
	gcc $(OPTS) -o file_search $(SRCS)
