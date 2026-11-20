# RigPlan Integrator — versão portátil para Windows

## Usar o pacote

Extraia o ZIP para uma pasta onde você tenha permissão de gravação e execute `RigPlanIntegrator.exe`. Mantenha a pasta `portable_data` ao lado do executável: predefinições e cálculos personalizados serão salvos nela, sem depender da pasta AppData do Windows. Para transportar seus dados, copie o pacote completo, incluindo `portable_data`.

O pacote é independente do Qt instalado na máquina. O Windows ainda pode solicitar o Microsoft Visual C++ Redistributable se o build tiver sido feito com MSVC; builds MinGW incluem o runtime do compilador quando disponível via `windeployqt --compiler-runtime`.

## Gerar o pacote

Requisitos:

- Windows 10 ou posterior, PowerShell e um kit Qt 6 para Windows com os módulos Core, Gui, Widgets e PrintSupport.
- O compilador usado pelo kit Qt: Visual Studio Build Tools/Developer PowerShell para kits MSVC, ou MinGW correspondente para kits MinGW.
- `qmake.exe`, `windeployqt.exe` e a ferramenta de build correspondente disponíveis no `PATH`, ou informados ao script.

No diretório do projeto, execute `scripts\build-windows-portable.ps1`. O script compila em Release, coleta as DLLs/plugins Qt necessários com `windeployqt` e cria:

- `dist\RigPlanIntegrator-Windows\` — pasta pronta para copiar/extrair e executar.
- `dist\RigPlanIntegrator-Windows.zip` — arquivo portátil para distribuição.

O ZIP contém a pasta do pacote, com `RigPlanIntegrator.exe`, as DLLs/plugins do Qt e a pasta `portable_data`, no mesmo formato portátil da versão 1.0.0.

### Executar a compilação local no Windows

Este projeto está sendo desenvolvido no Linux; o script abaixo precisa ser executado no Windows, não no terminal Linux. Instale o Visual Studio 2022 Build Tools com a carga **Desenvolvimento para desktop com C++** e o Windows SDK. No Qt Maintenance Tool, instale o Qt 6 para o kit **MSVC 2022 64-bit**, incluindo Core, Gui, Widgets e PrintSupport.

Abra o **Developer PowerShell for VS 2022**, entre na pasta do projeto e execute (ajuste a versão ou o caminho do Qt se necessário):

```powershell
.\scripts\build-windows-portable.ps1 `
	-Qmake "C:\Qt\6.8.3\msvc2022_64\bin\qmake.exe" `
	-Windeployqt "C:\Qt\6.8.3\msvc2022_64\bin\windeployqt.exe"
```

O script usa o `nmake` disponibilizado pelo Developer PowerShell. Ao terminar, o executável e o ZIP estarão na pasta `dist` do projeto. Se você estiver trabalhando somente no Linux, será necessário usar uma máquina Windows ou uma máquina virtual Windows para executar essa compilação; instalar apenas um compilador Linux não gera o `.exe` portátil.

Para especificar o kit quando as ferramentas não estiverem no `PATH`, passe os caminhos completos com os parâmetros `-Qmake` e `-Windeployqt`; use `-BuildTool` se necessário. Qmake, windeployqt e compilador precisam pertencer ao mesmo kit/arquitetura.