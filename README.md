# 🎮 Projeto Breakout com RayLib

![Status](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)
![Linguagem](https://img.shields.io/badge/C-RayLib-blue)
![Licença](https://img.shields.io/badge/licença-MIT-green)

> Recriação moderna do clássico jogo **Breakout**, utilizando a linguagem **C** com a biblioteca gráfica **RayLib**. Este projeto foi desenvolvido como parte de um trabalho universitário.

---

## 🧠 Sobre o Projeto

O **Breakout** é um jogo de arcade dos anos 70 onde o jogador controla uma barra para rebater uma bola e destruir blocos no topo da tela. Este projeto recria essa experiência com visual moderno, efeitos sonoros e novos elementos de jogabilidade, como:

- Sistema de fases com tijolos que descem do topo.
- Poderes que afetam a jogabilidade (vida extra, barra maior, múltiplas bolas).
- Interface personalizada com menus, pause e tela de morte.
- Efeitos sonoros integrados e personalização de cor da bola.

---

## 🛠️ Tecnologias Utilizadas

- **Linguagem:** C (padrão C99)
- **Biblioteca Gráfica:** [RayLib](https://www.raylib.com/)
- **Extensões:** RayGUI para a interface da tela de personalização
- **Compilador:** GCC / MinGW
- **Sistema Operacional Alvo:** Windows (compatível com Linux)

---

## 🎮 Mecânicas do Jogo

- 🎯 **Colisão da bola** com paddle, paredes e tijolos.
- 🧱 **Destruição dos blocos** e sistema de pontuação.
- 🎁 **Poderes aleatórios**: barra maior, bola extra e vida extra.
- 🧠 **Tela de Personalização** da cor da bolinha.
- 💀 **Tela de fim de jogo** com pontuação e recorde.
- 🔊 **Controle de som** com botão ativar/desativar.
- ⏸️ **Menu de pausa** com opção de retorno ou sair.

---

## 📦 Como Executar o Projeto

### ⚙️ Pré-requisitos

- Compilador C (GCC ou MinGW)
- Biblioteca RayLib instalada
- (Opcional) RayGUI para a tela de personalização

### 🐧 Linux

```bash
.\build.sh
./breakout
```

### 🪟 Windows (MinGW)

```bash
.\build.bat
./breakout.exe
```

> Certifique-se de que as DLLs da RayLib estejam no mesmo diretório do `.exe`.

---

## 📁 Estrutura do Projeto

📂 breakout/
├── 📁 resources/
│   ├── 📂 images/
│   │   ├── somOn.png
│   │   └── somOff.png
│   └── 📂 sfx/
│       ├── batida.wav
│       ├── batidaTijolo.wav
│       ├── comecando.wav
│       ├── morte.wav
│       └── proximaFase.wav
├── main.c
└── README.md

---

## 👥 Equipe

- [Guilherme Campelo](https://www.linkedin.com/in/guilherme-campelo/) — lógica principal, menus e interface
- Nicolas Busa — física da bola, colisões e mecânicas de jogo

---

## 🎯 Objetivos Acadêmicos

- Implementar lógica de colisão, renderização e controle de fluxo.
- Trabalhar com bibliotecas gráficas e eventos de input.
- Praticar controle de versão com Git e colaboração em grupo.
- Compreender o ciclo completo de um jogo 2D: do menu à vitória/derrota.

---

## 📌 Status do Projeto

| Funcionalidade                     | Status        |
|-----------------------------------|---------------|
| Estrutura base do jogo            | ✅ Concluído   |
| Colisões e física da bolinha      | ✅ Concluído   |
| Pontuação e vidas                 | ✅ Concluído   |
| Sistema de poderes                | ✅ Concluído   |
| Sons e efeitos sonoros            | ✅ Concluído   |
| Tela de personalização            | ✅ Concluído   |
| Animações (tijolos descendo)      | ✅ Concluído   |
| Sistema de fases                  | ✅ Concluído   |
| Compatibilidade Windows/Linux     | ✅ Concluído   |

---

## 📃 Licença

Distribuído sob a Licença MIT.  
Veja o arquivo [`LICENSE`](LICENSE) para mais detalhes.

---

## 📸 Preview

![Gameplay](https://imgur.com/CsGMuY6)
![Gameplay](https://imgur.com/qAnFz6C)
---

🧾 *Este projeto é parte de um trabalho acadêmico e está em constante evolução. Feedbacks, sugestões e melhorias são bem-vindos!*