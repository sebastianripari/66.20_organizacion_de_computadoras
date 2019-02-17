# si funciona, el valor final de t2 deberia quedar en 15
# caso contrario debería quedar en 10
addi $t2, $zero, 20
subi $t2, $t2, 1
j end
target:
subi $t2, $t2, 5
subi $t2, $t2, 4
end:
subi $t2, $t2, 4
