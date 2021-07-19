
# Projeto de Sistemas Operativos 2019/2020:
## Administração Portuária

Este projeto será realizado principalmente na linguagem de programação C em conjunto com as APIs de Linux e POSIX, mas também irá incluir alguma programação em *shell* e *makefile*.

O propósito geral do projeto será simular o fluxo central de um serviço de administração portuária com várias operações, tais como efetuar cargas, descargas, armazenagem de mercadorias, etc. Esta aplicação, chamada SO_AdmPor, envolverá múltiplos processos cooperativos para efetuar  as  suas  operações.  O  fluxo  de  chamadas  entre  processos  envolve: 

1. Envio de descrições de operações pretendidas.
2. Emissão de pedidos destas operações.
3. Agendamento/execução das mesmas. De forma a se poder aferir a qualidade do serviço prestado, são também registadas informações de progresso sob a forma de um *log*, que podem posteriormente ser analisadas.

**Tecnologias principais:** Multi-threading, memory buffers, semáforos, locks, sinais e alarmes .

Todo o projeto e a sua especificação encontra-se apresentada em maior detalhe em ambos enunciados.

---

## Compilação e makefile

A compilação de todos os ficheiros de código fonte presentes no projeto está definida no makefile, através do comando **make**, na shell, de onde resulta o executável *soadmpor* em `/bin/`.

Será possível definir o input de teste, através dos ficheiros presentes em `tests/in/`, onde já se encontram quatro ficheiros *scenario1-scenario4*. A utilização dos comandos **make run1-4** define o ficheiro de input correspondente e, após execução, será criado o ficheiro de output em `tests/out/` de respetiva denominação.

Se o executável fornecido pelos docentes estiver presente em `/bin/` com o nome *soadmpor_prof*, ao utilizar o comando **runtest** serão corridos ambos executáveis, de ficheiros de input iguais, cujos ficheiros de output correspondentes serão comparados **(ver limitações)**. 

---

## Utilização do Script

O ficheiro script.sh recebe o nome **(não o path)** de dois ficheiros que estejam em qualquer diretoria do projeto e compara-os, onde linhas em posições diferentes serão contabilizadas na mesma.

---

## Limitações

time\_write\_log_timed (time.c) não está devidamente implementada, logo 
o output para a consola não é completamente correto.

time_difference (time.c) tem problemas em tempos com valores em ordens 
de unidades diferentes e como tal alguns cálculos vão resultar em erro.

file\_write\_log_file (file.c) está incorretamente implementada, os ficheiros
log não seguem o formato definido.

Devido à aleatoriedade introduzida pelo valor do tempo inicial, execuções sucessivas resultam em ficheiros de output diferentes, mesmo com inputs iguais. Como tal, ao comparar o output do executável fornecido pelos docentes e outro output produzido por este executável, não pode ser garantido que estes sejam iguais. Quanto menor for o número de clientes menor será a probabilidade de isto acontecer. É possível, no entanto, correr os testes várias vezes e eventualmente será obtido o resultado esperado.

---

## Autores

Projeto realizado por Grupo 26:

- **João Cotralha** Nº51090  
- **Cláudio Esteves** Nº51098
