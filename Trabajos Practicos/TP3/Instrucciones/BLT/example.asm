li $t2, 31
li $t3, 31
blt $t2, $t3, MENOR_ESTRICTO

MAYOR_O_IGUAL:
li $t7, 6
b END

MENOR_ESTRICTO:
li $t7, 7

END:

