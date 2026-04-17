# projetounreal
SegundaparteRecrutamento

# Simulador USV Otter - Unreal Engine 5

## Descrição do Projeto
Este projeto é um simulador físico e de sensores desenvolvido em Unreal Engine 5 (C++ e Blueprints) para o Veículo de Superfície Não Tripulado - Otter Maritime Robotics. O simulador foca-se no realismo hidrodinâmico, implementando modelos matemáticos avançados baseados nas equações 6-DOF de Fossen, e um conjunto de sensores marítimos. Para mais informações vêr o documento pdf.

## Principais Funcionalidades

###  Física e Hidrodinâmica
Modelo 6-DOF (Fossen): Implementação em C++ dos 6 graus de liberdade (Surge, Sway, Heave, Roll, Pitch, Yaw) utilizando tensores de massa adicionada (Added Mass) e amortecimento linear/angular (Damping).
Propulsão: Simulação de dois motores (Left e Right Thrusters) utilizando a curva de conversão de corrente para força da BlueRobotics, incluindo cálculo de torques baseados na distância ao centro de massa.

### Sensores Simulados
O projeto inclui a simulação de vários sensores com injeção de ruído paramétrico para recriar condições reais:
-LiDAR 2D: Simulação através de *Raycasting* (36 feixes a 360º) utilizando a API de colisões do Unreal Engine.
-GPS (Frente e Trás): Leitura de coordenadas mundiais com ruído XY adicionado.
-AHRS: Sistema de Referência de Atitude e orientação (Roll, Pitch, Yaw) com ruído ajustável.
-Câmera: Feed visual em tempo real acoplado ao mastro.

## Estrutura do Projeto (Content / tentativa2)
Com base na organização dos Blueprints no projeto:
* `tentativa2` (Blueprint Class): O *Pawn* principal que herda as lógicas de C++, contendo a malha 3D do barco e os componentes (Câmera, Sockets).
* `AHRS`, `GPS`, `LiDAR` (Blueprint Classes): Atores/Componentes responsáveis pela integração ao ROS de cada sensor.
* `ROS` (Blueprint Class): Módulo de integração com o Robot Operating System 2 (ROS2), permitindo a comunicação entre o simulador no Unreal e algoritmos de controlo/navegação externos.
* `InputMappingContext1` & `Move1` (Data Assets): Configurações do Enhanced Input System do UE5 para mapeamento moderno de comandos de controlo do barco.

## Dependências e Requisitos
-Motor de Jogo: Unreal Engine 5.0+
-Linguagem: C++ e Blueprints

## Como Utilizar
1. Compilar o projeto via Visual Studio para garantir que as classes  em C++ (`MyPawn.cpp`) estão atualizadas.
2. Abrir o mapa de simulação no Unreal Engine Editor.
3. Certificar que o nível da água (`nivelaguaz`) está configurado corretamente.
4. Pressionar Play. Utilizar os Inputs de (`Move1`) para controlar os motores do barco.

## Autor
- Mariana
