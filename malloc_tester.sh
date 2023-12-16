#!/bin/bash

##############################################################################################

show_welcome_message() {
  echo -e "\n\033[32m";
  echo -e " _    _                                _    ";
  echo -e "| |  (_)                              | |   ";
  echo -e "| | ___ _ __   __ _  ___ _ __ ___  ___| | __";
  echo -e "| |/ / | '_ \ / _\` |/ __| '__/ _ \/ _ \ |/ /";
  echo -e "|   <| | | | | (_| | (__| | |  __/  __/   < ";
  echo -e "|_|\_\_|_| |_|\__, |\___|_|  \___|\___|_|\_\\";
  echo -e "               __/ |                        ";
  echo -e "              |___/                         ";
  echo -e "\033[0m\n";
}

show_welcome_message

FOLDER=".malloc_tester"

##############################################################################################

##############################################################################################
CURRENTVERSION="2.9.6"

# github_url="https://github.com/kingcreek/42-Malloc_tester/raw/main/version.txt"
# if ! curl -s -L "$github_url" | grep -q $CURRENTVERSION; then
#   echo -n "\n\033[32m UPDATING... \033[0m\n"
#   bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh)"
#   exit
# fi
github_url="https://github.com/kingcreek/42-Malloc_tester/blob/main/version.txt"
downloaded_version=$(curl -s -L "$github_url" | grep -o 'rawLines":\["[^"]*' | awk -F '[:,[]"' '{print $2}')

if [ "$downloaded_version" != "$CURRENTVERSION" ]; then
  echo -e "\n\033[32m UPDATING... \033[0m\n"
  bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh)"
  exit
fi
##############################################################################################

##############################################################################################

ADDRESSFILE=$HOME/$FOLDER/address.0x00
TRACE_FILE="$HOME/$FOLDER/trace"
LEAKS_FILE="$HOME/$FOLDER/leaks"
NO_ALLOC="$HOME/$FOLDER/noalloc"

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

HISTFILE="$HOME/$FOLDER/script_history"
touch "$HISTFILE"
if [ -e "$HISTFILE" ]; then
    history -r "$HISTFILE"
fi

echo -e "\nMake sure that the program is compiled with the "-g" flag as well as the libraries that you use (ft_printf, libft..) for a correct operation of the tester"
read -e -p "Enter the path of the executable: " EXECUTABLE_PATH

if [ $(history | wc -l) -ge 50 ]; then
    history -d 1
fi

if ! history | grep -q "$EXECUTABLE_PATH"; then
    history -s "$EXECUTABLE_PATH"
fi
history -w "$HISTFILE"

##############################################################################################



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

##############################################################################################

if [[ "$EXECUTABLE_PATH" == "leaks "* ]]; then
  PROGRAM_NAME=$(echo "$EXECUTABLE_PATH" | cut -d' ' -f2)
  while true; do
    leaks -q "$PROGRAM_NAME"
  done
  exit 1
fi

if [[ "$EXECUTABLE_PATH" == "functions "* ]]; then
  PROGRAM_NAME=$(echo "$EXECUTABLE_PATH" | cut -d' ' -f2)
  nm -n "$PROGRAM_NAME"
  exit 1
fi

if [[ "$EXECUTABLE_PATH" == "asm "* ]]; then
	PROGRAM_NAME=$(echo "$EXECUTABLE_PATH" | cut -d' ' -f2)
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  		objdump -d "$PROGRAM_NAME"
  		exit 1
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		otool -tV "$PROGRAM_NAME"
  		exit 1
	fi
fi

##############################################################################################

EJECUTABLE=$(echo "$EXECUTABLE_PATH" | awk '{print $1}')

if [ ! -f "$EJECUTABLE" ]; then
  echo "The executable does not exist in the specified location."
  exit 1
fi
file_type=$(file -b "$EJECUTABLE")
if [[ $file_type != *64* ]]; then
	echo "Invalid file."
	exit 1
fi

rm -f $ADDRESSFILE
touch $ADDRESSFILE

if [[ ! "$EXECUTABLE_PATH" =~ ^\./ ]]; then
    EXECUTABLE_PATH="./$EXECUTABLE_PATH"
fi

