#include "Generator.h"

#include <cassert>
#include <vector>

namespace Bloodshot::HeaderTool
{
	FGeneratorOutput FGenerator::Generate(const FHeaderFileInfo& HeaderInfo)
	{
		Clear();
		CurrentOutputStream = &SourceOutputStream;

		WriteLine("// Generated code by HeaderTool");
		WriteLine("// Do not modify this manually!");
		WriteLine("#include \"Object/ObjectGlobals.h\"");
		WriteLine("#include \"{}\"", HeaderInfo.Path.string());
		WriteLine("namespace Bloodshot");
		WriteLine("{");
		PushScope();
		WriteClassConstructionImpl(HeaderInfo);
		PopScope();
		WriteLine("}");

		return FGeneratorOutput{SourceOutputStream.str(), HeaderOutputStream.str()};
	}

	void FGenerator::WriteLine(std::string_view String)
	{
		for (size_t i = 0; i < PushedScopes; ++i)
		{
			*CurrentOutputStream << "\t";
		}

		*CurrentOutputStream << String << "\n";
	}

	void FGenerator::PushScope()
	{
		++PushedScopes;
	}

	void FGenerator::PopScope()
	{
		assert(PushedScopes && "PopScope() called when PushedScopes == 0");
		--PushedScopes;
	}

	void FGenerator::Clear()
	{
		SourceOutputStream = std::stringstream();
		HeaderOutputStream = std::stringstream();

		CurrentOutputStream = nullptr;
		PushedScopes = 0;
	}

