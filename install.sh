#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    SO="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    SO="macos"
else
    echo "Unsupported operating system."
    exit 1
fi

FOLDER=".malloc_tester"

mkdir -p "$HOME/$FOLDER"

if [ "$SO" == "linux" ]; then
    curl -L -o "$HOME/$FOLDER/malloc_tester.so" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.so"
elif [ "$SO" == "macos" ]; then
    curl -L -o "$HOME/$FOLDER/malloc_tester.dylib" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.dylib"
fi
curl -L -o "$HOME/$FOLDER/malloc_tester.sh" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.sh"

chmod +x "$HOME/$FOLDER/malloc_tester.sh"

RC_FILE="$HOME/.zshrc"

if [ "$OSTYPE" != "Darwin" ]; then
	RC_FILE="$HOME/.bashrc"
	if [[ -f "$HOME/.zshrc" ]]; then
		RC_FILE="$HOME/.zshrc"
	fi
fi

if ! grep "malloc_tester=" "$RC_FILE" &> /dev/null; then
	printf "\nalias malloc_tester=%s/malloc_tester.sh\n" "$HOME/$FOLDER" >> "$RC_FILE"
fi

if ! grep "king=" "$RC_FILE" &> /dev/null; then
	printf "\nalias king=%s/malloc_tester.sh\n" "$HOME/$FOLDER" >> "$RC_FILE"
fi

if ! grep "amparette=" "$RC_FILE" &> /dev/null; then
	printf "\nalias amparette=%s/malloc_tester.sh\n" "$HOME/$FOLDER" >> "$RC_FILE"
fi

echo "Configuration completed. You can run 'malloc_tester' to run the program."

exec "$SHELL"
