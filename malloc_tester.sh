#!/bin/bash

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

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  # Linux
  REMOTE_LIBRARY_URL="https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.so"
  LOCAL_LIBRARY_NAME="malloc_tester.so"
  LOAD_FUNCTION="LD_PRELOAD"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  # MacOS
  REMOTE_LIBRARY_URL="https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.dylib"
  LOCAL_LIBRARY_NAME="malloc_tester.dylib"
  LOAD_FUNCTION="DYLD_INSERT_LIBRARIES"
else
  echo "Sistema operativo no compatible."
  exit 1
fi

# Carga la biblioteca readline
if [ -n "$BASH_VERSION" ]; then
  bind 'set bell-style none' 2>/dev/null
  bind 'set editing-mode emacs' 2>/dev/null
  bind 'TAB:menu-complete' 2>/dev/null
  bind '"\e[A": history-search-backward' 2>/dev/null
  bind '"\e[B": history-search-forward' 2>/dev/null
fi

echo "Descargando la biblioteca compartida desde $REMOTE_LIBRARY_URL..."
#curl -L -o $LOCAL_LIBRARY_NAME $REMOTE_LIBRARY_URL

if [ $? -ne 0 ]; then
  echo "Error al descargar la biblioteca compartida."
  exit 1
fi

#read -p "Ingrese la ruta del ejecutable: " EXECUTABLE_PATH
EXECUTABLE_PATH="./a.out"
if [ ! -f "$EXECUTABLE_PATH" ]; then
  echo "El ejecutable no existe en la ubicación especificada."
  rm -f $LOCAL_LIBRARY_NAME
  exit 1
fi

touch address.0x00
#touch repeater

echo "Ejecutando $EXECUTABLE_PATH con la biblioteca $LOCAL_LIBRARY_NAME cargada..."

ok_flag=0

while true; do
  program_output=$(LD_PRELOAD=$(readlink -f ./$LOCAL_LIBRARY_NAME) $EXECUTABLE_PATH 2>&1)
  program_result=$?


  #if [ "$program_output" == *"Segmentation fault"* ]; then
  #  echo -e "\n\e[31mKO\e[0m"
  #  rm -f repeater
  #  break
  #  elif [ $program_result -eq 0 ]; then
  if [ $program_result -eq 0 ]; then
	if [ -f "repeater" ]; then
		continue
	fi
	ok_flag=1
	#rm -f repeater
	break
  else
	rm -f repeater
    break
  fi
done

if [ $ok_flag -eq 1 ]; then
  echo -e "\n\e[32mOK\e[0m"
else
  echo -e "\n\e[31mKO\e[0m"
fi

rm -f $LOCAL_LIBRARY_NAME
rm -f address.0x00

echo "Ejecución completa."