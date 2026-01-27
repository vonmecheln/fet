#!/bin/bash

FET_VERSION=""
OUTPUT_FILE="fet.tar.xz"

check_arguments()
{
    if [ -z "$1" ]; then
        echo -e "\nPlease call '$0 <version>' to run this command!\n"
        exit 1
    fi
    FET_VERSION=$1
}

download_file(){

    URL_DEFAULT="https://lalescu.ro/liviu/fet/download/"
    URL_OLD="https://lalescu.ro/liviu/fet/download/old/"
    URL_MORE_OLD="https://www.timetabling.de/download/old/"
    

    URL="${URL_DEFAULT}fet-${FET_VERSION}.tar.xz"
    wget -Nq -O $OUTPUT_FILE $URL
    if [ $? -ne 0 ]
    then

        URL="${URL_OLD}fet-${FET_VERSION}.tar.xz"
        wget -Nq -O $OUTPUT_FILE $URL
        if [ $? -ne 0 ]
        then

            URL="${URL_MORE_OLD}fet-${FET_VERSION}.tar.xz"
            wget -Nq -O $OUTPUT_FILE $URL
            if [ $? -ne 0 ]
            then

                echo -e "\nVersion '$FET_VERSION' not found!\n"
                exit 1
            fi        
        fi
    fi


}

#receber a versão por parametro
check_arguments $1

#baixar o fonte da versão
download_file

#limpar a pasta fet
rm -rf fet/

#descompactar na pasta fet
tar xf $OUTPUT_FILE
mv "fet-${FET_VERSION}" fet/
rm -rf $OUTPUT_FILE

#commitar as alterações
git add fet/
git commit -m "fet-${FET_VERSION}"

#criar uma tag
git tag -a "fet-${FET_VERSION}" -m "fet-${FET_VERSION}"

#enviar para o git
git push origin "fet-${FET_VERSION}"
git push

