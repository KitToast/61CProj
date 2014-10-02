.data

lfsr:
        .align 4
        .half
        0x1

.text

# Implements a 16-bit lfsr
#
# Arguments: None
lfsr_random:

	addiu $sp, $sp, -4
	sw $s2, 0($sp)
	
        la $t0 lfsr #Load inital 0x01 into $v0
        lhu $v0 0($t0)
        
	li $t8, 0 # $t8 is i  
	li $t9, 16 #limit of the for loop
	ori $t7,$0, 0xFFFF #get only lower 16 bits
	
Loop:	beq  $t8, $t9, End
	srl $t1, $v0, 2
	srl $t2, $v0, 3
	srl $t3, $v0, 5
	xor $t5, $v0, $t1
	xor $t5, $t5, $t2
	xor $t5, $t5, $t3 
	move $s2, $t5 #s2 will be u_int16_t highest (half word)
	srl $t1, $v0, 1
	sll $t2, $s2, 15
	or $t3, $t1, $t2
	move $v0, $t3 #reset reg to be new value 
	and $v0,$v0,$t7 #remove upper 16 bits. 
	addiu $t8, $t8, 1 #update counter
	j Loop 	    
	
End:
        la $t0 lfsr #load $vo into $t0 again for return 
        sh $v0 0($t0)
        
        lw $s2 0($sp)
        addiu $sp, $sp,4
        jr $ra
