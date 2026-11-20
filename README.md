# Guia de compilação e execução — RigPlan Integrator

Este guia explica como copiar o código-fonte para outro computador, instalar as ferramentas necessárias, compilar o aplicativo e iniciá-lo. Os passos principais são para Linux. O projeto também pode ser compilado no Windows; nesse caso, consulte a seção específica e o guia de pacote portátil.

## 1. O que copiar

Copie ou baixe a pasta completa do projeto, preservando esta estrutura:

- `RigPlanIntegrator.pro`
- `src/`
- `resources/`
- `scripts/`
- `WINDOWS_PORTABLE.md`

Não é necessário copiar os arquivos gerados em `build/`, `Makefile` ou `.qmake.stash`: eles são específicos do computador e serão recriados durante a compilação. Não compile usando um executável produzido em outro sistema operacional ou com outro kit de compilador.

## 2. Linux: instalar as ferramentas

O projeto usa C++17, Qt 6 e qmake. São necessários um compilador C++, `make` e os módulos Qt Core, Gui, Widgets e PrintSupport.

Exemplos de instalação (use o comando correspondente à sua distribuição):

### Ubuntu/Debian

```sh
sudo apt update
sudo apt install build-essential qt6-base-dev qt6-base-dev-tools
```

### Fedora

```sh
sudo dnf install gcc-c++ make qt6-qtbase-devel
```

### Arch Linux

```sh
sudo pacman -S --needed base-devel qt6-base
```

Confirme que `qmake6` está disponível. Em algumas distribuições, o comando pode se chamar `qmake`:

```sh
qmake6 -v
```

Se esse comando não existir, tente `qmake -v`. Use o comando que informar uma versão do Qt 6 nos passos seguintes.

## 3. Linux: compilar

Abra um terminal na pasta raiz do projeto — a pasta onde está `RigPlanIntegrator.pro` — e execute:

```sh
mkdir -p build/linux-release
cd build/linux-release
qmake6 ../../RigPlanIntegrator.pro CONFIG+=release
make -j"$(nproc)"
```

Se sua distribuição usa `qmake` em vez de `qmake6`, substitua apenas o nome do comando. O primeiro comando do qmake gera a configuração de compilação; o `make` compila o aplicativo. A opção `-j"$(nproc)"` permite usar os processadores disponíveis.

Ao final, o executável deverá estar em `build/bin/RigPlanIntegrator`, relativo à pasta raiz do projeto. Se ocorrer erro, confira se está usando o Qt 6 e se todos os pacotes de desenvolvimento foram instalados.

## 4. Linux: iniciar o aplicativo

Na pasta raiz do projeto, execute:

```sh
./build/bin/RigPlanIntegrator
```

Também é possível iniciar pelo gerenciador de arquivos, abrindo o executável `build/bin/RigPlanIntegrator`. Se o sistema informar que faltam bibliotecas Qt, instale os pacotes de runtime do Qt 6 correspondentes à distribuição ou compile no próprio computador onde o programa será usado.

## 5. Windows: compilar e executar

Para gerar uma versão portátil no Windows, siga os requisitos e os passos de [WINDOWS_PORTABLE.md](WINDOWS_PORTABLE.md). Em resumo, é necessário instalar um kit Qt 6 para Windows com Core, Gui, Widgets e PrintSupport, além do compilador compatível com esse kit. O script `scripts/build-windows-portable.ps1` compila o projeto e reúne as DLLs e os plugins Qt necessários.

O script gera `dist/RigPlanIntegrator-Windows/` e um ZIP com o mesmo pacote. Extraia o ZIP em uma pasta com permissão de gravação e execute `RigPlanIntegrator.exe`. Mantenha `portable_data` junto do executável; essa pasta guarda os dados do usuário no pacote portátil.

Não é possível gerar o executável Windows executando a compilação Linux descrita acima. Para Windows, compile em uma máquina Windows, máquina virtual Windows ou ambiente Windows compatível com o kit Qt escolhido.

## 6. Primeiro início e dados do aplicativo

Não há uma etapa separada de instalação do banco de dados ou de configuração inicial. Ao abrir o aplicativo pela primeira vez, ele inicializa os cálculos e predefinições padrão automaticamente e abre a janela principal maximizada.

O aplicativo grava predefinições e outros dados conforme o sistema operacional:

- No pacote portátil do Windows, usa `portable_data` ao lado do executável, se essa pasta existir.
- Fora do modo portátil, usa a pasta de dados de aplicativo definida pelo Qt para o usuário atual.

Garanta que o usuário tenha permissão de escrita no local usado. Para levar os dados de um computador a outro, feche o aplicativo e copie a pasta de dados correspondente; no pacote portátil, copie a pasta completa, incluindo `portable_data`.

## 7. Problemas comuns

- **`qmake6: command not found`:** instale os pacotes de desenvolvimento do Qt 6 ou tente o comando `qmake`.
- **Módulo Qt não encontrado:** confirme a instalação do Qt 6 e dos componentes Core, Gui, Widgets e PrintSupport.
- **Compilação falha após mudar de kit Qt ou compilador:** dentro de `build/linux-release`, execute `make clean` e repita os passos de configuração com qmake e compilação. Não reutilize arquivos de objeto gerados por outro kit ou sistema operacional.
- **Aplicativo não inicia por biblioteca ausente:** compile e execute no mesmo computador, ou instale os runtimes Qt 6 correspondentes. O executável Linux não é, por si só, um pacote portátil para qualquer distribuição.
- **Dados não são salvos:** verifique permissões de escrita na pasta de dados de aplicativo ou, no pacote portátil, na pasta do pacote e em `portable_data`.
