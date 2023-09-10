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

FOLDER=".malloc_tester"
ADDRESSFILE=$HOME/$FOLDER/address.0x00
TRACE_FILE="$HOME/$FOLDER/trace"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  LOCAL_LIBRARY_NAME="$HOME/$FOLDER/malloc_tester.so"
  LOAD_FUNCTION="LD_PRELOAD"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  LOCAL_LIBRARY_NAME="$HOME/$FOLDER/malloc_tester.dylib"
  LOAD_FUNCTION="DYLD_INSERT_LIBRARIES"
else
  echo "Unsupported operating system."
  exit 1
fi

#echo "Downloading the shared library from $REMOTE_LIBRARY_URL..."
#curl -L -o $LOCAL_LIBRARY_NAME $REMOTE_LIBRARY_URL

#if [ $? -ne 0 ]; then
#  echo "Error downloading shared library."
#  exit 1
#fi
echo -e "\nMake sure that the program is compiled with the "-g" flag as well as the libraries that you use (ft_printf, libft..) for a correct operation of the tester"
read -e -p "Enter the path of the executable: " EXECUTABLE_PATH

if [[ "$EXECUTABLE_PATH" == "leaks "* ]]; then
  PROGRAM_NAME=$(echo "$EXECUTABLE_PATH" | cut -d' ' -f2)
  while true; do
    leaks -q "$PROGRAM_NAME"
  done
fi

if [[ ! "$EXECUTABLE_PATH" =~ ^\./.* ]]; then
  EXECUTABLE_PATH="./$EXECUTABLE_PATH"
fi

if [ ! -f "$EXECUTABLE_PATH" ]; then
  echo "The executable does not exist in the specified location."
  exit 1
fi

rm -f $ADDRESSFILE
touch $ADDRESSFILE

echo "Launch $EXECUTABLE_PATH with lib injected..."

#handle_sigint() {
#	echo $pid
#	kill -INT $pid
#}
#trap 'handle_sigint' INT

ok_flag=1

while true; do

	rm -f $HOME/$FOLDER/trace
	touch $HOME/$FOLDER/trace
	#program_output=$(eval "$LOAD_FUNCTION=./$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" 2>&1 | tee /dev/tty)
	program_output=$(eval "$LOAD_FUNCTION=$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" | tee /dev/tty)
  	#eval "$LOAD_FUNCTION=./$LOCAL_LIBRARY_NAME $EXECUTABLE_PATH" < /dev/tty &
  	#pid=$!
  	#wait $pid
  	program_result=$?
  
  	if [ $program_result -eq 139 ]; then
    	ok_flag=0
    	break
	elif [[ "$program_output" == *"seg fault handle"* ]]; then
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
	if [ -f "$TRACE_FILE" ]; then
		echo -e "\n----TRACE----"

		mapfile -t lines < "$TRACE_FILE"
		num_lines=${#lines[@]}
		for ((i = 0; i < num_lines - 1; i++)); do
			line="${lines[i]}"
			result=$(eval "$line")
			mapfile -t result_lines <<< "$result"
			if [ "${#result_lines[@]}" -ge 2 ]; then
      			if [[ ${result_lines[1]} != "??:?"* ]]; then
					echo "- ${result_lines[1]}"
				else
					echo "- This trace is not traceable, have you compiled with -g the program and libraries?"
				fi
    		fi
		done
	fi
fi

rm -f $ADDRESSFILE
rm -f $TRACE_FILE

echo -e "\nFinish."
