#!/bin/bash


EXE=ft_ping
sec=2
WHITE='\033[1;97m'
RESET='\033[0m'
# selon une liste d'hosts verifier l'output /leaks et errors
# lance la commande entre ping et ft_ping
# comparer les resultats
# verifier les error et le leaks

mandatory=("_" 
"invalidhost.wdfw.com" #invalid
"invalidhost" #invalid
"google.com" #valid
"go.com" #valid
"intra.42.fr" #valid
"192.68.5.5" #invalid
"147.168.42.5" #invalid
"255.255.255.255" #valid
)


if [ ! -f ./inetutils-2.0/ping/ping ]; then
    echo -e "${WHITE} Installing ping (inetutils-2.0)${RESET}"
    echo -e "${WHITE} Installing inetutils-2.0.tar ...${RESET}"
    wget https://ftp.gnu.org/gnu/inetutils/inetutils-2.0.tar.gz > /dev/null
    echo -e "${WHITE} Decompressing inetutils-2.0.tar ...${RESET}"
    tar -zxf inetutils-2.0.tar.gz > /dev/null
    cd ./inetutils-2.0 
    echo -e "${WHITE} Configuring inetutils-2.0.tar ...${RESET}"
    ./configure > /dev/null
    echo -e "${WHITE} Compiling inetutils-2.0.tar ...${RESET}"
    make > /dev/null
    cd .. > /dev/null
    echo -e "${WHITE} Ping installed ...${RESET}"
fi

if [ ! -d ./out ]; then
    mkdir -p out/ping out/ft_ping > /dev/null
fi  

print_res_file() {
    echo -e "${WHITE}$2${RESET}"
    errors_line=$(grep "ERROR SUMMARY" $1)
    leaks_line=$(grep "total heap usage" $1)
    if [ $2 != "ping" ]; then
        echo -e ${errors_line}
    fi
    echo -e ${leaks_line}
}

echo -e "${WHITE} Launching tests ${RESET}"
for host in ${mandatory[@]}; do
    echo -e "\n${WHITE} $host ...${RESET}"
    timeout -s SIGINT 2s valgrind --leak-check=full --show-leak-kinds=all ./inetutils-2.0/ping/ping $host &> out/ping/$host
    print_res_file out/ping/$host ping
    timeout -s SIGINT 2s valgrind --leak-check=full --show-leak-kinds=all ./ft_ping $host &> out/ft_ping/$host
    print_res_file out/ft_ping/$host ft_ping
done
