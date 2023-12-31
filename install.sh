#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    SO="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    SO="macos"
else
    echo "Unsupported operating system."
    exit 1
fi

CURRENT_PWD="$PWD"
FOLDER=".malloc_tester"
#mkdir -p "$HOME/$FOLDER"
rm -rf "$HOME/$FOLDER"

cd $HOME
git clone https://github.com/kingcreek/42-Malloc_tester.git .malloc_tester

cd .malloc_tester
make

# if [ "$SO" == "linux" ]; then
#     curl -L -o "$HOME/$FOLDER/malloc_tester.so" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.so"
# elif [ "$SO" == "macos" ]; then
#     curl -L -o "$HOME/$FOLDER/malloc_tester.dylib" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.dylib"
# fi
# curl -L -o "$HOME/$FOLDER/malloc_tester.sh" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.sh"

chmod +x "$HOME/$FOLDER/malloc_tester.sh"

RC_FILE="$HOME/.zshrc"

if [ "$SO" == "linux" ]; then
	RC_FILE="$HOME/.bashrc"
	# if [[ -f "$HOME/.zshrc" ]]; then
	# 	RC_FILE="$HOME/.zshrc"
	# fi
fi

if ! grep "king=" "$RC_FILE" &> /dev/null; then
	printf "\nalias king=%s/malloc_tester.sh\n" "$HOME/$FOLDER" >> "$RC_FILE"
fi

if ! grep "amparette=" "$RC_FILE" &> /dev/null; then
	printf "\nalias amparette=%s/malloc_tester.sh\n" "$HOME/$FOLDER" >> "$RC_FILE"
fi

cd "$CURRENT_PWD"
echo "Configuration completed. You can run 'amparette' to run the program."

exec "$SHELL"
