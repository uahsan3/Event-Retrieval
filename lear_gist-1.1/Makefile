# Lear's GIST implementation, version 1.1, (c) INRIA 2009, Licence: GPL


all: compute_gist

WFFTINC=-I/usr/local/include
WFFTLIB=-L/usr/local/lib
# WFFTINC=-I/scratch/adonis/mordelet/install/include
# WFFTLIB=-L/scratch/adonis/mordelet/install/lib


# gist.c: ../../descriptors/gist.c 
#	cp $< $@


gist.o: gist.c gist.h standalone_image.h
	gcc -c -Wall -g $< $(WFFTINC) -DUSE_GIST -DSTANDALONE_GIST -lm

standalone_image.o: standalone_image.c standalone_image.h
	gcc -c -Wall -g $< -lm

compute_gist: compute_gist.c gist.o standalone_image.o
	gcc -Wall -g -o $@ $^ $(WFFTLIB) -lfftw3f -lm



clean: 
	rm -f *.o compute_gist