if nm -a "$EXECUTABLE_PATH" 2>&1 | grep -q _asan; then
    echo -e "\n\x1B[31m Your program is compiled with the fsanitize flag, please recompile the program without that flag for correct functioning of the tester. \x1B[0m"
	exit
fi

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	if ! readelf -S "$EXECUTABLE_PATH" | grep -q '\.debug'; then
    	echo -e "\n\x1B[31m Your program is not compiled with -g, please compile with said flag for better results. \x1B[0m"
		exit
	fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
	if ! nm -a "$EXECUTABLE_PATH" | grep 'ENSYM'; then
    	echo -e "\n\x1B[31m Your program is not compiled with -g, please compile with said flag for better results. \x1B[0m"
		exit
	fi
fi

echo "Launch $EXECUTABLE_PATH with lib injected..."

ok_flag=99

while true; do

	rm -f $TRACE_FILE
	touch $TRACE_FILE
	rm -f $LEAKS_FILE
	touch $LEAKS_FILE
	rm -f $NO_ALLOC
	touch $NO_ALLOC

	# fi
	# program_output=$(eval "$LOAD_FUNCTION=$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" | tee /dev/tty)
	program_output=$(eval "$LOAD_FUNCTION=$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" | tee /dev/tty)
  	program_result=$?
	
	if [[ "$program_output" == *"Program: Abort"* || "$program_output" == *"Program: Bus error"* ]]; then
    	echo -e "\n\x1B[31m HAVE PROBLEMS HERE!. \x1B[0m"
		echo -e "\n\x1B[31m Your program works without amparette? \x1B[0m"
		exit
  	elif [ $program_result -eq 139 ]; then
    	ok_flag=0
    	break
	elif [[ "$program_output" == *"segmentation fault"* ]]; then
		ok_flag=0
    	break
	elif [[ "$program_output" == *"Segmentation fault"* ]]; then
		ok_flag=0
    	break
	elif [[ "$program_output" == *"Pointer being freed was not allocated"* ]]; then
		ok_flag=1
    	break
  	elif [[ "$program_output" == *"Test completed correctly"* ]]; then
    	break
  	fi
done

executable_name=$(basename "$EJECUTABLE")

if [ $ok_flag -eq 99 ]; then
  echo -e "\n\033[32m OK!! \033[0m\n"
elif [ $ok_flag -eq 1 ]; then
  echo -e "\n\x1B[31m KO (double free) \x1B[0m"
else
	echo -e "\n\x1B[31m KO (Segfault) \x1B[0m"
	if [ -f "$TRACE_FILE" ]; then
		echo -e "\n----TRACE----"
    	while IFS= read -r line; do
        	result=$(eval "$line")
			if [[ "$OSTYPE" == "linux-gnu"* ]]; then
				echo $result
			elif [[ "$OSTYPE" == "darwin"* ]]; then
				if [[ "${result}" == *"$executable_name"* ]]; then
        			echo $result
				fi
			fi
    	done < "$TRACE_FILE"
	fi
fi

if [ -f "$LEAKS_FILE" ] && [ -s "$LEAKS_FILE" ]; then
	echo -e "\n\x1B[33m PROGRAM END WITHOUT FREEING ALL MEMORY \x1B[0m"
	line_number=0
	while IFS= read -r line; do
		((line_number++))
		if [ $((line_number % 2)) -eq 0 ]; then
			result=$(eval "$line")
			echo $result
		else
            echo "$line"
        fi
    done < "$LEAKS_FILE"
fi

# if [ -f "$NO_ALLOC" ] && [ -s "$NO_ALLOC" ]; then
# 	echo -e "\n\x1B[33m POINTER BEING FREED WAS NOT ALLOCATED \x1B[0m"
# 	line_number=0
# 	while IFS= read -r line; do
# 		result=$(eval "$line")
# 		if [[ "$OSTYPE" == "linux-gnu"* ]]; then
# 			echo $result
# 		elif [[ "$OSTYPE" == "darwin"* ]]; then
# 			if [[ "${result}" == *"$executable_name"* ]]; then
#         		echo $result
# 			fi
# 		fi
#     done < "$NO_ALLOC"
# fi



rm -f $ADDRESSFILE
rm -f $TRACE_FILE
rm -f $LEAKS_FILE
rm -f $NO_ALLOC

echo -e "\n"

##############################################################################################
