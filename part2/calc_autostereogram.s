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
addiu $sp $sp -24
sw $s0 0($sp)
sw $s1 4($sp)
sw $s2 8($sp)
sw $s3 12($sp)
sw $s4 16($sp)
sw $s5 20($sp)

# autostereogram
lw $s0 24($sp)
# depth_map
lw $s1 28($sp)
# width
lw $s2 32($sp)
# height
lw $s3 36($sp)
# strip_size
lw $s4 40($sp)

li $t6, 0 #i = 0
li $t7, 0   #j = 0, Need to save this somwehwere before random call
ori $s5, $0, 0x00FF #Used to put only lower 8 bits
move $t1, $s0
move $t2, $s1
#Registers lsfr uses:
# t0,t8,t9,t7,t1,t2,t5,s2,t3
#Important registers:
# t1, t2, s2, t6, t7

OuterForLoop:		beq $t6, $s2, ExitWidth
InnerForLoop:   	beq $t7, $s3, ExitHeight
slt $t8, $t6, $s4 #Checks if i < Strip_Size
beq $t8, $0, Else #if false

addiu $sp,$sp,-20 #Allocate stack pointer for function call #Store Registers needed for function call next
sw $t1 0($sp)
sw $t2 4($sp)
sw $t6 8($sp)
sw $t7 12($sp)
sw $ra 16($sp)

jal lfsr_random #call random function
#Restore state
lw $t1 0($sp)
lw $t2 4($sp)
lw $t6 8($sp)
lw $t7 12($sp)
lw $ra 16($sp)
addiu $sp, $sp,20 #Deallocate stack pointer after function call and restore state.
and  $t9, $v0, $s5 #Get result from v0
sb $t9, 0($t1) #load 8 bit into autostereogram
j InnerIn
Else:
lbu $t8, 0($t2) #load from current depth map pointer
addu $t8, $t8, $t6
subu $t8, $t8, $s4 #Calculated width
mult $t7, $s2
mflo $t9
addu $t8, $t8, $t9 #add j (height offset) to width in $t8.
addu $t8, $s0, $t8 #add offset to beginning pointer
lbu $t8, 0($t8) #Get stereo gram from position calculating above
sb $t8, 0($t1) #load it into current sterogram pointer
InnerIn:	addu $t1,$t1,$s2 #increment stereo pointer to next character down
addu $t2, $t2,$s2 #increment depth pointer as well for convienence
addiu $t7,$t7,1 #increment j counter
j InnerForLoop
ExitHeight:
li $t7, 0 #reset the j counter for the next iteration

addi $t6, $t6, 1 #increment i counter first

addu $t8 $s0 $t6 #Add column offsets
addu $t9 $s1 $t6

move $t1, $t8 #set the pointers to the next COLUMN
move $t2, $t9


j OuterForLoop
ExitWidth:
lw $s0 0($sp)
lw $s1 4($sp)
lw $s2 8($sp)
lw $s3 12($sp)
lw $s4 16($sp)
lw $s5 20($sp)
addiu $sp $sp 24
jr $ra
