IDIR = include
CC = gcc
CFLAGS = -I $(IDIR)

LIBS = -lrt -lpthread

ODIR = obj
_OBJ = main.o memory.o cliente.o control.o empresa.o file.o \
			 intermediario.o prodcons.o scheduler.o time.o
OBJ = $(addprefix $(ODIR)/,$(_OBJ))


%.o: src/%.c
	$(CC) -c -o $(ODIR)/$@ $< $(CFLAGS)

soadmpor: $(_OBJ)
	$(CC) -o bin/$@ $(OBJ) $(ODIR)/so.o $(CFLAGS) $(LIBS)

.PHONY: clean run1 run2 run3 run4 runtest

clean:
	rm -f $(OBJ)
	rm -f bin/soadmpor
	#rm -f tests/out/*

#nestas regras o ficheiro de input e o cenario fornecido pelos docentes

run1:
	./bin/soadmpor tests/in/scenario1 tests/out/scenario1test -l \
																		tests/log/scenario1test.log -t 1000

run2:
	./bin/soadmpor tests/in/scenario2 tests/out/scenario2test -l \
																		tests/log/scenario2test.log -t 1000

run3:
	./bin/soadmpor tests/in/scenario3 tests/out/scenario3test -l \
																		tests/log/scenario3test.log -t 1000

run4:
	./bin/soadmpor tests/in/scenario4 tests/out/scenario4test -l \
	 																	tests/log/scenario4test.log -t 1000

#executavel fornecido pelos docentes deve estar em bin com o nome soadmpor_prof
#
#devido a execucoes consecutivas nao resultar em valores iguais nos ficheiros
#de output execucoes desta regra terao resultados diferentes em cada execucao
runtest:

	###################################ONE#######################################
	./bin/soadmpor_prof tests/in/scenario1 tests/out/scenario1control -t 1000
	./bin/soadmpor tests/in/scenario1 tests/out/scenario1test -l \
																		tests/log/scenario1test.log -t 1000

  ###################################TWO#######################################
	./bin/soadmpor_prof tests/in/scenario2 tests/out/scenario2control -t 1000
	./bin/soadmpor tests/in/scenario2 tests/out/scenario2test -l \
																		tests/log/scenario2test.log -t 1000

	##################################THREE######################################
	./bin/soadmpor_prof tests/in/scenario3 tests/out/scenario3control -t 1000
	./bin/soadmpor tests/in/scenario3 tests/out/scenario3test -l \
	 																	tests/log/scenario3test.log -t 1000

	##################################FOUR#######################################
	./bin/soadmpor_prof tests/in/scenario4 tests/out/scenario4control -t 1000
	./bin/soadmpor tests/in/scenario4 tests/out/scenario4test -l \
																		tests/log/scenario4test.log -t 1000

	@#quanto menor o numero de clientes maior a probabilidade de serem iguais

	@echo "\nScenario 1 test:"
	@./script.sh scenario1control scenario1test

	@echo "\nScenario 2 test:"
	@./script.sh scenario2control scenario2test

	@echo "\nScenario 3 test:"
	@./script.sh scenario3control scenario3test

	@echo "\nScenario 4 test:"
	@./script.sh scenario4control scenario4test
