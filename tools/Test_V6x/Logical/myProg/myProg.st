(********************************************************************
 * COPYRIGHT -- B&R Industrial Automation GmbH
 ********************************************************************
 * Program: myProg
 * File: myProg.st
 * Author: hilchenbachc
 * Created: November 22, 2022
 ********************************************************************
 * Implementation of program myProg
 ********************************************************************)

PROGRAM _INIT
	myComplexStruct.usint := 11;
	myComplexStruct.uint  := 12;
	myComplexStruct.udint := 13;
	myComplexStruct.sint := -11;
	myComplexStruct.int  := -12;
	myComplexStruct.dint := -13;
	myComplexStruct.real := 3.1415;
	myComplexStruct.lreal := 2 * myComplexStruct.real;
	myComplexStruct.string := 'The quick brown fox jumps over the lazy dog';
	myComplexStruct.wstring := "The quick brown fox jumps over the lazy dog";
	FOR i := 0 TO 9 DO
		myComplexStruct.vector[i+1] := 200 + i;
		myComplexStruct.intvector[i] := 300+ i;
		FOR j := 0 TO 3 DO
			myComplexStruct.matrix[i,j] := 1000 + (10 * i + j);
		END_FOR
	END_FOR

	FOR i := -1 TO +1 DO
		myComplexStruct.myStruct3[i].member1 := INT_TO_USINT(5+i);
		myComplexStruct.myStruct3[i].member2[0] := INT_TO_USINT(10+i);
		myComplexStruct.myStruct3[i].member2[1] := INT_TO_USINT(20+i);
		myComplexStruct.myStruct3[i].member2[2] := INT_TO_USINT(30+i);
	END_FOR

	FOR i := 0 TO 2 DO
		FOR j := 0 TO 3 DO
			myComplexStruct.myStruct4[i,j].member1 := INT_TO_USINT(10 * i +j);
			myComplexStruct.myStruct4[i,j].member2[0] := INT_TO_USINT(20 * i +j);
			myComplexStruct.myStruct4[i,j].member2[1] := INT_TO_USINT(30 * i +j);
			myComplexStruct.myStruct4[i,j].member2[2] := INT_TO_USINT(40 * i +j);
		END_FOR
	END_FOR

	FOR n := 0 TO 2 DO
		FOR i := 0 TO 2 DO
			myComplexStruct.myStruct5[n].struct1.member1 := INT_TO_USINT(3 + i+n);
			FOR j := 0 TO 2 DO
				myComplexStruct.myStruct5[n].structList[i].member1 := INT_TO_USINT(10 + i + n);
				myComplexStruct.myStruct5[n].struct1.member2[j] := INT_TO_USINT(+j+i+n);
				myComplexStruct.myStruct5[n].structList[i].member2[j] := INT_TO_USINT((i+1)*10 + j);
			END_FOR
		END_FOR
	END_FOR


	myComplexStruct.myEnum := drei;
	myComplexStruct.enumlist[0] := eins;
	myComplexStruct.enumlist[1] := zwei;
	myComplexStruct.enumlist[2] := drei;
	myComplexStruct.enumlist[3] := eins;
	myComplexStruct.enumlist[4] := zwei;

	myComplexStruct.derived2[3] := 3;
	myComplexStruct.derived2[4] := 4;
	myComplexStruct.derived2[5] := 5;
	myComplexStruct.derived2[6] := 6;
	sizeof_myComplexStruct := SIZEOF(myComplexStruct);	

END_PROGRAM

PROGRAM _CYCLIC

END_PROGRAM
