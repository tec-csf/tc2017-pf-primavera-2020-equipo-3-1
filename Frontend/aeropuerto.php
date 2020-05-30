<?php	
    $NT = $_GET['Terminales'];
    $NB =  $_GET['bandas'];
    $Capacidad = $_GET['capacidad'];
    $file = fopen("Valores.txt","w");
     fwrite($file,$NT);
     fwrite($file,"\n");
     fwrite($file,$NB);
     fwrite($file,"\n");
     fwrite($file,$Capacidad);
     fclose($file);
     header("Location: Aeropuerto.html");
?>
