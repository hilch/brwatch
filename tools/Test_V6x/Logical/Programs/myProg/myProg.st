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
	myUSINT := 91;
	myUINT := 92;
	mySTRING := 'Hello, world';
	myWSTRING := "Hello, world";
	myTIME := T#314s;
	myTOD := TOD#14:15:30.000;
	myDT := DT#2025-02-07-06:28:15;
	myDINT := -93;
	myINTArray;
	mySTRINGArray;
	myWSTRINGArray;
	myUDINT := 94;
	myDATE := D#2025-02-07 ;
	myLREAL := 95.6;
	myREAL := 96.7;
	myINT := 97;
	mySINT := 98;
	myBOOL := TRUE;
	myStructArray;

	FOR i := 0 TO 9 DO
		myComplexStruct.vector[i+1] := 200 + i;
		FOR j := 0 TO 3 DO
			myComplexStruct.matrix[i,j] := 1000 + (10 * i + j);
		END_FOR
	END_FOR
	myComplexStruct.myEnum := drei;
	sizeof_myComplexStruct := SIZEOF(myComplexStruct);	

END_PROGRAM

PROGRAM _CYCLIC

END_PROGRAM
