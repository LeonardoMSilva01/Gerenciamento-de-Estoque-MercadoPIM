1. Sobre o projeto

O Mercado PIM é um sistema de gerenciamento de estoque desenvolvido em linguagem C como projeto acadêmico. O sistema permite cadastrar e consultar produtos, controlar entradas e saídas de estoque e registrar as movimentações realizadas.

2. Tecnologias utilizadas

• Linguagem C

• GCC

• Visual Studio Code

• Git e GitHub

3. Funcionalidades

• Cadastro de produtos

• Consulta de produtos

• Busca por nome

• Filtro por marca

• Controle de entrada de estoque

• Controle de saída de estoque

• Consulta de movimentações

• Controle de estoque mensal

• Registro de movimentações

• Persistência dos dados em arquivos

• Validação dos dados informados

4. Estrutura do sistema

Produto

• Código de barras

• Nome

• Marca

• Categoria

• Lote

• Data de validade

• Preço de venda

• Quantidade em estoque

Movimentação

• Produto

• Tipo de movimentação

• Quantidade

• Saldo anterior

• Saldo atual

• Data

5. Como executar

Compile o programa utilizando o GCC:

gcc mercado_pim.c -o mercado_pim

No Windows, execute:

mercado_pim.exe

6. Demonstração

Para demonstrar o funcionamento do sistema, podem ser adicionados ao repositório prints ou GIFs das principais funcionalidades:

• Tela inicial/menu

• Cadastro de produto

• Consulta de produtos

• Entrada de estoque

• Saída de estoque

• Matriz de estoque mensal

• Consulta de movimentações

7. Contexto acadêmico

Projeto desenvolvido para aplicação prática de conceitos de lógica de programação, estruturas de dados, manipulação de arquivos e desenvolvimento de sistemas em linguagem C.

8. Melhorias futuras

• Implementação de banco de dados

• Interface gráfica

• Sistema de usuários e permissões

• Relatórios de estoque

• Melhorias na pesquisa de produtos

• Integração com outros sistemas
