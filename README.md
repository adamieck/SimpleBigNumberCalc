# SimpleBigNumberCalc
Simple BigInt calculator made as an assignment during the 1st semester of Computer Science and Information Systems course.

## Features
- **Addition (+)**
- **Multiplication (*)**
- **Division (/)**
- **Exponentiation (^)**
- **Modulo (%)**
- **Numeral system conversion (2-16)**

## How to use
Create `in.txt` with following structure:
> `<operation symbol> <numeral system>`
>
>`<first number>`
>
>`<second number>`

Or in case of system conversion
> `<system> <desired system>`
>
>`<number>`

Example data can be found in `in.txt` provided with this repo.

## Handling incorrect data
The program includes basic handling of incorrect data. It will respond to division by zero, finding the remainder of division by zero, or an invalid operation symbol in the input file. When an error is detected, the program will display an appropriate message, write it to the output file, and then attempt to continue the calculations.