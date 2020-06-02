<?php
    $identificador = $_GET['id'];
        if($identificador ==1)
{       
    system("sudo su");
    system("killall -9 proyecto");
    $NT = $_GET['Terminales'];
    $NB =  $_GET['bandas'];
    $Capacidad = $_GET['capacidad'];
    $file = fopen("/var/www/html/Valores.txt","w");
     fwrite($file,$NT);
     fwrite($file,"\n");
     fwrite($file,$NB);
     fwrite($file,"\n");
     fwrite($file,$Capacidad);
     fclose($file);
     system("g++ proyecto.cpp -o proyecto -fopenmp");
     system("./proyecto");
     header("Location: index.html");
}
else
{
        system("sudo su");
        system("killall -9 proyecto");
        header("Location: index.html");
}
?>
