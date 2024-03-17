flex -o lab1.5.c lab1.5.flex.c
if [ $? -eq 0 ]; then
    gcc -o lab1.5 lab1.5.c
    if [ $? -eq 0 ]; then
        ./lab1.5
        rm lab1.5
    fi
    rm lab1.5.c
fi
