REM sd edit ...
copy %1\BoogieAbsInt.dll .
copy %1\BoogieAbsInt.pdb .
copy %1\AliasAnalysis.exe .
copy %1\AliasAnalysis.pdb .
copy %1\AvHarnessInstrumentation.exe .
copy %1\AvHarnessInstrumentation.pdb .
copy %1\AngelicVerifierNull.exe .
copy %1\AngelicVerifierNull.pdb .
copy %1\AvUtil.dll .
copy %1\AvUtil.pdb .
copy %1\BoogieBasetypes.dll .
copy %1\BoogieBasetypes.pdb .
copy %1\BoogieCodeContractsExtender.dll .
copy %1\BoogieCodeContractsExtender.pdb .
copy %1\BoogieCore.dll .
copy %1\BoogieCore.pdb .
copy %1\CorralCoreLib.dll .
copy %1\CorralCoreLib.pdb .
copy %1\BoogieConcurrency.dll .
copy %1\BoogieConcurrency.pdb .
copy %1\ExplainError.dll .
copy %1\ExplainError.pdb .
copy %1\BoogieExecutionEngine.dll .
copy %1\BoogieExecutionEngine.pdb .
copy %1\BoogieGraph.dll .
copy %1\BoogieGraph.pdb .
copy %1\BoogieHoudini.dll .
copy %1\BoogieHoudini.pdb .
copy %1\BoogieModel.dll .
copy %1\BoogieModel.pdb .
copy %1\BoogieParserHelper.dll .
copy %1\BoogieParserHelper.pdb .
copy %1\ProgTransformation.dll .
copy %1\ProgTransformation.pdb .
copy %1\PropInstUtils.dll .
copy %1\PropInstUtils.pdb .
copy %1\Provers.SMTLib.dll .
copy %1\Provers.SMTLib.pdb .
copy %1\TraceDebugger.dll .
copy %1\TraceDebugger.pdb .
copy %1\CorralUtil.dll .
copy %1\CorralUtil.pdb .
copy %1\BoogieVCExpr.dll .
copy %1\BoogieVCExpr.pdb .
copy %1\BoogieVCGeneration.dll .
copy %1\BoogieVCGeneration.pdb .
copy %1\corral.exe .
copy %1\corral.pdb .
copy %1\FastAvn.exe .
copy %1\FastAvn.pdb .
copy %1\PropInst.exe .
copy %1\PropInst.pdb .
copy %1\ExampleProperties\*.avp ExampleProperties\
copy corral.exe corral32.exe
corflags.exe /32BIT+ corral32.exe

