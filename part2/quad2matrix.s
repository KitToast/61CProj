.text

# Decodes a quadtree to the original matrix
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

# Assume that a qNode has size 4 * 9 = 36 bytes

quad2matrix:
	
	# allocate for quadtree
	addiu $sp, $sp, -36
	sw $s0, 0($sp)
	lw $s0, 36($sp)
	
	# we play with $t0, a copy of $s0
	move $t0, $s0
	
Recurse: 
	
	# preparing for double loop
	li $t8, 0 # i = 0;
	li $t9, 0 # j = 0; 
	
	sw $t1, 0($t0) # store leaf in $t1
	
	bne $t1, $zero, Leaf # if the value leaf is not zero, it must be one, hence it must be a leaf
	beq $t1, $zero, Else # otherwise, go to else
	
Else: 
	# allocation for the children
	addiu $sp, $sp, -36
	
	sw $t0, 0($sp) # save $t0 so we can return to it
	
	# NW
	lw $t0, 36($sp)
	lw $t0, 20($t0)
	j Recurse
	
	#NE
	lw $t0, 0($sp)
	lw $t0, 24($t0)
	j Recurse
	
	#SE
	lw $t0, 0($sp)
	lw $t0, 28($t0)
	j Recurse
	
	#SW
	lw $t0, 0($sp)
	lw $t0, 32($t0)
	j Recurse
	
	# free 
	lw $t0, 0($sp)
	addiu $sp, $sp, 36
	
	j Exit

Leaf: # t0 points to the node of the leaf. Fills the matrix pointer. 
	#grab all the int variables
	
	lw $t2, 4($t0)  # store size in $t2
	lw $t3, 8($t0)  # store x in $t3
	lw $t4, 12($t0) # store y in $t4
	lw $t5, 16($t0) # store gray_value in $t5
	li $t7, 0       # $t7 will store our current offset
	
	mult
	div 
	
	beq $t8, $t2, ExitRow # if i == width, break
	beq $t9, $t2, ExitCol # if j == height, break
	
	

ExitRow: 

ExitCol: 

Exit: 
	lw $s0, 0($sp)
	addiu $sp, $sp, 36
	move $v0, $s1
	jr $ra
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	blt 
	
	
	
	
	
	
	