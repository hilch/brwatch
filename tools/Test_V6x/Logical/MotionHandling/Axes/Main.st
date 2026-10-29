
PROGRAM _INIT
	gAxisHandling.ax[AX_X].name := 'X';
	gAxisHandling.ax[AX_X].pAxis := ADR(gAxisX);
	axX ACCESS ADR(gAxisHandling.ax[AX_X]);

	size1 := SIZEOF(axX);
	(* fill with test data *)
	FOR n := 0 TO SIZEOF(axX.errorText)/SIZEOF(axX.errorText[0])-1 DO
		axX.errorText[n] := 'error text ';
		brsitoa( n, ADR(tempstring));
		brsstrcat( ADR(axX.errorText[n]) , ADR(tempstring) ) ;
	END_FOR
	axX.errorTextLines := n;
	axX.n := 99;

	 
END_PROGRAM

PROGRAM _CYCLIC

END_PROGRAM

PROGRAM _EXIT
	 
END_PROGRAM

