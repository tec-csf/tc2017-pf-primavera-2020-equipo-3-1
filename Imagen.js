var i=0;
var pos=0;
var to_show = document.getElementById('to_show');
var imagenes = 

['Imagenes/azul.jpg','Imagenes/newlogo.jpg','Imagenes/azul.jpg','Imagenes/newlogo.jpg'];



function dibujar(pos ){
    //Cada vez que se llama esta funcion, vamos mostrando la imagen de la posicion que nos pasa.
    to_show.src = imagenes[pos-1];
    }



var to = setInterval(function(){
    pos++;
    dibujar(pos);
  //SI la variable que vamos incrementando es igual que la longitud de nuestro array, limpiamos el intervalo.
  if(pos == imagenes.length){
    clearInterval(to);
  }
},2000)