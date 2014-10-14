#-----------------------
# IMPORTANTE: Los valores de los parametros de los ejercicio?_test deben ajustarse.
# Se asume que los ficheros fuente se llaman ejercicio1.c, ejercicio2.c,...,ordenar.h
#-----------------------

CC = mpicc -pedantic -W
EXE = pi parallelPi sum parallelSum matrix parallelSmoothVector smoothVector deadlock deadlock64k blocksends bandwidth parallelMatrix
all : $(EXE)

.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o
	@echo "#---------------------------"
	@echo "# Generando $@ "
	$(CC)  -o $@ $@.o 

%.o : %.c 
	@echo "#---------------------------"
	@echo "# Generando $@"
	$(CC)  -c $<

