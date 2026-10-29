
TYPE
	myStructType2 : 	STRUCT 
		struct1 : myStructType;
		structList : ARRAY[0..2]OF myStructType;
	END_STRUCT;
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
	myDerivedType2 :ARRAY[3..6]OF USINT;
	myComplexStructType : 	STRUCT 
		bool : BOOL;
		usint : USINT;
		uint : UINT;
		udint : UDINT;
		sint : SINT;
		int : INT;
		dint : DINT;
		real : REAL;
		lreal : LREAL;
		time : TIME;
		tod : TIME_OF_DAY;
		date : DATE;
		dt : DATE_AND_TIME;
		string : STRING[80];
		wstring : WSTRING[80];
		stringlist : ARRAY[1..10]OF STRING[40] := ['A','B','C','D','E','F','G','H','I','J'];
		wstringlist : ARRAY[1..10]OF WSTRING[40] := ["A","B","C","D","E","F","G","H","I","J"];
		vector : ARRAY[1..10]OF REAL;
		intvector : ARRAY[0..9]OF INT;
		matrix : ARRAY[0..9,0..3]OF REAL;
		myStruct0 : myStructType;
		myStruct1 : ARRAY[0..1]OF myStructType;
		myStruct2 : ARRAY[2..2]OF myStructType;
		myStruct3 : ARRAY[-1..1]OF myStructType;
		myStruct4 : ARRAY[0..2,0..3]OF myStructType;
		myStruct5 : ARRAY[0..2]OF myStructType2;
		myEnum : myEnumType;
		enumlist : ARRAY[0..4]OF myEnumType;
		subrange1 : SINT(-3..6) ;
		derived1 : myDerivedType1;
		derived2 : myDerivedType2;
		n : USINT := 99;
	END_STRUCT;
END_TYPE
