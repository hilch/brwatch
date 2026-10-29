
FUNCTION_BLOCK MC_BR_InitHome_AcpAx (*initializes homing parameters for usage with MC_Home*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		HomingParameters : McAcpAxHomingParType; (*parameters used with MC_Home if HomingMode=mcHomeInit*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ProcessDataBlock_AcpAx (*reads or writes the value of a data block according to the input "Mode"*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType;  (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		ParID : UINT; (*Parameter ID number of the data block to be read or written*)
		DataAddress : UDINT; (*address of the data buffer for the data block*)
		DataLength : UDINT; (*size of the data buffer for the data block transfer*)
		Mode : McAcpAxProcessDataBlockModeEnum; (*selection if the data block should be read or written*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		DataBlockLength : UDINT; (*length of the read data block*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ProcessParID_AcpAx (*reads or writes the value of a ParID according to the input "Mode"*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType;  (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		DataAddress : UDINT; (*address of an array with data type MCProcessParIDAcpType*)
		NumberOfParIDs : UDINT; (*number of ParIDs which should be read/written*)
		Mode : McAcpAxProcessParIDModeEnum; (*selection if the ParIDs should be read or written*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ProcessParTab_AcpAx (*transfers parameters of an ACOPOS parameter table according to the input "Mode"*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType;  (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Data : McAcpAxProcessParTabDataType; (*data of the ACOPOS parameter table*)
		Mode : McAcpAxProcessParTabModeEnum; (*mode for the transfer of the ACOPOS parameter(s)*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		AdditionalInfo : McAcpAxProcessParTabAddInfoType; (*additional information*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_CyclicProcessParID_AcpAx (*reads or writes the value of a ParID cyclic according to the input "Mode"*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType;  (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
		DataAddress : UDINT; (*address of an array with data type MC_CYCLIC_PARID_ACP_TYPE*)
		NumberOfParIDs : UDINT; (*number of ParIDs which should be read/written cyclically*)
		Mode : McAcpAxCycParIDModeEnum; (*selection if the ParIDs should be read/written cyclically*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*FB reads/writes ParIDs cyclically*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_MoveAbsoluteTrgStop_AcpAx (*commands a controlled motion to a specified absolute position, at trigger event a specified distance is moved*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Position : LREAL; (*target position for the motion*)
		Velocity : REAL; (*maximum velocity*)
		Acceleration : REAL; (*maximum acceleration*)
		Deceleration : REAL; (*maximum deceleration*)
		Direction : McDirectionEnum; (*movement direction*)
		BufferMode : McBufferModeEnum; (*defines the chronological sequence of FB*)
		TriggerStopParameters : McAcpAxTriggerStopType; (*parameters used for defining the trigger source*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Active : BOOL; (*FB has control over the axis*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		StoppedAtTargetPosition : BOOL; (*movement was stopped at specified target position*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_MoveAdditiveTrgStop_AcpAx (*commands a controlled motion for the specified distance, at trigger event a specified distance is moved*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Distance : LREAL; (*distance for the motion*)
		Velocity : REAL; (*maximum velocity*)
		Acceleration : REAL; (*maximum acceleration*)
		Deceleration : REAL; (*maximum deceleration*)
		BufferMode : McBufferModeEnum; (*defines the chronological sequence of FB*)
		TriggerStopParameters : McAcpAxTriggerStopType; (*parameters used for defining the trigger source*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Active : BOOL; (*FB has control over the axis*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		StoppedAtTargetPosition : BOOL; (*movement was stopped at specified target position*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_MoveVelocityTrgStop_AcpAx (*commands a controlled motion with a specified velocity, at trigger event a specified distance is moved*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Velocity : REAL; (*maximum velocity*)
		Acceleration : REAL; (*maximum acceleration*)
		Deceleration : REAL; (*maximum deceleration*)
		Direction : McDirectionEnum; (*movement direction*)
		BufferMode : McBufferModeEnum; (*defines the chronological sequence of FB*)
		TriggerStopParameters : McAcpAxTriggerStopType; (*parameters used for defining the trigger source*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Active : BOOL; (*FB has control over the axis*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		InVelocity : BOOL; (*Commanded velocity reached*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_LimitLoad_AcpAx (*limits the motor torque to a set value*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (*when set limitation is activated*)
		LoadPosAccel : REAL; (*limit for acceleration in positive direction [Nm]*)
		LoadPosDecel : REAL; (*limit for deceleration in positive direction [Nm]*)
		LoadNegAccel : REAL; (*limit for acceleration in negative direction [Nm]*)
		LoadNegDecel : REAL; (*limit for deceleration in negative direction [Nm]*)
		InitData : BOOL; (*initialize new parameters*)
		Mode : McAcpAxLimitLoadModeEnum; (*torque limitation mode*)
		AdvancedParameters : McAcpAxAdvLimitLoadParType; (*structure for using additional functions*)
    END_VAR
    VAR_OUTPUT
		Busy : BOOL; (*FB is active and needs to be called*)
	    Ready : BOOL; (*parameters initialized*)
		Active : BOOL; (*torque is limited*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		DataInitialized : BOOL; (*new parameters are initialized*)
    END_VAR
    VAR
		Internal : McInternalType; (*internal variable*)
    END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_BrakeSetPar_AcpAx (*sets  parameters for the holding brake*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Parameters : McAcpAxBrakeParType; (*brake  parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_BrakeTest_AcpAx (*sets  parameters and starts test for the holding brake*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Command :  McAcpAxBrakeTestCmdEnum; (*selects the command for this FB*)
		Parameters :  McAcpAxBrakeTestParType; (*brake test  parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		PositionError : LREAL; (*Maximal position error during brake test*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_LoadSimulationSetPar_AcpAx (*transfers parameters for the load simulation on the axis*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Parameters : McAcpAxSimulationParType; (*parameters used for defining the simulated load*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_LoadSimulationGetPar_AcpAx (*reads parameters for the load simulation from the axis*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Parameters : McAcpAxSimulationParType; (*parameters used for defining the simulated load*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_GetDriveInfo_AcpAx (*read information from the axis and the corresponding drive*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		AxisInfo : McAcpAxAxisInfoType; (*output structure for axis information*)
		ModuleInfo : McAcpAxModuleInfoType; (*output structure for module information*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneSpeedCtrl_AcpAx (*execute auto tuning for speed controller*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Parameters : McAcpAxAutoTuneParType; (*auto tuning parameters*)
		AdvancedParameters : McAcpAxAdvAutoTuneSpeedCtrlType; (*advanced auto tuning  parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTuneSpeedCtrlOutType; (*output parameters of auto tuning speed control*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneLoopFilters_AcpAx (*execute auto tuning for loop filters*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		LoopFilters : McAcpAxAutoTuneLoopFiltersType; (*loop filters parameters*)
		Parameters : McAcpAxAutoTuneParType; (*auto tuning parameters*)
		AdvancedParameters : McAcpAxAdvAutoTuneLoopFilterType; (*advanced auto tuning parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTuneLoopFilterOutType; (*output parameters of auto tuning loop filters*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTunePositionCtrl_AcpAx (*execute auto tuning for position controller*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Parameters : McAcpAxAutoTuneParType; (*auto tuning parameters*)
		AdvancedParameters : McAcpAxAdvAutoTunePosCtrlType; (*advanced auto tuning parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTunePosCtrlOutType; (*output parameters of auto tuning position control*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneTest_AcpAx (*tests the stability of the controller*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Mode : McAcpAxAutoTuneTestModeEnum; (*mode for the stability test of the controller*)
		Parameters : McAcpAxAutoTuneParType; (*auto tuning parameters*)
		AdvancedParameters : McAcpAxAdvAutoTuneTestType; (*advanced auto tuning parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTuneTestOutType; (*output parameters of auto tuning test*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneInductMotor_AcpAx (*tune an induction motor*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Mode : McAcpAxAutoTuneMotorModeEnum; (*Mode for the auto tuning*)
		Parameters : McAcpAxAutoTuneIndMotParType; (*auto tuning parameters*)
		AdvancedParameters : McAcpAxAdvAutoTuneIndMotType; (*advanced auto tuning parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTuneIndMotOutType; (*output parameters of auto tuning for induction motor*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneSyncMotor_AcpAx (*tune a synchronous motor*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Mode : McAcpAxAutoTuneMotorModeEnum; (*Mode for the auto tuning*)
		Parameters : McAcpAxAutoTuneSyncMotParType; (*auto tuning parameters*)
		AdvancedParameters : McAcpAxAdvAutoTuneSyncMotType; (*advanced auto tuning parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTuneSyncMotOutType; (*output parameters of auto tuning for synchronous motor*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneMotorPhasing_AcpAx (*tune for phasing*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Mode : McAcpAxAutoTuneMotPhasModeEnum; (*Mode for the phasing auto tuning*)
		Parameters : McAcpAxAutoTuneMotPhasParType; (*auto tuning parameters*)
		AdvancedParameters : McAcpAxAdvAutoTuneMotPhasType; (*advanced auto tuning parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTuneMotPhasOutType; (*output parameters of auto tuning for motor phasing*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneLoadModel_AcpAx (*tune load model parameters*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Parameters : McAcpAxAutoTuneParType; (*auto tuning parameters*)
		AdvancedParameters :  McAcpAxAdvAutoTuneLoadModelType; (*advanced auto tuning parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output :  McAcpAxAutoTuneLoadModelOutType; (*output parameters of auto tuning for motor phasing*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_AutoTuneFeedForward_AcpAx (*execute auto tuning for feed forward*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		Command : McAcpAxAutoTuneFeedFwdCmdEnum;
		Mode : McAcpAxAutoTuneFeedFwdModeEnum; (*operating (identification) mode*)
		Parameters : McAcpAxAutoTuneFeedFwdParType; (*basic operating parameters*)
		AdvancedParameters : McAcpAxAdvAutoTuneFeedFwdType; (*advanced operating parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		CommandAborted : BOOL; (*Command was aborted by another command*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Output : McAcpAxAutoTuneFeedFwdOutType; (*output parameters of auto tuning feed forward*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK  MC_BR_PhasingVelocity_AcpAx (*creates phase shift in the master position of a slave axis using a velocity motion profile*)
	VAR_INPUT
		Slave : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
		Acceleration : REAL; (*maximum acceleration to reach additive velocity*)
		AdvancedParameters : McAcpAxAdvPhasingParType; (*structure for using advanced parameters*)
		CyclicVelocity : REAL; (*cyclically transferred additive velocity*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*initialization complete, "CyclicVelocity" is processed*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		VelocityAttained : BOOL; (*additive velocity reached*)
		ActualPhaseShift : LREAL; (*currently covered phase shift*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK  MC_BR_OffsetVelocity_AcpAx (*creates phase shift in the  slave position of a slave axis using a velocity motion profile*)
	VAR_INPUT
		Slave : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
		Acceleration : REAL; (*maximum acceleration to reach additive velocity*)
		AdvancedParameters : McAcpAxAdvPhasingParType; (*structure for using advanced parameters*)
		CyclicVelocity : REAL; (*cyclically transferred additive velocity*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*initialization complete, "CyclicVelocity" is processed*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		VelocityAttained : BOOL; (*additive velocity reached*)
		ActualOffsetShift : LREAL; (*currently covered phase shift*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_WrLoadSimTorque_AcpAx (*transmit the simulated torque from the MTLoadSim function blocks to the axis being used*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
		Torque : REAL; (*Simulated torque that is transmitted to the axis [Nm]*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*Torque is being transferred to the axis*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_RdLoadSimTorque_AcpAx (*reads air gap torque ACOPOS simulation on PLC *)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (* The function block is active as long as this input is set *)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*Indicates that the value output at the "Torque" output is valid*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Torque : REAL; (*Air gap torque of the electric motor [Nm]*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_WrLoadSimPosition_AcpAx (*transmit the simulated position of the rotor to the axis being used*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
		Position : LREAL; (*Simulated angular position of the rotor that is transmitted to the axis [rad]*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*Position is being transferred to the axis*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_WrLoadSimTwoEncPos_AcpAx (*transfer the simulated position of the rotor and simulated position of the load to the axis being used*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
		MotorPosition : LREAL; (*Simulated angular position of the rotor that is transferred to the axis [rad]*)
		LoadPosition : LREAL; (*Simulated angular position of the load encoder that is transmitted to the axis [rad]*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*Positions are being transferred to the axis*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_RdLoadSimInputData_AcpAx (* This function block provides the values needed for the MTLoadSimCoupling function block *)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (* The function block is active as long as this input is set *)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*Indicates that the value output at the "Torque" output is valid*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		LoadSimInputData : McAcpAxLoadSimInputDataType; (*Data which are needed for the load simulation. (E.g. for the FB MTLoadSimCoupling)*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ApsmPowerOn_AcpAx (* This function block switches on the output of an ACOPOS auxiliary power supply module*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Indicates whether the output is on*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		CommandAborted : BOOL; (*Command aborted by another command.*)
		Error : BOOL; (*Execution error*)
		ErrorID : DINT; (*Error number*)
	END_VAR
	VAR
		Internal : McInternalType;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ApsmPowerOff_AcpAx (* This function block switches off the output of an ACOPOS auxiliary power supply module*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Indicates whether the output is off*)
		Busy : BOOL; (*Function block is active and must continue to be called.*)
		CommandAborted : BOOL; (*Command aborted by another command.*)
		Error : BOOL; (*Execution error*)
		ErrorID : DINT; (*Error number*)
	END_VAR
	VAR
		Internal : McInternalType;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_InitParIDTransfer_AcpAx (*This function block initializes a cyclic ParID transfer from the master to the slave*)
	VAR_INPUT
		Master : REFERENCE TO McAxisType; (*master axis reference*)
		Slave : REFERENCE TO McAxisType; (*slave axis reference*)
		Execute : BOOL; (*initializes transfer on a rising edge*)
		MasterParID : UINT; (*ParID whose value shall be transferred from the master axis to the slave axis*)
		InterpolationMode : McIplModeEnum; (*Interpolation mode for the received value*)
		AdvancedParameters : McAcpAxAdvInitParIDTransferType; (*Additional advanced parameters*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		SlaveReceiveParID : UINT; (* The received ParID on the slave axis*)
	END_VAR
	VAR
		Internal : McInternalTwoRefType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_GetParIDTransferInfo_AcpAx (*This function block provides administrative information about the ParID transfer of an axis.*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*Axis reference.*)
		Execute : BOOL; (*Gets transfer information on a rising edge.*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. FB finished.*)
		Busy : BOOL; (*FB is active and needs to be called.*)
		Error : BOOL; (*Error occurred during operation.*)
		ErrorID : DINT; (*Error number.*)
		FreeMasterSendChannels : USINT; (*Number of send channels still available for the axis.*)
		FreeSlaveReceiveChannels : USINT; (*Number of receive channels still available for the axis.*)
		ParIDTransferInfo : McAcpAxParIDTransferInfoType; (*Detailed information about the current configuration of the send and receive channels for the axis.*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal variable.*)
 	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_InitReceiveNetData_AcpAx (*This function block can be used to read 2-byte or 4-byte information from a POWERLINK station to a drive within the network.*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*Axis reference.*)
		Execute : BOOL; (*Activates cyclic receiving of POWERLINK data at rising edge.*)
		ChannelMapping : STRING[250]; (*Channel mapping of the data source.*)
		DataType : McAcpAxDataTypeEnum; (*Data type of the POWERLINK data.*)
		InterpolationMode : McIplModeEnum; (*Interpolation mode for the received value.*)
		AdvancedParameters : McAcpAxAdvInitReceiveNetDataType; (*Additional advanced parameters.*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. FB finished.*)
		Busy : BOOL; (*FB is active and needs to be called.*)
		Error : BOOL; (*Error occurred during operation.*)
		ErrorID : DINT; (*Error number.*)
		ReceiveParID : UINT; (*The received ParID on the axis.*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal variable.*)
 	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ReceiveParIDOnPLC_AcpAx (*This function block cyclically reads the value of a ParID, transmitted by an axis, on the PLC.*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*Axis reference of the ParID source.*)
		Enable : BOOL; (*Enables the cyclic reading of the ParID *)
		ParID : UINT; (*Source ParID.*)
		AdvancedParameters : McAcpAxAdvReceiveParIDOnPLCType; (*Additional advanced parameters.*)
 	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*Initialization completed, FB is reading and outputting ParID.*)
		Busy : BOOL; (*FB is active and needs to be called.*)
		Error : BOOL; (*Error occurred during operation.*)
		ErrorID : DINT; (*Error number.*)
		CyclicValue : LREAL; (*Cyclically read value in raw format (i.e. the same value what is present on the drive).*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal variable.*)
 	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_GetCyclicDataInfo_AcpAx (*This function block provides administrative information about telegram records that are cyclically written to and read from a drive channel.*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*Axis reference.*)
		Execute : BOOL; (*Get information on rising edge.*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*Execution successful. FB finished.*)
		Busy : BOOL; (*FB is active and needs to be called.*)
		Error : BOOL; (*Error occurred during operation.*)
		ErrorID : DINT; (*Error number.*)
		CyclicDataInfo : McAcpAxCyclicDataInfoType; (*Information about the configuration of the cyclic write (to) and cyclic read (from) telegram records of the drive channel hosting the axis.*)
	END_VAR
	VAR
		Internal : McInternalType; (*Internal variable.*)
 	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_SctrlLimitLoad_AcpAx (*Limits the motor torque using the velocity controller current reference value*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*Axis reference.*)
		Enable : BOOL; (*The FB is active as long as this input is set.*)
		InitData : BOOL; (*Initialize new parameters.*)
		LoadPositive : REAL; (*Positive load (torque) limit value [Nm].*)
		LoadNegative : REAL; (*Negative load (torque) limit value [Nm].*)
		Mode : McAcpAxSctrlLimitLoadModeEnum; (*Torque limitation mode.*)
		AdvancedParameters : McAcpAxAdvSctrlLimitLoadParType; (*Additional advanced parameters.*)
    END_VAR
    VAR_OUTPUT
		Ready : BOOL; (*Torque limitation function on the axis is ready.*)
		Busy : BOOL; (*FB is active and needs to be called.*)
		Error : BOOL; (*Error occurred during operation.*)
		ErrorID : DINT; (*Error number.*)
		DataInitialized : BOOL; (*New parameters are initialized.*)
		LimitPositiveActive : BOOL; (*Positive load (torque) limitation is active.*)
		LimitNegativeActive : BOOL; (*Negative load (torque) limitation is active.*)
    END_VAR
    VAR
		Internal : McInternalType; (*internal variable*)
    END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_SetParIDText_AcpAx (*Set (write) a single ACOPOS parameter in text format.*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*Axis reference.*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		ParID : UINT; (*ACOPOS parameter ID to be set (read)*)
		DataText : STRING[32];
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_GetParIDText_AcpAx (*Get (read) a single ACOPOS parameter in text format.*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*Axis reference.*)
		Execute : BOOL; (*execution of this FB is started on rising edge of the input*)
		ParID : UINT; (*ACOPOS parameter ID to be got (written)*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL; (*execution successful. FB finished*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		DataText : STRING[32];
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ReadSafeOutData_AcpAx (*reads SafeOUT data of an SafeMOTION axis*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType;  (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*FB output values can be used*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		SafeOutData : McAcpAxSafeOutDataType; (*SafeOUT data*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_ReadSafeInData_AcpAx (*reads SafeIN data of an SafeMOTION axis*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType;  (*axis reference*)
		Enable : BOOL; (*FB is active as long as input is set*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL; (*FB output values can be used*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		SafeInData : McAcpAxSafeInDataType; (*SafeIN data*)
	END_VAR
	VAR
		Internal : McInternalType; (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK MC_BR_VibrationCtrlStatus_AcpAx (*outputs vibratory control status*)
	VAR_INPUT
		Axis : REFERENCE TO McAxisType; (*axis reference*)
		Enable : BOOL; (*when set limitation is activated*)
	END_VAR
    VAR_OUTPUT
		Valid : BOOL; (*FB output values can be used*)
		Busy : BOOL; (*FB is active and needs to be called*)
		Error : BOOL; (*error occurred during operation*)
		ErrorID : DINT; (*error number*)
		Status : McAcpAxVibCtrlStatusType; (*Status info*)
	END_VAR
    VAR
		Internal : McInternalType; (*internal variable*)
    END_VAR
END_FUNCTION_BLOCK
