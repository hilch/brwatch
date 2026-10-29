FUNCTION_BLOCK MC_BR_ProcessConfig
	VAR_INPUT
		Execute : BOOL; (*Execution of the function block begins on a rising edge of this input.*)
		Name: STRING[250]; (*Name of config element*)
		DataType: UDINT; (*Datatype of the structure element*)
		DataAddress: UDINT; (*Reference to the selected data structure*)
		Mode : McProcessConfigModeEnum; (*references establishing connection between the function block and the axis group, axes etc.*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. Function block is finished.*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		Error : BOOL; (*Error occurred during execution.*)
		ErrorID : DINT; (*Error number*)
	END_VAR
	VAR
		Internal : McExec1InternalType; (*Data for internal use*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ProcessParam
	VAR_INPUT
		Execute : BOOL; (*Execution of the function block begins on a rising edge of this input.*)
		Component: McComponentType; (*Reference to the component*)
		DataType: UDINT; (* Datatype of the structure element*)
		DataAddress: UDINT; (*Reference to the selected data structure*)
		Mode : McProcessParamModeEnum;
		AdvancedParameters :    McProcessParamAdvParType;
		ExecutionMode : McExecutionModeEnum;
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. Function block is finished.*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		Active : BOOL; (*Function block is active and must continue to be called.*)
		Error : BOOL; (*Error occurred during execution.*)
		ErrorID : DINT; (*Error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal data*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_GetCoordSystemIdent
	VAR_INPUT
		Execute : BOOL; (*Execution of the function block begins on a rising edge of this input.*)
		CoordSystemName : STRING[260]; (*frame name*)
		Parameter : McGetCoordSystemIdentParType; (*references establishing connection between the function block and the axis group, axes etc.*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. Function block is finished.*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		Error : BOOL; (*Error occurred during execution.*)
		ErrorID : DINT; (*Error number*)
		Ident : UDINT; (*coordinate system ident*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal data*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_TransformPosition
	VAR_INPUT
		Execute : BOOL; (*Execution of the function block begins on a rising edge of this input.*)
		Position : McFrameType; (*Position to be transformed*)
		SourceCoordSystem : UDINT; (*Source coordinate system*)
		TargetCoordSystem : UDINT; (*Target coordinate system*)
		Parameter : McTransformPositionParType; (*references establishing connection between the function block and the axis group, axes etc.*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. Function block is finished.*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		Error : BOOL; (*Error occurred during execution.*)
		ErrorID : DINT; (*Error number*)
		Offset : McFrameType; (*Offset of transformed position*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal data*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ReadErrorText
	VAR_INPUT
		Component: McComponentType; (*Reference to the component*)
		Enable : BOOL; (*The function block is active as long as this input is set.*)
		AdvancedParameters : McAdvReadErrTxtParType;
		ReadNext : BOOL;
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*Execution successful. Function block is finished.*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		Error : BOOL; (*Error occurred during execution.*)
		ErrorID : DINT; (*Error number.*)
		NumberOfRecords : UINT; (*Indicates number of records available to read.*)
		ReadDone : BOOL; (*Read of error record is done and record data is available in ErrorRecords output.*)
		ErrorRecords : McErrorRecordsType; (*Error record output.*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal data*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_CheckComponentReference
	VAR_INPUT
		Component: McComponentType; (*The reference which should be checked if it is linked to a component or not.*)
		Execute : BOOL; (*Execution of the function block begins on a rising edge of this input.*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. Function block is finished.*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		Error : BOOL; (*Error occurred during execution.*)
		ErrorID : DINT; (*Error number*)
		ReferenceValid : BOOL; (*TRUE ... the reference is linked to a component; FALSE ... the reference is not linked to a component*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal data*)
	END_VAR
END_FUNCTION_BLOCK
