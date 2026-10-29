TYPE
   McPureVAxStatusInputsPvIfType : STRUCT
        PoweredOn : BOOL; (*Variable for powered on status signal*)
        CommunicationReady : BOOL; (*Variable for communication ready status signal*)
        ReadyForPowerOn : BOOL; (*Variable for ready for power on status signal*)
        ErrorReactionActive : BOOL; (*Variable for error reaction active status signal*)
        Error : BOOL; (*Variable for error occurred*)
        ErrorID : DINT; (*Variable for number specifying the occurred error*)
        Warning : BOOL; (*Variable for warning occurred*)
        WarningID : DINT; (*Variable for number specifying the occurred warning*)
    END_STRUCT;

    McPureVAxControlOutputsPvIfType : STRUCT
        PowerOn : BOOL; (*Variable for signal to switch on the power*)
        ErrorReset : BOOL; (*Variable to reset the error*)
        BrakeControl : BOOL; (*Variable for signal to open the holding brake*)
        SetSpeed : REAL; (*Variable for the setpoint speed value*)
        SetPosition : DINT; (*Variable for the setpoint position value*)
    END_STRUCT;

    McPureVAxGeneralPvInterfaceType : STRUCT
        DigitalInputs : McDigitalInputsPvIfType; (*Variable for digital inputs*)
        StatusInputs : McPureVAxStatusInputsPvIfType; (*Variable for status inputs*)
        ControlOutputs : McPureVAxControlOutputsPvIfType; (*Variable for control outputs*)
    END_STRUCT;

    McPureVAxDS402CspAdapterPvType : STRUCT
        DigitalInputs : McDigitalInputsPvIfType; (*Variable for digital inputs*)
        ActualPosition : DINT; (*Variable for the actual position value*)
        ModuleOk : BOOL; (*Variable for communication ready status signal*)
        StatusWord : UINT; (*Variable for the status word*)
        ErrorCode : UINT; (*Variable for number specifying the occurred error or warning*)
        ControlWord : UINT; (*Variable for the control word*)
        TargetPosition : DINT; (*Variable for the target position value*)
    END_STRUCT;

    McPureVAxDS402VlAdapterPvType : STRUCT
        DigitalInputs : McDigitalInputsPvIfType; (*Variable for digital inputs*)
        ActualVelocity : INT; (*Variable for the actual velocity value*)
        ModuleOk : BOOL; (*Variable for communication ready status signal*)
        StatusWord : UINT; (*Variable for the status word*)
        ErrorCode : UINT; (*Variable for number specifying the occurred error or warning*)
        ControlWord : UINT; (*Variable for the control word*)
        TargetVelocity : INT; (*Variable for the target velocity value*)
    END_STRUCT;

    McPureVAxRestorePosType: STRUCT
		Data : ARRAY[0..17] OF DINT; (*Data for restoring the position*)
	END_STRUCT;

END_TYPE