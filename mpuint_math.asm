ifndef X64
.686p
.XMM
.model flat
endif

EXTRN numeric_overflow:PROC

.data
.code
mpuint_add_asm PROC
	;protect registers
	PUSH RBX
	PUSH RAX
	PUSH R10
		;setup initial values
		MOV R10B, 0
		MOV RBX, 0
		DEC R8
		;main addition loop
		loopBegin:
			MOV RAX, [RDX+4*RBX]
			CMP R10B, 1
			JNE noAddCarry
				INC RAX
				JNZ noSecCarry
					CMP RBX, R8
					JE bailOutWithError
					MOV R10B, 1
					JMP noAddCarry
				noSecCarry:
				MOV R10B, 0
			noAddCarry:
			CMP RBX, R8
			JBE noOverflowCheck
				CMP RAX, 0
				JNE bailOutWithError
			noOverflowCheck:
			ADD [RCX+4*RBX], RAX
			JNC noCarry
				CMP RBX, R8
				JE bailOutWithError
				MOV R10B, 1
			noCarry:
			ADD RBX,2
			CMP RBX, R9
			JNE loopBegin
			JMP endGracefully
		bailOutWithError:
		CALL numeric_overflow
		JMP stopAdding
		endGracefully:
		CMP RBX, R8
		JA stopAdding
		CMP R10B, 1
		JNE stopAdding
		addCarry:
				INC QWORD PTR [RCX+4*RBX]
				JNC stopAdding
					CMP RBX, R8
					JE bailOutWithError
					ADD RBX,2
					JMP addCarry
		stopAdding:
	;restore registers
	POP R10
	POP RAX
	POP RBX
	RET
mpuint_add_asm ENDP

mpuint_sub_asm PROC
	;protect registers
	PUSH RBX
	PUSH RAX
	PUSH R10
		;setup initial values
		MOV R10B, 0
		MOV RBX, 0
		DEC R8
		;main addition loop
		loopBegin:
			MOV RAX, [RDX+4*RBX]
			CMP R10B, 1
			JNE noSubBorrow
				DEC QWORD PTR [RCX+4*RBX]
				JNZ noSecBorrow
					CMP RBX, R8
					JE bailOutWithError
					MOV R10B, 1
					JMP noSubBorrow
				noSecBorrow:
				MOV R10B, 0
			noSubBorrow:
			CMP RBX, R8
			JBE noOverflowCheck
				CMP RAX, 0
				JNE bailOutWithError
			noOverflowCheck:
			CMP [RCX+4*RBX], RAX
			JAE noBorrow
				CMP RBX, R8
				JE bailOutWithError
				MOV R10B, 1
			noBorrow:
			SUB [RCX+4*RBX], RAX
			ADD RBX,2
			CMP RBX, R9
			JNE loopBegin
			JMP endGracefully
		bailOutWithError:
		CALL numeric_overflow
		JMP stopSubtracting
		endGracefully:
		CMP RBX, R8
		JA stopSubtracting
		CMP R10B, 1
		JNE stopSubtracting
		subBorrow:
				DEC QWORD PTR [RCX+4*RBX]
				JNC stopSubtracting
					CMP RBX, R8
					JE bailOutWithError
					ADD RBX,2
					JMP subBorrow
		stopSubtracting:
	;restore registers
	POP R10
	POP RAX
	POP RBX
	RET
mpuint_sub_asm ENDP

end