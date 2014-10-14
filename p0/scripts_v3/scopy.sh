#!/bin/sh

#Autor: Stefan Wehner

if [ $# != 2 ] 
then
    echo -e "USAGE: $0 fichero_hosts fichero\n\nNota: En el fichero de los hosts el primero de la lista debe ser esta maquina."
    exit 1
fi

HOSTFILE=$1
#comprobar si el path es relativo o absoluto
#eliminar el mayor sufijo que cumple /*
if [ "${2%%/*}" == "" ]
    then
    #empieza con / => es absoluto
    DIRECTORY=${2%/*}
    else
    #no empieza con / => es relativo
    DIRECTORY=$PWD/${2%/*}
fi
echo $DIRECTORY
#elimiar el mayor prefijo cumpliendo */ - deja el nombre del fichero
FILE=${2##*/}
CREATEFILE=.__create

#distribuir el fichero
if [ -e $HOSTFILE ]
then
    if [ -e $DIRECTORY/$FILE ]
	then
	echo "(Info ) Distribuyendo $FILE"
	i=0
	for host in `cat $HOSTFILE`
	  do
	  if [ "$i" == "1" ]
	      then
	      
	  #probar copiarlo
	      echo "(Info ) Copiando $FILE a $host"
	      scp $DIRECTORY/$FILE $host:$DIRECTORY/$FILE
	      if [ $? == 0 ]
		  then
		  echo "(Ok   ) Fichero copiado...!"
	      else
		  echo "(Error) Copia fallada"
		  echo "(Info ) Se intenta crear la estructura de directorios"
		  echo "(Info ) Probablemente salga un error al crear algun directorio ya existente"
	  #intentar crear la estructura de directorios en todos
		  if [ -e $CREATEFILE ]
		      then
		      rm $CREATEFILE
		  fi
		  
		  inc=" "
		  
		  for dir in `echo $DIRECTORY|sed s/[/]/\ /g`
		    do
		    inc=$inc/$dir
		    echo "mkdir $inc" >> $CREATEFILE
		  done
		  
		  echo "quit" >> $CREATEFILE
		  
		  cat $CREATEFILE | sftp $host > /dev/null
		  rm $CREATEFILE
		  
		  echo "(Info ) Reintentar copia del fichero"
		  scp $DIRECTORY/$FILE $host:$DIRECTORY/$FILE
		  if [ $? == 0 ]
		      then
		      echo "(Ok   ) Fichero copiado...!"
		  else
		      echo "(Error) Copia fallada"
		      exit 1
		  fi
	      fi
	  fi
	  i=1;
	done
    else
	echo "(Error) \"$DIRECTORY/$FILE\" no existe"
    fi
else
    echo "(Error) \"$HOSTFILE\" no existe"
fi
