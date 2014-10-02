.text

# Decodes a quadtree to the original matrix -Replicate that is used to complete 
#
# Arguments:
#     quadtree (qNode*)
#     matrix (void*)
#     matrix_width (int)
#
# Recall that quadtree representation uses the following format:
#     struct qNode {
#         int leaf; 0
#         int size; 4 
#         int x; 8
#         int y; 12
#         int gray_value; 16
#         qNode *child_NW, *child_NE, *child_SE, *child_SW; 20 24 28 32 
#     }

# Assume that a qNode has size 4 * 9 = 36 bytes. Further assume that the temporary variables are not stored across jar

quad2matrix:
	
	# load quadtree variables into stack
	addiu $sp, $sp, -4
	
	sw $ra, 0($sp) # to store $ra
	
	# we play with $t0, a copy of $s0
	move $t0, $s0
		
	sw $t1, 0($t0) # store leaf in $t1
	bne $t1, $zero, Leaf # if the value leaf is not zero, it must be one, hence it must be a leaf
	
	# allocation for the children
	
	addiu $sp, $sp, -8 #Store oringal pointer to parent quadtree struct
	sw $s0, 0($sp)
	sw $t0, 4($sp) #store $t0 to keep oringal pointer
	
	# NW
	lw $s0, 20($t0) #Pointer to NW
	jal quad2matrix
	
	# NE
	lw $t0, 4($sp)
	lw $s0, 24($t0) #Pointer to NE
	jal quad2matrix
	
	# SE
	lw $t0, 4($sp)
	lw $s0, 28($t0) #Pointer to SE
	jal quad2matrix
	
	# SW
	lw $t0, 4($sp)
	lw $s0, 32($t0) #Pointer to SW
	jal quad2matrix
	
	# free
	lw $s0, 0($sp)
	addiu $sp, $sp, 8
	
	j Exit

Leaf: # t0 points to the node of the leaf. Fills the matrix pointer. 
	#grab all the int variables
	
	lw $t2, 4($t0)  # store size in $t2
	lw $t3, 8($t0)  # store x in $t3
	lw $t4, 12($t0) # store y in $t4
	lw $t5, 16($t0) # store gray_value in $t5
	li $t7, 0       # $t7 will store our current offset
	
	mult $s2, $t4
	mflo $t7
	addu $t7, $t7, $t3 #Calculate offset
	addu $t6, $s1, $t7 #t6 is the offset from beginning of matrix
	
OuterLoop:	beq $t8, $t2, Exit # if i == width, break
InnerLoop:	beq $t9, $t2, ExitCol # if j == height, break
		sb $t5, 0($t6)
		addiu $t6, $t6, 1 #increment j 
		j InnerLoop	
ExitCol: 	addiu $t8, $t8, 1 #increment i
		addu $t7, $t7, $s2 #Go to next row 
		move $t6, $t7
		j OuterLoop
Exit: 
	sw $ra, 0($sp)
	addiu $sp, $sp, 4
	move $v0, $s1
	jr $ra