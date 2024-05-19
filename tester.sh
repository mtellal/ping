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
    mkdir -p out/ping/m \
    out/ft_ping/m \
    out/ft_ping/opt \
    out/ping/opt \
    out/ping/bonus \
    out/ft_ping/bonus \
    > /dev/null
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

############################################################################
#                           M A N D A T O R Y                              #
############################################################################


function mandatory() {
echo -e "${WHITE} Launching tests ${RESET}"
for host in ${mandatory[@]}; do

    file_ping=out/ping/m/$host
    file_ft_ping=out/ft_ping/m/$host

    echo -e "\n${WHITE} $host ...${RESET}"
   
    timeout -s SIGINT $sec valgrind --leak-check=full --show-leak-kinds=all ./inetutils-2.0/ping/ping $host &> $file_ping
    print_res_file $file_ping ping
    
    timeout -s SIGINT $sec valgrind --leak-check=full --show-leak-kinds=all ./ft_ping $host &> $file_ft_ping
    print_res_file $file_ft_ping ft_ping
done
}

############################################################################
#                      M A N D A T O R Y   O P T                           #
############################################################################



#tester les options -? -v
args=(
    " -v"
    " -vwdf"
    " -v google.com"
    " -a -v"
    " -a -v google.com"
    " -av google.com"
    " -? google.com"
    " -?google.com"
    " -? "
    " -?v "
    " -v? "
    " -vvvvvvvvvvvvv "
    " -vvvvvvvvvvvvv google.com"
)

mandatory_opt() {

rm -rf out/ft_ping/opt/* out/ping/opt/*

echo -e "${WHITE} Launching tests ${RESET}"
for ((i=0; i<${#args[@]};i++)); do

    file_ping=out/ping/opt/$i
    file_ft_ping=out/ft_ping/opt/$i

    echo -e "\n${WHITE} ${args[$i]} ...${RESET}"
    timeout -s SIGINT $sec valgrind --leak-check=full --show-leak-kinds=all ./inetutils-2.0/ping/ping ${args[$i]} &> ${file_ping}
    print_res_file ${file_ping} ping
    
    timeout -s SIGINT $sec valgrind --leak-check=full --show-leak-kinds=all ./ft_ping ${args[$i]} &> ${file_ft_ping}
    print_res_file ${file_ft_ping} ft_ping
done
}


############################################################################
#                          B O N U S   O P T --TTL                         #
############################################################################


#tester les options -? -v
bonus_args_ttl=(
    " -ttl=5"
    " -ttl="
    " -ttl=5 google.com"
    " -ttl=5 google.com"
    " --ttl=5 google.com"
    " --ttl=-5 google.com"
    " --ttl=0 google.com"
    " --ttl=256 google.com"
    " --ttl=255 google.com"
    " --ttl=65465 google.com"
    " --ttl=sdfwdfw google.com"
)

bonus_ttl() {

rm -rf out/ft_ping/bonus/* out/ping/bonus/*

echo -e "${WHITE} Launching tests ${RESET}"
for ((i=0; i<${#bonus_args_ttl[@]};i++)); do

    file_ping=out/ping/bonus/$i
    file_ft_ping=out/ft_ping/bonus/$i

    echo -e "\n${WHITE} ${bonus_args_ttl[$i]} ...${RESET}"
    timeout -s SIGINT $sec valgrind --leak-check=full --show-leak-kinds=all ./inetutils-2.0/ping/ping ${bonus_args_ttl[$i]} &> ${file_ping}
    print_res_file ${file_ping} ping
    
    timeout -s SIGINT $sec valgrind --leak-check=full --show-leak-kinds=all ./ft_ping ${bonus_args_ttl[$i]} &> ${file_ft_ping}
    print_res_file ${file_ft_ping} ft_ping
done
}

############################################################################
#                          B O N U S   O P T --COUNT                       #
############################################################################


#tester les options -? -v
bonus_args_count=(
    " -count=5"
    " -count="
    " -count=5 google.com"
    " -count=5 google.com"
    " --count=5 google.com"
    " --count=-5 google.com"
    " --count=0 google.com"
    " --count=65465 google.com"
    " --count=sdfwdfw google.com"
)

bonus_count() {

rm -rf out/ft_ping/bonus/* out/ping/bonus/*

echo -e "${WHITE} Launching tests ${RESET}"
for ((i=0; i<${#bonus_args_count[@]};i++)); do

    file_ping=out/ping/bonus/c_$i
    file_ft_ping=out/ft_ping/bonus/c_$i

    echo -e "\n${WHITE} ${bonus_args_count[$i]} ...${RESET}"
    timeout -s SIGINT 10s valgrind --leak-check=full --show-leak-kinds=all ./inetutils-2.0/ping/ping ${bonus_args_count[$i]} &> ${file_ping}
    print_res_file ${file_ping} ping
    
    timeout -s SIGINT 10s valgrind --leak-check=full --show-leak-kinds=all ./ft_ping ${bonus_args_count[$i]} &> ${file_ft_ping}
    print_res_file ${file_ft_ping} ft_ping
done
}

if [ "$1" == "count" ];then
    bonus_count
elif [ "$1" == "ttl" ];then
    bonus_ttl
elif [ "$1" == "bonus" ];then
    bonus_ttl
    bonus_count
else
    mandatory
    mandatory_opt
fi