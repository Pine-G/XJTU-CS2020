.text
	lui $t0 0x1001	# data segment base address
	lui $t1 0xd188
	ori $t1 $t1 0x97e3
	sw $t1 252($t0) # initial memory
	
	start:
	lb $t7 252($t0)
	lbu $t6 252($t0)
	lh $t5 252($t0)
	lhu $t4 252($t0)
	lw $t3 252($t0)
	
	sb $zero 255($t0)
	sh $zero 248($t0)
	sw $zero 244($t0)
	
	add $t7 $t3 $t4
	addu $t6 $t3 $t4
	sub $t7 $t3 $t4
	subu $t6 $t3 $t4
	mult $t3 $t4
	multu $t3 $t4
	div $t3 $t4
	divu $t3 $t4
	
	lui $t2 0x8000
	
	sll $t7 $t3 4
	srl $t7 $t3 4
	sra $t7 $t3 4
	sllv $t6 $t3 $t4
	srlv $t6 $t3 $t4
	srav $t6 $t3 $t4
	
	and $t5 $t6 $t7
	or $t5 $t6 $t7
	xor $t5 $t6 $t7
	nor $t5 $t6 $t7
	
	addi $t4 $t2 0x1234
	addiu $t3 $t2 0x1234
	
	andi $t7 $t6 0xffff
	ori $t7 $t6 0xffff
	xori $t7 $t6 0xffff
	
	slti $t6 $t2 0x0001
	sltiu $t6 $t2 0x0001
	
	bne $t1 $t2 loop1
	ori $t8 $t8 0xffff
	loop1: addi $t8 $zero 1
	
	beq $s0 $s1 loop2
	ori $t8 $t8 0xffff
	loop2: addi $t9 $zero 1
	
	j jump1
	andi $t8 $t8 0x0000
	jump1: addi $t8 $zero 2
	
	jal jump2
	andi $t8 $t8 0x0000
	jump2: addi $t9 $zero 2
	
	nop
	la $s3 start
	jr $s3
