#include "Generator.h"

#include <cassert>
#include <vector>

namespace Bloodshot::HeaderTool::Private
{
	FGeneratorOutput FGenerator::Generate(const std::vector<FClassInfo>& ClassInfos,
		const std::filesystem::path& PreprocessedHeaderPath)
	{
		this->ClassInfos = ClassInfos;
		this->PreprocessedHeaderPath = PreprocessedHeaderPath;

		SourceOutputStream = std::stringstream();
		HeaderOutputStream = std::stringstream();

		CurrentOutputStream = nullptr;
		PushedScopes = 0;

		GenerateReflectionAPI();

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

	void FGenerator::EmptyLine()
	{
		*CurrentOutputStream << "\n";
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

	void FGenerator::GenerateReflectionAPI()
	{
		CurrentOutputStream = &SourceOutputStream;

		WriteLine("// Generated code by HeaderTool");
		WriteLine("// Do not modify this manually!");
		EmptyLine();
		WriteLine("#include \"{}\"", PreprocessedHeaderPath.string());
		WriteLine("#include \"Reflection/Mirror.h\"");
		EmptyLine();
		WriteLine("namespace Bloodshot");
		WriteLine("{");
		PushScope();

		for (const FClassInfo& ClassInfo : ClassInfos)
		{
			WriteLine("template<>");
			WriteLine("FClass FMirror::GetClass<{}>()", ClassInfo.Name);
			WriteLine("{");

			PushScope();
			WriteLine("FClass Class;");
			WriteLine("Class.Name = \"{}\";", ClassInfo.Name);
			WriteLine("Class.bAbstract = {};", ClassInfo.bAbstract);
			WriteLine("Class.bFinal = {};", ClassInfo.bFinal);
			WriteLine("Class.bDerived = {};", ClassInfo.bDerived);
			EmptyLine();

			WriteLine("FClass Temp;");
			for (const std::string& BaseClassName : ClassInfo.BaseClassNames)
			{
				WriteLine("Temp.Name = \"{}\";", BaseClassName);
				WriteLine("Class.BaseClasses.push_back(Temp);");
				EmptyLine();
				WriteLine("Temp = {};");
			}

			EmptyLine();
			WriteLine("FProperty Property;");
			for (const FPropertyInfo& PropertyInfo : ClassInfo.Properties)
			{
				WriteLine("Property =");
				WriteLine("{");
				PushScope();
				WriteLine("\"{}\",", PropertyInfo.Type);
				WriteLine("\"{}\",", PropertyInfo.Name);
				WriteLine("{},", PropertyInfo.bStatic);
				WriteLine("{},", PropertyInfo.bSerialized);
				WriteLine("{}", PropertyInfo.bReplicated);
				PopScope();
				WriteLine("};");

				WriteLine("Class.Properties.push_back(Property);");
				EmptyLine();
				WriteLine("Property = {};");
			}

			EmptyLine();
			WriteLine("FFunction Function;");
			WriteLine("FParameter Parameter;");
			for (const FFunctionInfo& FunctionInfo : ClassInfo.Functions)
			{
				WriteLine("Function.ReturnType = \"{}\";", FunctionInfo.ReturnType);
				WriteLine("Function.Name = \"{}\";", FunctionInfo.Name);
				WriteLine("Function.bStatic = {};", FunctionInfo.bStatic);
				WriteLine("Function.bConst = {};", FunctionInfo.bConst);
				WriteLine("Function.bNoexcept = {};", FunctionInfo.bNoexcept);
				for (const FParameterInfo& ParameterInfo : FunctionInfo.Parameters)
				{
					WriteLine("Parameter =");
					WriteLine("{");
					PushScope();
					WriteLine("\"{}\"", ParameterInfo.Type);
					WriteLine("\"{}\"", ParameterInfo.Name);
					PopScope();
					WriteLine("};");
					WriteLine("Parameter = {};");
					WriteLine("Function.Parameters.push_back(Parameter);");
				}
				WriteLine("Class.Functions.push_back(Function);");
				EmptyLine();
				WriteLine("Function = {};");
			}
			WriteLine("return Class;");
			PopScope();
			WriteLine("}");

			std::unordered_map<std::string, std::vector<std::string>> GroupedByTypeProperties;

			for (const FPropertyInfo& PropertyInfo : ClassInfo.Properties)
			{
				GroupedByTypeProperties[PropertyInfo.Type].push_back(PropertyInfo.Name);
			}

			for (const std::pair<std::string, std::vector<std::string>>& PropertyPair : GroupedByTypeProperties)
			{
				const std::string& PropertyType = PropertyPair.first;
				const std::vector<std::string>& PropertyNames = PropertyPair.second;

				EmptyLine();
				WriteLine("template<>");
				WriteLine("{0}& FMirror::GetPropertyValue<{1}, {0}>({1}* const Object, FStringView PropertyName)",
					PropertyType,
					ClassInfo.Name);
				WriteLine("{");
				PushScope();
				WriteLine("if (false) {}");
				for (const std::string& PropertyName : PropertyNames)
				{
					WriteLine("else if (PropertyName == \"{}\")", PropertyName);
					WriteLine("{");
					PushScope();
					WriteLine("return Object->{};", PropertyName);
					PopScope();
					WriteLine("}");
				}
				WriteLine("return *({}*)nullptr;", PropertyType);
				PopScope();
				WriteLine("}");
			}

			PopScope();
			WriteLine("}");
		}
	}

	void FGenerator::GenerateSerializationAPI()
	{

	}

	void FGenerator::GenerateReplicationAPI()
	{

	}
}
