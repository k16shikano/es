YFLAGS = -d
OBJS = es.o read.o utils.o apply.o eval.o env.o symbol.o

es: $(OBJS)
	gcc $(OBJS) -o es

es.o: es.h

objectlist: es.h y.tab.h

clean:
	rm -f $(OBJS) y.tab.[ch]
