package main

import (
	"fmt"
	"time"
)

var ____NOW____ = time.Now()

func fib(a int) int {
	fmt.Printf("Starts %s %s\n", "fib", time.Since(____NOW____).String())
	defer fmt.Printf("Ends %s %s\n", "fib", time.Since(____NOW____).String())
	if a <= 1 {
		return 1
	}
	return fib(a-1) + fib(a-2)
}

func main() {
	fmt.Printf("Starts %s %s\n", "main", time.Since(____NOW____).String())
	defer fmt.Printf("Ends %s %s\n", "main", time.Since(____NOW____).String())
	fib(3)
}
