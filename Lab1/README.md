/*///////////////////////////////////////////////////////
// Trabalho 2 - Criação de um programa shell           //
//                                                     //
// Autores:                                            //
// Nome: Roseval Donisete Malaquias Junior             //
// RA: 758597                                          //
// Turma: A                                            //
//                                                     //
// Nome: Luís Felipe Corrêa Ortolan                    //
// RA: 759375                                          //
// Turma: A                                            //
//                                                     //
// Principal objetivo desse trabalho é entender como o //
// shell se comporta internamente. Dessa forma, utili- //
// ando as informações adquiridas para desenvolver um  //
// programa shell que possa ser utilizado.             //
///////////////////////////////////////////////////////*/

Como utilizar o miniShell:
"$make"
"$./miniShell"

Principais objetivos de implementação para o miniShell:

a. $ prog 

b. $ prog &

c. $ prog parâmetros                        // parâmetros é uma lista de parâmetros separados por espaços

d. $ prog parâmetros > arquivo         // arquivo é um nome de arquivo
e. $ prog parâmetros < arquivo

[ f. $ prog parâmetros > arq_saída < arq_entrada ]

[ g. $ prog parâmetros >> arq_saída ]

[ h. $ prog parâmetros 2> arq_erros ]

[ i. $ prog parâmetros 2>> arq_erros ]

[ j. $ prog1 para_1 | prog2 param_2 ]    // Implementação do pipeline era opcional, então nao foi implementada.

k. $ jobs

k. $ fg %num_job                        // num_job é um número associado a um job, como listado pelo comando jobs

l. $ bg %num_job 

m. $ cd diretório                          // diretório pode ser: .. , / , /diretório , ou um nome de diretório.

n. $ pwd 

o. $ exit                                      