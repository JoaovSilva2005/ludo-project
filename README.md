# 🎲 Ludo em C

Implementação do clássico jogo de tabuleiro **Ludo** em linguagem **C**, rodando no console do Windows com tabuleiro colorido, sistema de turnos e menu interativo. Projeto Integrador desenvolvido durante o curso de Análise e Desenvolvimento de Sistemas.

> Jogo de terminal para 4 jogadores, com renderização do tabuleiro em tempo real usando a API de console do Windows.

---

## 📸 Telas do jogo

<!-- PASSO IMPORTANTE: rode o jogo (main.exe), tire prints do menu e do tabuleiro,
     salve como docs/menu.png e docs/tabuleiro.png no repositório, e as imagens
     aparecerão aqui automaticamente. -->
![Menu do jogo Ludo](./docs/menu.png)
![Tabuleiro em execução](./docs/tabuleiro.png)

---

## ✨ Funcionalidades

- 🎨 **Tabuleiro colorido** renderizado no console (11×11) com áreas de base, caminho e centro
- 👥 **4 jogadores** (Vermelho, Amarelo, Azul e Verde), com 4 peças cada
- 🎲 **Rolagem de dado** (1 a 6) a cada turno
- 🚦 **Sistema de turnos** rotativo entre os jogadores
- 🏠 **Regra da base**: a peça só sai da base ao tirar **6** no dado
- 🛤️ **Movimentação** das peças ao longo do caminho de 40 casas, com **caminhos finais** exclusivos por cor
- 📋 **Menu interativo**: Jogar, Regras, Ranking e Sair
- 👋 Tela de boas-vindas e tela de regras

---

## 🛠️ Tecnologias e requisitos

- **Linguagem:** C (100%)
- **Plataforma:** Windows (usa `windows.h` e `conio.h` para cor, posicionamento do cursor e leitura de teclas)
- **Compilador:** GCC / MinGW (ou qualquer compilador C compatível com Windows)

> ⚠️ Por depender de `windows.h` e `conio.h`, o projeto foi feito para **Windows**. Não compila diretamente em Linux/macOS sem adaptação.

---

## ▶️ Como compilar e executar

Clone o repositório:

```bash
git clone https://github.com/JoaovSilva2005/ludo-project.git
cd ludo-project
```

Compile com GCC (MinGW):

```bash
gcc main.c -o main.exe
```

Execute:

```bash
main.exe
```

> Também é possível apenas rodar o `main.exe` já incluído no repositório.

---

## 🎮 Como jogar

1. Na tela inicial, pressione qualquer tecla para avançar.
2. No **menu**, digite `1` para começar a partida (ou `2` para ver as regras).
3. A cada turno, o dado é rolado automaticamente e o valor é exibido.
4. Escolha qual peça mover digitando um número de **1 a 4**.
5. Lembre-se: uma peça **só sai da base quando você tira 6**.
6. O turno passa para o próximo jogador e o ciclo se repete.

---

## 📁 Estrutura do projeto

```
ludo-project/
├── main.c                 # Código-fonte principal (jogo completo integrado)
├── main.exe               # Executável compilado (Windows)
├── test.c                 # Testes / experimentos
├── Telas/
│   └── PRONTOS/           # Telas modularizadas em arquivos separados
│       ├── Tela Inicial.c
│       ├── boas vindas.c
│       ├── CADASTROJOGADOR.c
│       ├── TABULEIRO.c
│       ├── rank.c
│       └── SAIR.c
└── README.md
```

---

## 🚧 Limitações e próximos passos

O jogo já é **jogável**, mas ainda está em evolução. Melhorias planejadas:

- [ ] Detecção automática de **vitória** (quando um jogador leva todas as peças ao centro)
- [ ] **Captura** de peças adversárias ao cair na mesma casa
- [ ] Validação de jogadas inválidas (peça na base sem tirar 6, índice fora do intervalo)
- [ ] Turno extra ao tirar 6
- [ ] Ranking funcional ao final da partida

---

## 👥 Autores

- **João Vitor da Silva** — [@JoaovSilva2005](https://github.com/JoaovSilva2005)
- **Murilo Cobriski**

Projeto Integrador — Análise e Desenvolvimento de Sistemas.
