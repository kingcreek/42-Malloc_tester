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
  echo "Unsupported operating system."
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

echo "Downloading the shared library from $REMOTE_LIBRARY_URL..."
curl -L -o $LOCAL_LIBRARY_NAME $REMOTE_LIBRARY_URL

if [ $? -ne 0 ]; then
  echo "Error downloading shared library."
  exit 1
fi
echo "Make sure your program is compiled with flag -g if you want to know the location of the error if there is one."
read -p "Enter the path of the executable: " EXECUTABLE_PATH

EJECUTABLE=$(echo "$EXECUTABLE_PATH" | awk '{print $1}')
if [ ! -f "$EJECUTABLE" ]; then
  echo "The executable does not exist in the specified location."
  rm -f $LOCAL_LIBRARY_NAME
  exit 1
fi

rm -f address.0x00
touch address.0x00

echo "Launch $EXECUTABLE_PATH with lib $LOCAL_LIBRARY_NAME..."

ok_flag=1

while true; do
  program_output=$(eval "$LOAD_FUNCTION=./$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH")
  program_result=$?
  
  if [ $program_result -eq 139 ]; then
    ok_flag=0
    break
  elif [[ "$program_output" == *"Finished tester"* ]]; then
    break
  fi
done

if [ $ok_flag -eq 1 ]; then
  echo -e "\n\e[32mOK\e[0m"
else
  echo -e "\n\e[31mKO\e[0m"
  if [[ $program_output == *"addr2line -e"* || $program_output == *"atos -o"* ]]; then
    command_to_execute=$(echo "$program_output" | grep -oE "(addr2line -e [^[:space:]]+ \+[[:space:]]0x[0-9a-fA-F]+)|(atos -o [^[:space:]]+ \+[[:space:]]0x[0-9a-fA-F]+)|(atos -o [^[:space:]]+ [[:space:]]0x[0-9a-fA-F]+)")

    eval_result=$(eval "$command_to_execute")
    echo "Segmentation fault: $eval_result"
  else
    echo "Not cmd"
  fi
fi

rm -f $LOCAL_LIBRARY_NAME
rm -f address.0x00

echo "Finish."
