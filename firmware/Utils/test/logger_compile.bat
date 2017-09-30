cls
gcc -o logger_test.exe ../logger.c logger_test.c port/logger_port.c -lc -I../ -I.
logger_test.exe
pause