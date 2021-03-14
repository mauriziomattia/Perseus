CC=gcc

perseo: commands.o connectivity.o delays.o erflib.o events.o \
        init.o invar.o modules.o nalib.o neurons.o perseo.o \
        queue.o randdev.o results.o sortedqueue.o stimuli.o \
        synapses.o timer.o
	${CC} -O2 -o perseo commands.o connectivity.o delays.o erflib.o events.o \
        init.o invar.o modules.o nalib.o neurons.o perseo.o \
        queue.o randdev.o results.o sortedqueue.o stimuli.o \
        synapses.o timer.o -lm

perseo.o: perseo.c queue.h timer.h invar.h randdev.h perseo.h \
          init.h results.h stimuli.h events.h commands.h modules.h \
          delays.h neurons.h
	${CC} -O2 -c perseo.c

commands.o: commands.c types.h events.h stimuli.h perseo.h \
            results.h modules.h connectivity.h
	${CC} -O2 -c commands.c

connectivity.o: connectivity.c invar.h randdev.h types.h \
                perseo.h modules.h connectivity.h delays.h synapses.h
	${CC} -O2 -c connectivity.c

delays.o: delays.c nalib.h randdev.h types.h connectivity.h delays.h
	${CC} -O2 -c delays.c

erflib.o: erflib.c erflib.h
	${CC} -O2 -c erflib.c

events.o: events.c sortedqueue.h events.h perseo.h
	${CC} -O2 -c events.c

init.o: init.c invar.h randdev.h types.h perseo.h results.h \
        stimuli.h init.h events.h modules.h neurons.h \
        connectivity.h synapses.h delays.h commands.h
	${CC} -O2 -c init.c

invar.o: invar.c invar.h
	${CC} -O2 -c invar.c

modules.o: modules.c erflib.h randdev.h types.h perseo.h \
           neurons.h modules.h events.h
	${CC} -O2 -c modules.c

nalib.o: nalib.c nalib.h
	${CC} -O2 -c nalib.c

neurons.o: neurons.c randdev.h types.h perseo.h init.h modules.h \
           connectivity.h neurons.h results.h delays.h
	${CC} -O2 -c neurons.c

queue.o: queue.c queue.h
	${CC} -O2 -c queue.c

randdev.o: randdev.c randdev.h
	${CC} -O2 -c randdev.c

results.o: results.c queue.h invar.h perseo.h stimuli.h \
           modules.h connectivity.h synapses.h neurons.h \
           events.h
	${CC} -O2 -c results.c

sortedqueue.o: sortedqueue.c sortedqueue.h
	${CC} -O2 -c sortedqueue.c

stimuli.o: stimuli.c randdev.h types.h events.h stimuli.h
	${CC} -O2 -c stimuli.c

synapses.o: synapses.c erflib.h randdev.h types.h perseo.h \
            connectivity.h synapses.h results.h modules.h
	${CC} -O2 -c synapses.c

timer.o: timer.c
	${CC} -O2 -c timer.c


clean:
	rm -f perseo commands.o connectivity.o delays.o erflib.o \
        events.o init.o invar.o modules.o nalib.o neurons.o perseo.o \
        queue.o randdev.o results.o sortedqueue.o stimuli.o \
        synapses.o timer.o
