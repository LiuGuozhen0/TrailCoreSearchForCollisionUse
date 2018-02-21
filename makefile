obj = \
	L.o \
	List.o \
	misc.o \
	main.o\
    duplex.o \
    genKATShortMsg.o \
    Keccak.o \
    KeccakCrunchyContest.o \
    Keccak-f.o \
    Keccak-f25LUT.o \
    Keccak-fAffineBases.o \
    Keccak-fCodeGen.o \
    Keccak-fDCEquations.o \
    Keccak-fDCLC.o \
    Keccak-fDisplay.o \
    Keccak-fEquations.o \
    Keccak-fParity.o \
    Keccak-fParityBounds.o \
    Keccak-fParts.o \
    Keccak-fPositions.o \
    Keccak-fPropagation.o \
    Keccak-fState.o \
    Keccak-fTrailCore3Rounds.o \
    Keccak-fTrailCoreInKernelAtC.o \
    Keccak-fTrailCoreParity.o \
    Keccak-fTrailCoreRows.o \
    Keccak-fTrailExtension.o \
    Keccak-fTrails.o \
    Keyakv2.o \
    Keyakv2-test.o \
    Motorist.o \
    padding.o \
    progress.o \
    sponge.o \
    spongetree.o \
    transformations.o \
	compareTrails.o \
	kkeccak.o \

	
main : $(obj)
	g++  -o main $(obj) -lm -lm4ri
	
kkeccak.o : kkeccak.h
L.o : kkeccak.h L.h
List.o: L.h List.h
misc.o : kkeccak.h misc.h
duplex.o : duplex.h
genKATShortMsg.o : duplex.h Keccak.h
Keccak.o : Keccak.h
KeccakCrunchyContest.o : KeccakCrunchyContest.h Keccak.h
Keccak-f.o : Keccak-f.h
Keccak-f25LUT.o : Keccak-f25LUT.h
Keccak-fAffineBases.o : Keccak-fAffineBases.h Keccak-fDisplay.h
Keccak-fCodeGen.o : Keccak-fCodeGen.h
Keccak-fDCEquations.o : Keccak-fDCEquations.h
Keccak-fDCLC.o : Keccak-fPropagation.h Keccak-fDCLC.h
Keccak-fDisplay.o : Keccak-fDisplay.h
Keccak-fEquations.o : Keccak-fEquations.h
Keccak-fParity.o : Keccak-fParity.h
Keccak-fParityBounds.o : Keccak-fParityBounds.h Keccak-fPositions.h progress.h translationsymmetry.h
Keccak-fParts.o : Keccak-fParts.h
Keccak-fPositions.o : Keccak-fPositions.h
Keccak-fPropagation.o : Keccak-fDCLC.h Keccak-fDisplay.h Keccak-fParity.h Keccak-fPropagation.h Keccak-fTrails.h
Keccak-fState.o : Keccak-fState.h
Keccak-fTrailCore3Rounds.o : Keccak-fDisplay.h Keccak-fTrailCore3Rounds.h translationsymmetry.h
Keccak-fTrailCoreInKernelAtC.o : Keccak-fDisplay.h Keccak-fTrailCoreInKernelAtC.h translationsymmetry.h
Keccak-fTrailCoreParity.o : Keccak-fTrailCoreParity.h
Keccak-fTrailCoreRows.o : Keccak-fTrailCoreRows.h
Keccak-fTrailExtension.o : Keccak-fTrailExtension.h translationsymmetry.h
Keccak-fTrails.o : Keccak-fDisplay.h Keccak-fPropagation.h Keccak-fTrails.h
Keyakv2.o : Keyakv2.h
Keyakv2-test.o : Keyakv2.h
Motorist.o : Motorist.h
padding.o : padding.h
progress.o : progress.h
sponge.o : sponge.h
spongetree.o : spongetree.h
transformations.o : transformations.h
compareTrails.o : compareTrails.h kkeccak.h List.h misc.h L.h
main.o : duplex.h Keccak.h KeccakCrunchyContest.h Keccak-f25LUT.h Keccak-fCodeGen.h Keccak-fDCEquations.h Keccak-fTrailCoreInKernelAtC.h\
         Keccak-fDCLC.h Keccak-fEquations.h Keccak-fPropagation.h Keccak-fTrailExtension.h Keccak-fTrails.h Keyakv2-test.h misc.h \
		 compareTrails.h

.PHONY : clean
clean :
	-rm main $(obj)
