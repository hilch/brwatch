
TYPE
	myStructType : 	STRUCT 
		member1 : USINT := 3;
		member2 : ARRAY[0..2]OF USINT := [33,44,55];
	END_STRUCT;
	myEnumType : 
		(
		eins,
		zwei,
		drei
		);
	myDerivedType1 :USINT(4..6) ;
	myDerivedType :ARRAY[3..6]OF USINT;
	myComplexStructType : 	STRUCT 
		element : USINT;
		vector : ARRAY[1..10]OF REAL;
		matrix : ARRAY[0..9,0..3]OF REAL;
		myStruct0 : myStructType;
		myStruct1 : ARRAY[0..1]OF myStructType;
		myStruct2 : ARRAY[2..2]OF myStructType;
		myStruct3 : ARRAY[-1..1]OF myStructType;
		myStruct4 : ARRAY[0..2,0..3]OF myStructType;
		myEnum : myEnumType;
		subrange1 : SINT(-3..6) ;
		derived1 : myDerivedType1;
		myDerived1 : myDerivedType;
		n : USINT := 99;
	END_STRUCT;
END_TYPE
