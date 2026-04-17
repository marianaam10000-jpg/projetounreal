// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

// STRUCT - estado

USTRUCT(BlueprintType)
struct FVetor6DOF
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|estado")
    float x; // Surge 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|estado")
    float y; // Sway 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|estado")
    float z; // Heave 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|estado")
    float roll; // Rotação X

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|estado")
    float pitch; // Rotação Y

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|estado")
    float yaw; // Rotação Z

    // inicialização
    FVetor6DOF() : x(0), y(0), z(0), roll(0), pitch(0), yaw(0) {}
};

UCLASS()
class MYPROJECT6_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
    AMyPawn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    //Array fixo para dados lidar
    float LeiturasLidar[36];

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //Componentes Visuais
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* BoatMesh;
    
    // CÂMERA
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent* BoatCamera;

    // Dados câmera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fossen|Sensors|Camera")
    FVector posicao_CAM;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fossen|Sensors|Camera")
    FRotator rotacao_CAM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|Camera")
    FName nomeSocket_CAM = FName("Camera"); 

    //ESTADO BARCO

 //  velocidades atuais (u, v, w, p, q, r)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Estado")
    FVetor6DOF velocidade;

    // acelerações
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Estado")
    FVetor6DOF aceleracao;

    // Parâmetros (inicias 3DOF)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Mass = 55.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Iz = 15.95f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float AddedMass_X = 5.2815f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float AddedMass_Y = 82.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float AddedMass_N = 27.115f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Damping_X = 77.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Damping_Y = 137.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Damping_N = 46.0f;

    // Restantes parametros 6DOF
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Fluid_Density = 1025.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Gravity = 9.81f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float ZG = 1.135974f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Vehicle_Density = 950.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Radius = 127.772095f;

    // tensores inercia
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Inertia_Tensor_X = 12.4643f; // Roll

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Inertia_Tensor_Y = 18.15f;   // Pitch

    // restantes de damping
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Linear_Damping_Tensor_Z = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Angular_Damping_Tensor_X = 54.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Angular_Damping_Tensor_Y = 246.0f;

    // restantes de added mass
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Added_Mass_Linear_Z = 55.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Added_Mass_Angular_X = 2.4929f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Parameters")
    float Added_Mass_Angular_Y = 14.52f;

    // Inputs de Força
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Input")
    float Thrust_X = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Input")
    float Thrust_Y = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Input")
    float Torque_N = 0.0f;

    //Sockets dos Motores
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Input")
    FName LeftThrusterSocket = FName("LeftThruster");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Input")
    FName RightThrusterSocket = FName("RightThruster"); 
    

    // 4 pontos buoyancy-sockets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Buoyancy")
    FName SocketFrontLeft = FName("frenteesquerda");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Buoyancy")
    FName SocketFrontRight = FName("frentedireita");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Buoyancy")
    FName SocketBackLeft = FName("trasesquerda");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Buoyancy")
    FName SocketBackRight = FName("trasdireita");

    // A altura da agua- eixo z
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Buoyancy")
    float nivelaguaz = 0.0f;

    // Força de buoyancy inicializada
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Buoyancy")
    float BuoyancyCoefficient = 0.0f;

    //função que calcula a força dos motores disponivel blueprint
    UFUNCTION(BlueprintCallable, Category = "Fossen|Input")
    void motores(float frente, float lado);
    //função que converte a corrente em froça
    float convertecorrente(float Corrente);


    // Função que le e coloca ruido nos dados dos sensores
    void atualizarsensores();


    // SENSORES

    //variavel de falha
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|Settings")
    bool simulacaoFalha = false;
    
    // GPS frente
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fossen|Sensors|GPS_Front")
    FVector GPSfrente_pos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|GPS_Front")
    FName nomeSocket_GPSfrente = FName ("GPSFront");
    //GPS tras
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fossen|Sensors|GPS_Back")
    FVector GPStras_pos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|GPS_Back")
    FName nomeSocket_GPStras = FName ("GPSBack");

    // configurações GPS  - ruido
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|Settings")
    float variacaoruido_GPS = 200.0f;

   

    // AHRS
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fossen|Sensors|AHRS")
    FRotator rotacao_AHRS;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|AHRS")
    FName nomeSocket_AHRS = FName("AHRS");

    // Ruído em graus
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|Settings")
    float variacaoruido_AHRS = 1.0f;

    
    UPROPERTY(EditAnywhere, Category = "Camera")
    class UTextureRenderTarget2D* CameraRenderTarget;

    // LiDAR
    FTimerHandle LidarTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensors|LiDAR")
    FName nomeSocket_LiDAR = FName("Lidar");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fossen|Sensors|LiDAR")
    FVector posicao_LiDAR;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fossen|Sensors|LiDAR")
    FRotator rotacao_LiDAR;

    //os dados a serem enviados para ros
    UFUNCTION(BlueprintPure, Category = "Sensores|LiDAR")
    TArray<float> FleiturasLidar();//tentei usar array fixo mas deu erro
    
    //configuracoes do lidar

    // lidar
    void SimularLidar2D();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensores|LiDAR")
    float DistanciaMaxima = 2000.0f; // Alcance em cm

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fossen|Sensores|LiDAR")
    float AnguloTotal = 360.0f; // Campo de visão do sensor
};
