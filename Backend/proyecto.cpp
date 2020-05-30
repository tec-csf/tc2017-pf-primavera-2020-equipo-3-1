  
/// -*- lsst-c++ -*-

/*
 * This file is part of tc2017-pf-primavera-2020-equipo-3-1
 *
 * Developed for Algorithms Design and Analysis Class.
 * See the README file at the repository's root for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,
 *
 * This program is distributed in the hope that it will help
 * its creators develop further understanding in nodes algorithms and parallelism implementation.
 */

#include <omp.h>
#include <tuple>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <stdlib.h>
#include <time.h>
#include <chrono>  
#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>

using namespace std;

/**
 * La estructura Persona contemplará a cada una de las personas siendo simuladas
 * en tránsito entre terminales. Cuenta con una termina destino y la banda asignada 
 * para su movimiento.
 */ 
struct Persona
{
    int terminalDestino;
    int banda;
};

/**
 * La estructura Banda contemplará a cada una de las bandas que conformen el sistema
 * generado cada vez que el programa proyecto.cpp sea ejecutado. Cada banda cuenta con
 * una terminal origen, terminal destino, estado (0 es aagada, 1 prendida y -2 volteada),
 * un atributo tiempo que indicará cuando sacar a una persona que esté sobre ella y una 
 * capacidad máxima.
 */ 
struct Banda
{
    int origen;
    int destino;
    int estado;
    double tiempo;
    int capacidad;
};

/**
 * La estructura Terminal contemplará a cada una de las terminales que  conformen el sistema
 * generado cada vez que el programa proyecto.cpp sea ejecutado. Cada terminal cuenta con un
 * Identificador, un número de bandas, un vector que contendrá a todas las bandas que salgan
 * de ella, dos vectores que servirán para definir las probabilidades de asignación de bandas 
 * a las personas, un atributo de llegadas (cuantas personas llegan en promedio a ella), un
 * atributo servicio (capacidad de cada una de las bandas), un atributo de capacidad máxima
 * de la terminal, una cola doblemente terminada (donde estarán las personas que aún no reciben
 * su banda), una cola para aquellas personas en tránsito y una cola para personas que lleguen
 * de otras terminales.
 */ 
struct Terminal
{
    string ID;
    int numeroBandas; 
    vector<Banda> bandas;
    vector <int> probabsNumero;
    vector <double> probabsFracciones;
    int llegadas; //ALPHA
    int servicio; //MU
    double capacidadMaxima; //LAMBDA
    int bandasSalida;
    deque <Persona> personasSalidas;
    queue <Persona> enTransito;
    queue< Persona> agregaraTerminal;
};

vector<Terminal> listaTerminales; //Vector Global que contiene a todas las terminales del sistema
vector<int> alphas; //Vector global que contiene el flujo promedio que entra a cada terminal
ofstream probabilidades; //Archivo de escritura en donde se escribe las probabilidades de asignar a una persona a una banda desde cualquier terminal.
int servicio; //Variable global que indica la capacidad máxima de las bandas del sistema generado por el programa.
//ofstream asignaciones;


/**
* Basado en la distribución de bandas en todo el sistema de terminales, se puede calcular una capacidad Máxima de personas por terminal para evitar saturaciones
* @param numeroTerminales, el cual representa el número de terminales que el sistema en ese momento tiene.
* @note Se utiliza un programa auxiliar (Lamdas.py) para calclar bajo los principios de Redes de Jackson, las capacidades máximas por terminal. Estos resultados
* se guardan en un archivo auxiliar (Lamdas.txt).
* @note Para este proceso, primero se debe conocer el número de bandas que salen entre las distintas terminales y Salidas del sistema. Posteriormente, se realiza una
* matriz de numeroTerminales * numeroTerminales y se resuelve un sistema de ecuaciones que proporciona el dato previamente mencionado.
*/
void CalculoLamdas(int numeroTerminales)
{
    probabilidades.open("Probabilidades.txt");
    probabilidades << to_string(numeroTerminales) + "\n";
    for (int i = 0; i < numeroTerminales; i++)
    {
        probabilidades << "-"+to_string(alphas[i]) + "\n";
    }
    ifstream lambdas;
    //CALCULO DE LAMBDAS PARA CADA POSIBLE RUTA DE CADA TERMINAL
    for(int x = 0; x < listaTerminales.size(); x++)
    {       
        for (int i = 0; i < numeroTerminales+1; i++)
        {
            int sumaBandas = 0;
            for (int j = 0; j < numeroTerminales + 1; j++)
            {
                sumaBandas = sumaBandas + listaTerminales[x].probabsNumero[j];
            }
            listaTerminales[x].probabsFracciones[i] = double(listaTerminales[x].probabsNumero[i]) / double(sumaBandas); //Probabilidades por banda por terminal
        }   
    }
    for(int x = 0; x < listaTerminales.size(); x++)
    {
        for(int i = 0; i < listaTerminales.size(); i++)
        {
            probabilidades << to_string(listaTerminales[i].probabsFracciones[x]) + "\n";
        }
    }
    probabilidades.close();
    lambdas.open("Lamdas.txt");
    system("python3 Lamdas.py");

    int x = 0;
    string line;
    while (getline(lambdas, line))
    {
        double lambda = stoi(line);
        listaTerminales[x].capacidadMaxima= lambda; //Se asigna capacidad máxima a terminales después de haber corrido el archivo Lamdas.py
        x++;
    }
    lambdas.close();   
}

