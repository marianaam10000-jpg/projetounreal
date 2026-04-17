// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoatMesh"));
	RootComponent = BoatMesh;

	// Desligar a física do Unreal Engine
	BoatMesh->SetSimulatePhysics(false);

    // Criar a câmara
    BoatCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BoatCamera"));

    // Colar a câmera ao socket "Camera"
    BoatCamera->SetupAttachment(BoatMesh, FName("Camera"));

}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

    float FluidDensity = 1025.0f;
    float VehicleDensity = 950.0f; 


    //  Calcular o Volume Total usando a Densidade do Veículo
    float VolumeTotal_m3 = Mass / VehicleDensity;

    
    // altura casco
    float AlturaTotal_m = 0.4f;

    // Calcular a Área de flutuação
    float AreaAw_m2 = VolumeTotal_m3 / AlturaTotal_m;

    // Aplicar a fórmula de Fossen usando a Densidade da Água
    float Z_restauro_total = FluidDensity * Gravity * AreaAw_m2;

    // Dividir por 4 sockets e por 100 para passar a N/cm
    BuoyancyCoefficient = (Z_restauro_total / 4.0f) / 100.0f;
}
	

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    // Declaração variáveis
    float ForcaBuoyancyz = 0.0f;
    float BuoyancyRoll = 0.0f;
    float BuoyancyPitch = 0.0f;

    if (BoatMesh)
    {   //CALCULAR POSIÇÕES

        //posições sockets
        // Em relacao ao mundo
        FVector Posfl = BoatMesh->GetSocketLocation(SocketFrontLeft);
        FVector Posfr = BoatMesh->GetSocketLocation(SocketFrontRight);
        FVector Posbl = BoatMesh->GetSocketLocation(SocketBackLeft);
        FVector Posbr = BoatMesh->GetSocketLocation(SocketBackRight);

        //Em relação ao pivot
        FVector Localfl = BoatMesh->GetSocketTransform(SocketFrontLeft, RTS_Actor).GetLocation();
        FVector Localfr = BoatMesh->GetSocketTransform(SocketFrontRight, RTS_Actor).GetLocation();
        FVector Localbl = BoatMesh->GetSocketTransform(SocketBackLeft, RTS_Actor).GetLocation();
        FVector Localbr = BoatMesh->GetSocketTransform(SocketBackRight, RTS_Actor).GetLocation();

        //POSIÇÃO CENTRO DE MASSA

        // Em relação ao pivot
        FVector centromassa = BoatMesh->GetBodyInstance()->GetMassSpaceLocal().GetLocation();

        // Calcular a Profundidade
        float DistanciaFundo = 15.0f;

        float Profundidadefl = FMath::Max(0.0f, nivelaguaz - (Posfl.Z - DistanciaFundo));
        float Profundidadefr = FMath::Max(0.0f, nivelaguaz - (Posfr.Z - DistanciaFundo));
        float Profundidadebl = FMath::Max(0.0f, nivelaguaz - (Posbl.Z - DistanciaFundo));
        float Profundiadebr = FMath::Max(0.0f, nivelaguaz - (Posbr.Z - DistanciaFundo));

        // Força aplicada em cada socket
        float Forcafl = Profundidadefl * BuoyancyCoefficient;
        float Forcafr = Profundidadefr * BuoyancyCoefficient;
        float Forcabl = Profundidadebl * BuoyancyCoefficient;
        float Forcabr = Profundiadebr * BuoyancyCoefficient;

        //calcula distância do socket em relação ao centro de massa
        FVector momentofl = Localfl - centromassa/100.0f;
        FVector momentofr = Localfr - centromassa/100.0f;
        FVector momentobl = Localbl - centromassa/100.0f;
        FVector momentobr = Localbr - centromassa/100.0f;

        // Força vertical Total = Força da qua àgua faz(heave) - Gravidade
        float fheave = Forcafl + Forcafr + Forcabl + Forcabr;
        float Forca_Gravidade = Mass * Gravity * 100.0f; // * 100 por causa dos cm do Unreal
        ForcaBuoyancyz = fheave - Forca_Gravidade;

        // Torques (Força x Distância = momento)
        BuoyancyRoll = (Forcafl * momentofl.Y) + (Forcafr * momentofr.Y) + (Forcabl * momentobl.Y) + (Forcabr * momentobr.Y);
        BuoyancyPitch = -((Forcafl * momentofl.X) + (Forcafr * momentofr.X) + (Forcabl * momentobl.X) + (Forcabr * momentobr.X));
    }


    //FÍSICA FOSSEN 6DOF

    // Massa total
    float m_surge = Mass + AddedMass_X;
    float m_sway = Mass + AddedMass_Y;
    float m_yaw = Iz + AddedMass_N;

    float m_heave = Mass + Added_Mass_Linear_Z;
    float m_roll = Inertia_Tensor_X + Added_Mass_Angular_X;
    float m_pitch = Inertia_Tensor_Y + Added_Mass_Angular_Y;

    // Forças Finais 
    float Force_X = Thrust_X - (Damping_X * velocidade.x);
    float Force_Y = Thrust_Y - (Damping_Y * velocidade.y);
    float Torque_N_Total = Torque_N - (Damping_N * velocidade.yaw);
    float Force_Z = ForcaBuoyancyz - (Linear_Damping_Tensor_Z * velocidade.z);
    float Torque_K = BuoyancyRoll - (Angular_Damping_Tensor_X * velocidade.roll);
    float Torque_M = BuoyancyPitch - (Angular_Damping_Tensor_Y * velocidade.pitch);

    // Acelerações (Aceleração = Força / Massa)
    aceleracao.x = Force_X / m_surge;
    aceleracao.y = Force_Y / m_sway;
    aceleracao.yaw = Torque_N_Total / m_yaw;

    aceleracao.z = Force_Z / m_heave;
    aceleracao.roll = Torque_K / m_roll;
    aceleracao.pitch = Torque_M / m_pitch;

    // atulização das Velocidades
    velocidade.x = velocidade.x + aceleracao.x * DeltaTime;
    velocidade.y =velocidade.y + aceleracao.y * DeltaTime;
    velocidade.z = velocidade.z + aceleracao.z * DeltaTime;

    velocidade.roll = velocidade.roll + aceleracao.roll * DeltaTime;
    velocidade.pitch = velocidade.pitch + aceleracao.pitch * DeltaTime;
    velocidade.yaw = velocidade.yaw + aceleracao.yaw * DeltaTime;

    
    //Movimento no unreal
   
    // multiplicar as posições por 100 porque o unreal move-se em centímetros
    FVector movimentobarco = FVector((velocidade.x * DeltaTime) * 100.0f, (velocidade.y * DeltaTime) * 100.0f, (velocidade.z * DeltaTime) * 100.0f);

    // passar o roll pitch yaw para radianos
    FRotator rotacaobarco = FRotator(
        (velocidade.pitch * DeltaTime) * (180.0f / PI),
        (velocidade.yaw * DeltaTime) * (180.0f / PI),
        (velocidade.roll * DeltaTime) * (180.0f / PI)
    );

    // Aplicar o movimento local ao otter
    AddActorLocalOffset(movimentobarco, true);
    AddActorLocalRotation(rotacaobarco);

    // Atualizar as leituras dos sensores depois do barco se mexer
    AMyPawn::atualizarsensores();
    //atualizar feixes do LiDAR
    AMyPawn ::SimularLidar2D();
}

