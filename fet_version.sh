#!/bin/bash

FET_VERSION=""

check_arguments()
{
    if [ -z "$1" ]; then
        echo -e "\nPlease call '$0 <version>' to run this command!\n"
        exit 1
    fi
    FET_VERSION=$1
}

#receber a versão por parametro
check_arguments $1
echo $FET_VERSION
#baixar o fonte da versão
#limpar a pasta fet
#descompactar na pasta fet
#commitar as alterações
#criar uma tag
#enviar para o git




