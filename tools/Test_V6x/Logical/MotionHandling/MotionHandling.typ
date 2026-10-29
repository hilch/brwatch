
TYPE
	AxisHandlingAxesGroupStepEnum : 
		(
		AHG_W_EVENT, (*warte auf Ereignis*)
		AHG_READ_NEXT_ERROR_RECORD, (*nächsten Fehlerdatensatz lesen*)
		AHG_COPY_ERROR_RECORDS, (*kopiere alle Fehlerinformationen*)
		AHG_W_RESET (*alle Fehlertexte wurden gelesen*)
		);
	AxisHandlingAxesStepEnum : 
		(
		AX_STARTUP,
		AX_W_CONFIGURATION_READ,
		AX_W_EVENT, (*warte auf Ereignis*)
		AX_READ_NEXT_ERROR_RECORD, (*nächsten Fehlerdatensatz lesen*)
		AX_COPY_ERROR_RECORDS, (*kopiere alle Fehlerinformationen*)
		AX_W_RESET, (*alle Fehlertexte wurden gelesen*)
		AX_WRITE_CONFIGURATION (*schreibe neue Achskonfiguration*)
		);
	AxisHandlingAxisType : 	STRUCT 
		name : STRING[32];
		step : AxisHandlingAxesStepEnum; (*Schrittkette*)
		errorTextHeader : STRING[80];
		errorText : ARRAY[0..14]OF STRING[120]; (*Fehlertexte*)
		errorTextLines : UINT; (*Fehlertexte: Anzahl der Zeilen*)
		pAxis : UDINT;
		fbMpAxis : MpAxisBasic;
		mpAxisParameters : MpAxisBasicParType;
		fbMoveAbsolute : MC_MoveAbsolute;
		nullpos : LREAL; (*Fahrt auf diese Position nach Homing*)
		pureVaxAdapter : McPureVAxDS402VlAdapterPvType;
		statusInputSTO : BOOL; (*Status STO/Enable*)
		fbForceHardwareInputs : MC_BR_ForceHardwareInputs;
		fbMpAxisBasicConfig : MpAxisBasicConfig;
		mpAxisBasicConfigData : MpAxisBasicConfigType;
		fbReadErrorText : MC_BR_ReadErrorText;
		fbCommandError : MC_BR_CommandError;
		n : UINT;
	END_STRUCT;
	AxisHandlingAxesGroupType : 	STRUCT 
		name : STRING[32];
		step : AxisHandlingAxesGroupStepEnum; (*Schrittkette*)
		errorTextHeader : STRING[80];
		errorText : STRING[16384]; (*Fehlertexte*)
		errorTextLines : UINT; (*Fehlertexte: Anzahl der Zeilen*)
		pAxGroup : UDINT;
		fbGroupPower : MC_BR_GroupPower;
		fbGroupHome : MC_BR_GroupHome_15;
		fbGroupReset : MC_GroupReset;
		fbGroupReadInfo : MC_BR_GroupReadInfo;
		fbGroupReadStatus : MC_GroupReadStatus;
		fbReadErrorText : MC_BR_ReadErrorText;
		n : UINT;
	END_STRUCT;
	AxisHandlingType : 	STRUCT 
		ax : ARRAY[0..MAX_AX_MINUS_ONE]OF AxisHandlingAxisType;
		axGroupCnc : AxisHandlingAxesGroupType;
		axGroupAux : AxisHandlingAxesGroupType;
	END_STRUCT;
END_TYPE