/**
* Función ecargada de tomar el conjunto de bandas por terminal y plasmarlas en represerntación gràfica utilizando Graphviz (Boost)
* @param numeroTerminales, el cual representa el número de terminales que el sistema en ese momento tiene.
* @note Se utiliza el archivo Visualización.txt para escribir todas las conexiones entre terminales y Salidas.
*/
void RepresentacionVisual(int numeroTerminales)
{
    ofstream myfile;
    myfile.open("Visualizacion.txt");
    myfile << "digraph G {\n";
    for (int i = 0; i < numeroTerminales; i++)
    {
        for(int j = 0; j<listaTerminales[i].bandas.size(); j++)
        {
           if(listaTerminales[i].bandas[j].estado==1 && listaTerminales[i].bandas[j].destino!=-1)
           {
                myfile << "edge[color = black];\n";
                int restante = servicio - listaTerminales[i].bandas[j].capacidad;
                myfile << "Terminal"<<listaTerminales[i].bandas[j].origen+1<<" -> "<<"Terminal"<<listaTerminales[i].bandas[j].destino+1<<" [style=bold,label=\"Cp. " << restante  << "\"]"<<";\n";
           }
           else if(listaTerminales[i].bandas[j].destino==-1)
           {
                myfile << "edge[color = black];\n";
                myfile << "Terminal"<<listaTerminales[i].bandas[j].origen+1<<" -> "<<"SALIDA"<<";\n";
           }
        }
    }
    myfile << "}\n";
    myfile.close();
    system("dot -Tpng Visualizacion.txt -o demo_dot.png");
}

