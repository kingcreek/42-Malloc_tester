#!/bin/bash

##############################################################################################

show_welcome_message() {
  echo " _    _                                _    ";
  echo "| |  (_)                              | |   ";
  echo "| | ___ _ __   __ _  ___ _ __ ___  ___| | __";
  echo "| |/ / | '_ \ / _\` |/ __| '__/ _ \/ _ \ |/ /";
  echo "|   <| | | | | (_| | (__| | |  __/  __/   < ";
  echo "|_|\_\_|_| |_|\__, |\___|_|  \___|\___|_|\_\\";
  echo "               __/ |                        ";
  echo "              |___/                         ";
}

show_welcome_message

FOLDER=".malloc_tester"

##############################################################################################

##############################################################################################
CURRENTVERSION="0.4"

github_url="https://github.com/kingcreek/42-Malloc_tester/raw/main/version.txt"
if ! curl -s -L "$github_url" | grep -q $CURRENTVERSION; then
  echo "UPDATING..."
  bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh)"
  exit
fi
##############################################################################################

##############################################################################################

ADDRESSFILE=$HOME/$FOLDER/address.0x00
TRACE_FILE="$HOME/$FOLDER/trace"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	# LOCAL_LIBRARY_NAME="./malloc_tester.so"
	LOCAL_LIBRARY_NAME="$HOME/$FOLDER/malloc_tester.so"
	LOAD_FUNCTION="LD_PRELOAD"
elif [[ "$OSTYPE" == "darwin"* ]]; then
	# LOCAL_LIBRARY_NAME="./malloc_tester.dylib"
	LOCAL_LIBRARY_NAME="$HOME/$FOLDER/malloc_tester.dylib"
	LOAD_FUNCTION="DYLD_INSERT_LIBRARIES"
else
  echo "Unsupported operating system."
  exit 1
fi
##############################################################################################

##############################################################################################

function add_flags() {
	local makefile="$1"
	sed -i 's/\(-Werror\)/\1 -g -rdynamic/' "$makefile"
}
# Remove flags in Makefile
function remove_flags() {
	local makefile="$1"
	sed -i 's/ -g -rdynamic//' "$makefile"
}
# makefile array
makefiles=()
# Process makefile in current path and subdirectories
function process_directory() {
	local directorio="$1"
	local makefile
	makefile="$directorio/Makefile"
	if [ -f "$makefile" ]; then
		add_flags "$makefile"
		makefiles+=("$makefile")
	fi
	for subdir in "$directorio"/*/; do
		if [ -d "$subdir" ]; then
			process_directory "$subdir"
		fi
	done
}

#read -e -p "Do you want the flags (-g -rdynamic) to be added to your makefile? The program will be compiled and the flags will be removed later automatically.\n y/n: " PROCESSMAKE
PROCESSMAKE="n"
if [[ "$PROCESSMAKE" == "y" || "$PROCESSMAKE" == "Y" ]]; then
	echo -e  "processing make"
	#add flags
	process_directory "$(pwd)"
	# do make
	make
	# Remove flags in all makefiles
	for makefile in "${makefiles[@]}"; do
		remove_flags "$makefile"
	done
fi

##############################################################################################

##############################################################################################

echo -e "\nMake sure that the program is compiled with the "-g" flag as well as the libraries that you use (ft_printf, libft..) for a correct operation of the tester"
read -e -p "Enter the path of the executable: " EXECUTABLE_PATH

##############################################################################################
#EXECUTABLE_PATH=""

#possible_executables=("a.out" "anotherprogram" "yetanotherprogram")
# Función para verificar si existe un archivo con un nombre específico
#function find_executable() {
#    local name="$1"
#    if [ -f "$name" ]; then
#        EXECUTABLE_PATH="./$name"
#    fi
#}

# Verifica si existe alguno de los posibles ejecutables
#for name in "${possible_executables[@]}"; do
#    find_executable "$name"
#    if [ -n "$EXECUTABLE_PATH" ]; then
#        break
#    fi
#done

# Si se encontró un ejecutable, muestra su nombre
#if [ -n "$EXECUTABLE_PATH" ]; then
#    echo "Ejecutable encontrado: $EXECUTABLE_PATH"
#else
#    echo "Debes estar en un proyecto válido."
#fi
##############################################################################################

if [[ "$EXECUTABLE_PATH" == "leaks "* ]]; then
  PROGRAM_NAME=$(echo "$EXECUTABLE_PATH" | cut -d' ' -f2)
  while true; do
    leaks -q "$PROGRAM_NAME"
  done
fi

EJECUTABLE=$(echo "$EXECUTABLE_PATH" | awk '{print $1}')

if [ ! -f "$EJECUTABLE" ]; then
  echo "The executable does not exist in the specified location."
  exit 1
fi
file_type=$(file -b "$EJECUTABLE")
echo $EJECUTABLE
if [[ $file_type != *executable* ]]; then
	echo "Invalid file."
	exit 1
fi

rm -f $ADDRESSFILE
touch $ADDRESSFILE

echo "Launch $EXECUTABLE_PATH with lib injected..."

ok_flag=99

while true; do

	# if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	rm -f $TRACE_FILE
	touch $TRACE_FILE
	# fi
	#program_output=$(eval "$LOAD_FUNCTION=./$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" 2>&1 | tee /dev/tty)
	program_output=$(eval "$LOAD_FUNCTION=$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" | tee /dev/tty)
  	#eval "$LOAD_FUNCTION=./$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" < /dev/tty &
  	#pid=$!
  	#wait $pid
  	program_result=$?
  
  	if [ $program_result -eq 139 ]; then
    	ok_flag=0
    	break
	elif [[ "$program_output" == *"segmentation fault"* ]]; then
		ok_flag=0
    	break
	elif [[ "$program_output" == *"pointer being freed was not allocated"* ]]; then
		ok_flag=1
    	break
  	elif [[ "$program_output" == *"Finished tester"* ]]; then
    	break
  	fi
done

if [ $ok_flag -eq 99 ]; then
  echo -e "\n\033[32mOK\033[0m\n"
elif [ $ok_flag -eq 1 ]; then
  echo -e "\n\x1B[31mKO (double free)\x1B[0m"
else
	echo -e "\n\x1B[31mKO (Segfault)\x1B[0m"
	if [ -f "$TRACE_FILE" ]; then
		echo -e "\n----TRACE----"
    	while IFS= read -r line; do
      		result=$(eval "$line")
	  		echo $result
    	done < "$TRACE_FILE"
	fi
fi




rm -f $ADDRESSFILE
rm -f $TRACE_FILE

echo -e "\nFinish."

##############################################################################################
