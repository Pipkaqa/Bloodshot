#include "Generator.h"

#include <cassert>
#include <vector>

namespace Bloodshot::HeaderTool
{
	FGeneratorOutput FGenerator::GenerateToSingleFiles(const std::vector<FHeaderFileInfo>& HeaderInfos)
	{
		Clear();
		CurrentOutputStream = &SourceOutputStream;

		WriteLine("// Generated code by HeaderTool");
		WriteLine("// Do not modify this manually!");
		EmptyLine();
		WriteLine("#include \"Reflection/Mirror.h\"");

		for (const FHeaderFileInfo& HeaderInfo : HeaderInfos)
		{
			WriteLine("#include \"{}\"", HeaderInfo.Path.string());
		}

		EmptyLine();
		WriteLine("namespace Bloodshot");
		WriteLine("{");
		PushScope();

		for (const FHeaderFileInfo& HeaderInfo : HeaderInfos)
		{
			WriteMirrorAPI(HeaderInfo);
			EmptyLine();
		}

		PopScope();
		WriteLine("}");

		return FGeneratorOutput{SourceOutputStream.str(), HeaderOutputStream.str()};
	}

	FGeneratorOutput FGenerator::Generate(const FHeaderFileInfo& HeaderInfo)
	{
		Clear();
		CurrentOutputStream = &SourceOutputStream;

		WriteLine("// Generated code by HeaderTool");
		WriteLine("// Do not modify this manually!");
		EmptyLine();
		WriteLine("#include \"{}\"", HeaderInfo.Path.string());
		WriteLine("#include \"Reflection/Mirror.h\"");
		EmptyLine();
		WriteLine("namespace Bloodshot");
		WriteLine("{");
		PushScope();

		WriteMirrorAPI(HeaderInfo);

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

	void FGenerator::Clear()
	{
		SourceOutputStream = std::stringstream();
		HeaderOutputStream = std::stringstream();

		CurrentOutputStream = nullptr;
		PushedScopes = 0;
	}

	void FGenerator::WriteMirrorAPI(const FHeaderFileInfo& HeaderInfo)
	{
		for (const FClassInfo& ClassInfo : HeaderInfo.ClassInfos)
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
				WriteLine("Class.BaseClasses.EmplaceBack(std::move(Temp));");
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

				WriteLine("Class.Properties.EmplaceBack(std::move(Property));");
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
					WriteLine("Function.Parameters.EmplaceBack(std::move(Parameter));");
				}
				WriteLine("Class.Functions.EmplaceBack(std::move(Function));");
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

				EmptyLine();
				WriteLine("template<>");
				WriteLine("const {0}& FMirror::GetPropertyValue<{1}, {0}>(const {1}* const Object, FStringView PropertyName)",
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
		}
	}

	void FGenerator::GenerateSerializationAPI()
	{

	}

	void FGenerator::GenerateReplicationAPI()
	{

	}
}
