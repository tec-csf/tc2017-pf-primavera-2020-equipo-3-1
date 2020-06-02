# Proyecto Final: Cintas Transportadoras http://35.237.49.79/

---

##### Integrantes:
1. Esteban Manrique de Lara Sirvent - A01027077 - Campus Santa Fe
2. Miguel Ángel Hernández Maldonado - A01022398 - Campus Santa Fe
3. Luis Emilio Alcántara Guzmán - A01027304 - Campus Santa Fe
## 1. Aspectos generales

Las orientaciones de la tarea se encuentran disponibles en la plataforma **Canvas**.

Este documento es una guía sobre qué información debe entregar como parte del proyecto, qué requerimientos técnicos debe cumplir y la estructura que debe seguir para organizar su entrega.

### 1.1 Requerimientos técnicos

A continuación se mencionan los requerimientos técnicos mínimos del proyecto, favor de tenerlos presente para que cumpla con todos.

* El equipo tiene la libertad de elegir las tecnologías de desarrollo a utilizar en el proyecto, sin embargo, debe tener presente que la solución final se deberá ejecutar en una de las siguientes plataformas en la nube: [Google Cloud Platform](https://cloud.google.com/?hl=es), [Amazon Web Services](https://aws.amazon.com/) o [Microsoft Azure](https://azure.microsoft.com/es-mx/).
* El proyecto deberá utilizar una interfaz Web.
* La arquitectura deberá estar separada claramente por capas (*frontend*, *backend*, *API RESTful*, datos y almacenamiento) según se necesite.
* Todo el código, *datasets* y la documentación del proyecto debe alojarse en este repositorio de GitHub. Favor de mantener la estructura de carpetas propuesta.

### 1.2 Estructura del repositorio

El proyecto debe seguir la siguiente estructura de carpetas, la cual generamos por usted:
```
- / 			        # Raíz de todo el proyecto
    - README.md			# Archivo con los datos del proyecto (este archivo)
    - frontend			# Carpeta con la solución del frontend (Web app)
    - backend			  # Carpeta con la solución del backend (CMS)
    - api			      # Carpeta con la solución de la API
    - datasets		  # Carpeta con los datasets y recursos utilizados (csv, json, audio, videos, entre otros)
    - dbs			      # Carpeta con los modelos, catálogos y scripts necesarios para generar las bases de datos
    - docs			    # Carpeta con la documentación del proyecto
```

### 1.3 Documentación  del proyecto

Como parte de la entrega final del proyecto, se debe incluir la siguiente información:

* Descripción del problema a resolver.
* Diagrama con la arquitectura de la solución.
* Descripción de cada uno de los componentes de la solución.
* Guía de configuración, instalación y despliegue de la solución en la plataforma en la nube seleccionada.
* Documentación de la API. Puede ver un ejemplo en [Swagger](https://swagger.io/). 
* El código debe estar documentado siguiendo los estándares definidos para el lenguaje de programación seleccionado.

## 2. Descripción del proyecto

*Se plantea un sistema basado en flujos constantes con base en redes de Jackson y Distribución de Poisson para manejar la asignación de personas dentro de un sistema de aeropuertos a bandas de transporte que les permita moverse entre terminales o salir del sistema cuando lo requieran. El sistema se compone de un número N de terminales, cada una con un número B de bandas, las cuales pueden tener direcciones Tn -> Tm o Tn <- Tm. Estas bandas deben estar apagadas hasta que se les requiera. Deben tener el mayor nivel de concurrencia posible y poder ser volteadas en caso de que el flujo de una terminal lo requiera y siempre y cuando no tenga personas siendo transportadas entre terminales. Se plantea una solución en donde un programa desarrollado en C++ desarrolle todo la creación de terminales, bandas y personas con sus respectivas asignaciones; un archivo auxiliar en Python servirá para desarrollar la parte matemáticas y estadística del programa. Lo anterior lo consideramos como un problema NP (es aleatorio el comportamiento del problema), con solución orientada hacia programación dinámica y con complejidad espacial lineal.*

## 3. Solución

A continuación aparecen descritos los diferentes elementos que forman parte de la solución del proyecto.

### 3.1 Arquitectura de la solución

<img src= "https://github.com/tec-csf/tc2017-pf-primavera-2020-equipo-3-1/blob/master/Datasets/Architectura.png">

*Se plantea una solución que esté hosteada en una máquina virtual generadas por los servicios de nube de Google. Esta solución cuenta con una interfaz web en la que el usuario puede leer y comprender el problema que se nos pidió resolver a profundidad (se presentan al lector los requerimientos que el sistema debe cumplir, lo anterior con ayuda de una descarga de pdf). En esta misma interfaz, el usuario puede asignar valores que el/ella decida en torno al número de terminales y bandas y capacidad de las mismas en un sistema aleatorio a ser presentado en la interfaz web. Cuando el usuario empiece la simulación, estos valores son guardados en un arcvhivo .txt ("Valores.txt") para luego ser asignados a variables en nuestro programa de c++, el cual controla el funcionamiento del sistema de bandas y terminales. Para la parte matemática del proyecto (Redes de Jackson), se ejecutan un archivo auxiliar (Lamdas,py) y genera un archivo .txt (Lamdas.txt), en donde ser guardan las capacidades máximas de las terminales del sistema para evitar saturación; lo anterior es posible a que el archivo Lamdas.py obtiene datos de probabilidad, número de terminales y bandas saliendo de cada una de ellas de un archivo .txt (Probabilidades.txt). Una vez realizado lo anterior, el c++ genera constantemente representaciones visuales (con la ayuda de la librería Graphviz y con soporte de un archivo .txt -Visualizacion.txt-) a la par de que va realizando las asignaciones con base en el principio matemático previamente mencionado. Asimismo, se irán guardando las asignaciones en un archivo .txt (Asignaciones.txt). Las asignaciones y el esquema del sistema se irán actualizando periódicamente en la interfaz web.*

### 3.2 Descripción de los componentes

*Frontend: Se tiene una interfaz web desarrollada con HTML y CSS en el cual se proyecta los requerimientos del proyecto y sirve como puente para la interacción de los usuarios con los programas que ejecutan y resuelven el sistema para cada caso planteado. En ella, los usuarios pueden personalizar el sistema a ser ejecutado (# de terminales, bandas, capacidad de las mismas), al igual que visualizar los resultados del sistema, en una forma gráfica (imagen del sistema trabajando) y escrita (asignaciones hacia terminales). Es fundamental para que la arquitectura de 3 capas pueda brindar información continua a la parte de backend para una ejecución personlizada por parte del usuario.+

*Backend: Se tienen 2 secciones principales del Backend. La primera desarrollada en Python por la facilidad que este lenguaje brinda para cuestiones matemáticas; siendo un proyecto muy orientado al análisis de un sistema de redes (Jackson), se desarrolló que la obtención de valores fundamentales para este sistema de colas fueran obtenidos mediante la ejecución del archivo Lamdas.py, además se tienen dos (<script>) de Javascript que ayudan a recargar la imagen del mapa de bandas y la información de las personas que entran y salen de las terminales dentro de la página web. Por otro lado, el funcionamiento de las terminales se planteó en c++, por ser un programa en el que del manejo de estructuras y apuntadores facilita planteamientos que se acercan más al planteamiento de objetos; con este script, se diseñó la manera en que las asignaciones fueran realizadas de la manera más optima pero siempre siguiendo los principios de Redes de Jackson. El backend es la piedra angular del proyecto; fundamental para la arquitectura.*

*Datos: Diferentes archivos .txt permiten la comunicación entre el Frontend y los diferntes componentes del Backend (entre ellos mismos también). El intercambio de información y datos entre la interfaz web y el programa c++ y a su vez, el cambio y proporcionamiento entre el script de resolución matemática (Python) y el de resolución y visualización del proyecto (c++), son permitidos por esta capa. Sin ella, no se podrían entrelazar los primeros 2 componentes mencionados en este apartado. Adicionalmente, el usuario no sería capaz de actualizar el sistema a ser simulado por el proyecto las veces que el/ella quisiera.*

*Hosting: Se contempló como un componente obligatorio del proyecto entregable. Permite el acceso al mismo de manera remota, lo que facilita y abarca el rango de uso y distribución de todo el sistema previamente descrito.*


### 3.3 Frontend

*El frontend presenta una descripción del problema a ser abordado por la solución desarrollada en su conjunto; plantea los elementos a ser resueltos por el backend de este proyecto y también sirve como puente entre usuario y el backend. Permite a los usuarios especificar las características del sistema de terminales y bandas. Asimismo, proyecta la simulación y las asignaciones del sistema en forma de un esquema generado por Graphviz y la impresión de un archivo .txt con los movimientos de las personas entre las Terminales y las Salidas del sistema.*

#### 3.3.1 Lenguaje de programación
*HTML*

*CSS*
#### 3.3.2 Framework
*Se construyó todo el sitio de manera independiente a algún framework previamente creado por terceros.*
#### 3.3.3 Librerías de funciones o dependencias
*No se utilizó ningún tipo de Librería de funciones o dependencias para la elaboración del Frontend de este proyecto. Se utilizó una fuente especial descargada de Google Fonts (https://fonts.googleapis.com/css2?family=Bebas+Neue&family=Fira+Sans+Condensed:wght@200&display=swap)*

### 3.4 Backend

*Se decidió realizar una implementación programa en 2 lenguajes en Backend: C++ y Python. Por un lado, se sabía que el manejo rápido, preciso y accesible de matrices sería importante, por lo que se adoptó implementar la librería numoy para lograr lo anterior. Por otro lado, el funcionamiento de terminales se asemejaba más a un programa de modelado y funcionamiento de objetos, por lo que se adoptó un lenguaje como C++ que se encargará del manejo de bandas y asignaciones de personas sobre ellas. El uso de librerías como chornos y Graphviz permitió reducir la carga de trabajo en frontend y en el mismo Backend. El uso de archivos para lectura y escritura fue clave para el funcionamiento de esta sección de la arquitectura.*

#### 3.4.1 Lenguaje de programación
*Python (Parte Matemática)*

*C++ (Funcionamiento de Terminales y asignación de bandas)*
#### 3.4.2 Framework
*No se requirió de la implementación de algún framework para el desarrollo del backend de este proyecto.* 
#### 3.4.3 Librerías de funciones o dependencias
*OpenMP -> C++; Utilizada para implementar paralelismo y computo basado en hilos independientes. Uso de omp critical, num_threads y omp master. https://www.openmp.org/wp-content/uploads/cspec20.pdf*

*Numpy -> Python; Utilizada para el manejo de matrices y resolución de sistema de ecuaciones de matriz cuadrada para obtención de capacidades máximas de terminales con base en número de bandas y probabilidades de asignación de personas moviéndose entre las terminales. Uso de numpy.array. https://claudiovz.github.io/scipy-lecture-notes-ES/intro/numpy/array_object.html*

*Graphviz y dot -> Implementado desde C++; Utilizada para la generación de la representación visual del sistema de bandas, terminales y capacidades de las mismas. Uso del comando de sistema "dot -Tps". https://www.graphviz.org/pdf/dotguide.pdf*

*Chrono -> C++; Usada para controlar el flujo de personas, impresiones y asignaciones en el sistema, así como para el manejo, volteo y apagado/prendido de las bandas del sistema de terminales. https://en.cppreference.com/w/cpp/chrono*

## 3.5 Pasos a seguir para utilizar el proyecto

*Para el la descarga del código a su computadora, realizar los siguientes pasos:*
*1. Clonar el repositorio con link https://github.com/tec-csf/tc2017-pf-primavera-2020-equipo-3-1/*

*2. Garantizar que su máquina tenga instalada las librerías de Graphviz (sudo apt-get install graphviz) y numpy (pip install numpy).*

*3. Compilar código "proyecto.cpp" con ayuda de g++.*

*4. Ejecutar código si en su computadora (realmente en ese proyecto no es muy relevante el correrlo de manera local).*

*Para utilizar la solución con interfaz web, realizar los siguientes pasos:*
*1. Acceder a la siguiente dirección IP: http://35.237.49.79/*

*2. Si desea conocer más acerca del proyecto, dar click en hipervínculo local "Cintas Transportadoras".*

*3. Colocar en los campos Número de Terminales, Número de bandas Máximas y Capacidad de bandas los valores que desee para la simulación a ser ejecutada.*

*4. Dar click en el botón "Ejecutar".*

*5. Visualziar como la solucion va proporcionando las diferentes asignaciones de bandas y los diferentes layouts de terminlaes de bandas del sistema.*

*6.Cuando se decida terminaer la simulación, dar click en botón "Terminar"; lo anterior para que el proceso en la máquina virtual no se quede corriendo indefinidamente.*
## 4. Referencias

*https://www.openmp.org/wp-content/uploads/cspec20.pdf*

*https://www.graphviz.org/pdf/dotguide.pdf*

*http://courses.washington.edu/inde411/QueueingTheoryPart5.pdf*

*http://www.cse.cuhk.edu.hk/~cslui/CSC5420/queueing_network_beamer.pdf*

*https://link.springer.com/chapter/10.1007%2F978-1-4757-5301-1_2*

*https://en.wikipedia.org/wiki/Jackson_network*

