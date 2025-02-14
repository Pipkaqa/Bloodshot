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
			WriteLine("FClass* IClassConstructor::ConstructClass<{}>({}* Object)", ClassInfo.Name, ClassInfo.Name);
			WriteLine("{");
			PushScope();
			WriteLine("TArray<FClass*> BaseClasses;");

			for (const std::string& BaseClassName : ClassInfo.BaseClassNames)
			{
				static size_t BaseClassCount = 0;
				const std::string& BaseClassVarName = std::format("BaseClass_{}", BaseClassCount++);
				WriteLine("FClass* {} = new FClass(", BaseClassVarName);
				PushScope();
				WriteLine("\"{}\",", BaseClassName);
				WriteLine("{},");
				WriteLine("{},");
				WriteLine("{},");
				WriteLine("{},");
				WriteLine("{},");
				WriteLine("{},");
				WriteLine("{});");
				PopScope();
				WriteLine("BaseClasses.EmplaceBack({});", BaseClassVarName);
			}

			WriteLine("TArray<FProperty*> Properties;");

			for (const FPropertyInfo& PropertyInfo : ClassInfo.Properties)
			{
				static size_t PropertyCount = 0;
				const std::string& PropertyName = std::format("Property_{}", PropertyCount++);
				WriteLine("FProperty* {} = new FProperty(", PropertyName);
				PushScope();
				WriteLine("\"{}\",", PropertyInfo.Type);
				WriteLine("\"{}\",", PropertyInfo.Name);
				WriteLine("{},", PropertyInfo.bStatic);
				WriteLine("{},", PropertyInfo.bSerialized);
				WriteLine("{},", PropertyInfo.bReplicated);
				WriteLine("(void*)&Object->{});", PropertyInfo.Name);
				PopScope();
				WriteLine("Properties.EmplaceBack({});", PropertyName);
			}

			WriteLine("TArray<FFunction*> Functions;");

			for (const FFunctionInfo& FunctionInfo : ClassInfo.Functions)
			{
				static size_t FunctionCount = 0;
				const std::string& FunctionName = std::format("Function_{}", FunctionCount++);

				WriteLine("TArray<FParameter*> {}_Parameters;", FunctionName);

				for (const FParameterInfo& ParameterInfo : FunctionInfo.Parameters)
				{
					static size_t ParameterCount = 0;
					const std::string& ParameterName = std::format("Parameter_{}", ParameterCount++);
					WriteLine("FParameter* {} = new FParameter(", ParameterName);
					PushScope();
					WriteLine("\"{}\",", ParameterInfo.Type);
					WriteLine("\"{}\");", ParameterInfo.Name);
					PopScope();
					WriteLine("{}_Parameters.EmplaceBack({});", FunctionName, ParameterName);
				}

				WriteLine("FFunction* {} = new FFunction(", FunctionName);
				PushScope();
				WriteLine("\"{}\",", FunctionInfo.ReturnType);
				WriteLine("\"{}\",", FunctionInfo.Name);
				WriteLine("std::move({}_Parameters),", FunctionName);
				WriteLine("{},", FunctionInfo.bStatic);
				WriteLine("{},", FunctionInfo.bConst);
				WriteLine("{},", FunctionInfo.bNoexcept);

				std::string Buffer = std::format("[](IObject* Object, FFunctionParams* Params){{IFunctionCaller<{}, {} ({}::*)(", QualifiedClassName, FunctionInfo.ReturnType, QualifiedClassName);

				for (size_t i = 0; i < FunctionInfo.Parameters.size(); ++i)
				{
					Buffer += FunctionInfo.Parameters[i].Type;
				}

				Buffer += std::format("), {},", QualifiedClassName);

				for (size_t i = 0; i < FunctionInfo.Parameters.size(); ++i)
				{
					Buffer += FunctionInfo.Parameters[i].Type;
				}

				Buffer.pop_back();

				Buffer += std::format(">::Call(({}*)Object,", QualifiedClassName);

				for (size_t i = 0; i < FunctionInfo.Parameters.size(); ++i)
				{
					Buffer += std::format("std::move(*({}*)Pack->", FunctionInfo.Parameters[i].Type);

					for (size_t j = 0; j < i; ++j)
					{
						Buffer += "Next->";
					}

					Buffer += "Value),";
				}

				Buffer.pop_back();
				Buffer += ");});";

				WriteLine(Buffer);
				PopScope();
				WriteLine("Functions.EmplaceBack({});", FunctionName);
			}

			WriteLine("return new FClass(");
			PushScope();
			WriteLine("\"{}\",", ClassInfo.Name);
			WriteLine("\"{}\",", ClassInfo.Namespace);
			WriteLine("std::move(BaseClasses),");
			WriteLine("std::move(Properties),");
			WriteLine("std::move(Functions),");
			WriteLine("{},", ClassInfo.bAbstract);
			WriteLine("{},", ClassInfo.bFinal);
			WriteLine("{},", ClassInfo.bDerived);
			WriteLine("sizeof({}));", QualifiedClassName);
			PopScope();
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
			WriteLine("{},");
			WriteLine("{},");
			WriteLine("{},");
			WriteLine("{},", ClassInfo.bAbstract);
			WriteLine("{},", ClassInfo.bFinal);
			WriteLine("{},", ClassInfo.bDerived);
			WriteLine("sizeof({}));", QualifiedClassName);
			PopScope();
			WriteLine("return &Instance;");
			PopScope();
			WriteLine("}");
		}
	}

	void FGenerator::WriteSerializeionImpl()
	{

	}

	void FGenerator::WriteReplicationImpl()
	{

	}
}
