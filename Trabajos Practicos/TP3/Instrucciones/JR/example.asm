#Si va a la direccion saltar t2 valdra 15,
#Caso contrario t2 valdra 10 y seria un error
la $t1, saltar #se carga la dir en t1
addi $t2, $zero, 20
subi $t2, $t2, 1
jr $t1
sigue:
subi $t2, $t2, 5
subi $t2, $t2, 4
saltar:
subi $t2, $t2, 4