/**
* Función encargada de distribuir a la gente en las diferentes bandas conectando las diferentes terminales del sistema.
* @param numeroTerminales, el cual representa el número de terminales que el sistema en ese momento tiene.
* @param idTerminal, el cual reprsenta el numero de la terminal con el que un hilo en particular trabaja.
* @note Se utiliza el planteamiento de Redes de Jackson aborado en la funcion CalculoLamdas()
*/
void FuncionamientoTerminales(int idTerminal, int nTerminales)
{
    vector <tuple<double, double>> indicadores; //Rangos para definir a que banda va una persona
    double limiteInferior = 0.f; //Rangos
    string asign;
    int cont=0;
    double limiteSuperior = 0.f; //Rangos
    for (int index = 0; index < listaTerminales.size()+1; index++) //FOR para deifinición de rangos con base en probabilidad.
    {
        if ((listaTerminales[idTerminal].probabsFracciones[index]) == 0)
        {
            indicadores.push_back(make_tuple(-1.f, -1.f));
        }
        else
        {
            limiteSuperior = limiteSuperior + (listaTerminales[idTerminal].probabsFracciones[index]) * 100;
            indicadores.push_back(make_tuple(limiteInferior, limiteSuperior));
            limiteInferior = limiteSuperior + .1f;
        }   
    }
    while(1)
    {
        freopen("Asignaciones.txt", "a", stdout);
        if(!listaTerminales[idTerminal].agregaraTerminal.empty()) //Definición de personas que llegan a terminal y esperan banda
        {   
            while(!listaTerminales[idTerminal].agregaraTerminal.empty()) 
            {
                #pragma omp critical
                {
                    int mike = 0;
                    int numeroRandom = rand() % 100;
                    for(int i=0; i<listaTerminales.size()+1 && mike==0;i++)
                    {
                        if(numeroRandom>=get<0>(indicadores[i])&& numeroRandom<=get<1>(indicadores[i])) //Designación de terminal destino para personas previamente en el sistema
                        {  
                            listaTerminales[idTerminal].agregaraTerminal.front().terminalDestino = i;
                            listaTerminales[idTerminal].personasSalidas.push_back(listaTerminales[idTerminal].agregaraTerminal.front());
                            listaTerminales[idTerminal].agregaraTerminal.pop();
                            mike=1;
            
                        }
                    }
                }
            }
        }
        while(listaTerminales[idTerminal].personasSalidas.size()<listaTerminales[idTerminal].capacidadMaxima)
        {
            Persona persona;
            int mike = 0;
            int numeroRandom = rand() % 100;
            for(int i=0; i<listaTerminales.size()+1 && mike==0;i++) //Designación de terminal destino para personas que recién entran al sistema
            {
                if(numeroRandom>=get<0>(indicadores[i])&& numeroRandom<=get<1>(indicadores[i]))
                {
                    persona.terminalDestino = i;
                    listaTerminales[idTerminal].personasSalidas.push_back(persona);
                    mike=1;
                }
            }
        }
        auto start = std::chrono::high_resolution_clock::now();
        while(listaTerminales[idTerminal].personasSalidas.size()>0) //Asignación de bandas a personas en terminal
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
            if(int_ms.count() >=2&&!listaTerminales[idTerminal].enTransito.empty()) //Simulaciòn de tiempo de tránsito y salida de persona en banda
            {
                start=end;
                #pragma omp critical
                {
                    listaTerminales[idTerminal].bandas.at(listaTerminales[idTerminal].enTransito.front().banda).capacidad+=1; 
                    listaTerminales[idTerminal].agregaraTerminal.push(listaTerminales[idTerminal].enTransito.front());
                    listaTerminales[idTerminal].enTransito.pop();
                }
               
            }
            if(listaTerminales[idTerminal].personasSalidas.front().terminalDestino!=nTerminales) //Personas que NO VAN A LA SALIDA
            {
                int banderaAsignacion = 1;
                for(int j = 0; j<(listaTerminales[idTerminal].bandas.size()) && banderaAsignacion==1; j++) //Recorrido de bandas excluyendo las de las Salidas
                {
                    #pragma omp critical 
                    {
                        if(listaTerminales[idTerminal].bandas.at(j).destino==listaTerminales[idTerminal].personasSalidas.front().terminalDestino && listaTerminales[idTerminal].bandas.at(j).estado==1 && listaTerminales[idTerminal].bandas.at(j).capacidad>0) //Asignación de banda en caso de que esté prendida y con capacidad.
                        {      
                            listaTerminales[idTerminal].bandas.at(j).capacidad -= 1;
                            listaTerminales[idTerminal].personasSalidas.front().banda = j;
                            asign = "Terminal"+to_string(idTerminal+1)+" -> "+"Terminal"+to_string(listaTerminales[idTerminal].bandas.at(j).destino+1)+"\n";
                            printf(asign.c_str());
                            usleep(15);
                            banderaAsignacion = 0;
                            listaTerminales[idTerminal].enTransito.push(listaTerminales[idTerminal].personasSalidas.front());
                            listaTerminales[idTerminal].personasSalidas.pop_front();
                        }
                        else if(listaTerminales[idTerminal].bandas.at(j).destino==listaTerminales[idTerminal].personasSalidas.front().terminalDestino && listaTerminales[idTerminal].bandas.at(j).estado==0) //Prende banda que pueda ser utilizada y empieza a ser utilizada.
                        {
                                
                            listaTerminales[idTerminal].bandas.at(j).estado = 1;
                            listaTerminales[idTerminal].bandas.at(j).capacidad -= 1;
                            listaTerminales[idTerminal].personasSalidas.front().banda = j;
                            asign = "Terminal"+to_string(idTerminal+1)+" -> "+"Terminal"+to_string(listaTerminales[idTerminal].bandas.at(j).destino+1)+"\n";
                            printf(asign.c_str());
                            usleep(15);
                            banderaAsignacion = 0;
                            listaTerminales[idTerminal].enTransito.push(listaTerminales[idTerminal].personasSalidas.front());
                            listaTerminales[idTerminal].personasSalidas.pop_front();
                            //asignaciones<<"Terminaljej"<<endl;
                            //asignaciones.close();
                        }
                    }
                    //cout<<"Terminal"<<endl;
                    //asignaciones<<"Terminal"<<endl;
                    //asignaciones.close(); 
                }   
                if(banderaAsignacion==1) //No se encontraron bandas y se busca un cambio de dirección con base en la terminal destino
                { 
                    for(int j = 0; j<listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.size()&&banderaAsignacion==1; j++) //Recorrido de bandas de terminal destino para ver si se voltea alguna
                    {   
                        #pragma omp critical
                        {
                            if(listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.at(j).destino==idTerminal && listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.at(j).estado==0) //Encontrar banda apagada que pueda llegar a ser utilizada.
                            {
                                listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.at(j).estado=1;
                                listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.at(j).origen=idTerminal;
                                listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.at(j).destino=listaTerminales[idTerminal].personasSalidas.front().terminalDestino;
                                listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.at(j).capacidad -= 1;
                                listaTerminales[idTerminal].bandas.push_back(listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas[j]);
                                listaTerminales[listaTerminales[idTerminal].personasSalidas.front().terminalDestino].bandas.at(j).estado=-2;
                                listaTerminales[idTerminal].personasSalidas.front().banda = listaTerminales[idTerminal].bandas.size()-1;
                                banderaAsignacion = 0;
                                listaTerminales[idTerminal].enTransito.push(listaTerminales[idTerminal].personasSalidas.front());
                                listaTerminales[idTerminal].personasSalidas.pop_front();
                                //asignaciones << "Persona sale de la Terminal"<<(idTerminal+1)<<" hacia Terminal"<<(listaTerminales[idTerminal].bandas.at(j).destino+1)<<" en la banda"<<listaTerminales[idTerminal].personasSalidas.front().banda<<".\n";
                            }       
                        }
                    }
                    if(banderaAsignacion==1) //No se encontró ninguna banda y persona es puesta en cola de espera.
                    {
                        #pragma omp critical
                        {
                            listaTerminales[idTerminal].agregaraTerminal.push(listaTerminales[idTerminal].enTransito.front());
                            listaTerminales[idTerminal].personasSalidas.pop_front();
                        }
                    }
                }
            }
            else
            {
                #pragma omp critical  //Persona se dirige a la salida
                {
                    listaTerminales[idTerminal].personasSalidas.front().banda = -10;
                    //freopen("Asignaciones.txt","w",stdout);
                    //cout<<"Persona sale de Terminal"<<idTerminal<<" a Salida"<<"\n";
                    asign = "Termimal"+to_string(idTerminal+1)+" -> "+"SALIDA"+"\n";
                    printf(asign.c_str());
                    usleep(15);
                    listaTerminales[idTerminal].personasSalidas.pop_front();
                } 
            }
            
            //#pragma omp critical
            //{
                /*asignaciones.open("Asignaciones.txt");
                asignaciones << asign;
                asignaciones.close();
                usleep(300000);*/
                #pragma omp master //Generación de esquema visual en el que se ve actualizando el transporte de personas en bandas del sistema.
                {
                    RepresentacionVisual(nTerminales);
                    usleep(300000);
                }
            //}
        } 
        while(!listaTerminales[idTerminal].enTransito.empty()) //Ultimas personas que se quedaron en sistema y en tránsito. Son agregadas a queue de Terminal destino.
        {
            #pragma omp critical 
            {
                listaTerminales[idTerminal].bandas.at(listaTerminales[idTerminal].enTransito.front().banda).capacidad+=1; 
                listaTerminales[idTerminal].agregaraTerminal.push(listaTerminales[idTerminal].enTransito.front());
                listaTerminales[idTerminal].enTransito.pop();
            }
        }
        int z;
        #pragma omp parallel for private(z)
        for(z=0;z<listaTerminales[idTerminal].bandas.size();z++) //Se apagan bandas para poder adaptarlas para la siguiente oleada de personas.
        {
            if(listaTerminales[idTerminal].bandas.at(z).estado!=2) //Bandas que fueron volteadas previamente se quedan en mismo estado para que no sean tomadas en cuenta y puedan llegar a ser volteadas de nuevo.
            {
                listaTerminales[idTerminal].bandas.at(z).estado=0;
            }     
        }
    fclose (stdout);
    }
}

