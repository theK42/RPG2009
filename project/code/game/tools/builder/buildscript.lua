require "luabuildlib"
require "resdblib"

function BuildScript(ScriptInputName, ResourceOutputName)
	scriptObject = LuaBuildLib.CreateLuaTextResourceObject(ScriptInputName)
	builder = ResDBLib.CreateResDBBuilder()
	builder:AddObject(scriptObject)
	outputFile = ResDBLib.CreateSyncFile()
	outputFile:Open(ResourceOutputName)
	builder:Finalize(outputFile, true)
end