void AMyPawn::motores(float frente, float lado)
{
    // corrente máxima
    float correntemax = 20.0f;

    // motor esquerdo
    float LeftMotor_Current = (frente + lado) * correntemax;
    //impor limites
    if (LeftMotor_Current > correntemax) {LeftMotor_Current = correntemax;}
    else if (LeftMotor_Current < -correntemax)
    {LeftMotor_Current = -correntemax;}
    
    //motor direito
    float RightMotor_Current = (frente - lado) * correntemax;
    // impor limites
    if (RightMotor_Current > correntemax)
    {RightMotor_Current = correntemax;}
    else if (RightMotor_Current < -correntemax)
    {RightMotor_Current = -correntemax;}
    
    //coverter a corrente usando a função definida abaixo
    float LeftMotor_Force = convertecorrente(LeftMotor_Current);
    float RightMotor_Force = convertecorrente(RightMotor_Current);

    //valores de seguranca
    float LeftDistance = 0.5f;
    float RightDistance = 0.5f;

    if (BoatMesh)
    {
        if (BoatMesh->DoesSocketExist(LeftThrusterSocket))
        {   //posição socket esquedo em relação ao pivot
            float disSocketesq = BoatMesh->GetSocketTransform(LeftThrusterSocket, RTS_Actor).GetLocation().Y;
            LeftDistance = FMath::Abs(disSocketesq) / 100.0f;
        }

        if (BoatMesh->DoesSocketExist(RightThrusterSocket))
        {    //posição socket direito em relação ao pivot
            float disSocketdir = BoatMesh->GetSocketTransform(RightThrusterSocket, RTS_Actor).GetLocation().Y;
            RightDistance = FMath::Abs(disSocketdir) / 100.0f;
        }
    }
    // para usar no fossen depois
    Thrust_X = LeftMotor_Force + RightMotor_Force;

    // O motor esquerdo empurra o barco para a direita (Torque Positivo)
    // O motor direito empurra o barco para a esquerda (Torque Negativo)
    Torque_N = (LeftMotor_Force * LeftDistance) - (RightMotor_Force * RightDistance);

    Thrust_Y = 0.0f;
}
//converte corrente para forca atraves da curva blueRobotics
float AMyPawn::convertecorrente(float Corrente)
{
    if (Corrente == 0.0f) return 0.0f;

    float Forca_kgf = 0.0f;
    if (Corrente > 0.0f)
    {Forca_kgf = 0.012f * (Corrente * Corrente) + 0.15f * Corrente;}
    else
    {float AbsC = FMath::Abs(Corrente);
     Forca_kgf = -(0.01f * (AbsC * AbsC) + 0.12f * AbsC);}
    return Forca_kgf * 9.81f;
}
void AMyPawn::atualizarsensores()
{
    //se ativarmos o simular falha os sensores deixam de funcionar e ficam na posição que estavam
    if (simulacaoFalha) return;

    FVector posicaoreal_GPSf = GetActorLocation(); // Posição de reserva pivot
    FVector posicaoreal_GPSt = GetActorLocation();  // Posição de reserva pivot

    // se a mesh exitir usar a localização dos sockets do GPS
    if (BoatMesh)
    {
        if (BoatMesh->DoesSocketExist(nomeSocket_GPSfrente))
        {
            posicaoreal_GPSf = BoatMesh->GetSocketLocation(nomeSocket_GPSfrente);
        }

        if (BoatMesh->DoesSocketExist(nomeSocket_GPStras))
        {
            posicaoreal_GPSt = BoatMesh->GetSocketLocation(nomeSocket_GPStras);
        }
    }

    // criar ruido para o gps front
    FVector direcaoaleatoria1 = FMath::VRand();
    float distanciaerro1 = FMath::RandRange(0.0f, variacaoruido_GPS);
    
    FVector ruidofrente = direcaoaleatoria1 * distanciaerro1;
    ruidofrente.Z *= 0.1f; // Achatar o erro vertical

    //criar ruido para o gps back
    FVector direcaoaleatoria2 = FMath::VRand();
    float distanciaerro2 = FMath::RandRange(0.0f, variacaoruido_GPS);
    
    FVector ruidotras = direcaoaleatoria2 * distanciaerro2;
    ruidotras.Z *= 0.1f; // Achatar o erro vertical

    // posições finais (Posição real + ruido)
    GPSfrente_pos = posicaoreal_GPSf + ruidofrente;
    GPStras_pos = posicaoreal_GPSt + ruidotras;

    //SENSOR AHRS
    FRotator posicaoreal_AHRS = GetActorRotation(); // Posição de reserva

    // Se a mesh e o socket existirem vai buscar a rotacao do socket AHRS
    if (BoatMesh) {
        if (BoatMesh->DoesSocketExist(nomeSocket_AHRS))
        {
            posicaoreal_AHRS = BoatMesh->GetSocketRotation(nomeSocket_AHRS);
        }
    }
    // ruido para os 3 eixos de rotacao
    FRotator ruidoAHRS = FRotator(
        FMath::RandRange(-variacaoruido_AHRS, variacaoruido_AHRS), // Pitch
        FMath::RandRange(-variacaoruido_AHRS, variacaoruido_AHRS), // Yaw
        FMath::RandRange(-variacaoruido_AHRS, variacaoruido_AHRS)  //Roll
    );

    // rotação final (Rotação Real + ruido)
    rotacao_AHRS = posicaoreal_AHRS + ruidoAHRS;

    // CÂMERA
    if (BoatMesh) {
       if(BoatMesh->DoesSocketExist(nomeSocket_CAM)){
            posicao_CAM = BoatMesh->GetSocketLocation(nomeSocket_CAM);
            rotacao_CAM = BoatMesh->GetSocketRotation(nomeSocket_CAM);
       }
    }
    else
    {
        posicao_CAM = GetActorLocation(); // caso não cosniga encontrar o socket-loc pivot
        rotacao_CAM = GetActorRotation(); // caso não consiga encontrar o socket-rot pivot
    }
}
void AMyPawn::SimularLidar2D()
{
    int numero_raios = 36;
    if (BoatMesh) {
        if (BoatMesh->DoesSocketExist(nomeSocket_LiDAR)) {
            posicao_LiDAR = BoatMesh->GetSocketLocation(nomeSocket_LiDAR);
            rotacao_LiDAR = BoatMesh->GetSocketRotation(nomeSocket_LiDAR);
        }
    }
    else
    {
        posicao_LiDAR = GetActorLocation(); // caso não cosniga encontrar o socket-loc pivot
        rotacao_LiDAR = GetActorRotation(); // caso não consiga encontrar o socket-rot pivot
    }

    // 2. O Loop que dispara os lasers à volta do barco
    for (int i = 0; i < numero_raios; i++)
    {
        // ângulo para cada feixe de luz
        float AnguloAtual = (AnguloTotal / numero_raios) * i;

        // Juntar a rotação do barco com o ângulo do laser
        FRotator rotacaofeixe = rotacao_LiDAR;
        rotacaofeixe.Yaw = rotacaofeixe.Yaw + AnguloAtual;

        //direção do laser
        FVector Direcaofeixe = rotacaofeixe.Vector();
        //alcance a partir do socker/pivot
        FVector alcance_direcao = Direcaofeixe * DistanciaMaxima;

        // alcance a partir do referencial mundo
        FVector FimLaser = posicao_LiDAR + (alcance_direcao);

        //onde será guardado as informações sobre o impacto
        FHitResult ResultadoDoImpacto;

        // definir os parametros de colisao dos feixes
        FCollisionQueryParams ParametrosColisao;
        ParametrosColisao.AddIgnoredActor(this);//fazar os feixes ignorarem o barco

        UWorld* mundo = GetWorld();

        // variavel que indicará se bateu ou nao, inicializada com falso
        bool colidir = false;

        // se o mundo existir- se o ponteiro n for nulo
        if (mundo != nullptr)
        {   //tracar a linha de acordo com os critérios impostos
            colidir = mundo->LineTraceSingleByChannel(
                ResultadoDoImpacto,
                posicao_LiDAR,
                FimLaser,
                ECC_Visibility,
                ParametrosColisao
            );
        }

        // se houve colisão
        if (colidir == true)
        {
            // no unreal a distancia é dada em cm
            float Distanciacm = ResultadoDoImpacto.Distance;

            // passar de cm para metros
            float Distanciam = Distanciacm / 100.0f;

            // guarda distância  no array
            LeiturasLidar[i] = Distanciam;
        }
        else
        {   //se o feixe não colidiu com nada irá alcancar a distância máxima
            float Distanciam = DistanciaMaxima / 100.0f;

            LeiturasLidar[i] = Distanciam;
        }
    }
}
TArray<float> AMyPawn::FleiturasLidar()
{
     // array não fixo vazio
     TArray<float> ArrayParaOBlueprint;

     // copiar as distancias dos 36 feixes
     for (int i = 0; i < 36; i++)
     {
         ArrayParaOBlueprint.Add(LeiturasLidar[i]);
     }

     // para depois usar no blueprint
     return ArrayParaOBlueprint;
}
