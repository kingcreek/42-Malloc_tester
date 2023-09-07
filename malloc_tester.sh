#!/bin/bash

# Función para mostrar el mensaje de bienvenida
show_welcome_message() {
    echo " ________  ________  ________   ________          ________  ________  ___      ___ ________  ________  ________  ___      ___ ________"
    echo "|\\   __  \\|\\   __  \\|\\   __  \\ |\\   __  \\        |\\   __  \\|\\   __  \\|\\  \\    /  /|\\   __  \\|\\   __  \\|\\   __  \\|\\  \\    /  /|\\   __  \\"
    echo "\\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\\\ \\  \\|\\  \\       \\ \\  \\|\\  \\ \\  \\|\\  \\\\ \\  \\  /  / | \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\\\ \\  \\  /  / | \\  \\|\\  \\"
    echo " \\ \\   __  \\ \\   __  \\ \\   ____\\\\ \\   __  \\       \\ \\  \\\\\\  \\ \\   ____\\\\ \\  \\/  / / \\ \\   __  \\ \\   __  \\ \\   __  \\\\ \\  \\/  / / \\ \\   __  \\"
    echo "  \\ \\  \\ \\  \\ \\  \\ \\  \\ \\  \\___| \\ \\  \\ \\  \\       \\ \\  \\\\\\  \\ \\  \\___| \\ \\    / /   \\ \\  \\ \\  \\ \\  \\ \\  \\ \\  \\\\ \\    / /   \\ \\  \\ \\  \\"
    echo "   \\ \\__\\ \\__\\ \\__\\ \\__\\ \\__\\     \\ \\__\\ \\__\\       \\ \\_______\\ \\__\\     \\\ \\__/ /     \\ \\__\\ \\__\\ \\__\\ \\__\\ \\__\\\\ \\__/ /     \\ \\__\\ \\__\\"
    echo "    \\|__|\\|__|\\|__|\\|__|\\|__|      \\|__|\\|__|        \\|_______|\\|__|      \\|__|/       \\|__|\\|__|\\|__|\\|__|\\|__| \\|__|/       \\|__|\\|__|"
    echo ""
}

# Mostrar el mensaje de bienvenida
show_welcome_message

# Detectar si el sistema operativo es Linux o macOS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  # Sistema Linux
  REMOTE_LIBRARY_URL="http://tu.url.remota/mymalloc_linux.so"
  LOCAL_LIBRARY_NAME="mymalloc_linux.so"
  LOAD_FUNCTION="LD_PRELOAD"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  # macOS
  REMOTE_LIBRARY_URL="http://tu.url.remota/mymalloc_macos.dylib"
  LOCAL_LIBRARY_NAME="mymalloc_macos.dylib"
  LOAD_FUNCTION="DYLD_INSERT_LIBRARIES"
else
  echo "Sistema operativo no compatible."
  exit 1
fi

# Descargar la biblioteca compartida desde la URL remota
echo "Descargando la biblioteca compartida desde $REMOTE_LIBRARY_URL..."
curl -o $LOCAL_LIBRARY_NAME $REMOTE_LIBRARY_URL

# Verificar si la descarga fue exitosa
if [ $? -ne 0 ]; then
  echo "Error al descargar la biblioteca compartida."
  exit 1
fi

# Preguntar por la ruta del ejecutable
read -p "Ingrese la ruta del ejecutable: " EXECUTABLE_PATH

# Verificar si el ejecutable existe
if [ ! -f "$EXECUTABLE_PATH" ]; then
  echo "El ejecutable no existe en la ubicación especificada."
  exit 1
fi

# Cargar la biblioteca compartida junto con el ejecutable
echo "Ejecutando $EXECUTABLE_PATH con la biblioteca $LOCAL_LIBRARY_NAME cargada..."
$LOAD_FUNCTION=./$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH

# Limpiar la biblioteca compartida descargada
rm -f $LOCAL_LIBRARY_NAME

echo "Ejecución completa."