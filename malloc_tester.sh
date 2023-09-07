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
  REMOTE_LIBRARY_URL="https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.dylib?raw=true"
  LOCAL_LIBRARY_NAME="malloc_tester.dylib"
  LOAD_FUNCTION="DYLD_INSERT_LIBRARIES"
else
  echo "Sistema operativo no compatible."
  exit 1
fi

echo "Descargando la biblioteca compartida desde $REMOTE_LIBRARY_URL..."
curl -L -o $LOCAL_LIBRARY_NAME $REMOTE_LIBRARY_URL

if [ $? -ne 0 ]; then
  echo "Error al descargar la biblioteca compartida."
  exit 1
fi

read -p "Ingrese la ruta del ejecutable: " EXECUTABLE_PATH

if [ ! -f "$EXECUTABLE_PATH" ]; then
  echo "El ejecutable no existe en la ubicación especificada."
  rm -f $LOCAL_LIBRARY_NAME
  exit 1
fi

echo "Ejecutando $EXECUTABLE_PATH con la biblioteca $LOCAL_LIBRARY_NAME cargada..."
$LOAD_FUNCTION=./$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH

rm -f $LOCAL_LIBRARY_NAME

echo "Ejecución completa."