	void FGenerator::WriteClassConstructionImpl(const FHeaderFileInfo& HeaderInfo)
	{
		for (const FClassInfo& ClassInfo : HeaderInfo.ClassInfos)
		{
			static uint32_t ClassUniqueID = 1;
			const std::string& QualifiedClassName = ClassInfo.Namespace + "::" + ClassInfo.Name;

			WriteLine("namespace Private");
			WriteLine("{");
			PushScope();

			for (const FFunctionInfo& Function : ClassInfo.Functions)
			{
				WriteLine("template<IsObject ObjectType, typename... ArgTypes>");

				std::string Buffer;

				Buffer += std::format("struct IFunctionCaller<{}, {} ({}::*)(", QualifiedClassName, Function.ReturnType, QualifiedClassName);

				for (const FParameterInfo& Parameter : Function.Parameters)
				{
					Buffer += Parameter.Type;
					Buffer += ",";
				}

				if (Function.Parameters.size())
				{
					Buffer.pop_back();
				}

				Buffer += "), ObjectType, ArgTypes...> final";

				WriteLine(Buffer);
				WriteLine("{");
				PushScope();
				WriteLine("static void Call(ObjectType* Object, ArgTypes&&... Args)");
				WriteLine("{");
				PushScope();
				WriteLine("(void)Object->{}(std::forward<ArgTypes>(Args)...);", Function.Name);
				PopScope();
				WriteLine("}");
				PopScope();
				WriteLine("};");
			}

			WriteLine("template<>");
			WriteLine("FClass* Object::FObjectCore::ConstructClass<{}>({}* const InObject, void* const InMemory)", QualifiedClassName, QualifiedClassName);
			WriteLine("{");
			PushScope();

			const size_t BaseClassCount = ClassInfo.BaseClassNames.size();
			const size_t PropertyCount = ClassInfo.Properties.size();
			const size_t FunctionCount = ClassInfo.Functions.size();
			size_t ParameterCount = 0;

			for (const FFunctionInfo& Function : ClassInfo.Functions)
			{
				ParameterCount += Function.Parameters.size();
			}

			WriteLine("void* const Memory = InMemory ? InMemory : FMemory::Malloc(sizeof(FClass) + sizeof(FClass) * {} + sizeof(FProperty) * {} + sizeof(FFunction) * {} + sizeof(FParameter) * {});",
				BaseClassCount,
				PropertyCount,
				FunctionCount,
				ParameterCount);

			WriteLine("FClass* const Class = (FClass*)Memory;");
			WriteLine("Class->Name = \"{}\";", ClassInfo.Name);
			WriteLine("Class->Namespace = \"{}\";", ClassInfo.Namespace);
			WriteLine("Class->bAbstract = {};", ClassInfo.bAbstract);
			WriteLine("Class->bFinal = {};", ClassInfo.bFinal);
			WriteLine("Class->bDerived = {};", ClassInfo.bDerived);
			WriteLine("Class->Size = sizeof({});", QualifiedClassName);
			WriteLine("Class->TypeID = {};", ClassUniqueID);

			WriteLine("FClass* BaseClassPtr = Class + 1;");
			WriteLine("Class->BaseClasses = BaseClassPtr;");

			for (const std::string& BaseClassName : ClassInfo.BaseClassNames)
			{
				WriteLine("TryConstructOrDefaultClass<{}>(({}*)InObject, \"{}\", BaseClassPtr);", 
					BaseClassName, 
					BaseClassName, 
					BaseClassName);
				WriteLine("++BaseClassPtr;");
			}
			
			WriteLine("Class->BaseClassCount = {};", BaseClassCount);
			WriteLine("Class->PropertyCount = 0;");
			WriteLine("Class->FunctionCount = 0;");

			//WriteLine("");
			//
			//
			//WriteLine("TArray<FProperty*> Properties;");
			//
			//for (const FPropertyInfo& PropertyInfo : ClassInfo.Properties)
			//{
			//	static size_t PropertyCount = 0;
			//	const std::string& PropertyName = std::format("Property_{}", PropertyCount++);
			//	WriteLine("FProperty* {} = new FProperty(", PropertyName);
			//	PushScope();
			//	WriteLine("\"{}\",", PropertyInfo.Type);
			//	WriteLine("\"{}\",", PropertyInfo.Name);
			//	WriteLine("{},", PropertyInfo.bStatic);
			//	WriteLine("{},", PropertyInfo.bSerialized);
			//	WriteLine("{},", PropertyInfo.bReplicated);
			//	WriteLine("(void*)&Object->{});", PropertyInfo.Name);
			//	PopScope();
			//	WriteLine("Properties.EmplaceBack({});", PropertyName);
			//}
			//
			//WriteLine("TArray<FFunction*> Functions;");
			//
			//for (const FFunctionInfo& FunctionInfo : ClassInfo.Functions)
			//{
			//	static size_t FunctionCount = 0;
			//	const std::string& FunctionName = std::format("Function_{}", FunctionCount++);
			//
			//	WriteLine("TArray<FParameter*> {}_Parameters;", FunctionName);
			//
			//	for (const FParameterInfo& ParameterInfo : FunctionInfo.Parameters)
			//	{
			//		static size_t ParameterCount = 0;
			//		const std::string& ParameterName = std::format("Parameter_{}", ParameterCount++);
			//		WriteLine("FParameter* {} = new FParameter(", ParameterName);
			//		PushScope();
			//		WriteLine("\"{}\",", ParameterInfo.Type);
			//		WriteLine("\"{}\");", ParameterInfo.Name);
			//		PopScope();
			//		WriteLine("{}_Parameters.EmplaceBack({});", FunctionName, ParameterName);
			//	}
			//
			//	WriteLine("FFunction* {} = new FFunction(", FunctionName);
			//	PushScope();
			//	WriteLine("\"{}\",", FunctionInfo.ReturnType);
			//	WriteLine("\"{}\",", FunctionInfo.Name);
			//	WriteLine("std::move({}_Parameters),", FunctionName);
			//	WriteLine("{},", FunctionInfo.bStatic);
			//	WriteLine("{},", FunctionInfo.bConst);
			//	WriteLine("{},", FunctionInfo.bNoexcept);
			//
			//	std::string Buffer = std::format("[](IObject* Object, FFunctionParams* Params){{IFunctionCaller<{}, {} ({}::*)(", QualifiedClassName, FunctionInfo.ReturnType, QualifiedClassName);
			//
			//	for (size_t i = 0; i < FunctionInfo.Parameters.size(); ++i)
			//	{
			//		Buffer += FunctionInfo.Parameters[i].Type;
			//	}
			//
			//	Buffer += std::format("), {},", QualifiedClassName);
			//
			//	for (size_t i = 0; i < FunctionInfo.Parameters.size(); ++i)
			//	{
			//		Buffer += FunctionInfo.Parameters[i].Type;
			//	}
			//
			//	Buffer.pop_back();
			//
			//	Buffer += std::format(">::Call(({}*)Object,", QualifiedClassName);
			//
			//	for (size_t i = 0; i < FunctionInfo.Parameters.size(); ++i)
			//	{
			//		Buffer += std::format("std::move(*({}*)Pack->", FunctionInfo.Parameters[i].Type);
			//
			//		for (size_t j = 0; j < i; ++j)
			//		{
			//			Buffer += "Next->";
			//		}
			//
			//		Buffer += "Value),";
			//	}
			//
			//	Buffer.pop_back();
			//	Buffer += ");});";
			//
			//	WriteLine(Buffer);
			//	PopScope();
			//	WriteLine("Functions.EmplaceBack({});", FunctionName);
			//}
			WriteLine("return Class;");
			PopScope();
			WriteLine("}");
			PopScope();
			WriteLine("}");
			
			WriteLine("FClass* {}::GetPrivateStaticClass()", QualifiedClassName);
			WriteLine("{");
			PushScope();
			WriteLine("static FClass Instance(");
			PushScope();
			WriteLine("\"{}\",", ClassInfo.Name);
			WriteLine("\"{}\",", ClassInfo.Namespace);
			WriteLine("nullptr,");
			WriteLine("0,");
			WriteLine("nullptr,");
			WriteLine("0,");
			WriteLine("nullptr,");
			WriteLine("0,");
			WriteLine("{},", ClassInfo.bAbstract);
			WriteLine("{},", ClassInfo.bFinal);
			WriteLine("{},", ClassInfo.bDerived);
			WriteLine("sizeof({}),", QualifiedClassName);
			WriteLine("{});", ClassUniqueID);
			PopScope();
			WriteLine("return &Instance;");
			PopScope();
			WriteLine("}");

			++ClassUniqueID;
		}
	}

	void FGenerator::WriteSerializeionImpl()
	{

	}

	void FGenerator::WriteReplicationImpl()
	{

	}
}
