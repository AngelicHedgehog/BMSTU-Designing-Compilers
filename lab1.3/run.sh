g++ -std=c++23 lab1.3.cpp -o lab1.3
if [ $? -eq 0 ]; then
    ./lab1.3
    rm lab1.3
fi