int main()
{
    ifstream variablesUsuario;
    variablesUsuario.open("Valores.txt");
    string lineV;
    int indeexx=0;
    int numeroTerminales;
    int numeroMaxBandas;
    while(getline(variablesUsuario, lineV))
    {
        if(indeexx == 0)
        {
            numeroTerminales = stoi(lineV);
            indeexx++;
        }
        else if(indeexx == 1)
        {
            numeroMaxBandas = stoi(lineV);
            indeexx++;
        }
        else
        {
            servicio = stoi(lineV);
        }  
    }
    variablesUsuario.close();
    freopen("Asignaciones.txt", "w", stdout);
    fclose (stdout);
    ofstream myfile;
    variablesUsuario.open("Variables.txt");
    myfile.open("Visualizacion.txt");
    myfile << "digraph G {\n";
    srand(time(NULL));
    for (int j = 0; j < numeroTerminales + 1; j++)
    {
        alphas.push_back(0); //Reserva de memoria para vector de alphas
    }
    for (int i = 0; i < numeroTerminales; i++)
    {
        Terminal terminal;
        terminal.numeroBandas = rand() % numeroMaxBandas + 1;
        alphas[i] = rand() % 20 + 1;
        terminal.servicio = servicio;
        listaTerminales.push_back(terminal);
        for (int j = 0; j < numeroTerminales + 1; j++)
        {
            listaTerminales[i].probabsNumero.push_back(0); //Reserva de memoria para vector de probabilidades
            listaTerminales[i].probabsFracciones.push_back(0); //Reserva de memoria para vector de probabilidades
        }
    }
    for (int i = 0; i < numeroTerminales; i++) //Generacion Original de Sistema y Proyección del mismo en esquema 
    {
        int estado = 0;
        for(int j = 0; j<listaTerminales[i].numeroBandas; j++)
        {
            int destino;
            do
            {
                destino = rand() % numeroTerminales;
            } while (destino == (i));
           
            Banda banda;
            banda.capacidad = servicio;
            banda.destino = destino;
            banda.estado = estado;
            banda.origen = i;
            banda.tiempo = 0;
            listaTerminales[i].bandas.push_back(banda);
            myfile << "edge[color = red];\n";
            myfile << "Terminal"<< banda.origen+1<<" -> "<<"Terminal"<<banda.destino+1<<";\n";
            listaTerminales[i].probabsNumero[destino] = listaTerminales[i].probabsNumero[destino] + 1;
            Banda bandaContraria;
            bandaContraria.capacidad = servicio;
            bandaContraria.destino = i;
            bandaContraria.estado = estado;
            bandaContraria.origen = destino;
            bandaContraria.tiempo = 0;
            listaTerminales[destino].bandas.push_back(bandaContraria);
            myfile << "edge[color = black];\n";
            myfile << "Terminal"<< banda.destino+1<<" -> "<<"Terminal"<<banda.origen+1<<";\n";
            listaTerminales[destino].probabsNumero[i]++;
        }
        listaTerminales[i].bandasSalida = 1;
        Banda salida;
        salida.capacidad = 1000;
        salida.destino = -1;
        salida.estado = 1;
        salida.origen = i;
        salida.tiempo = 0;
        listaTerminales[i].bandas.push_back(salida);
        myfile << "edge[color = black   ];\n";
        myfile << "Terminal" << (i + 1) << " -> " << "SALIDA" <<";\n";
        listaTerminales[i].probabsNumero[numeroTerminales]++;
    }
    myfile << "}\n";
    myfile.close();
    system("dot -Tpng Visualizacion.txt -o demo_dot.png"); //Graphviz para generación de esquema
    CalculoLamdas(numeroTerminales); //Calculo de Lamdas
    sleep(3);
    #pragma omp parallel num_threads(numeroTerminales) //Se correo de manera paralela la asignación de bandas y personas para todas las terminales
    {
       int threadId = omp_get_thread_num();
       FuncionamientoTerminales(threadId, numeroTerminales); 
    }
    //asignaciones.close();
    return 0;
}