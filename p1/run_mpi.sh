#!/bin/bash

GREEN="\033[3;32m"
RED="\033[3;31m"
NO_COLOUR="\033[0m"

MACHINES_FILE=""

# GATHER THE MPIRUN OPTIONS

while test $# -gt 0; do
        case "$1" in
                -h|--help)
                        echo "run_mpi - executes an mpi program"
                        echo " "
                        echo "./run_mpi.sh [options] application [arguments]"
                        echo " "
                        echo "options:"
                        echo "-h, --help                        show brief help"
                        echo "-n, --np=processes                specify the number of processes"
                        echo "-m, --machines=MACHINES_FILE      specify the machines file"
                        exit 0
                        ;;
                -n)
                        shift
                        if test $# -gt 0; then
                                N_PROCESS=$1
                        else
                                echo "number of processes not specified"
                                exit 1
                        fi
                        shift
                        ;;
                --np*)
                        N_PROCESS=`echo $1 | sed -e 's/^[^=]*=//g'`
                        shift
                        ;;
                -m)
                        shift
                        if test $# -gt 0; then
                                MACHINES_FILE=$1
                        fi
                        shift
                        ;;
                --machines*)
                        MACHINES_FILE=`echo $1 | sed -e 's/^[^=]*=//g'`
                        shift
                        ;;
                *)
                        break
                        ;;
        esac
done

# CHECK THE EXISTENCE OF THE APPLICATION
if [ $# -eq 0 ]
then
        echo "Usage: ./run_mpi.sh [options] application [arguments]"
        echo "Try './run_mpi.sh -h/--help' for more information."
        exit 1
else
        APP=$1
        shift
fi

# COMPILE/LINKING
echo -e "[$GREEN INFO $NO_COLOUR] Compiling/Linking $APP..."

make $APP
if [ $? -eq 0 ]
then
        echo -e "[$GREEN INFO $NO_COLOUR] Done..."
else
        echo -e "[$RED ERROR $NO_COLOUR] Make $APP..."
        exit 1
fi

# DISTRIBUYING COPIES OF THE APPLICATION IN CASE WE ARE NOT IN LOCAL ONLY
if [ "$MACHINES_FILE" != "" ]
then
        echo -e "[$GREEN INFO $NO_COLOUR] Distribuying copies of the application..."
        ./scopy $MACHINES_FILE $APP
        if [ $? -eq 0 ]
        then
                echo -e "[$GREEN INFO $NO_COLOUR] Done..."
        else
                echo -e "[$RED ERROR $NO_COLOUR] Scopy $APP..."
                exit 1
        fi
fi

# SAVE THE ARGUMENTS OF THE APPLICATION
while test $# -gt 0; do
        ARGS="$ARGS $1"
        shift
done

EXECUTE="./$APP $ARGS"

echo -e "[$GREEN INFO $NO_COLOUR] Running $APP with arguments $ARGS\n"

# RUNING THE APPLICATION
# DISTRIBUTED
if [ "$MACHINES_FILE" != "" ]
then
        mpirun -machinefile $MACHINES_FILE -n $N_PROCESS $EXECUTE
        if [ $? -eq 0 ]
        then
                echo -e "\n[$GREEN INFO $NO_COLOUR] Done..."
        else
                echo -e "\n[$RED ERROR $NO_COLOUR] mpirun $APP..."
                exit 1
        fi
# LOCAL ONLY
else
        mpirun -n $N_PROCESS $EXECUTE
        if [ $? -eq 0 ]
        then
                echo -e "\n[$GREEN INFO $NO_COLOUR] Done..."
        else
                echo -e "\n[$RED ERROR $NO_COLOUR] mpirun $APP..."
                exit 1
        fi
fi