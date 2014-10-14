#!/bin/sh

#Autor: Stefan Wehner

if [ $# != 1 ] 
then
    echo -e "USAGE: $0 fichero_hosts\n\nNota: En el fichero de los hosts el primero de la lista debe ser esta maquina."
    exit
fi

HOSTFILE=$1
CREATEFILE=.__create

#elimiar claves viejas si existen
echo "(Info ) Eliminando claves viejas"
if [ -e ~/.ssh/id_dsa ]
then
    rm ~/.ssh/id_dsa
fi

if [ -e ~/.ssh/id_dsa.pub ]
then
    rm ~/.ssh/id_dsa.pub
fi

#generar las claves con una passphrase vacia
echo "(Info ) Generando claves nuevas"
ssh-keygen -t dsa -N "" -f ~/.ssh/id_dsa
if [ $? == 0 ] 
    then
    echo "(Ok   ) Claves generadas"
else
    echo "(Error) Claves no se pudieron generer"
    exit 1
fi


#distribuir la clave publica a todos los ordenadores (menos el primero de la lista)
i=0;
echo "(Info ) Distribuyendo claves"
for host in `cat $HOSTFILE`
do
  if [ "$i" == "1" ]
  then
      echo "(Info ) Copiando la clave a $host"
      #copiar la clave al fichero authorized_keys
      echo -e "mkdir .ssh\nput $HOME/.ssh/id_dsa.pub .ssh/authorized_keys" > $CREATEFILE
      sftp $host < $CREATEFILE
      if [ $? == 0 ]
	  then
	  echo "(Ok   ) Clave copiada"
      else
	  echo "(Error) Clave no se pudo copiar a $host"
      fi
      rm $CREATEFILE
  fi
  i=1;
done
