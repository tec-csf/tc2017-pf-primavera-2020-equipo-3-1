import numpy as np
g=open('Probabilidades.txt',"r")
lines=g.readlines()
g.close()
lista = [ ]
cont=0
contelem=0
contex=-1
lin0 = 1
for i in lines:
    if contelem==0 and cont>lin0:
        contex=contex+1
    if cont==0:
        lin0=float(i)
    else:
        w=float(i)
        if contex==contelem and w==0.0:
            w=-1
        contelem=contelem+1
        lista.append(w)
        if contelem==lin0:
            contelem=0
    cont=cont+1   
contvector=0
contaux=0
sistemaEcua=[ ]
for j in range(int(lin0+1)):
    sistemaEcua.append([])
    aux=int(j*(lin0))
    for n in range(int(lin0)):
      sistemaEcua[j].append(lista[n+aux])
listaA = [ ] 
listaB= [ ]   
for mike in range(2):
    if mike ==0:
        listaB=sistemaEcua[mike]
    else:
        for k in range(int(lin0)):
            if contaux==0:
                listaA.append(sistemaEcua[contaux+1],)
            else:
                if contaux==(lin0):
                    listaA= listaA+sistemaEcua[contaux+1]
                else:
                    listaA.append(sistemaEcua[contaux+1],)
            contaux=contaux+1   
    
a = np.array(listaA)
b= np.array(listaB)
X2 = np.linalg.solve(a, b)   
           
f=open('Lamdas.txt','w')
for j in X2:
    y=round(j, 2)
    if y<0:
        y=y*-1
    y=y*15
    if(y>1000):
        y=y/2
    x=str(y)
    f.write(x+ '\n')
f.close()            
           
