TYPE
	McAGAPhsAxAxType : STRUCT
		AxisReference : McCfgReferenceType; (*Name of the axis reference from mapp axis configuration (e.g. gAxis1)*)
	END_STRUCT;
	McAGAPhsAxType : STRUCT (*Defines the axes which are part of the axes group*)
		Axis : McCfgUnboundedArrayType; (*Connect array of type McAGAPhsAxAxType*)
	END_STRUCT;
	McAGSRQSPwrOnAStopEnum :
		( (*Power on after stop selector setting*)
		mcAGSRQSPONAS_NOT_USE := 0, (*Not used - Quickstop functionality is disabled*)
		mcAGSRQSPONAS_USE := 1 (*Used - Quickstop functionality is enabled*)
		);
	McAGSRQSPUseSrcEnum :
		( (*Source selector setting*)
		mcAGSRQSPUS_VAR := 0, (*Variable - Use PV as trigger source*)
		mcAGSRQSPUS_IO_CH := 1 (*I/O channel - Get value from an I/O channel*)
		);
	McAGSRQSPUseSrcVarType : STRUCT (*Type mcAGSRQSPUS_VAR settings*)
		PVMapping : STRING[250];
	END_STRUCT;
	McAGSRQSPUseSrcIOChType : STRUCT (*Type mcAGSRQSPUS_IO_CH settings*)
		ChannelMapping : STRING[250]; (*Input source for status input*)
	END_STRUCT;
	McAGSRQSPUseSrcType : STRUCT (*Source which is used for this functionality*)
		Type : McAGSRQSPUseSrcEnum; (*Source selector setting*)
		Variable : McAGSRQSPUseSrcVarType; (*Type mcAGSRQSPUS_VAR settings*)
		IOChannel : McAGSRQSPUseSrcIOChType; (*Type mcAGSRQSPUS_IO_CH settings*)
	END_STRUCT;
	McAGSRQSPUseLvlEnum :
		( (*Level of the source which leads to an stop reaction*)
		mcAGSRQSPUL_LOW := 0, (*Low - Low level of source triggers the stop reaction*)
		mcAGSRQSPUL_HIGH := 1 (*High - High level of source triggers the stop reaction*)
		);
	McAGSRQSPUseType : STRUCT (*Type mcAGSRQSPONAS_USE settings*)
		Source : McAGSRQSPUseSrcType; (*Source which is used for this functionality*)
		Level : McAGSRQSPUseLvlEnum; (*Level of the source which leads to an stop reaction*)
	END_STRUCT;
	McAGSRQSPwrOnAStopType : STRUCT (*Controller stays in status on after stop reaction*)
		Type : McAGSRQSPwrOnAStopEnum; (*Power on after stop selector setting*)
		Used : McAGSRQSPUseType; (*Type mcAGSRQSPONAS_USE settings*)
	END_STRUCT;
	McAGSRQSPwrOffAStopEnum :
		( (*Power off after stop selector setting*)
		mcAGSRQSPOFFAS_NOT_USE := 0, (*Not used - Quickstop functionality is disabled*)
		mcAGSRQSPOFFAS_USE := 1 (*Used - Quickstop functionality is enabled*)
		);
	McAGSRQSPwrOffAStopType : STRUCT (*Controller is switched off after stop reaction*)
		Type : McAGSRQSPwrOffAStopEnum; (*Power off after stop selector setting*)
		Used : McAGSRQSPUseType; (*Type mcAGSRQSPOFFAS_USE settings*)
	END_STRUCT;
	McAGSRQSType : STRUCT (*Enables Quickstop functionality for the axis group*)
		PowerOnAfterStop : McAGSRQSPwrOnAStopType; (*Controller stays in status on after stop reaction*)
		PowerOffAfterStop : McAGSRQSPwrOffAStopType; (*Controller is switched off after stop reaction*)
	END_STRUCT;
	McAGAGFType : STRUCT
		FeatureReference : McCfgUnboundedArrayType; (*Name of the axes group feature reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McCfgAxGrpAdminType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AXGRP_ADMIN*)
		ProcessingTaskClass : McPTCEnum; (*Cyclic task class for command processing*)
		PhysicalAxes : McAGAPhsAxType; (*Defines the axes which are part of the axes group*)
		AxesGroupFeatures : McAGAGFType;
	END_STRUCT;
	McAGFHOTogGrpType : STRUCT
		AxisReference : McCfgUnboundedArrayType; (*Name of the axis reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McCfgAxGrpFeatHomeOrdType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AXGRP_FEAT_HOME_ORD*)
		TogetherGroup : McCfgUnboundedArrayType; (*Connect array of type McAGFHOTogGrpType*)
	END_STRUCT;
	McAGFPOOTogGrpType : STRUCT
		AxisReference : McCfgUnboundedArrayType; (*Name of the axis reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McCfgAxGrpFeatPwrOnOrdType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AXGRP_FEAT_PWR_ON_ORD*)
		TogetherGroup : McCfgUnboundedArrayType; (*Connect array of type McAGFPOOTogGrpType*)
	END_STRUCT;
	McAGFESAGrpStopExType : STRUCT (*Group stop command will have no effect on the axes referenced in this list*)
		AxisReference : McCfgUnboundedArrayType; (*Name of the axis reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McAGFESAGrpOvrExType : STRUCT (*Group axis override will have no effect on the axes referenced in this list*)
		AxisReference : McCfgUnboundedArrayType; (*Name of the axis reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McCfgAxGrpFeatExSngAxType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AXGRP_FEAT_EX_SNG_AX*)
		GroupStopExclusion : McAGFESAGrpStopExType; (*Group stop command will have no effect on the axes referenced in this list*)
		GroupOverrideExclusion : McAGFESAGrpOvrExType; (*Group axis override will have no effect on the axes referenced in this list*)
	END_STRUCT;
	McAGFSBSBCAxType : STRUCT
		AxisReference : McCfgUnboundedArrayType; (*Name of the axis reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McAGFSBSBCErrBxEnum :
		( (*Error behavior selector setting*)
		mcAGFSBSBCEB_CLOSE_IMMED := 0, (*Close immediately - Close Brake immediately, risk of abraision of the brakes*)
		mcAGFSBSBCEB_CLOSE_A_STOP := 1 (*Close after stop - Close Brake after stop to prevent damage. Robot may fall down!*)
		);
	McAGFSBSBCErrBxType : STRUCT (*Defines the reaction to the composite if one axis gets error with CTRL off*)
		Type : McAGFSBSBCErrBxEnum; (*Error behavior selector setting*)
	END_STRUCT;
	McAGFSBSBCOutEnum :
		( (*Output selector setting*)
		mcAGFSBSBCO_AX := 0, (*Axis - Standard brake output of ACOPOS is used*)
		mcAGFSBSBCO_VAR := 1 (*Variable - A variable that is connected to the output which controls the brakes*)
		);
	McAGFSBSBCOutAxType : STRUCT (*Type mcAGFSBSBCO_AX settings*)
		AxisReference : McCfgReferenceType; (*Name of the axis reference*)
	END_STRUCT;
	McAGFSBSBCOutVarType : STRUCT (*Type mcAGFSBSBCO_VAR settings*)
		PVMapping : STRING[250]; (*Process variable to activate the holding brake*)
	END_STRUCT;
	McAGFSBSBCOutType : STRUCT (*Defines the brake signal destination*)
		Type : McAGFSBSBCOutEnum; (*Output selector setting*)
		Axis : McAGFSBSBCOutAxType; (*Type mcAGFSBSBCO_AX settings*)
		Variable : McAGFSBSBCOutVarType; (*Type mcAGFSBSBCO_VAR settings*)
	END_STRUCT;
	McAGFSBSBCFdbkEnum :
		( (*Feedback selector setting*)
		mcAGFSBSBCF_TIME_BASED := 0 (*Time based - Brake is activated/released after a certain waiting time*)
		);
	McAGFSBSBCFdbkTimeBasedType : STRUCT (*Type mcAGFSBSBCF_TIME_BASED settings*)
		ActivationDelay : REAL; (*Holding torque build-up time after switching off the operating voltage [s]*)
		ReleaseDelay : REAL; (*Holding torque decaying time after switching on the operating voltage [s]*)
	END_STRUCT;
	McAGFSBSBCFdbkType : STRUCT (*Status intformation of the brake*)
		Type : McAGFSBSBCFdbkEnum; (*Feedback selector setting*)
		TimeBased : McAGFSBSBCFdbkTimeBasedType; (*Type mcAGFSBSBCF_TIME_BASED settings*)
	END_STRUCT;
	McAGFSBSBCType : STRUCT
		Axes : McAGFSBSBCAxType;
		ErrorBehavior : McAGFSBSBCErrBxType; (*Defines the reaction to the composite if one axis gets error with CTRL off*)
		Output : McAGFSBSBCOutType; (*Defines the brake signal destination*)
		Feedback : McAGFSBSBCFdbkType; (*Status intformation of the brake*)
	END_STRUCT;
	McCfgAxGrpFeatShrBrkSigType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AXGRP_FEAT_SHR_BRK_SIG*)
		BrakeComposite : McCfgUnboundedArrayType; (*Connect array of type McAGFSBSBCType*)
	END_STRUCT;
	McAGFModalDatBxEnum :
		( (*Modal data behaviour selector setting*)
		mcAGFMDB_USE_AX_GRP_SET := 0, (*Use axes group settings - The settings from the axes group are used.*)
		mcAGFMDB_RST_TO_DEF := 1, (*Reset to default - The values are reseted to the configured/default values at program end.*)
		mcAGFMDB_KEEP_CUR_VAL := 2 (*Keep current values - The values at program end are used for the next program.*)
		);
	McAGFModalDatBxType : STRUCT (*Defines the modal data behaviour of the feature*)
		Type : McAGFModalDatBxEnum; (*Modal data behaviour selector setting*)
	END_STRUCT;
END_TYPE