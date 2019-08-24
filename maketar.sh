#!/bin/bash

DIRNAME='ep1-Bruno_Carneiro-Daniel_Martinez'

# exit when any command fails
set -e

# keep track of the last executed command
#trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
#trap 'echo "\"${last_command}\" command filed with exit code $?."' EXIT

make clean

rm -rf "$DIRNAME"*

mkdir "$DIRNAME"

cp -r src "$DIRNAME"
cp -r hdr "$DIRNAME"
cp -r ext "$DIRNAME"
cp    Makefile "$DIRNAME"
cp    LEIAME "$DIRNAME"
cp    CMakeLists.txt "$DIRNAME"

tar -czvf "$DIRNAME.tar.gz" "$DIRNAME"

rm -rf "$DIRNAME"

echo "Tar criado com sucesso! Verificando se ele compila..."

mkdir tartest
tar -C tartest -xzvf "$DIRNAME.tar.gz"

cd "tartest/$DIRNAME"

make

cd -

rm -rf "tartest"

echo "Tar criado e testado com sucesso!"
exit