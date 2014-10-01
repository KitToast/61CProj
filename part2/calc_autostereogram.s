.text

# Generates an autostereogram inside of buffer
#
# Arguments:
#     autostereogram (unsigned char*)
#     depth_map (unsigned char*)
#     width
#     height
#     strip_size
calc_autostereogram:

        # Allocate 5 spaces for $s0-$s5
        # (add more if necessary)
        addiu $sp $sp -20
        sw $s0 0($sp)
        sw $s1 4($sp)
        sw $s2 8($sp)
        sw $s3 12($sp)
        sw $s4 16($sp)

        # autostereogram
        lw $s0 20($sp)
        # depth_map
        lw $s1 24($sp)
        # width
        lw $s2 28($sp)
        # height
        lw $s3 32($sp)
        # strip_size
        lw $s4 36($sp)

	li $t6, 0 #i = 0
	li $t7, 0   #j = 0, Need to save this somwehwere before random call 
	ori $s5, $0, 0x00FF #Used to put only lower 8 bits
	move $t1, $s0
	move $t2, $s1
	#Free temp registers to keep in mind: 
	
OuterForLoop:		beq $t6, $s2, ExitWidth	 
InnerForLoop:   	beq $t7, $s3, ExitHeight
	slt $t8, $t6, $s4 #Checks if i < Strip_Size
	beq $t8, $0, Else #if false
	move $s6, $t7 #save the current j counter to get ready for random call
	jal lfsr_random #call random function
	move $t7, $s6 #reload counter into proper register 
	and  $t9, $v0, $s4 #Get result from v0
	sh $t9, 0($t1) #load 8 bit into autostereogram 
	j InnerIn
Else:
	lh $t8, 0($t2) 
	addu $t8, $t8, $t6
	subu $t8, $t8, $s4
	mult $t8, $s2
	mflo $t8 #Get height of else statement
	addu $t8, $t8, $t7 #add j
	addu $t0, $s0, $t8 #add offset to beginning pointer
	lh $t0, 0($t0) #Get stereo gram from position calculating above
	sh $t0, 0($t1) #load it into current sterogram pointer 
InnerIn:	addiu $t1,$t1,1 #increment stereo pointer by one byte
		addiu $t2, $t2,1 #increment depth pointer as well for convienience
		addiu $t7,$t7,1 #increment j counter
		j InnerForLoop 
ExitHeight:
	li $t7, 0 #reset the j counter for the next iteration
	addi $t6, $t6, 1 #increment i counter 
	j OuterForLoop
ExitWidth:
        lw $s0 0($sp)
        lw $s1 4($sp)
        lw $s2 8($sp)
        lw $s3 12($sp)
        lw $s4 16($sp)
        addiu $sp $sp 20
        jr $ra
        
	.include "lfsr_random.s"
